/*
 * mst.c
 * 
 * Copyright 2015 Vyshnav Kakivaya <vyshnav@msn.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*

Input: An undirected graph with integer edge costs.
Format:
	[number_of_nodes] [number_of_edges]
	[one_node_of_edge_1] [other_node_of_edge_1] [edge_1_cost]
	[one_node_of_edge_2] [other_node_of_edge_2] [edge_2_cost]

or example, the third line of the file is "2 3 -8874", indicating that there is an edge connecting vertex #2 and vertex #3 that has cost -8874. You should NOT assume that edge costs are positive, nor should you assume that they are distinct.

Your task is to run Prim's minimum spanning tree algorithm on this graph. You should report the overall cost of a minimum spanning tree --- an integer, which may or may not be negative --- in the box below.

IMPLEMENTATION NOTES: 
This graph is small enough that the straightforward O(mn) time implementation of Prim's algorithm should work fine.

OPTIONAL: Implement a heap-based version.

The simpler approach:
Maintain relevant edges in a heap (with keys = edge costs).

The superior approach:
Store the unprocessed vertices in the heap.

Note:
Requires a heap that supports deletions.
Need to maintain a mapping between vertices and their positions in the heap.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>

#include "graph.h"
#include "heap.h"

typedef struct _mst_t
{
    vertex_t* vertex;
    int distance;
}mst_t;

typedef struct _element_t
{
    int vertex_index;
    int edge_index;
    int distance;
}element_t;


// Print function.
void print_data(void const* ptr)
{
    element_t* element = (element_t*) ptr;
    printf("Element:\n Vertex: %d, Distance: %d\n", element->vertex_index + 1, element->distance);

    return;
}

// Comparison function. Defines the heap as a minimum heap.
int min_data(void const* left_void_ptr, void const* right_void_ptr)
{
    element_t* left_element = (element_t*) left_void_ptr;
    element_t* right_element = (element_t*) right_void_ptr;
    int left_weight = left_element->distance;
    int right_weight = right_element->distance;

    if(left_weight < right_weight)
    {
        return -1;
    }

    if(left_weight > right_weight)
    {
        return 1;
    }

    return 0;
}

bool find(void const* element, void const* key)
{
    element_t* local = (element_t*) element;
    int distance = *(int*) key;

    if(local->distance == distance)
    {
        return true;
    }

    return false;
}

void modify(void* element, void const* replace)
{
    int modification = *(int*) replace;
    if(((element_t*) element)->distance > modification)
    {
        ((element_t*) element)->distance = modification;
    }

    return;
}

/*
Prim's MST Basic Description:
    1. Initialize tree with a single vertex, chosen arbitrarily from the graph.
    2. Find the minimum-weight edge from the current vertex to a vertex not seen and add it to the tree.
    3. Repeat 2 until all vertices are in tree.

Prim's MST Pseudocode:
prim_mst()
{
    min_heap = heap_init();
    for(each vertex v in Graph G)
    {
        key(v) = INT_MAX;
        prev(v) = NULL;
        heap_insert(min_heap, v);
    }

    key(start_vertex) = 0;
    while(min_heap->size != 0)
    {
        vertex = heap_remove_top(min_heap);
        for(each edge in vertex)
        {
            if(dest is in heap)
            {
                edge = vertex to dest;
                if(key(dest) > edge->weight)
                {
                    decrease key(dest, edge, min_heap);
                    prev(dest) = v;
                }
            }
        }
    }
}
*/

/*
OPTIONAL: Implement a heap-based version.

The simpler approach:
Maintain relevant edges in a heap (with keys = edge costs).

The superior approach:
Store the unprocessed vertices in the heap.

Note:
Requires a heap that supports deletions.
Need to maintain a mapping between vertices and their positions in the heap.
*/
/*
Heap:
extract-min results in a vertex that crosses the set of seen set and unseen set.
 */

int dijkstra_min_data(void const* left_void_ptr, void const* right_void_ptr)
{
    vertex_t* left_element = (vertex_t*) left_void_ptr;
    vertex_t* right_element = (vertex_t*) right_void_ptr;
    int left_weight = left_element->heap_element.distance;
    int right_weight = right_element->heap_element.distance;

    if(left_weight < right_weight)
    {
        return -1;
    }

    if(left_weight > right_weight)
    {
        return 1;
    }

    return 0;
}

