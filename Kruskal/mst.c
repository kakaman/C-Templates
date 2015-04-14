/*
 * mst.c
 *
 *  Created on: Mar 31, 2015
 *      Author: vyshnav
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>

#include "dsets.h"
#include "graph.h"
#include "heap.h"
#include "hashtable.h"

#define INVALID INT_MIN

// Functions passed to the heap as arguments.
int kruskal_min_data(void const* left_void_ptr, void const* right_void_ptr)
{
    edge_t* left_element = (edge_t*) left_void_ptr;
    edge_t* right_element = (edge_t*) right_void_ptr;
    int left_weight = left_element->weight;
    int right_weight = right_element->weight;

    if (left_weight < right_weight)
    {
        return -1;
    }

    if (left_weight > right_weight)
    {
        return 1;
    }

    return 0;
}

bool kruskal_find(void const* element, void const* key)
{
    edge_t* local = (edge_t*) element;
    int id = *(int*) key;

    if (local->index == id)
    {
        return true;
    }

    return false;
}

void kruskal_print_data(void const* ptr)
{
    edge_t* element = (edge_t*) ptr;
    printf("Edge: %d, Src: %d, Dest: %d, Weight: %d\n", element->index, element->src->id, element->dest->id,
           element->weight);

    return;
}

int find_max(int* distances, int size)
{
    int max_distance = 0;
    for (int i = 0; i < size; i++)
    {
        if (max_distance < distances[i])
            max_distance = distances[i];
    }

    return max_distance;
}

/*
 Kruskal's MST Basic Description:
 1. Create a forest F (a set of trees), where each vertex in the graph is a separate tree
 2. Create a set S containing all the edges in the graph
 3. While S is nonempty and F is not yet spanning
 1. Remove an edge with minimum weight from S
 2. If that edge connects Two different trees, then add it to the forest F, combining Two trees into a single tree
 4. When the while loop finishes. The result is an mst.
 */

graph_t* kruskal_heap_mst(graph_t* graph, int num_clusters, int expected_distance)
{
    int location_offset = offsetof(edge_t, heap_element.heap_index);
    heap_t* min_heap = heap_init(location_offset, kruskal_min_data, kruskal_find, kruskal_print_data);
    graph_t* copy = copy_graph(graph);

    for (int i = 0; i < graph->num_edges; i++)
    {
        heap_insert(min_heap, copy->edges[i]);
    }

    graph_t* mst = graph_init();

    while (min_heap->size != 0)
    {
        edge_t* top = heap_remove_top(min_heap);
        vertex_t* source = top->src;
        vertex_t* destination = top->dest;

        if (find_disjoint_set(copy->vertices_sets, source) == find_disjoint_set(copy->vertices_sets, destination))
        {
            continue;
        }
        union_disjoint_sets(copy->vertices_sets, source, destination);
        add_directed_edge(mst, source->id, destination->id, top->weight);
    }

    int total_distance = add_weights(mst);

    printf("Expected Clusters: %d Result Clusters: %d\nExpected Edges: %d Result Edges: %d\nExpected Distance: %d Result Distance: %d\n\n",
           num_clusters, 1, 7, mst->num_edges, expected_distance, total_distance);

    graph_delete(copy);
    clear_heap(min_heap);

    return mst;
}

// Kruskal's Minimum Spanning Tree Function.
graph_t* kruskal_disjoint_set_mst(graph_t* graph, int num_clusters, int expected_distance)
{
    if (graph == NULL)
    {
        printf("Invalid input. kruskal_disjoint_set_mst()\n");
        return NULL;
    }

    graph_t* mst = graph_init();
    graph_t* copy = copy_graph_sort_edges_min(graph);
    edge_t** edges = copy->edges;

    for (int i = 0; i < copy->num_edges; i++)
    {
        vertex_t* source = edges[i]->src;
        vertex_t* destination = edges[i]->dest;

        if (find_disjoint_set(copy->vertices_sets, source) != find_disjoint_set(copy->vertices_sets, destination))
        {
            union_disjoint_sets(copy->vertices_sets, source, destination);
            add_directed_edge(mst, source->id, destination->id, edges[i]->weight);
        }
    }

    int total_distance = add_weights(mst);

    printf("Expected Clusters: %d Result Clusters: %d\nExpected Edges: %d Result Edges: %d\nExpected Distance: %d Result Distance: %d\n\n",
           num_clusters, 1, 7, mst->num_edges, expected_distance, total_distance);

    graph_delete(copy);

    return mst;
}

