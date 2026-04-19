#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dsets.h"

typedef struct _vertex_t vertex_t;
typedef struct _edge_t edge_t;

typedef struct _heap_elements_t
{
    int heap_index;
    int distance;
}heap_elements_t;

typedef struct _edge_t
{
    heap_elements_t heap_element;
    disjoint_set_t set;

    int index;
    int weight;

    vertex_t* src;
    vertex_t* dest;

} edge_t;

typedef struct _vertex_t
{
    heap_elements_t heap_element;
    disjoint_set_t set;

    int index;
    int id;

    int in_degree;
    int in_size;
    edge_t** in_edges;

    int out_degree;
    int out_size;
    edge_t** out_edges;
} vertex_t;

typedef struct _graph_t
{
    int num_vertices;
    int vertices_size;
    vertex_t** vertices;

    int total_weight;

    int num_edges;
    int edges_size;
    edge_t** edges;
} graph_t;

graph_t* graph_init();
void graph_delete(graph_t* graph);

void add_directed_edge(graph_t* graph, int source, int destination, int weight);
void remove_directed_edge(graph_t* graph, edge_t* edge);
void add_directed_edge_ids(graph_t* graph, int source_id, int destination_id, int weight);

vertex_t* add_vertex(graph_t* graph, int index, int id);
vertex_t* add_or_get_vertex(graph_t* graph, int id);
int add_edge_to_vertex_list(edge_t*** edges, edge_t* edge, int count, int* size);

void remove_vertex(graph_t* graph, int id);
int remove_edge_from_vertex_list(edge_t** edges, int i, int count);

void remove_edge(graph_t* graph, vertex_t* src, vertex_t* dest);
void remove_edge_from_graph(graph_t* graph, int src, int dest);

void print_graph(graph_t* graph);
int get_total_weight(graph_t* graph);

int add_weights(edge_t** edges, int size);

edge_t* find_edge(vertex_t* src, vertex_t* dest);

graph_t* parse_adjacency_list_only(char* str);
graph_t* parse_adjacency_list(char* str);

graph_t* parse_undirected_adjacency_only(char* str);
graph_t* parse_undirected_adjacency_list(char* str);

graph_t* parse_adjacency_matrix(char* str);

graph_t* copy_graph(graph_t* graph);
graph_t* copy_graph_sort_edges_min(graph_t* graph);
graph_t* copy_graph_sort_edges_max(graph_t* graph);

edge_t** copy_edges_array(graph_t* graph);
edge_t** sort_copy_edges_min(graph_t* graph);
edge_t** sort_copy_edges_max(graph_t* graph);

void sort_edges_min(graph_t* graph);
void sort_edges_max(graph_t* graph);
