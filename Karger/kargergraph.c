#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "kargergraph.h"

unsigned int get_tick_count()
{
    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts);

    unsigned int ticks = ts.tv_nsec / 1000000;
    ticks += ts.tv_sec * 1000;
    return ticks;
}

graph_t* parse_graph()
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    printf("creating graph\n");

    graph_t* graph = graph_init();

    while ((read = getline(&line, &len, stdin)) != -1)
    {
        char* vertex = strtok(line, " \t\r");

        int src = atoi(vertex);

        vertex = strtok(NULL, " \t\r");

        while (vertex != NULL)
        {
            int dest = atoi(vertex);
            if (dest == 0)
            {
                break;
            }

            add_directed_edge(graph, src, dest);
            vertex = strtok(NULL, " \t\r");

            if (vertex == NULL)
            {
                break;
            }
        }
    }

    free(line);

    return graph;
}

void remove_self_loops(graph_t* graph)
{
    edge_t* edge;
    int i;
    for (i = 0; i < graph->edges_size; i++)
    {
        edge = graph->edges[i];
        if (edge->src == edge->dest)
        {
            edge->src = NULL;
            edge->dest = NULL;
            free(edge);
        }
    }
}

void contract(graph_t* graph, int edge_index)
{
    edge_t* edge = graph->edges[edge_index];
    vertex_t* src = edge->src;
    vertex_t* dest = edge->dest;

    int i = 0;
    while (i < dest->in_count)
    {
        edge_t* in_edge = dest->in_edges[i];
        if (in_edge->src == src)
        {
            remove_directed_edge(graph, in_edge);
            continue;
        }

        in_edge->dest = src;
        src->in_count = add_edge_to_vertex_list(&src->in_edges, in_edge, src->in_count, &src->in_size);
        i++;
    }

    i = 0;
    while (i < dest->out_count)
    {
        edge_t* out_edge = dest->out_edges[i];
        if (out_edge->dest == src)
        {
            remove_directed_edge(graph, out_edge);
            continue;
        }

        out_edge->src = src;
        src->out_count = add_edge_to_vertex_list(&src->out_edges, out_edge, src->out_count, &src->out_size);
        i++;
    }

    // Decrement the counters.
    remove_vertex(graph, dest->index);

    return;
}

void karger_algorithm(graph_t* graph)
{
    while (graph->num_vertices > 2) // put condition
    {
        int random_edge = rand() % graph->num_edges;

        contract(graph, random_edge); // Needs to decrement the size counter. Can only contract if there is an edge between them.
    }

    print_graph(graph);
    return;
}

int main()
{
    graph_t* graph = parse_graph();

    printf("Parse finished\n");

    print_graph(graph);

    // Run Karger algorithm
    printf("running karger algo\n");

    srand(get_tick_count());
    karger_algorithm(graph);

    //graph_delete(graph);

    return 1;
}