// Return graph after the expected number of clusters are created.
graph_t* create_clusters(graph_t* graph, int expected_clusters, int expected_spacing)
{
    if (graph == NULL)
    {
        printf("Invalid input. question_one()\n");
        return NULL;
    }

    int current_clusters = graph->num_vertices;
    int* distances = calloc(current_clusters, sizeof(int));

    graph_t* mst = graph_init();
    graph_t* copy = copy_graph_sort_edges_min(graph);
    edge_t** edges = copy->edges;

    int min = INT_MAX;

    for (int i = 0; i < copy->num_edges; i++)
    {
        vertex_t* source = edges[i]->src;
        vertex_t* destination = edges[i]->dest;

        vertex_t* source_root = find_disjoint_set(copy->vertices_sets, source);
        vertex_t* destination_root = find_disjoint_set(copy->vertices_sets, destination);

        if (source_root != destination_root)
        {
            if (current_clusters > expected_clusters)
            {
                vertex_t* root_vertex = union_disjoint_sets(copy->vertices_sets, source, destination);

                if (root_vertex == source_root)
                {
                    distances[root_vertex->index] += distances[destination_root->index] + edges[i]->weight;
                    distances[destination_root->index] = 0;
                }
                else
                {
                    distances[root_vertex->index] += distances[source_root->index] + edges[i]->weight;
                    distances[source_root->index] = 0;
                }

                add_directed_edge(mst, source->id, destination->id, edges[i]->weight);
                vertex_t* mst_src = mst->vertices[source->index];
                vertex_t* mst_dest = mst->vertices[destination->index];
                union_disjoint_sets(mst->vertices_sets, mst_src, mst_dest);

                current_clusters -= 1;

                if (current_clusters == 1)
                    break;
                else
                    continue;
            }

            min = edges[i]->weight;
            break;
        }
    }

    int max_distance = find_max(distances, graph->num_vertices); // Not needed.
    int total_weight = add_weights(mst);
    int original_weight = add_weights(graph); // Not needed.
    int current_spacing = max_distance;

    if (current_clusters != 1)
        current_spacing = min;

    printf("\n");
    printf("MST Graph:\n");
    printf("Largest MST distance: %d MST Weight: %d Original Weight: %d\n\n", max_distance, total_weight,
           original_weight);

    printf("Results:\n");
    printf("Expected clusters: %d Current clusters: %d \n", expected_clusters, current_clusters);
    printf("Expected Spacing: %d Current Spacing: %d \n", expected_spacing, current_spacing);

    free(distances);
    graph_delete(copy);

    return mst;
}

/*
 Problem 1:

 Input:
 [number_of_nodes]
 [edge 1 node 1] [edge 1 node 2] [edge 1 cost]
 [edge 2 node 1] [edge 2 node 2] [edge 2 cost]

 There is One edge (i,j) for each choice of 1≤i<j≤n, where n is the number of nodes.
 For Example, the third line of the file is "1 3 5250", indicating that the distance between nodes 1 and 3.
 The distances are always positive but not distinct.

 Code up the clustering algorithm from lecture for computing a max-spacing k-clustering.
 Run the clustering algorithm from lecture on this data set, where the target number k of clusters is set to 4.
 What is the maximum spacing of a 4-clustering?
 */

