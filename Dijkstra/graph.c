#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "graph.h"
#include "quicksort.h"

void print_graph(graph_t* graph)
{
    int i;
    for (i = 0; i < graph->vertices_size; i++)
    {
        vertex_t* vertex = graph->vertices[i];
        if (vertex != NULL)
        {
            printf("Vertex: %d, %d, %d\n", vertex->id, vertex->in_degree, vertex->out_degree);
        }
    }

    for (i = 0; i < graph->num_edges; i++)
    {
        edge_t* edge = graph->edges[i];
        printf("Edge: %d, %d, %d, %d \n", edge->index, edge->src->id, edge->dest->id, edge->weight);
    }


    printf("Num_vertices: %d\n", graph->num_vertices);
    printf("Num_edges: %d\n", graph->num_edges);

    return;
}

void* realloc_clear(void* ptr, size_t element_size, int index, int* count_ptr)
{
    int current_count = *count_ptr;
    int new_count = ((index / current_count) + 1) * current_count;
    char* realloced_ptr = realloc(ptr, element_size * new_count);
    memset(realloced_ptr + (element_size * current_count), 0, element_size * (new_count - current_count));

    *count_ptr = new_count;

    return realloced_ptr;
}

graph_t* graph_init()
{
    // Create a new graph
    graph_t* graph = malloc(sizeof(graph_t));

    //Initialize the variables
    graph->num_vertices = 0;
    graph->num_edges = 0;

    graph->edges_size = 1;
    graph->edges = malloc(sizeof(edge_t*) * graph->edges_size);
    memset(graph->edges, 0, sizeof(edge_t*) * graph->edges_size);

    graph->vertices_size = 1;
    graph->vertices = malloc(sizeof(vertex_t*) * graph->vertices_size);
    memset(graph->vertices, 0, sizeof(vertex_t*) * graph->vertices_size);

    return graph;
}

void delete_vertex(vertex_t* vertex)
{
    if(vertex == NULL)
        return;

    free(vertex->in_edges);
    free(vertex->out_edges);
    free(vertex);

    return;
}

void delete_edge(edge_t* edge)
{
    if(edge == NULL)
        return;

    free(edge);

    return;
}


void remove_vertices(graph_t* graph)
{
    for(int i = 0; i < graph->vertices_size; i++)
    {
        delete_vertex(graph->vertices[i]);
    }

    return;
}

void remove_edges(graph_t* graph)
{
    for(int i = 0; i < graph->edges_size; i++)
    {
        delete_edge(graph->edges[i]);
    }

    return;
}

void graph_delete(graph_t* graph)
{
    remove_edges(graph);
    remove_vertices(graph);

    free(graph->edges);
    free(graph->vertices);
    free(graph);

    return;
}

vertex_t* vertex_malloc(int index, int id, int size)
{
    vertex_t* vertex = malloc(sizeof(vertex_t));

    // Set heap_element
    vertex->heap_element.distance = -1;
    vertex->heap_element.heap_index = -1;

    // Set set
    vertex->set.parent = &(vertex->set);
    vertex->set.rank = 0;

    vertex->index = index;
    vertex->id = id;

    // Allocate memory for in_edges array.
    vertex->in_degree = 0;
    vertex->in_size = size;
    vertex->in_edges = calloc(size, sizeof(edge_t*));

    // Allocate memory for out_edges array.
    vertex->out_degree = 0;
    vertex->out_size = size;
    vertex->out_edges = calloc(size, sizeof(edge_t*));

    return vertex;
}

edge_t* edge_malloc(int index)
{
    edge_t* edge = malloc(sizeof(edge_t));

    edge->heap_element.distance = -1;
    edge->heap_element.heap_index = -1;

    edge->set.parent = &(edge->set);
    edge->set.rank = 0;

    edge->index = index;
    edge->weight = 0;
    edge->src = NULL;
    edge->dest = NULL;

    return edge;
}

void set_edge(edge_t* edge, vertex_t* source, vertex_t* dest)
{
    // Set the dest variable.
    edge->dest = dest;

    // Set the srouce variable.
    edge->src = source;

    return;
}

