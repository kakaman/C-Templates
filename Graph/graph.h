#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct _vertex_t vertex_t;

typedef struct _edge_t
{
    int index;
    int weight;

    vertex_t* src;
    vertex_t* dest;
} edge_t;

typedef struct _vertex_t
{
    int index;
    int id;

    int in_count;
    int in_size;
    edge_t** in_edges;

    int out_count;
    int out_size;
    edge_t** out_edges;
} vertex_t;

typedef struct _graph_t
{
    int num_vertices;
    int vertices_size;
    vertex_t** vertices;

    int num_edges;
    int edges_size;
    edge_t** edges;
} graph_t;

graph_t* graph_init();
void graph_malloc(graph_t* graph);
void graph_delete(graph_t* graph);
void print_graph(graph_t* graph);

void add_directed_edge(graph_t* graph, int source, int destination, int weight);
void remove_directed_edge(graph_t* graph, edge_t* edge);

vertex_t* add_or_get_vertex(graph_t* graph, int id);
void remove_vertex(graph_t* graph, int id);
int add_edge_to_vertex_list(edge_t*** edges, edge_t* edge, int count, int* size);
int remove_edge_from_vertex_list(edge_t** edges, int i, int count);

// Use this when a normal adjacency_list is given as an input.
graph_t* parse_adjacency_list_only(char* str);
// Use this when number of edges and number of vertices are given as first line.
graph_t* parse_adjacency_list(char* str);
// Use this when constructing a graph from a file with an adjacency matrix.
graph_t* parse_adjacency_matrix(char* str);