// Expected 106
// The MST return graph is technically not needed for the problem.
void question_one(graph_t* graph, int expected_clusters, int expected_spacing)
{
    if (graph == NULL)
    {
        printf("Invalid input. question_one()\n");
        return;
    }

    int current_clusters = graph->num_vertices;

    graph_t* copy = copy_graph_sort_edges_min(graph);
    edge_t** edges = copy->edges;

    int min = INT_MAX;

    for (int i = 0; i < copy->num_edges; i++)
    {
        vertex_t* source = edges[i]->src;
        vertex_t* destination = edges[i]->dest;

        vertex_t* source_root = find_disjoint_set(copy->vertices_sets, source);
        vertex_t* destination_root = find_disjoint_set(copy->vertices_sets, destination);

        if (source_root != destination_root)
        {
            if (current_clusters > expected_clusters)
            {
                union_disjoint_sets(copy->vertices_sets, source, destination);
                current_clusters -= 1;

                if (current_clusters == 1)
                    break;
                else
                    continue;
            }

            min = edges[i]->weight;
            break;
        }
    }

    int current_spacing = 0;

    if (current_clusters != 1)
        current_spacing = min;

    printf("\n");
    printf("Results:\n");
    printf("Expected clusters: %d Current clusters: %d \n", expected_clusters, current_clusters);
    printf("Expected Spacing: %d Current Spacing: %d \n", expected_spacing, current_spacing);

    graph_delete(copy);

    return;
}

// If right is within 2 bits.

int count_set_bits(int id)
{
    unsigned int u_id = (unsigned int) id;
    int count;

    for(count = 0; u_id; count++)
    {
        u_id = u_id & (u_id - 1);
    }

    return count;
}

bool within_bounds(int id, int bound)
{
    return (count_set_bits(id) < bound) ? true : false;
}

#define FMT_BUF_SIZE (CHAR_BIT*sizeof(uintmax_t)+1)
char* binary_fmt(uintmax_t x, char buf[static FMT_BUF_SIZE], int num_bits)
{
    char *s = buf + FMT_BUF_SIZE;
    *--s = 0;
    if (!x)
    {
        *--s = '0';
    }

    for(int i = 0; (x > 0) || (i < num_bits); x/=2, i++)
    {
        *--s = '0' + x%2;
    }

    return s;
}

unsigned int* create_binary_permutations(int id, int num_bits)
{
    static char tmp[FMT_BUF_SIZE];

    unsigned int* permutations = malloc(sizeof(unsigned int) * 300);
    int index = 0;

//    printf("%s %d\n", binary_fmt(id, tmp, num_bits), num_bits);
    for(int i = 0; i < num_bits; i++)
    {
        unsigned int u_id = (unsigned int) id;
        permutations[index] = (u_id ^ (0x1 << i));
//        printf("%s\n", binary_fmt(permutations[index], tmp, num_bits));
        index++;
    }

    // printf("Index: %d\n", index);
    for(int i = 0; i < num_bits; i++)
    {
//        printf("\n");
        unsigned int u_id = (unsigned int) id;
        u_id = (u_id ^ (0x1 << i));
        for(int j = i + 1; j < num_bits; j++)
        {
            permutations[index] = (u_id ^ (0x1 << j));
//            printf("%s\n", binary_fmt(permutations[index], tmp, num_bits));
            index++;
        }
    }
    // printf("Index: %d\n", index);
    return permutations;
}


void store_bits(char* input, int* id, int num_bits)
{
    int size = strlen(input);
    int count = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        if (input[i] == '0')
        {
            count++;
        }
        else if (input[i] == '1')
        {
            *id |= 1 << count;
            count++;
        }
    }

    return;
}

int str_to_int(char* str, int string_length)
{
    int id = 0;
    for (int i = 0; i < string_length; i++)
    {
        if (str[i] == '1')
        {
            id *= 2;
            id += 1;
        }
        else if (str[i] == '0')
        {
            id *= 2;
            id += 0;
        }
    }

    return id;
}

char* int_to_str(int convert)
{
    char id_str[7];
    sprintf(id_str, "%d", convert);

    return &id_str;
}

int diff_bits(int left, int right, int num_bits)
{
    int difference = 0;

    for(int i = 0; i < num_bits; i++)
    {
        difference += ((left << i) != (right << i)) ? 1 : 0;
    }

    return difference;
}

