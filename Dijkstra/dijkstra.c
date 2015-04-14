#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#include "graph.h"
#include "heap.h"

// Naive implementation of Dijkstra's shortest path algorithm. Use Prim's algorithm for a more efficient version.
#define INFINITY 1000000

graph_t* parse_graph()
{
    graph_t* graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1)
    {
        line = strtok(line, " \t\r");

        int src;
        sscanf(line, "%d", &src);

        line = strtok(NULL, " \t\r");
        while (line != NULL)
        {
            int dest, weight;
            sscanf(line, "%d,%d", &dest, &weight);
            add_directed_edge(graph, src, dest, weight);
            line = strtok(NULL, " \t\r");
        }
    }

    free(line);

    return graph;
}

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

void dijkstra_array(graph_t* graph, int source)
{
    int distance[graph->num_vertices];
    bool visited[graph->num_vertices];
    int via[graph->num_vertices];
    int current_vertex = source - 1;

    for(int i = 0; i < graph->num_vertices; i++)
    {
        distance[i] = INFINITY;
        via[i] = -1;
        visited[i] = false;
    }

    distance[current_vertex] = 0;
    via[current_vertex] = 0;
    while(visited[current_vertex] == false)
    {
        visited[current_vertex] = true;
        vertex_t* vertex = graph->vertices[current_vertex];
        for(int i = 0; i < vertex->out_degree; i++)
        {
            edge_t* edge = vertex->out_edges[i];
            int neighbor = edge->dest->index;
            int weight = edge->weight;
            int dist = distance[current_vertex] + weight;
            if(distance[neighbor] > dist)
            {
                distance[neighbor] = dist;
                via[neighbor] = current_vertex + 1;
            }
        }

        current_vertex = 1;
        int shortest_dist = INFINITY;
        for(int i = 0; i < graph->num_vertices; i++)
        {
            if((visited[i] == false) && (shortest_dist > distance[i]))
            {
                shortest_dist = distance[i];
                current_vertex = i;
            }
        }
    }

    for(int i = 0; i < graph->num_vertices; i++)
    {
        printf("Vertex: %d, Distance: %d, Via: %d\n", i + 1, distance[i], via[i]);
    }
}

// Given vertex_id, find the location in graph->vertices
// Simple solution, take the index instead of the ID.

graph_t* dijkstra_min_heap(graph_t* graph, int source_vertex_index)
{

    // Initialize the Heap.
    int location_offset = offsetof(vertex_t, heap_element.heap_index);
    heap_t* heap = heap_init(location_offset, dijkstra_min_data, dijkstra_find, dijkstra_print_data);
    printf("Heap Initialized.\n");

    graph_t* copy = copy_graph(graph);
    printf("Input graph copied.\n");

    graph_t* shortest_path = graph_init();
    printf("shortest_path graph initialized.\n");

    int num_vertices = copy->num_vertices;
    bool visited[num_vertices];
    int via[num_vertices];

    printf("Inserting into min_heap.\n");

    visited[source_vertex_index] = false;
    via[source_vertex_index] = 0;
    copy->vertices[source_vertex_index]->heap_element.distance = 0;
    heap_insert(heap, copy->vertices[source_vertex_index]);

    for(int i = 0; i < source_vertex_index; i++)
    {
        visited[i] = false;
        via[i] = INT_MAX;
        copy->vertices[i]->heap_element.distance = INT_MAX; // The Distances are initialized to INT_MIN in graph.c.
        heap_insert(heap, copy->vertices[i]);
    }

    for(int i = source_vertex_index + 1; i < copy->num_vertices; i++)
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
        add_vertex(shortest_path, shortest_path->num_vertices, top->id); // To save space, what matters is the id not the index for output.

        // Need to keep the distance data as well.
        for(int i = 0; i < top->out_degree; i++)
        {
            edge = top->out_edges[i];
            vertex_t* dest_vertex = edge->dest;

            if(visited[dest_vertex->index] == true || dest_vertex->heap_element.distance <= edge->weight)
            {
                continue;
            }

            via[dest_vertex->index] = dest_vertex->id;
            dest_vertex->heap_element.distance = edge->weight;
            heap_modify_element(heap, dest_vertex);
        }

        if(first == true)
        {
            first = false;
            continue;
        }

        // FIXME: It is currently adding the ID's.
        // ToDo: A fast/feasible way to map the ID to Index without needing too much extra memory or work.
        add_directed_edge_index(shortest_path, via[index], top->id, top->heap_element.distance);
    }

    printf("Finished.\n");

    graph_delete(copy);

    return shortest_path;
}

/*
        if (find_disjoint_set(copy->vertices_sets, source) != find_disjoint_set(copy->vertices_sets, destination))
        {
            union_disjoint_sets(copy->vertices_sets, source, destination);
            add_directed_edge(mst, source->id, destination->id, edges[i]->weight);
        }
 */

int main()
{
    graph_t* graph = parse_graph();
    print_graph(graph);
    dijkstra_array(graph, 1);
}
