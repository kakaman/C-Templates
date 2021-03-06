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

    int degree;
    int edges_size;
    edge_t* edges;
} vertex_t;

typedef struct _undirected_graph_t
{
    int num_vertices;
    int vertices_size;
    vertex_t** vertices;

    int num_edges;
    int edges_size;
    edge_t** edges;
} undirected_graph_t;

undirected_graph_t* graph_init();
void graph_malloc(undirected_graph_t* graph);
void graph_delete(undirected_graph_t* graph);
void print_graph(undirected_graph_t* graph);

void add_edge(undirected_graph_t* graph, int source, int destination, int weight);
void remove_edge(undirected_graph_t* graph, edge_t* edge);

void add_directed_edge(undirected_graph_t* graph, int source, int destination, int weight);
void remove_directed_edge(undirected_graph_t* graph, edge_t* edge);

vertex_t* add_or_get_vertex(undirected_graph_t* graph, int id);
void remove_vertex(undirected_graph_t* graph, int id);
int add_edge_to_vertex_list(edge_t*** edges, edge_t* edge, int count, int* size);
int remove_edge_from_vertex_list(edge_t** edges, int i, int count);

// Use this when a normal adjacency_list is given as an input.
undirected_graph_t* parse_adjacency_list_only(char* str);
// Use this when number of edges and number of vertices are given as first line.
undirected_graph_t* parse_adjacency_list(char* str);
// Use this when constructing a graph from a file with an adjacency matrix.
undirected_graph_t* parse_adjacency_matrix(char* str);