/*
 Greedy Algorithm:
 Initially, each point in a separate cluster (set)
 repeat until only k clusters:
 Let p, q = closest pair of separated points
 merge the clusters with p, q into a single cluster.
 */

/*
 Problem 2:
 Number of Nodes: 200,000
 Number of bits in label: 24
 Expected: 16508

 What is the largest value of k where there is a k clustering with spacing at least 3?
 i.e.
 How many clusters are needed to ensure no pair of nodes with all but two bits in common
 get split into different clusters.
 */

// Implementation if the graph has been created with distances.
void clusters_after_weight(graph_t* graph, int bits, int expected_clusters, int edge_cost, int expected_num_edges)
{
    if (graph == NULL)
    {
        printf("Invalid input. question_one()\n");
        return;
    }

    int current_clusters = graph->num_vertices;

    graph_t* copy = copy_graph_sort_edges_min(graph);
    edge_t** edges = copy->edges;
    int edge_count = 0;

    for (int i = 0; i < copy->num_edges; i++)
    {
        vertex_t* source = edges[i]->src;
        vertex_t* destination = edges[i]->dest;

        vertex_t* source_root = find_disjoint_set(copy->vertices_sets, source);
        vertex_t* destination_root = find_disjoint_set(copy->vertices_sets, destination);

        if (edge_cost > edges[i]->weight)
            break;

        if (source_root != destination_root)
        {
            // Union the source and destination.
            union_disjoint_sets(copy->vertices_sets, source, destination);

            if (edges[i]->weight == edge_cost)
                edge_count++;

            current_clusters -= 1;
        }
    }

    printf("\n");
    printf("Results:\n");
    printf("Expected clusters: %d Current clusters: %d \n", expected_clusters, current_clusters);
    printf("Searched for edges of weight: %d \n", edge_cost);
    printf("Expected edge count: %d Current edge count: %d \n", expected_num_edges, edge_count);

    graph_delete(copy);

    return;
}

int find_dest_id(hashtable_t* hashtable, unsigned int** permutations_array, int i)
{
    int count = 0;
    int dest_id = permutations_array[i][count];
    int dest_index = hashtable_lookup(hashtable, dest_id);

    while(dest_index == INVALID && count < 300)
    {
        dest_id = permutations_array[i][count];
        dest_index = hashtable_lookup(hashtable, dest_id);
        count++;
    }

    return dest_index;
}

//int possible_unique_vertices = exp2(24);
//printf("%d unique vertices possible with %d bits.\n", possible_unique_vertices, 24);


/*
 Implementation 1:
 Store every vertex into a hashtable as a key value pair. (Key is the index, Value is the id).
 Take a vertex from the vertex array and find a vertex whose distance is within the bounds.
 */
void question_two(graph_t* graph, hashtable_t* hashtable, unsigned int** permutations_array, int expected_clusters, int weight_bound,
                  int expected_num_edges)
{

    if (graph == NULL)
    {
        printf("Invalid input. question_one()\n");
        return;
    }

    int current_clusters = graph->num_vertices;
    int edge_count = 0;
    int current_weight = 0;

    printf("Current clusters: %d, Edge count: %d, Current weight: %d\n", current_clusters, edge_count, current_weight);
    graph_t* copy = copy_graph(graph);
    vertex_t** vertices = copy->vertices;

    while (current_weight <= weight_bound)
    {
        for (int i = 0; i < copy->num_vertices; i++)
        {
            //printf("Current clusters: %d, Edge count: %d, Current weight: %d\n", current_clusters, edge_count, current_weight);
            //printf("i: %d\n", i);
            int count = 0;

            vertex_t* source = vertices[i];

            int dest_id = permutations_array[i][count];
            int dest_index = hashtable_lookup(hashtable, dest_id);

            while(dest_index == INVALID && count < 300)
            {
                dest_id = permutations_array[i][count];
                dest_index = hashtable_lookup(hashtable, dest_id);
                count++;
            }

            printf("dest_id: %d dest_index\n", dest_id, dest_index);
            if(dest_index == INVALID)
                continue;

            vertex_t* destination = vertices[dest_index];

            vertex_t* source_root = find_disjoint_set(copy->vertices_sets, source);
            vertex_t* destination_root = find_disjoint_set(copy->vertices_sets, destination);

            if (source_root != destination_root)
            {
                union_disjoint_sets(copy->vertices_sets, source, destination);
                current_clusters--;
                edge_count++;
            }

        }
        current_weight++;
    }

    printf("\n");
    printf("Results:\n");
    printf("Expected clusters: %d Current clusters: %d \n", expected_clusters, current_clusters);
    printf("Searched for edges of weight less than: %d \n", weight_bound + 1);
    printf("Expected edge count: %d Current edge count: %d \n", expected_num_edges, edge_count);

    graph_delete(copy);

    return;
}