// Should have unique ID's
void add_vertex(graph_t* graph, int index, int id)
{
    if(graph->vertices_size <= index)
    {
        graph->vertices = realloc_clear(graph->vertices, sizeof(vertex_t*), index, &graph->vertices_size);
    }

    vertex_t* vertex = vertex_malloc(index, id, 1);
    graph->vertices[index] = vertex;
    graph->num_vertices++;

    return;
}

vertex_t* add_or_get_vertex(graph_t* graph, int index)
{
    if(graph->vertices_size <= index)
    {
        graph->vertices = realloc_clear(graph->vertices, sizeof(vertex_t*), index, &graph->vertices_size);
    }
    else if(graph->vertices[index] != NULL)
    {
        return graph->vertices[index];
    }

    vertex_t* vertex = vertex_malloc(index, index + 1, 1);
    graph->vertices[index] = vertex;
    graph->num_vertices++;

    return vertex;
}

int add_edge_to_vertex_list(edge_t*** edges, edge_t* edge, int count, int* size)
{
    if (count == *size)
    {
        *edges = realloc_clear(*edges, sizeof(edge_t*), count, size);
    }

    (*edges)[count] = edge;

    return count + 1;
}

void shrink_edges(graph_t* graph, edge_t* edge)
{
    graph->num_edges--;
    graph->edges[edge->index] = graph->edges[graph->num_edges];
    graph->edges[edge->index]->index = edge->index;
    graph->edges[graph->num_edges] = NULL;

    delete_edge(edge);

    return;
}

// ToDo: Implement properly. Need to remove any edges that have the vertex as a src or dest.
void remove_vertex(graph_t* graph, int index)
{
    vertex_t* vertex = graph->vertices[index];
    graph->num_vertices--;
    graph->vertices[index] = graph->vertices[graph->num_vertices];
    graph->vertices[index]->index = index;
    graph->vertices[graph->num_vertices] = NULL;

    for(int i = 0; i < vertex->in_size; i++)
    {
        if(vertex->in_edges[i] != NULL)
        {
            edge_t* edge = vertex->in_edges[i];
            shrink_edges(graph, edge);
        }
    }

    for(int i = 0; i < vertex->out_size; i++)
    {
        if(vertex->out_edges[i] != NULL)
        {
            edge_t* edge = vertex->out_edges[i];
            shrink_edges(graph, edge);
        }
    }

    free(vertex->in_edges);
    free(vertex->out_edges);
    free(vertex);

    return;
}

int remove_edge_from_vertex_list(edge_t** edges, int i, int count)
{
    count--;
    edges[i] = edges[count];
    edges[count] = NULL;

    return count;
}

void remove_directed_edge(graph_t* graph, edge_t* edge)
{
    int i;
    for(i = 0; i < edge->src->out_degree; i++)
    {
        if(edge->src->out_edges[i] == edge)
        {
            edge->src->out_degree = remove_edge_from_vertex_list(edge->src->out_edges, i, edge->src->out_degree);

            break;
        }
    }

    for(i = 0; i < edge->dest->in_degree; i++)
    {
        if(edge->dest->in_edges[i] == edge)
        {
            edge->dest->in_degree = remove_edge_from_vertex_list(edge->dest->in_edges, i, edge->dest->in_degree);

            break;
        }
    }

    shrink_edges(graph, edge);

    return;
}

void add_directed_edge(graph_t* graph, int source, int destination, int weight)
{
    edge_t* edge = edge_malloc(graph->num_edges);
    if(graph->num_edges == graph->edges_size)
    {
        graph->edges = realloc_clear(graph->edges, sizeof(edge_t*), graph->num_edges, &graph->edges_size);
    }

    edge->weight = weight;
    graph->edges[graph->num_edges] = edge;
    graph->num_edges++;

    // If vertex is seen for first time, add to graph.
    int src_index = source - 1;
    vertex_t* src_vertex = add_or_get_vertex(graph, src_index);
    edge->src = src_vertex;
    src_vertex->out_degree = add_edge_to_vertex_list(&src_vertex->out_edges, edge, src_vertex->out_degree, &src_vertex->out_size);

    // If vertex is seen for first time, add to graph.
    int dest_index = destination - 1;
    vertex_t* dest_vertex = add_or_get_vertex(graph, dest_index);
    edge->dest = dest_vertex;
    dest_vertex->in_degree = add_edge_to_vertex_list(&dest_vertex->in_edges, edge, dest_vertex->in_degree, &dest_vertex->in_size);

    //printf("Adding edge: Src: %d Dest: %d, Weight: %d.\n", source, destination, weight);
    return;
}

