/*
 * bellman-ford.c
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
    Bellman-Ford shortest path psuedocode:
    function BellmanFord(list vertices, list edges, vertex source)
       ::distance[],predecessor[]

   // This implementation takes in a graph, represented as
   // lists of vertices and edges, and fills two arrays
   // (distance and predecessor) with shortest-path
   // (less cost/distance/metric) information

       // Step 1: initialize graph
       for each vertex v in vertices:
           if v is source then distance[v] := 0
           else distance[v] := inf
           predecessor[v] := null

       // Step 2: relax edges repeatedly
       for i from 1 to size(vertices)-1:
           for each edge (u, v) with weight w in edges:
               if distance[u] + w < distance[v]:
                   distance[v] := distance[u] + w
                   predecessor[v] := u

       // Step 3: check for negative-weight cycles
       for each edge (u, v) with weight w in edges:
           if distance[u] + w < distance[v]:
               error "Graph contains a negative-weight cycle"
       return distance[], predecessor[]
*/

void bellman_ford(graph_t* graph, int source_index, int expected)
{
    int negative_cycles = 0;

    int num_vertices = graph->num_vertices;
    int num_edges = graph->num_edges;

    // Step 1: Initialize the graph.
    int* distance = malloc(sizeof(int) * num_vertices);
    memset(distance, INFINITY, sizeof(int) * num_vertices);
    distance[source_index] = 0;

    int* via = malloc(sizeof(int) * num_vertices);
    memset(via, INVALID, sizeof(int) * num_vertices);

    // Step 2: Relax edges repeatedly.
    for(int i = 0; i < num_vertices; i++)
    {
        for(int edge_index = 0; edge_index < num_edges; edge_index++)
        {
            edge_t* edge = graph->edges[edge_index];
            int source = edge->src->index;
            int destination = edge->dest->index;
            int weight = edge->weight;

            if(distance[source] + weight < distance[destination])
            {
                distance[destination] = distance[source] + weight;
                via[destination] = source;
            }
        }
    }

    // Step 3: Check for negative-weight cycles
    for(int i = 0; i < graph->num_edges; i++)
    {
        edge_t* negative_edge = graph->edges[i];
        int negative_source = negative_edge->src->index;
        int negative_destination = negative_edge->dest->index;
        int negative_weight = negative_edge->weight;

        if(distance[negative_source]  + negative_weight < distance[negative_destination])
        {
            negative_cycles++;
            printf("The graph contains a negative edge cycle.\n");
            printf("Current negative cycles: %d\n", negative_cycles);
        }
    }

    printf("Expected: %d\n", expected);
    printf("Received: %d\n", negative_cycles);

    free(distance);
    free(via);
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
        bellman_ford(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Bellman-Ford on g1.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    graph = parse("g2.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        bellman_ford(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Bellman-Ford on g2.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    graph = parse("g3.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        bellman_ford(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Bellman-Ford on g3.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);


    graph = parse("large.txt");

    time(&now);

    for(int i = 0; i < graph->num_vertices; i++)
    {
        bellman_ford(graph, i, UNKNOWN);
    }

    time(&end);
    double seconds = difftime(end, now);
    printf("Bellman-Ford on large.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);

    return 1;
}
