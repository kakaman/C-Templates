/*
 * johnson.c
 *
 *  Created on: Apr 14, 2015
 *      Author: vyshnav
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <limits.h>

#include "graph.h"
#include "heap.h"

#define INFINITY INT_MAX
#define INVALID -3
#define UNKNOWN -1

/*
    Johnson's Algorithm:

    1) A new node q is added to the graph, connected by zero-weight edges to each of the other nodes.

    2) The Bellman–Ford algorithm is used, starting from the new vertex q, to find for each vertex v the minimum weight h(v) of a path from q to v.
       If this step detects a negative cycle, the algorithm is terminated.

    3) The edges of the original graph are re-weighted using the values computed by the Bellman–Ford algorithm: an edge from u to v, having length w(u,v), is given the new length w(u,v) + h(u) − h(v).

    4) q is removed, and Dijkstra's algorithm is used to find the shortest paths from each node s to every other vertex in the re-weighted graph.
*/

void johnson(graph_t* graph, int source_index, int expected)
{
    return;
}

graph_t* parse(char* str)
{
    graph_t* graph = graph_init();

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen(str, "r");

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

    return graph;
}

int main()
{
    time_t now, end;
    graph_t* graph = parse("g1.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        johnson(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Johnson on g1.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    graph = parse("g2.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        johnson(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Johnson on g2.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    graph = parse("g3.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        johnson(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Johnson on g3.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);


    graph = parse("large.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        johnson(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Johnson on large.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    return 1;
}
