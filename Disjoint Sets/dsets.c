#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "dsets.h"

disjoint_sets_t* create_disjoint_sets(int offset, void (*print)(void const*))
{
    disjoint_sets_t* sets = malloc(sizeof(disjoint_sets_t));
    sets->offset = offset;
    sets->print = print;
    sets->num_sets = 0;
    sets->allocated_size = 1;
    sets->disjoint_sets = malloc(sizeof(void*));

    return sets;
}

disjoint_set_t* disjoint_set_init()
{
    disjoint_set_t* set = malloc(sizeof(disjoint_set_t));
    set->parent = NULL;
    set->next = NULL;
    set->prev = NULL;
    set->rank = 0;

    return set;
}

void delete_disjoint_sets(disjoint_sets_t* sets)
{
    free(sets->disjoint_sets);
    free(sets);
}

void merge(int offset, void* left_head, void* right_head)
{

    void* left_tail = ((disjoint_set_t*) (left_head + offset))->prev;
    void* right_tail = ((disjoint_set_t*) (right_head + offset))->prev;

    ((disjoint_set_t*) (left_tail + offset))->next = right_head;
    ((disjoint_set_t*) (left_head + offset))->prev = right_tail;
    ((disjoint_set_t*) (right_tail + offset))->next = left_head;
    ((disjoint_set_t*) (right_head + offset))->prev = left_tail;

    return;
}

void remove_ds(disjoint_sets_t* sets, void* root)
{
    int offset = sets->offset;
    int parent_offset = offset + offsetof(disjoint_set_t, parent);

    disjoint_set_t* set = root + offset;
    void* set_pointer = set->parent + parent_offset;
    int set_index = (set_pointer - ((void*) sets->disjoint_sets)) / sizeof(void*);

    int last_index = sets->num_sets - 1;
    printf("Set index: %d Last index: %d\n", set_index, last_index);
    if (set_index < last_index)
    {
        void* last_root = sets->disjoint_sets[last_index];
        disjoint_set_t* last_set = last_root + offset;
        last_set->parent = set->parent;
        sets->disjoint_sets[set_index] = last_root;

        sets->disjoint_sets[last_index] = set_pointer;
    }

    sets->disjoint_sets[last_index] = NULL;
    sets->num_sets--;

    return;
}

void add_set_to_disjoint_sets(disjoint_sets_t* sets, void* node)
{
    int offset = sets->offset;
    int parent_offset = offset + offsetof(disjoint_set_t, parent);
    int set_index = sets->num_sets;

    if (set_index == sets->allocated_size)
    {
        sets->disjoint_sets = realloc(sets->disjoint_sets, sets->allocated_size * 2 * sizeof(void*));
        void* reallocted_sets = sets->disjoint_sets;
        for (int i = 0; i < sets->allocated_size; i++)
        {
            disjoint_set_t* existing_set = sets->disjoint_sets[i] + offset;
            existing_set->parent = reallocted_sets + (i * sizeof(void*)) - parent_offset;
        }

        sets->allocated_size *= 2;
    }

    sets->disjoint_sets[set_index] = node;
    sets->num_sets++;

    disjoint_set_t* set = node + offset;
    void* disjoint_sets = sets->disjoint_sets;
    set->parent = disjoint_sets + (set_index * sizeof(void*)) - parent_offset;
    set->rank = 0;
    set->next = node;
    set->prev = node;

    return;
}

void delete_set_from_disjoint_sets(disjoint_sets_t* sets, void* root)
{
    remove_ds(sets, root);

    int offset = sets->offset;
    void *current_node = root;
    do
    {
        disjoint_set_t* set = current_node + offset;
        void* next_node = set->next;
        free(current_node);
        current_node = next_node;
    } while (current_node != root);

    return;
}

void* find_disjoint_set(disjoint_sets_t* sets, void* node)
{
    if (node == NULL)
    {
        return NULL;
    }

    int offset = sets->offset;
    int parent_offset = offset + offsetof(disjoint_set_t, parent);

    disjoint_set_t* set = node + offset;
    if (*((void**) (set->parent + parent_offset)) == node)
    {
        return node;
    }

    set->parent = find_disjoint_set(sets, set->parent);

    return set->parent;
}

void* union_disjoint_sets(disjoint_sets_t* sets, void* left, void* right)
{
    void* left_root = find_disjoint_set(sets, left);
    void* right_root = find_disjoint_set(sets, right);

    if(left_root == right_root)
        return left_root;

    int offset = sets->offset;
    disjoint_set_t* left_set = left_root + offset;
    disjoint_set_t* right_set = right_root + offset;
    if (left_set->rank < right_set->rank)
    {
        merge(offset, right_root, left_root);
        remove_ds(sets, left_root);

        left_set->parent = right_root;
        return right_root;
    }

    if (left_set->rank == right_set->rank)
    {
        left_set->rank++;
    }

    merge(offset, left_root, right_root);
    remove_ds(sets, right_root);

    right_set->parent = left_root;
    return left_root;
}

