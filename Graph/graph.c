#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#include "graph.h"

void print_graph(graph_t* graph)
{
    int i;
    for (i = 0; i < graph->vertices_size; i++)
    {
        vertex_t* vertex = graph->vertices[i];
        if (vertex != NULL)
        {
            printf("Vertex: %d, %d, %d\n", vertex->id, vertex->in_count, vertex->out_count);
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

vertex_t* vertex_malloc(int index, int id, int size)
{
    vertex_t* vertex = malloc(sizeof(vertex_t));
    vertex->index = index;
    vertex->id = id;

    // Allocate memory for in_edges array.
    vertex->in_count = 0;
    vertex->in_size = size;
    vertex->in_edges = calloc(size, sizeof(edge_t*));

    // Allocate memory for out_edges array.
    vertex->out_count = 0;
    vertex->out_size = size;
    vertex->out_edges = calloc(size, sizeof(edge_t*));

    return vertex;
}

edge_t* edge_malloc(int index)
{
    edge_t* edge = malloc(sizeof(edge_t));

    // Populate inner variables
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

void remove_vertex(graph_t* graph, int index)
{
    vertex_t* vertex = graph->vertices[index];
    graph->num_vertices--;
    graph->vertices[index] = graph->vertices[graph->num_vertices];
    graph->vertices[index]->index = index;
    graph->vertices[graph->num_vertices] = NULL;

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
    for(i = 0; i < edge->src->out_count; i++)
    {
        if(edge->src->out_edges[i] == edge)
        {
            edge->src->out_count = remove_edge_from_vertex_list(edge->src->out_edges, i, edge->src->out_count);

            break;
        }
    }

    for(i = 0; i < edge->dest->in_count; i++)
    {
        if(edge->dest->in_edges[i] == edge)
        {
            edge->dest->in_count = remove_edge_from_vertex_list(edge->dest->in_edges, i, edge->dest->in_count);

            break;
        }
    }

    graph->num_edges--;
    graph->edges[edge->index] = graph->edges[graph->num_edges];
    graph->edges[edge->index]->index = edge->index;
    graph->edges[graph->num_edges] = NULL;

    free(edge);

    return;
}

void remove_vertices(graph_t* graph)
{
    int i;
    for(i = 0; i < graph->vertices_size; i++)
    {
        int j;
        for(j = 0; j < graph->vertices[i]->in_size; j++)
        {
            free(graph->vertices[i]->in_edges[j]);
        }

        for(j = 0; j < graph->vertices[i]->out_size; j++)
        {
            free(graph->vertices[i]->out_edges[j]);
        }

        free(graph->vertices[i]);
    }
}

void remove_edges(graph_t* graph)
{
    int i;
    for(i = 0; i < graph->edges_size; i++)
    {
        free(graph->edges[i]);
    }
}

void graph_delete(graph_t* graph)
{
    remove_vertices(graph);

    remove_edges(graph);

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
    src_vertex->out_count = add_edge_to_vertex_list(&src_vertex->out_edges, edge, src_vertex->out_count, &src_vertex->out_size);

    // If vertex is seen for first time, add to graph.
    int dest_index = destination - 1;
    vertex_t* dest_vertex = add_or_get_vertex(graph, dest_index);
    edge->dest = dest_vertex;
    dest_vertex->in_count = add_edge_to_vertex_list(&dest_vertex->in_edges, edge, dest_vertex->in_count, &dest_vertex->in_size);

    return;
}

graph_t* parse_graph()
{
    graph_t* graph = graph_init();

    FILE* file = freopen("test.txt", "r", stdin);
    int src, dest, weight;

    while (scanf("%d %d,%d", &src, &dest, &weight) > 0)
    {
        add_directed_edge(graph, src, dest, weight);
    }

    fclose(file);

    return graph;
}

int main()
{
    graph_t* graph = parse_graph();
    print_graph(graph);
    return 1;
}