void print_bits_little_endian(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--)
    {
        for (j = 7; j >= 0; j--)
        {
            byte = b[i] & (1 << j);
            byte >>= j;

            printf("%u", byte);
        }
    }

    printf("\n");

    return;
}

void print_bits_big_endian(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = 0; i < size; i++)
    {
        for (j = 0; j < 8; j++)
        {
            byte = b[i] & (1 << j);
            byte >>= j;

            printf("%u", byte);
        }
    }

    printf("\n");

    return;
}

graph_t* parse_clustering_one(graph_t* graph, char* input_file, int num_lines, int num_nodes)
{
    graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(input_file, "r");

    int src, dest, weight;

    read = getline(&line, &len, file);
    sscanf(line, "%d", &num_nodes);
    num_lines++;

    while ((read = getline(&line, &len, file)) != -1)
    {
        sscanf(line, "%d %d %d", &src, &dest, &weight);
        add_directed_edge(graph, src, dest, weight);
        num_lines++;
    }

    free(line);
    fclose(file);

    return graph;
}

graph_t* parse_clustering_big(graph_t* graph, hashtable_t** hashtable, char* input_file, int* num_bits, unsigned int*** permutations)
{
    graph = graph_init();
    *hashtable = hashtable_create();

    int num_lines = 0;
    int num_nodes = 0;
    int data_size = sizeof(unsigned int);

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("\nCalled parse_clustering_big(). Beginning parsing.\n\n");

    FILE* file = fopen(input_file, "r");

    read = getline(&line, &len, file);
    sscanf(line, "Nodes: %d Bits: %d", &num_nodes, &num_bits[0]);
    permutations[0] = malloc(sizeof(unsigned int*) * (num_nodes));
    num_lines++;

    int count = 0;
    while ((read = getline(&line, &len, file)) != -1)
    {
        int id = 0;
        store_bits(line, &id, num_bits[0]);

        char key[10];
        sprintf(key, "%d", id);

        hashtable_data_t* data = hashtable_data_create(key, count, 0, data_size);

        char* existing = hashtable_insert(*hashtable, data);
        if (strcmp(key, existing) == 0)
        {
            add_vertex(graph, count, id);
        }
        permutations[0][count] = create_binary_permutations(id, num_bits[0]);
        count++;
    }

    free(line);
    fclose(file);
    printf("\nParsing finished.\n\n");
    return graph;
}

graph_t* parse_tests(graph_t* graph, char* input_file, int num_lines, int num_nodes, int* num_clusters,
                     int* expected_distance)
{
    graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(input_file, "r");

    int src, dest, weight, input_num_clusters, expected_cluster_distance;

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (num_lines > 4)
        {
            sscanf(line, "%d %d %d", &src, &dest, &weight);
            add_directed_edge(graph, src, dest, weight);
            num_lines++;
        }
        else if (num_lines == 4)
        {
            sscanf(line, "%d", &num_nodes);
            num_lines++;
        }
        else if (num_lines < 4)
        {
            sscanf(line, "%d %d", &input_num_clusters, &expected_cluster_distance);
            num_clusters[num_lines - 1] = input_num_clusters;
            expected_distance[num_lines - 1] = expected_cluster_distance;
            num_lines++;
        }
    }

    free(line);
    fclose(file);

    return graph;
}

