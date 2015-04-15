#pragma once

#include "graph.h"

graph_t* parse_graph();

int count_edges(graph_t* graph);
int count_vertices(graph_t* graph);
void karger_algorithm(graph_t* graph);

void merge_vertices(graph_t* graph, int src, int dst);
void remove_self_loops(graph_t* graph);
void contract(graph_t* graph, int edge_index);
