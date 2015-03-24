/** @file nanactrie.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "nanactrie.h"

#define BITMAP_TYPE unsigned int
#define HASH_TYPE unsigned int
#define STEP_SIZE __builtin_ctz(sizeof(BITMAP_TYPE) * 8)

static const unsigned int step_size = STEP_SIZE;
static const unsigned int step_mask = (0x1 << STEP_SIZE) - 1;
static const int max_depth = sizeof(HASH_TYPE) * 8 / STEP_SIZE;

typedef enum _node_type_t
{
    INODE, DNODE, CNODE, TNODE, LNODE
} node_type_t;

typedef struct _ctrie_node_t
{
    struct _ctrie_node_t* prev;
    node_type_t type;
} ctrie_node_t;

typedef struct _indirected_node_t
{
    ctrie_node_t node;
} indirected_node_t;

typedef struct _branch_node_t
{
    ctrie_node_t node;
} branch_node_t;

typedef struct _data_node_t
{
    branch_node_t branch_node;
    data_t data;
} data_node_t;

typedef struct _indirection_node_t
{
    branch_node_t branch_node;
    indirected_node_t *volatile ptr;
} indirection_node_t;

typedef struct _list_node_t
{
    indirected_node_t indirected_node;
    unsigned int count;
    data_node_t* data_nodes[0];
} list_node_t;

typedef struct _tomb_node_t
{
    indirected_node_t indirected_node;
    data_node_t* data_node;
} tomb_node_t;

typedef struct _container_node_t
{
    indirected_node_t indirected_node;
    BITMAP_TYPE bitmap;
    int depth;
    branch_node_t* branches[0];
} container_node_t;


static data_t sentinels[3];
static data_t* RESTART = &sentinels[0];

static HASH_TYPE get_string_hash(char const* s)
{
    HASH_TYPE hash = 0;
    while (*s)
        hash = hash * 101  +  *s++;

    return hash;
}

static inline BITMAP_TYPE get_bitmap_mask(HASH_TYPE hash, int depth)
{
    return (BITMAP_TYPE) (0x1L << (((hash >> (step_size * depth)) & step_mask)));
}

static inline unsigned int get_container_position(container_node_t* container_node, BITMAP_TYPE bitmap_hash)
{
    BITMAP_TYPE mask = (bitmap_hash - 1);
    BITMAP_TYPE masked_bitmap = container_node->bitmap & mask;

    if (masked_bitmap == 0)
        return 0;

    return __builtin_popcount(masked_bitmap);
}

static inline unsigned int get_container_count(container_node_t* container_node)
{
    return get_container_position(container_node, 0);
}

static inline indirection_node_t* get_root(ctrie_t* ctrie)
{
    return (indirection_node_t *) ctrie->root;
}

void free_data_contents(data_t* data)
{
    free(data->key);
    free(data->value);

    return;
}

void free_data(data_t* data)
{
    free_data_contents(data);
    free(data);

    return;
}

void free_node(ctrie_node_t* node)
{
    if (node->type == DNODE)
        free_data_contents(&((data_node_t *) node)->data);

    free(node);

    return;
}

void free_collided_node(indirection_node_t* free_indirection_node, data_node_t* data_node)
{
    indirection_node_t* indirection_node = free_indirection_node;
    do
    {
        indirected_node_t* indirected_node = indirection_node->ptr;
        if (indirected_node->node.type == LNODE)
        {
            list_node_t* list_node = (list_node_t *) indirected_node;
            if (list_node->data_nodes[0] == data_node)
            {
                free_node(&list_node->data_nodes[1]->branch_node.node);
            }
            else
            {
                free_node(&list_node->data_nodes[0]->branch_node.node);
            }

            free_node(&list_node->indirected_node.node);
            break;
        }

        assert(indirected_node->node.type == CNODE);
        container_node_t* container_node = (container_node_t *) indirected_node;
        unsigned int free_count = get_container_count(container_node);
        if (free_count > 1)
        {
            assert(free_count == 2);
            if (container_node->branches[0] == &data_node->branch_node)
            {
                free_node(&container_node->branches[1]->node);
            }
            else
            {
                free_node(&container_node->branches[0]->node);
            }

            free_node(&container_node->indirected_node.node);
            break;
        }

        assert(container_node->branches[0]->node.type == INODE);
        indirection_node = (indirection_node_t *) container_node->branches[0];
        free_node(&container_node->indirected_node.node);
    } while (true);

    return;
}

static inline void init_node(ctrie_node_t* node, node_type_t type)
{
    node->prev = NULL;
    node->type = type;

    return;
}

indirection_node_t* create_indirection_node(indirected_node_t* indirected_node)
{
    indirection_node_t* indirection_node = malloc(sizeof(indirection_node_t));
    init_node(&indirection_node->branch_node.node, INODE);
    indirection_node->ptr = indirected_node;

    return indirection_node;
}

container_node_t* create_container_node(BITMAP_TYPE bitmap, int depth, int count)
{
    container_node_t* container_node = malloc(sizeof(container_node_t) + sizeof(branch_node_t *) * count);
    init_node(&container_node->indirected_node.node, CNODE);
    container_node->bitmap = bitmap;
    container_node->depth = depth;
    memset(container_node->branches, 0, sizeof(branch_node_t *) * count);

    return container_node;
}

void init_ctrie(ctrie_t* ctrie)
{
    container_node_t* container_node = create_container_node(0, 0, 0);
    ctrie->root = create_indirection_node(&container_node->indirected_node);

    return;
}

ctrie_t* create_ctrie()
{
    ctrie_t* ctrie = malloc(sizeof(ctrie_t));
    pthread_mutex_init(&ctrie->mutex, NULL);
    ctrie->ref_count = 0;
    ctrie->freed_nodes_list = NULL;

    init_ctrie(ctrie);

    return ctrie;
}

void destroy_ctrie(ctrie_t* ctrie)
{
    pthread_mutex_destroy(&ctrie->mutex);

    free(ctrie);

    return;
}

void enter_ctrie(ctrie_t* ctrie)
{
    pthread_mutex_lock(&ctrie->mutex);

    assert((ctrie->ref_count != 0) || (ctrie->freed_nodes_list == NULL));
    ++ctrie->ref_count;

    pthread_mutex_unlock(&ctrie->mutex);

    return;
}

void exit_ctrie(ctrie_t* ctrie)
{
    ctrie_node_t* freed_nodes_list = NULL;

    pthread_mutex_lock(&ctrie->mutex);

    assert((ctrie->ref_count != 0) || (ctrie->freed_nodes_list == NULL));
    --ctrie->ref_count;
    if (ctrie->ref_count == 0)
    {
        freed_nodes_list = (ctrie_node_t*) ctrie->freed_nodes_list;
        ctrie->freed_nodes_list = NULL;
    }

    pthread_mutex_unlock(&ctrie->mutex);

    while (freed_nodes_list != NULL)
    {
        ctrie_node_t* node = freed_nodes_list;
        freed_nodes_list = freed_nodes_list->prev;
        free_node(node);
    }

    return;
}

data_t* copy_data(data_t* data)
{
    if (data == NULL)
        return NULL;

    data_t *data_copy = malloc(sizeof(data_t));
    data_copy->key = strdup(data->key);
    data_copy->value = strdup(data->value);
    data_copy->rev = data->rev;

    return data_copy;
}

branch_node_t* resurrect(branch_node_t* branch_node, ctrie_node_t* freed_nodes[], int* num_freed_nodes)
{
    if (branch_node->node.type == INODE)
    {
        indirection_node_t* indirection_node = (indirection_node_t *) branch_node;
        indirected_node_t* indirected_node = indirection_node->ptr;
        if (indirected_node->node.type == TNODE)
        {
            freed_nodes[*num_freed_nodes] = &branch_node->node;
            *num_freed_nodes = *num_freed_nodes + 1;

            freed_nodes[*num_freed_nodes] = &indirected_node->node;
            *num_freed_nodes = *num_freed_nodes + 1;

            tomb_node_t* tomb_node = (tomb_node_t *) indirected_node;
            return &tomb_node->data_node->branch_node;
        }
    }

    return branch_node;
}

void add_freed_node(ctrie_t* ctrie, ctrie_node_t* node)
{
    ctrie_node_t* current =  ctrie->freed_nodes_list;
    do
    {
        node->prev = current;
        current = __sync_val_compare_and_swap(&ctrie->freed_nodes_list, current, node);
    } while (current != node->prev);

    return;
}


void add_freed_nodes(ctrie_t* ctrie, ctrie_node_t* freed_nodes[], int num_freed_nodes)
{
    int i;
    for (i = 0; i < num_freed_nodes; i++)
        add_freed_node(ctrie, freed_nodes[i]);

    return;
}

tomb_node_t* create_tomb_node(data_node_t* data_node)
{
    tomb_node_t* tomb_node = malloc(sizeof(tomb_node_t));
    init_node(&tomb_node->indirected_node.node, TNODE);
    tomb_node->data_node = data_node;

    return tomb_node;
}

data_node_t* get_list_node(list_node_t* list_node, char *key)
{
    unsigned int i;
    for (i = 0; i < list_node->count; i++)
    {
        data_node_t* data_node = list_node->data_nodes[i];
        if (strcmp(data_node->data.key, key) == 0)
            return data_node;
    }

    return NULL;
}

list_node_t* create_list_node(unsigned int count)
{
    list_node_t* list_node = malloc(sizeof(list_node_t) + sizeof(data_node_t *) * count);
    init_node(&list_node->indirected_node.node, LNODE);
    list_node->count = count;
    memset(list_node->data_nodes, 0, sizeof(data_node_t *) * count);

    return list_node;
}

data_node_t* create_data_node(data_t* data)
{
    data_node_t* data_node = malloc(sizeof(data_node_t));
    init_node(&data_node->branch_node.node, DNODE);
    data_node->data.key = strdup(data->key);
    data_node->data.value = strdup(data->value);
    data_node->data.rev = data->rev;

    return data_node;

}

indirected_node_t* contract_list_node(list_node_t* list_node, data_t* delete_data,
                                      data_node_t** found_node)
{
    *found_node = NULL;

    unsigned int i;
    for (i = 0; i < list_node->count; i++)
    {
        data_node_t* data_node = list_node->data_nodes[i];
        if (strcmp(data_node->data.key, delete_data->key) == 0)
            break;
    }

    if (i == list_node->count)
        return &list_node->indirected_node;

    *found_node = list_node->data_nodes[i];
    if ((delete_data->rev != (unsigned long) -1) && ((*found_node)->data.rev != delete_data->rev))
        return &list_node->indirected_node;

    unsigned int removed_position = i;
    if (list_node->count == 2)
    {
        data_node_t* data_node = list_node->data_nodes[(removed_position == 0) ? 1 : 0];
        tomb_node_t* tomb_node = create_tomb_node(data_node);
        return &tomb_node->indirected_node;
    }

    list_node_t* contracted_node = create_list_node(list_node->count - 1);

    unsigned int j;
    for (i = 0; i < removed_position; i++)
        contracted_node->data_nodes[i] = list_node->data_nodes[i];
    for (i = removed_position + 1, j = removed_position; i < list_node->count; i++, j++)
        contracted_node->data_nodes[j] = list_node->data_nodes[i];

    return &contracted_node->indirected_node;
}

indirected_node_t* expand_list_node(list_node_t* list_node, data_t* add_data, data_node_t** found_or_added_node)
{
	*found_or_added_node = NULL;

	unsigned int i;
	for (i = 0; i < list_node->count; i++)
	{
		data_node_t* data_node = list_node->data_nodes[i];
		if (strcmp(data_node->data.key, add_data->key) == 0)
			break;
	}

	if (i < list_node->count)
	{
		*found_or_added_node = list_node->data_nodes[i];
		return &list_node->indirected_node;
	}

	*found_or_added_node = create_data_node(add_data);
	list_node_t* expanded_node = create_list_node(list_node->count + 1);
	for (i = 0; i < list_node->count; i++)
		expanded_node->data_nodes[i] = list_node->data_nodes[i];
	expanded_node->data_nodes[i] = *found_or_added_node;

	return &expanded_node->indirected_node;
}

list_node_t* update_list_node(list_node_t* list_node, data_t* update_data, int* found_node_index)
{
    *found_node_index = -1;

    data_node_t* data_node;
    unsigned int i;
    for (i = 0; i < list_node->count; i++)
    {
        data_node = list_node->data_nodes[i];
        if (strcmp(data_node->data.key, update_data->key) == 0)
            break;
    }

    if (i == list_node->count)
        return list_node;

    *found_node_index = i;
    if (data_node->data.rev != update_data->rev)
        return list_node;

    data_t data = { update_data->key, update_data->value, update_data->rev + 1 };
    data_node = create_data_node(&data);
    list_node_t* updated_node = create_list_node(list_node->count);
    for (i = 0; i < list_node->count; i++)
        updated_node->data_nodes[i] = list_node->data_nodes[i];
    updated_node->data_nodes[i] = data_node;

    return updated_node;
}

void assert_valid_containee(int depth, branch_node_t* branch_node)
{
    if (depth > 0)
    {
        ;
    }
    else if (branch_node == NULL)
    {
        ;
    }
}

void assert_valid_container(container_node_t* container_node)
{
    unsigned int count = get_container_count(container_node);

    unsigned int i;
    for (i = 0; i < count; i++)
        assert_valid_containee(container_node->depth, container_node->branches[i]);

    return;
}

container_node_t* copy_container_node(container_node_t* container_node, unsigned int count)
{
	container_node_t* container_node_copy = malloc(sizeof(container_node_t) + sizeof(branch_node_t *) * count);
	init_node(&container_node_copy->indirected_node.node, CNODE);
	container_node_copy->bitmap = container_node->bitmap;
	container_node_copy->depth = container_node->depth;
	memset(container_node_copy->branches, 0, sizeof(branch_node_t *) * count);

	return container_node_copy;
}

indirected_node_t* contract_container_node(container_node_t* container_node, int depth,
                                           BITMAP_TYPE removed_bit_mask,
                                           unsigned int removed_position)
{
    unsigned int count = get_container_count(container_node);
    assert(count > 0);

    if ((depth > 0) && (count == 2))
    {
        int position = (removed_position == 0) ? 1 : 0;
        branch_node_t* branch_node = container_node->branches[position];
        if (branch_node->node.type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            tomb_node_t* tomb_node = create_tomb_node(data_node);
            return &tomb_node->indirected_node;
        }
    }

    container_node_t* contracted_node = copy_container_node(container_node, count - 1);
    contracted_node->bitmap &= ~removed_bit_mask;

    unsigned int i;
    for (i = 0; i < removed_position; i++)
        contracted_node->branches[i] = container_node->branches[i];
    for (i++; i < count; i++)
        contracted_node->branches[i - 1] = container_node->branches[i];

    return &contracted_node->indirected_node;
}

container_node_t* expand_container_node(container_node_t* container_node,
                                        BITMAP_TYPE add_bit_mask,
                                        unsigned int add_position,
                                        data_t* data)
{
    unsigned int count = get_container_count(container_node);
    container_node_t* expanded_node = copy_container_node(container_node, count + 1);
    expanded_node->bitmap |= add_bit_mask;

    unsigned int i;
    for (i = 0; i < add_position; i++)
        expanded_node->branches[i] = container_node->branches[i];

    data_node_t* data_node = create_data_node(data);
    expanded_node->branches[i] = &data_node->branch_node;

    for (; i < count; i++)
        expanded_node->branches[i + 1] = container_node->branches[i];

    return expanded_node;
}

container_node_t* update_container_node(container_node_t* container_node,
                                        unsigned int update_position,
                                        branch_node_t* updated_branch_node)
{
    unsigned int count = get_container_count(container_node);
    container_node_t* updated_node = copy_container_node(container_node, count);

    unsigned int i;
    for (i = 0; i < count; i++)
        updated_node->branches[i] = container_node->branches[i];

    updated_node->branches[update_position] = updated_branch_node;

    return updated_node;
}

indirected_node_t* compress(container_node_t* container_node, int depth,
                            ctrie_node_t* freed_nodes[], int* num_freed_nodes)
{
    *num_freed_nodes = 0;

    unsigned int count = get_container_count(container_node);
    assert(count > 0);
    if ((depth > 0) && (count == 1))
    {
        branch_node_t* branch_node = container_node->branches[0];
        branch_node_t* resurrected_node = resurrect(branch_node, freed_nodes, num_freed_nodes);
        if (resurrected_node != branch_node)
        {
            tomb_node_t* tomb_node = create_tomb_node((data_node_t *) resurrected_node);
            return &(tomb_node->indirected_node);
        }

        return NULL;
    }

    container_node_t* compressed_node = copy_container_node(container_node, count);

    unsigned int i;
    for (i = 0; i < count; i++)
    {
        branch_node_t* branch_node = container_node->branches[i];
        branch_node_t* resurrected_node = resurrect(branch_node, freed_nodes, num_freed_nodes);
        compressed_node->branches[i] = resurrected_node;
    }

    if (*num_freed_nodes == 0)
    {
        free_node(&compressed_node->indirected_node.node);
        return NULL;
    }

    return &compressed_node->indirected_node;
}

void clean(ctrie_t* ctrie, indirection_node_t* current_node, int depth)
{
    indirected_node_t* indirected_node = current_node->ptr;
    if (indirected_node->node.type == CNODE)
    {
        ctrie_node_t* freed_nodes[2 * (step_mask + 1)];
        int num_freed_nodes;

        container_node_t* container_node = (container_node_t *) indirected_node;
        indirected_node_t* compressed_node = compress(container_node, depth, freed_nodes, &num_freed_nodes);
        if (compressed_node != NULL)
        {
            bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, compressed_node);
            if (success == true)
            {
                add_freed_nodes(ctrie, freed_nodes, num_freed_nodes);
                add_freed_node(ctrie, &indirected_node->node);
            }
            else
            {
                free_node(&compressed_node->node);
            }
        }
    }

    return;
}

data_t* internal_get(ctrie_t* ctrie, indirection_node_t* current_node,
                     char *key, HASH_TYPE hash,
                     int depth, indirection_node_t* parent_node)
{
    indirected_node_t* indirected_node = current_node->ptr;
    node_type_t indirected_node_type = indirected_node->node.type;
    if (indirected_node_type == CNODE)
    {
        container_node_t* container_node = (container_node_t *) indirected_node;
        BITMAP_TYPE bitmap_mask = get_bitmap_mask(hash, depth);
        if ((container_node->bitmap & bitmap_mask) == 0)
            return NULL;

        unsigned int container_position = get_container_position(container_node, bitmap_mask);
        branch_node_t* branch_node = container_node->branches[container_position];
        node_type_t branch_node_type = branch_node->node.type;
        if (branch_node_type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            data_t* data = &data_node->data;
            if (strcmp(data->key, key) != 0)
                return NULL;

            return data;
        }
        else if (branch_node_type == INODE)
        {
            // Tail recursive call
            return internal_get(ctrie, (indirection_node_t *) branch_node,
                                key, hash, depth + 1, current_node);
        }

        assert(false);
    }
    else if (indirected_node_type == LNODE)
    {
        data_node_t* data_node = get_list_node((list_node_t *) indirected_node, key);

        return &data_node->data;
    }
    else if (indirected_node_type == TNODE)
    {
        assert(parent_node != NULL);
        clean(ctrie, parent_node, depth - 1);

        return RESTART;
    }
    else
    {
        assert(false);
    }

    return NULL;
}

data_t ctrie_get_data(ctrie_t* ctrie, char *key)
{
    enter_ctrie(ctrie);

    HASH_TYPE hash = get_string_hash(key);

    data_t* data;
    do
    {
        indirection_node_t* root = get_root(ctrie);
        data = internal_get(ctrie, root, key, hash, 0, NULL);
    } while (data == RESTART);

    //data_t* data_copy = copy_data(data);
    data_t data_copy = { key, NULL, -1 };
    if (data != NULL)
        data_copy = *data;

    exit_ctrie(ctrie);

    return data_copy;
}

data_t* internal_delete(ctrie_t* ctrie, indirection_node_t* current_node,
                        data_t* delete_data, HASH_TYPE hash,
                        int depth, indirection_node_t* parent_node)
{
    indirected_node_t* indirected_node = current_node->ptr;
    node_type_t indirected_node_type = indirected_node->node.type;
    if (indirected_node_type == CNODE)
    {
        container_node_t* container_node = (container_node_t *) indirected_node;
        BITMAP_TYPE bitmap_mask = get_bitmap_mask(hash, depth);
        if ((container_node->bitmap & bitmap_mask) == 0)
            return NULL;

        unsigned int container_position = get_container_position(container_node, bitmap_mask);
        branch_node_t* branch_node = container_node->branches[container_position];
        node_type_t branch_node_type = branch_node->node.type;
        if (branch_node_type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            data_t* data = &data_node->data;
            if (strcmp(data->key, delete_data->key) != 0)
                return NULL;

            if ((delete_data->rev != (unsigned long) -1) && (data->rev != delete_data->rev))
                return delete_data;

            indirected_node_t* contracted_node = contract_container_node(container_node, depth,
                                                                         bitmap_mask, container_position);
            bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, contracted_node);
            if (success == false)
            {
                free_node(&contracted_node->node);

                return RESTART;
            }

            add_freed_node(ctrie, &indirected_node->node);
            add_freed_node(ctrie, &data_node->branch_node.node);

            ++delete_data->rev;
            return delete_data;
        }
        else if (branch_node_type == INODE)
        {
            // Tail recursive call
            return internal_delete(ctrie, (indirection_node_t *) branch_node,
                                   delete_data, hash, depth + 1, current_node);
        }

        assert(false);
    }
    else if (indirected_node_type == LNODE)
    {
        data_node_t* found_node;
        list_node_t* list_node = (list_node_t *) indirected_node;
        indirected_node_t* contracted_node = contract_list_node(list_node, delete_data, &found_node);
        if (found_node == NULL)
        {
            return NULL;
        }
        else if (contracted_node == indirected_node)
        {
            return delete_data;
        }

        bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, contracted_node);
        if (success == false)
        {
            free_node(&contracted_node->node);

            return RESTART;
        }

        add_freed_node(ctrie, &indirected_node->node);
        add_freed_node(ctrie, &found_node->branch_node.node);

        ++delete_data->rev;
        return delete_data;
    }
    else if (indirected_node_type == TNODE)
    {
        assert(parent_node != NULL);
        clean(ctrie, parent_node, depth - 1);

        return RESTART;
    }
    else
    {
        assert(false);
    }

    return NULL;
}

data_t* ctrie_delete_data(ctrie_t* ctrie, data_t* data)
{
    enter_ctrie(ctrie);

    HASH_TYPE hash = get_string_hash(data->key);

    data_t* deleted_data;
    do
    {
        indirection_node_t* root = get_root(ctrie);
        deleted_data = internal_delete(ctrie, root, data, hash, 0, NULL);
    } while (deleted_data == RESTART);

    exit_ctrie(ctrie);

    return deleted_data;
}

indirection_node_t* handle_hash_collision(data_node_t* data_node, HASH_TYPE data_node_hash,
                                          data_t* add_data, HASH_TYPE data_hash, int depth)
{
    BITMAP_TYPE data_node_mask = get_bitmap_mask(data_node_hash, depth);
    BITMAP_TYPE data_mask = get_bitmap_mask(data_hash, depth);

    indirected_node_t* indirected_node;
    if (data_node_mask == data_mask)
    {
        if (depth == max_depth)
        {
            list_node_t* list_node = create_list_node(2);
            list_node->data_nodes[0] = data_node;
            list_node->data_nodes[1] = create_data_node(add_data);

            indirected_node = &list_node->indirected_node;
        }
        else
        {
            indirection_node_t* indirection_node = handle_hash_collision(data_node, data_node_hash,
                                                                         add_data, data_hash,
                                                                         depth + 1);
            container_node_t* container_node = create_container_node(data_mask, depth, 1);
            container_node->branches[0] = &indirection_node->branch_node;

            indirected_node = &container_node->indirected_node;
        }
    }
    else
    {
        container_node_t* container_node = create_container_node(data_node_mask | data_mask, depth, 2);
        data_node_t* add_data_node = create_data_node(add_data);
        if (data_node_mask < data_mask)
        {
            container_node->branches[0] = &data_node->branch_node;
            container_node->branches[1] = &add_data_node->branch_node;
        }
        else
        {
            container_node->branches[1] = &data_node->branch_node;
            container_node->branches[0] = &add_data_node->branch_node;
        }

        indirected_node = &container_node->indirected_node;
    }

    return create_indirection_node(indirected_node);
}

data_t* internal_add(ctrie_t* ctrie, indirection_node_t* current_node,
                     data_t* add_data, HASH_TYPE hash,
                     int depth, indirection_node_t* parent_node)
{
    indirected_node_t* indirected_node = current_node->ptr;
    node_type_t indirected_node_type = indirected_node->node.type;
    if (indirected_node_type == CNODE)
    {
        container_node_t* container_node = (container_node_t *) indirected_node;
        BITMAP_TYPE bitmap_mask = get_bitmap_mask(hash, depth);
        unsigned int container_position = get_container_position(container_node, bitmap_mask);
        if ((container_node->bitmap & bitmap_mask) == 0)
        {
            container_node_t* expanded_node = expand_container_node(container_node, bitmap_mask,
                                                                    container_position, add_data);

            bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node,
                                                        &expanded_node->indirected_node);
            if (success == false)
            {
                data_node_t* added_data_node = (data_node_t *) expanded_node->branches[container_position];
                free_node(&added_data_node->branch_node.node);
                free_node(&expanded_node->indirected_node.node);

                return RESTART;
            }

            add_freed_node(ctrie, &container_node->indirected_node.node);
            return NULL;
        }

        branch_node_t* branch_node = container_node->branches[container_position];
        node_type_t branch_node_type = branch_node->node.type;
        if (branch_node_type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            data_t* data = &data_node->data;
            if (strcmp(data->key, add_data->key) == 0)
            {
                add_data->rev = data->rev;
                return add_data;
            }

            HASH_TYPE data_node_hash = get_string_hash(data_node->data.key);
            indirection_node_t* indirection_node = handle_hash_collision(data_node, data_node_hash,
                                                                         add_data, hash, depth + 1);
            container_node_t* updated_container_node = update_container_node(container_node, container_position,
                                                                             &indirection_node->branch_node);
            bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, updated_container_node);
            if (success == false)
            {
                free_collided_node(indirection_node, data_node);
                free_node(&updated_container_node->indirected_node.node);

                return RESTART;
            }

            add_freed_node(ctrie, &indirected_node->node);

            return NULL;
        }
        else if (branch_node_type == INODE)
        {
            // Tail recursive call
            return internal_add(ctrie, (indirection_node_t *) branch_node,
                                add_data, hash, depth + 1, current_node);
        }

        assert(false);
    }
    else if (indirected_node_type == LNODE)
    {
        data_node_t* found_or_added_node;
        list_node_t* list_node = (list_node_t *) indirected_node;
        indirected_node_t* expanded_node = expand_list_node(list_node, add_data, &found_or_added_node);
        if (expanded_node == indirected_node)
        {
            add_data->rev = found_or_added_node->data.rev;
            return add_data;
        }

        bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, expanded_node);
        if (success == false)
        {
            free_node(&expanded_node->node);
            free_node(&found_or_added_node->branch_node.node);

            return RESTART;
        }

        add_freed_node(ctrie, &indirected_node->node);
        return NULL;
    }
    else if (indirected_node_type == TNODE)
    {
        assert(parent_node != NULL);
        clean(ctrie, parent_node, depth - 1);

        return RESTART;
    }
    else
    {
        assert(false);
    }

    return NULL;
}

data_t* ctrie_add_data(ctrie_t* ctrie, data_t *data)
{
    enter_ctrie(ctrie);

    HASH_TYPE hash = get_string_hash(data->key);

    data_t* added_data;
    do
    {
        indirection_node_t* root = get_root(ctrie);
        added_data = internal_add(ctrie, root, data, hash, 0, NULL);
    } while (added_data == RESTART);

    exit_ctrie(ctrie);

    return added_data;
}

data_t* internal_update(ctrie_t* ctrie, indirection_node_t* current_node,
                        data_t* update_data, HASH_TYPE hash,
                        int depth, indirection_node_t* parent_node)
{
    indirected_node_t* indirected_node = current_node->ptr;
    node_type_t indirected_node_type = indirected_node->node.type;
    if (indirected_node_type == CNODE)
    {
        container_node_t* container_node = (container_node_t *) indirected_node;
        BITMAP_TYPE bitmap_mask = get_bitmap_mask(hash, depth);
        unsigned int container_position = get_container_position(container_node, bitmap_mask);
        if ((container_node->bitmap & bitmap_mask) == 0)
            return NULL;

        branch_node_t* branch_node = container_node->branches[container_position];
        node_type_t branch_node_type = branch_node->node.type;
        if (branch_node_type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            data_t* data = &data_node->data;
            if (strcmp(data->key, update_data->key) != 0)
                return NULL;

            if (data->rev != update_data->rev)
                return update_data;

            data_node_t* updated_data_node = create_data_node(update_data);
            ++updated_data_node->data.rev;
            container_node_t* updated_container_node = update_container_node(container_node, container_position,
                                                                             &updated_data_node->branch_node);
            bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node, updated_container_node);
            if (success == false)
            {
                free_node(&updated_data_node->branch_node.node);
                free_node(&updated_container_node->indirected_node.node);

                return RESTART;
            }

            add_freed_node(ctrie, &data_node->branch_node.node);
            add_freed_node(ctrie, &container_node->indirected_node.node);

            ++update_data->rev;
            return update_data;
        }
        else if (branch_node_type == INODE)
        {
            // Tail recursive call
            return internal_update(ctrie, (indirection_node_t *) branch_node,
                                   update_data, hash, depth + 1, current_node);
        }

        assert(false);
    }
    else if (indirected_node_type == LNODE)
    {
        int found_node_index;
        list_node_t* list_node = (list_node_t *) indirected_node;
        list_node_t* updated_node = update_list_node(list_node, update_data, &found_node_index);
        if (found_node_index == -1)
        {
            return NULL;
        }
        else if ((&updated_node->indirected_node) == indirected_node)
        {
            return &list_node->data_nodes[found_node_index]->data;
        }

        bool success = __sync_bool_compare_and_swap(&current_node->ptr, indirected_node,
                                                    &updated_node->indirected_node);
        if (success == false)
        {
            free_node(&updated_node->data_nodes[found_node_index]->branch_node.node);
            free_node(&updated_node->indirected_node.node);

            return RESTART;
        }

        add_freed_node(ctrie, &list_node->data_nodes[found_node_index]->branch_node.node);
        add_freed_node(ctrie, &indirected_node->node);

        ++update_data->rev;
        return update_data;
    }
    else if (indirected_node_type == TNODE)
    {
        assert(parent_node != NULL);
        clean(ctrie, parent_node, depth - 1);

        return RESTART;
    }
    else
    {
        assert(false);
    }

    return NULL;
}

data_t* update_data_node(data_node_t* data_node, data_t* update_data)
{
    data_t* data = &data_node->data;
    assert(strcmp(data->key, update_data->key) == 0);

    if (data->rev == update_data->rev)
    {
        bool success = __sync_bool_compare_and_swap(&data->rev, update_data->rev, update_data->rev + 1);
        if (success == true)
            ++update_data->rev;
    }

    return update_data;
}

data_t* internal_inplace_update(ctrie_t* ctrie, indirection_node_t* current_node,
                                data_t* update_data, HASH_TYPE hash,
                                int depth, indirection_node_t* parent_node)
{
    indirected_node_t* indirected_node = current_node->ptr;
    node_type_t indirected_node_type = indirected_node->node.type;
    if (indirected_node_type == CNODE)
    {
        container_node_t* container_node = (container_node_t *) indirected_node;
        BITMAP_TYPE bitmap_mask = get_bitmap_mask(hash, depth);
        unsigned int container_position = get_container_position(container_node, bitmap_mask);
        if ((container_node->bitmap & bitmap_mask) == 0)
            return NULL;

        branch_node_t* branch_node = container_node->branches[container_position];
        node_type_t branch_node_type = branch_node->node.type;
        if (branch_node_type == DNODE)
        {
            data_node_t* data_node = (data_node_t *) branch_node;
            data_t* data = &data_node->data;
            if (strcmp(data->key, update_data->key) != 0)
                return NULL;

            return update_data_node(data_node, update_data);
        }
        else if (branch_node_type == INODE)
        {
            // Tail recursive call
            return internal_inplace_update(ctrie, (indirection_node_t *) branch_node,
                                           update_data, hash, depth + 1, current_node);
        }

        assert(false);
    }
    else if (indirected_node_type == LNODE)
    {
        data_node_t* data_node = get_list_node((list_node_t *) indirected_node, update_data->key);
        if (data_node == NULL)
            return NULL;

        return update_data_node(data_node, update_data);
    }
    else if (indirected_node_type == TNODE)
    {
        assert(parent_node != NULL);
        clean(ctrie, parent_node, depth - 1);

        return RESTART;
    }
    else
    {
        assert(false);
    }

    return NULL;
}

data_t* ctrie_update_data(ctrie_t* ctrie, data_t *data)
{
    enter_ctrie(ctrie);

    HASH_TYPE hash = get_string_hash(data->key);

    data_t* updated_data;
    do
    {
        indirection_node_t* root = get_root(ctrie);
        updated_data = internal_update(ctrie, root, data, hash, 0, NULL);
    } while (updated_data == RESTART);

    exit_ctrie(ctrie);

    return updated_data;
}

typedef enum _walk_order_t { pre_order, in_order, post_order } walk_order_t;

typedef void (*walker_fn)(ctrie_node_t* node, walk_order_t order, void* context);

void walk_node(ctrie_node_t* node, walker_fn walker, void* context);

void walk_data_node(data_node_t* data_node, walker_fn walker, void* context)
{
    walker(&data_node->branch_node.node, pre_order, context);
    walker(&data_node->branch_node.node, in_order, context);
    walker(&data_node->branch_node.node, post_order, context);

    return;
}

void walk_list_node(list_node_t* list_node, walker_fn walker, void* context)
{
    walker(&list_node->indirected_node.node, pre_order, context);

    unsigned int i;
    for(i = 0; i < list_node->count; i++)
    {
        walk_data_node(list_node->data_nodes[i], walker, context);
        walker(&list_node->indirected_node.node, in_order, context);
    }

    walker(&list_node->indirected_node.node, post_order, context);

    return;
}

void walk_container_node(container_node_t* container_node, walker_fn walker, void* context)
{
    walker(&container_node->indirected_node.node, pre_order, context);

    unsigned int count = get_container_count(container_node);

    unsigned int i;
    for(i = 0; i < count; i++)
    {
        walk_node(&container_node->branches[i]->node, walker, context);
        walker(&container_node->indirected_node.node, in_order, context);
    }

    walker(&container_node->indirected_node.node, post_order, context);

    return;
}

void walk_indirection_node(indirection_node_t* indirection_node, walker_fn walker, void* context)
{
    walker(&indirection_node->branch_node.node, pre_order, context);

    walk_node(&indirection_node->ptr->node, walker, context);

    walker(&indirection_node->branch_node.node, in_order, context);

    walker(&indirection_node->branch_node.node, post_order, context);

    return;
}

void walk_tomb_node(tomb_node_t* tomb_node, walker_fn walker, void* context)
{
    walker(&tomb_node->indirected_node.node, pre_order, context);

    walk_data_node(tomb_node->data_node, walker, context);

    walker(&tomb_node->indirected_node.node, in_order, context);

    walker(&tomb_node->indirected_node.node, post_order, context);

    return;
}

void walk_node(ctrie_node_t* node, walker_fn walker, void* context)
{
    switch (node->type)
    {
        case CNODE:
            walk_container_node((container_node_t *) node, walker, context);
            break;

        case LNODE:
            walk_list_node((list_node_t *) node, walker, context);
            break;

        case INODE:
            walk_indirection_node((indirection_node_t *) node, walker, context);
            break;

        case DNODE:
            walk_data_node((data_node_t *) node, walker, context);
            break;

        case TNODE:
            walk_tomb_node((tomb_node_t *) node, walker, context);
            break;

        default:
            assert(false);
            break;
    }

    return;
}

void save_walker(ctrie_node_t* node, walk_order_t order, void* context)
{
    switch (order)
    {
        case pre_order:
            break;

        case in_order:
            if (node->type == DNODE)
            {
                data_node_t* data_node = (data_node_t *) node;
                FILE* fd = context;

                fprintf(fd, "%s : %s : %ld\n", data_node->data.key, data_node->data.value, data_node->data.rev);
            }
            break;

        case post_order:
            free_node(node);
            break;

        default:
            assert(false);
            break;
    }

    return;
}

void save_ctrie(ctrie_t* ctrie)
{
    pthread_mutex_lock(&ctrie->mutex);

    FILE* fd = fopen("./data/ctrie.dat", "w");
    if (fd == NULL)
    {
        perror("fopen:");
        exit(-1);
    }

    indirection_node_t* root = get_root(ctrie);
    walk_indirection_node(root, save_walker, fd);

    fclose(fd);

    pthread_mutex_unlock(&ctrie->mutex);

    destroy_ctrie(ctrie);

    return;
}

void load_ctrie(ctrie_t* ctrie)
{
    FILE* fd = fopen("./data/ctrie.dat", "a+");
    if (fd == NULL)
    {
        perror("fopen");
        exit(-1);
    }

    char key[256], value[256];
    unsigned long rev;

    do
    {
        int count = fscanf(fd, "%s : %s : %ld\n", key, value, &rev);
        if (count == EOF)
            break;

        data_t data = { key, value, rev };
        data_t* existing_data = ctrie_add_data(ctrie, &data);
        assert(existing_data == NULL);
    } while (true);

    fclose(fd);

    return;
}