//graph_t* parse_q2_tests(graph_t* graph, hashtable_t* hashtable, char* input_file, int* num_nodes, int* num_bits,
//                        unsigned int*** permutations)
//{
//    graph = graph_init();
//    hashtable = hashtable_create();
//
//    int num_lines = 1;
//    char * line = NULL;
//    size_t len = 0;
//    ssize_t read;
//
//    FILE* file = fopen(input_file, "r");
//
//    int edge_weight, num_nodes_expected;
//
//    read = getline(&line, &len, file);
//    int count = 0;
//    while ((read = getline(&line, &len, file)) != -1)
//    {
//        if (num_lines > 5)
//        {
//            int id = 0;
//            store_bits(line, &id, num_bits);
//
//            int_data_t data = { id, count, 0 };
//            int existing = hashtable_insert(hashtable, &data);
//            if (existing == id) // Do not add duplicates
//            {
//                add_vertex(graph, count, id);
//            }
//            permutations[0][count] = create_binary_permutations(id, num_bits);
//            count++;
//        }
//        else if (num_lines < 5)
//        {
//            sscanf(line, "%d %d", &edge_weight, &num_nodes_expected);
//            edge_cost[num_lines - 2] = edge_weight;
//            expected_nodes[num_lines - 2] = num_nodes_expected;
//        }
//        else
//        {
//            sscanf(line, "%d %d", &num_nodes, &num_bits);
//            permutations = malloc(sizeof(unsigned int*) * (num_nodes - 1));
//        }
//
//        num_lines++;
//    }
//
//    free(line);
//    fclose(file);
//
//    return graph;
//}

void print_permutations(unsigned int* permutations, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", permutations[i]);
    }

    return;
}