int get_num_disjoint_sets(disjoint_sets_t const* sets)
{
    return sets->num_sets;
}

void* get_disjoint_set(disjoint_sets_t const* sets, int set_number)
{
    return sets->disjoint_sets[set_number];
}


void print_disjoint_set(disjoint_sets_t* sets, int set_number)
{
    printf("\n");
    void* root = sets->disjoint_sets[set_number];
    int offset = sets->offset;
    void *current_node = root;
    do
    {
        disjoint_set_t* set = current_node + offset;
        sets->print(current_node);
        current_node = set->next;
    } while (current_node != root);

    return;
}

void enumerate_disjoint_set(disjoint_sets_t* sets, int set_number, void (*callback)(void *))
{
    void* root = sets->disjoint_sets[set_number];
    int offset = sets->offset;
    void *current_node = root;
    do
    {
        disjoint_set_t* set = current_node + offset;
        void *next_node = set->next;
        callback(current_node);
        current_node = next_node;
    } while (current_node != root);

    return;
}


//
//typedef struct _test_ds_t
//{
//    int key;
//    disjoint_set_t set;
//} test_ds_t;
//
//void test_ds_init(test_ds_t* test, int key)
//{
//    test = malloc(sizeof(test_ds_t));
//    test->key = key;
//
//    test->set.next = NULL;
//    test->set.prev = NULL;
//    test->set.parent = NULL;
//    test->set.rank = 0;
//
//    return;
//}
//
//void test_print(void const* ptr)
//{
//    test_ds_t* test = (test_ds_t*)ptr;
//    printf("Key: %d\n", test->key);
//}
//
//test_list_t* test_list_init(int key, int offset)
//{
//    test_list_t* test_list = malloc(sizeof(test_list_t));
//    test_list->test_array = malloc(sizeof(test_ds_t*) * 10);
//    test_list->test_size = 1;
//
//    test_ds_t* value = test_ds_init(key);
//    test_list->test_array[0] = value;
//
//    test_list->sets_array = create_disjoint_sets(offset, test_print);
//    add_set_to_disjoint_sets(test_list->sets_array, value);
//
//    return test_list;
//}
//
//void test_list_insert(test_list_t* list, int key)
//{
//    test_ds_t* value = test_ds_init(key);
//
//    list->test_array[list->test_size] = value;
//    add_set_to_disjoint_sets(list->sets_array, value);
//
//    list->test_size++;
//
//    return;
//}
//int main()
//{
//
//    int offset = offsetof(test_ds_t, set);
//
//    disjoint_sets_t* sets = create_disjoint_sets(offset, test_print);
//
//    test_ds_t test_one;
//    test_ds_init(&test_one, 1);
//    add_set_to_disjoint_sets(sets, &test_one);
//
//    test_ds_t test_two;
//    test_ds_init(&test_two, 2);
//    add_set_to_disjoint_sets(sets, &test_two);
//
//    union_disjoint_sets(sets, &(test_one), &(test_two));
//
//    test_ds_t test_three;
//    test_ds_init(&test_three, 3);
//    add_set_to_disjoint_sets(sets, &test_three);
//
//    test_ds_t test_four;
//    test_ds_init(&test_four, 4);
//    add_set_to_disjoint_sets(sets, &test_four);
//
//    union_disjoint_sets(sets, &(test_three), &(test_four));
//
//    test_ds_t test_five;
//    test_ds_init(&test_five, 5);
//    add_set_to_disjoint_sets(sets, &test_five);
//
//    test_ds_t test_six;
//    test_ds_init(&test_six, 6);
//    add_set_to_disjoint_sets(sets, &test_six);
//
//    union_disjoint_sets(sets, &(test_five), &(test_six));
//
//    for (int i = 0; i < get_num_disjoint_sets(sets); i++)
//    {
//        print_disjoint_set(sets, i);
//        //enumerate_disjoint_set(sets, i, test_print);
//    }
//
//    while(get_num_disjoint_sets(sets) > 1)
//    {
//        union_disjoint_sets(sets, get_disjoint_set(sets, 0), get_disjoint_set(sets, 1));
//    }
//
//    for (int i = 0; i < get_num_disjoint_sets(sets); i++)
//    {
//        print_disjoint_set(sets, i);
//        //enumerate_disjoint_set(sets, i, test_print);
//    }
//
//    return 1;
//}