edge_t* find_minimum_edge(graph_t* graph)
{
    if(graph == NULL)
        return NULL;

    sort_edges_min(graph);

    return graph->edges[0];
}

edge_t* find_edge(vertex_t* src, vertex_t* dest)
{
    edge_t* edge = NULL;
    for(int i = 0; i < src->out_degree; i++)
    {
        if(src->out_edges[i]->dest->id == dest->id)
        {
            edge = src->out_edges[i];
            break;
        }
    }

    return edge;
}

void remove_edge(graph_t* graph, vertex_t* src, vertex_t* dest)
{
    edge_t* edge = NULL;
    for(int i = 0; i < src->out_degree; i++)
    {
        if(src->out_edges[i]->dest->id == dest->id)
        {
            edge = src->out_edges[i];
            remove_directed_edge(graph, edge);
        }
    }

    return;
}

// Src and dest are the indexes of the respective vertices.
void remove_edge_from_graph(graph_t* graph, int src, int dest)
{
    vertex_t* source = graph->vertices[src];
    vertex_t* destination = graph->vertices[dest];

    remove_edge(graph, source, destination);

    return;
}

int add_weights(graph_t* graph)
{
    int total_weight = 0;
    for(int i = 0; i < graph->num_edges; i++)
    {
        total_weight += graph->edges[i]->weight;
    }

    return total_weight;
}


// Use this when number of edges and number of vertices are not given as first line.
graph_t* parse_adjacency_list_only(char* str)
{
    graph_t* graph = graph_init();

    FILE* file = fopen(str, "r");
    int src, dest, weight;

    while (scanf("%d %d,%d", &src, &dest, &weight) > 0)
    {
        add_directed_edge(graph, src, dest, weight);
    }

    fclose(file);

    return graph;
}

graph_t* parse_undirected_adjacency_only(char* str)
{
    graph_t* graph = graph_init();

    FILE* file = fopen(str, "r");
    int src, dest, weight;

    while (scanf("%d %d,%d", &src, &dest, &weight) > 0)
    {
        add_directed_edge(graph, src, dest, weight);
        add_directed_edge(graph, dest, src, weight);
    }

    fclose(file);

    return graph;
}

// Use this when number of edges and number of vertices are given as first line.
graph_t* parse_adjacency_list(char* str)
{
    graph_t* graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(str, "r");

    int num_vertices = 0;
    int num_edges = 0;

    read = getline(&line, &len, file);
    sscanf(line, "%d %d", &num_vertices, &num_edges);

    while ((read = getline(&line, &len, file)) != -1)
    {
        int src, dest, weight;

        sscanf(line, "%d %d,%d", &src, &dest, &weight);

        add_directed_edge(graph, src, dest, weight);
    }

    fclose(file);

    if(graph->num_edges != num_edges || graph->num_vertices != num_vertices)
    {
        printf("Graph does not have correct edges/vertices. Expected (vertices, edges): %d %d. Actual (vertices, edges): %d %d\n", num_vertices, num_edges, graph->num_vertices, graph->num_edges);
        return NULL;
    }

    return graph;
}

graph_t* parse_undirected_adjacency_list(char* str)
{
    graph_t* graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(str, "r");

    int num_vertices = 0;
    int num_edges = 0;

    read = getline(&line, &len, file);
    sscanf(line, "%d %d", &num_vertices, &num_edges);

    while ((read = getline(&line, &len, file)) != -1)
    {
        int src, dest, weight;

        sscanf(line, "%d %d,%d", &src, &dest, &weight);

        add_directed_edge(graph, src, dest, weight);
        add_directed_edge(graph, dest, src, weight);
    }

    fclose(file);

    if(graph->num_vertices != num_vertices) // graph->num_edges != num_edges ||
    {
        printf("Graph does not have correct edges/vertices. Expected (vertices, edges): %d %d. Actual (vertices, edges): %d %d\n", num_vertices, num_edges, graph->num_vertices, graph->num_edges);
        return NULL;
    }

    return graph;
}