int main(int argc, char** argv)
{
    char program[7];
    char input_file[100];

//    int a = 0;
//    print_bits_little_endian(sizeof(int), &a);

    if (argc <= 2)
    {
        printf("Can run three programs:\n Example\n One\n Two\n");
        printf("\nEnter program to run: ");
        scanf("%s", program);

        if (strcmp(program, "quit") == 0 || strcmp(program, "quit") == 0)
        {
            printf("Exiting.\n");
            return 1;
        }

        while (strcmp(program, "Example") != 0 && strcmp(program, "One") != 0 && strcmp(program, "Two") != 0)
        {
            printf("\nInvalid input\n");
            printf("Can run three programs:\n Example\n One\n Two\n");
            printf("\nEnter program to run: ");
            scanf("%s", program);

            if (strcmp(program, "quit") == 0 || strcmp(program, "quit") == 0)
            {
                printf("Exiting.\n");
                return 1;
            }
        }

        printf("Can parse three input files: \n clustering_big.txt\n clustering1.txt\n test.txt\n test_one.txt\n test_two.txt\n test_one_q2.txt\n test_10000.txt\n");
        printf("\nEnter File to parse: ");
        scanf("%s", input_file);

        if (strcmp(input_file, "quit") == 0 || strcmp(input_file, "quit") == 0)
        {
            printf("Exiting.\n");
            return 1;
        }

        while (strcmp(input_file, "clustering_big.txt") != 0 && strcmp(input_file, "clustering1.txt") != 0
               && strcmp(input_file, "test.txt") != 0 && strcmp(input_file, "test_one.txt") != 0
               && strcmp(input_file, "test_two.txt") != 0 && strcmp(input_file, "test_one_q2.txt") != 0
               && strcmp(input_file, "test_10000.txt") != 0)
        {
            printf("\nInvalid input\n");
            printf("Can parse three input files: \n clustering_big.txt\n clustering1.txt\n test.txt\n test_one.txt\n test_two.txt\n test_one_q2.txt\n test_10000.txt\n");
            printf("\nEnter File to parse: ");
            scanf("%s", input_file);

            if (strcmp(input_file, "quit") == 0 || strcmp(input_file, "quit") == 0)
            {
                printf("Exiting.\n");
                return 1;
            }
        }

    }
    else
    {
        strcpy(input_file, argv[1]);
        strcpy(program, argv[2]);
    }

    graph_t* graph = NULL;
    hashtable_t* hashtable = NULL;

    int num_lines = 1;
    int num_nodes = 0;
    int num_bits = 0;

    int* num_clusters = NULL;
    int* expected_distance = NULL;
    int* edge_cost = NULL;
    int* expected_nodes = NULL;
    unsigned int** permutations = NULL;

    bool question_input = true;
    bool mine = false;

    printf("\nParsing File: %s\n", input_file);

    if (strcmp(input_file, "clustering1.txt") == 0)
    {
        graph = parse_clustering_one(graph, input_file, num_lines, num_nodes);
    }
    else if (strcmp(input_file, "test_10000.txt") == 0 || strcmp(input_file, "test_1.txt") == 0)
    {
        graph = parse_clustering_big(graph, &hashtable, input_file, &num_bits, &permutations);
        question_input = false;
    }
    else if (strcmp(input_file, "clustering_big.txt") == 0)
    {
        graph = parse_clustering_big(graph, &hashtable, input_file, &num_bits, &permutations);
    }
    else
    {
        num_clusters = malloc(sizeof(int) * 3);
        expected_distance = malloc(sizeof(int) * 3);
        graph = parse_tests(graph, input_file, num_lines, num_nodes, num_clusters, expected_distance);
        question_input = false;
    }

    printf("Running Program: %s\n", program);

    if (strcmp(program, "Example") == 0)
    {
        printf("Kruskal Heap:\n");
        graph_t* kruskal_mst = kruskal_heap_mst(graph, num_clusters[0], expected_distance[0]);
        print_graph(kruskal_mst);

        printf("\nKruskal Quicksort:\n");
        graph_t* kruskal_qs_mst = kruskal_disjoint_set_mst(graph, num_clusters[0], expected_distance[0]);
        print_graph(kruskal_qs_mst);
        printf("\n");

        graph_delete(kruskal_mst);
        graph_delete(kruskal_qs_mst);
    }
    else if (strcmp(program, "One") == 0)
    {
        graph_t* mst = NULL;
        if (question_input == true)
        {
            question_one(graph, 4, 106);
            if(mine == true)
                mst = create_clusters(graph, 4, 106);
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {
                question_one(graph, num_clusters[i], expected_distance[i]);

                if (mine == true)
                {
                    mst = create_clusters(graph, num_clusters[i], expected_distance[i]);
                    graph_delete(mst);
                }
            }
        }
    }
    else if (strcmp(program, "Two") == 0)
    {
//        int** check = malloc(sizeof(int*) * 10);
//        for(int i = 0; i < 10; i++)
//        {
//            check[i] = malloc(sizeof(int) * 10);
//            for(int j = 0; j < 10; j++)
//            {
//                check[i][j] = j;
//            }
//        }
//
//        for(int i = 0; i < 10; i++)
//        {
//            for(int j = 0; j < 10; j++)
//            {
//                printf("%d\n", check[i][j]);
//            }
//        }

//        printf("Before print_permutations\n");
//        printf("Address of permutations[0]: %p\n", &permutations[0]);
//        //printf("Address of permutations[0][0]: %p\n", &&permutations[0][0]);
//        print_permutations(permutations[0], 300);
        if (question_input == true)
        {
            question_two(graph, hashtable, permutations, 6118, 2, 897);
        }
        else
        {
            /*
                Clusters: 9116
                Edges: 0 5
                Edges: 1 67
                Edges: 2 825
            */
            question_two(graph, hashtable, permutations, 9116, 2, 825);
        }

        for (int i = 0; i < graph->num_vertices; i++)
        {
            free(permutations[i]);
        }
        free(permutations);

    }
    else
    {
        printf("Program DNE: %s\n", program);
    }

    graph_delete(graph);
    hashtable_destroy(hashtable);
    free(num_clusters);
    free(expected_distance);
    free(edge_cost);
    free(expected_nodes);


    return 1;
}