bool dijkstra_find(void const* element, void const* key)
{
    vertex_t* local = (vertex_t*) element;
    int distance = *(int*) key;

    if(local->heap_element.distance == distance)
    {
        return true;
    }

    return false;
}

void dijkstra_print_data(void const* ptr)
{
    vertex_t* element = (vertex_t*) ptr;
    printf("Vertex: %d, Distance: %d, Heap_location: %d\n", element->id, element->heap_element.distance, element->heap_element.heap_index);

    return;
}


graph_t* prim_mst(graph_t* graph)
{
    // Initialize the Heap.
    int location_offset = offsetof(vertex_t, heap_element.heap_index);
    heap_t* heap = heap_init(location_offset, dijkstra_min_data, dijkstra_find, dijkstra_print_data);
    printf("Heap Initialized.\n");

    graph_t* copy = copy_graph(graph);
    printf("Input graph copied.\n");

    graph_t* mst = graph_init();
    printf("MST graph initialized.\n");

    int num_vertices = copy->num_vertices;
    bool visited[num_vertices];
    int via[num_vertices];

    printf("Inserting into min_heap.\n");

    visited[0] = false;
    via[0] = 0;
    copy->vertices[0]->heap_element.distance = 0;
    heap_insert(heap, copy->vertices[0]);

    for(int i = 1; i < copy->num_vertices; i++)
    {
        visited[i] = false;
        via[i] = INT_MAX;
        copy->vertices[i]->heap_element.distance = INT_MAX; // The Distances are initialized to INT_MIN in graph.c.
        heap_insert(heap, copy->vertices[i]);
    }

    vertex_t* top = NULL;
    edge_t* edge = NULL;
    int index = 0;

    bool first = true;

    printf("Running Algorithm.\n");
    while(heap->size > 0)
    {
        top = heap_remove_top(heap);
        index = top->index;

        if(visited[index] == true)
        {
            continue;
        }

        visited[index] = true;

        for(int i = 0; i < top->out_degree; i++)
        {
            edge = top->out_edges[i];
            vertex_t* dest_vertex = edge->dest;

            if(visited[dest_vertex->index] == true || dest_vertex->heap_element.distance <= edge->weight)
            {
                continue;
            }

            via[dest_vertex->index] = dest_vertex->index;
            dest_vertex->heap_element.distance = edge->weight;
            heap_modify_element(heap, dest_vertex);
        }

        for(int i = 0; i < top->in_degree; i++)
        {
            edge = top->in_edges[i];
            vertex_t* src_vertex = edge->src;

            if(visited[src_vertex->index] == true || src_vertex->heap_element.distance <= edge->weight)
            {
                continue;
            }

            via[src_vertex->index] = src_vertex->index;
            src_vertex->heap_element.distance = edge->weight;
            heap_modify_element(heap, src_vertex);
        }

        if(first == true)
        {
            first = false;
            continue;
        }

        add_directed_edge(mst, via[index], index, top->heap_element.distance);
    }

    printf("Finished.\n");

    graph_delete(copy);

    return mst;
}

int main(int argc, char** argv)
{
    if(argc == 1 || argv[1] == NULL)
    {
        printf("Invalid input\n");
        return -1;
    }
    printf("\n");

    // Create the graph from file.
    graph_t* graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(argv[1], "r");

    int src, dest, weight;

    read = getline(&line, &len, file);
    int num_nodes, num_edges;
    sscanf(line, "%d %d", &num_nodes, &num_edges);

    while ((read = getline(&line, &len, file)) != -1)
    {
        sscanf(line, "%d %d %d", &src, &dest, &weight);
        add_directed_edge(graph, src, dest, weight);
    }

    fclose(file);

    graph_t* mst = NULL;

    mst = prim_mst(graph);

    int total = add_weights(mst);
    printf("\nPrim's MST distance: %d\n", total);

    graph_delete(mst);
    graph_delete(graph);

    return 0;
}

