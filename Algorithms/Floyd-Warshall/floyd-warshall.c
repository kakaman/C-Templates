/*
 * floyd-warshall.c
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
#define UNKNOWN -1

/*
    Floyd-Warshall shortest path psuedocode:
    let dist be a |V| × |V| array of minimum distances initialized to ∞ (infinity)
    for each vertex v
        dist[v][v] ← 0
    for each edge (u,v)
        dist[u][v] ← w(u,v)  // the weight of the edge (u,v)
    for k from 1 to |V|
        for i from 1 to |V|
            for j from 1 to |V|
                if dist[i][j] > dist[i][k] + dist[k][j]
                    dist[i][j] ← dist[i][k] + dist[k][j]
                end if
*/

void floyd_warshall(graph_t* graph, int source_index, int expected)
{

    print_graph(graph);
    int negative_cycles = 0;

    int num_vertices = graph->num_vertices;
    int num_edges = graph->num_edges;

    graph_t* shortest_path = graph_init();
    // Initialize the distance
    int** distance = malloc(sizeof(int*) * num_vertices);
    for(int i = 0; i < num_vertices; i++)
    {
        distance[i] = malloc(sizeof(int) * num_vertices);
        for(int j = 0; j < num_vertices; j++)
        {
            distance[i][j] = INFINITY;
        }
        distance[i][i] = 0;
    }

    for(int i = 0; i < num_edges; i++)
    {
        edge_t* edge = graph->edges[i];

        int source = edge->src->index;
        int destination = edge->dest->index;
        int weight = edge->weight;

        distance[source][destination] = weight;
    }

    for(int k = 0; k < num_vertices; k++)
    {
        for(int i = 0; i < num_vertices; i++)
        {
            for(int j = 0; j < num_vertices; j++)
            {
                if(distance[i][k] == INFINITY || distance[k][j] == INFINITY)
                {
                    continue;
                }
                int cost = distance[i][k] + distance[k][j];
                if (distance[i][j] >= cost)
                {
                    distance[i][j] = cost;
                }
                else
                {
//                    negative_cycles++;
//                    printf("The graph contains a negative edge cycle.\n");
//                    printf("Current negative cycles: %d\n", negative_cycles);
                }
            }
        }
    }

    for(int i = 0; i < num_vertices; i++)
    {
        int min = 0;
        for(int j = 0; j < num_vertices; j++)
        {
            if( min != i && distance[j][i] < distance[min][i])
            {
                min = j;
            }
        }

        if(min != i)
        {
            add_directed_edge(shortest_path, i + 1, min + 1, distance[min][i]);
        }
    }
//    printf("Expected: %d\n", expected);
//    printf("Received: %d\n", negative_cycles);

    for(int i = 0; i < num_vertices; i++)
    {
        free(distance[i]);
    }

    print_graph(shortest_path);
    graph_delete(shortest_path);

    free(distance);
}

/*
    Construct the shortest path tree:
    let dist be a |V| × |V| array of minimum distances initialized to ∞ (infinity)
    let next be a |V| × |V| array of vertex indices initialized to null

    procedure FloydWarshallWithPathReconstruction ()
       for each edge (u,v)
          dist[u][v] ← w(u,v)  // the weight of the edge (u,v)
          next[u][v] ← v
       for k from 1 to |V| // standard Floyd-Warshall implementation
          for i from 1 to |V|
             for j from 1 to |V|
                if dist[i][k] + dist[k][j] < dist[i][j] then
                   dist[i][j] ← dist[i][k] + dist[k][j]
                   next[i][j] ← next[i][k]

    procedure Path(u, v)
       if next[u][v] = null then
           return []
       path = [u]
       while u ≠ v
           u ← next[u][v]
           path.append(u)
       return path
*/

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

    graph_t* graph = parse("test.txt");

    time(&now);

    floyd_warshall(graph, 0, 98);

    time(&end);
    double seconds = difftime(end, now);
    printf("Floyd-Warshall on test.txt:\n");
    printf("Ran for %.f seconds.\n\n", seconds);

    graph_delete(graph);


//    graph = parse("g1.txt");
//
//    time(&now);
//
//    for(int i = 0; i < graph->num_vertices; i++)
//    {
//        floyd_warshall(graph, i, UNKNOWN);
//    }
//
//    time(&end);
//    seconds = difftime(end, now);
//    printf("Floyd-Warshall on g1.txt:\n");
//    printf("Ran for %.f seconds.\n\n", seconds);
//
//    graph_delete(graph);
//
//    graph = parse("g2.txt");
//
//    time(&now);
//
//    for(int i = 0; i < graph->num_vertices; i++)
//    {
//        floyd_warshall(graph, i, UNKNOWN);
//    }
//
//    time(&end);
//    seconds = difftime(end, now);
//    printf("Floyd-Warshall on g2.txt:\n");
//    printf("Ran for %.f seconds.\n\n", seconds);
//
//    graph_delete(graph);
//
//    graph = parse("g3.txt");
//
//    time(&now);
//
//    for(int i = 0; i < graph->num_vertices; i++)
//    {
//        floyd_warshall(graph, i, UNKNOWN);
//    }
//
//    time(&end);
//    seconds = difftime(end, now);
//    printf("Floyd-Warshall on g3.txt:\n");
//    printf("Ran for %.f seconds.\n\n", seconds);
//
//    graph_delete(graph);
//
//
//    graph = parse("large.txt");
//
//    time(&now);
//
//    for(int i = 0; i < graph->num_vertices; i++)
//    {
//        floyd_warshall(graph, i, UNKNOWN);
//    }
//
//    time(&end);
//    seconds = difftime(end, now);
//    printf("Floyd-Warshall on large.txt:\n");
//    printf("Ran for %.f seconds.\n\n", seconds);
//
//    graph_delete(graph);

    return 1;
}