// ToDo: Construct a graph from a file with an adjacency matrix.
graph_t* parse_adjacency_matrix(char* str)
{
    free(str);
    str = NULL;
    return NULL;
}

edge_t* copy_edge(edge_t* edge)
{
    edge_t* copy = malloc(sizeof(edge_t));
    memcpy(copy, edge, sizeof(edge_t));

    return copy;
}

// For use in quicksort.
void print_edges(void* ptr)
{
    edge_t* edge_ptr = (edge_t*) ptr;
    printf("Location: %d Weight: %d Src: %d Dest: %d\n", edge_ptr->index, edge_ptr->weight, edge_ptr->src->id, edge_ptr->dest->id);

    return;
}

int min_compare(void* left_ptr, void* right_ptr)
{
    edge_t* left_edge = (edge_t*) left_ptr;
    edge_t* right_edge = (edge_t*) right_ptr;

    if(left_edge->weight < right_edge->weight)
    {
        return -1;
    }

    if(left_edge->weight > right_edge->weight)
    {
        return 1;
    }

    return 0;
}

int max_compare(void* left_ptr, void* right_ptr)
{
    return -min_compare(left_ptr, right_ptr);
}

graph_t* copy_graph(graph_t* graph)
{
    graph_t* copy = graph_init();
    for(int i = 0; i < graph->num_edges; i++)
    {
        int source = graph->edges[i]->src->id;
        int destination = graph->edges[i]->dest->id;
        int weight = graph->edges[i]->weight;

        add_directed_edge(copy, source, destination, weight);
    }

    return copy;
}

graph_t* copy_graph_sort_edges_min(graph_t* graph)
{
    graph_t* copy = copy_graph(graph);

    sort_edges_min(copy);

    return copy;
}

graph_t* copy_graph_sort_edges_max(graph_t* graph)
{
    graph_t* copy = copy_graph(graph);

    sort_edges_max(copy);

    return copy;
}

// Warning Pointers to vertices are also copied. The vertices point to original graph.
edge_t** copy_edges_array(graph_t* graph)
{
    edge_t** edges = malloc(sizeof(edge_t*) * graph->num_edges);
    for (int i = 0; i < graph->num_edges; i++)
    {
        edges[i] = copy_edge(graph->edges[i]);
    }

    return edges;
}

// Warning Pointers to vertices are also copied. The vertices point to original graph.
edge_t** sort_copy_edges_min(graph_t* graph)
{
    edge_t** edges = copy_edges_array(graph);

    qs_t min_sorted_edges;
    qs_init(&min_sorted_edges, edges, graph->num_edges, min_compare, print_edges);
    quicksort(&min_sorted_edges, 0, graph->num_edges - 1);

    return edges;
}

// Warning Pointers to vertices are also copied. The vertices point to original graph.
edge_t** sort_copy_edges_max(graph_t* graph)
{
    edge_t** edges = copy_edges_array(graph);

    qs_t max_sorted_edges;
    qs_init(&max_sorted_edges, edges, graph->num_edges, max_compare, print_edges);
    quicksort(&max_sorted_edges, 0, graph->num_edges - 1);

    return edges;
}

void sort_edges_min(graph_t* graph)
{
    qs_t min_sorted_edges;
    qs_init(&min_sorted_edges, graph->edges, graph->num_edges, min_compare, print_edges);

    quicksort(&min_sorted_edges, 0, graph->num_edges - 1);

    for (int i = 0; i < graph->num_edges; i++)
    {
        graph->edges[i]->index = i;
    }

    return;
}

void sort_edges_max(graph_t* graph)
{
    qs_t max_sorted_edges;
    qs_init(&max_sorted_edges, graph->edges, graph->num_edges, max_compare, print_edges);

    quicksort(&max_sorted_edges, 0, graph->num_edges - 1);

    for (int i = 0; i < graph->num_edges; i++)
    {
        graph->edges[i]->index = i;
    }

    return;
}
