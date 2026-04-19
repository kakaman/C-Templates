# Top-level Makefile for C-Templates.
#
# Each module is a self-contained subdirectory with its own Makefile.
# This top-level Makefile recurses into each module's Makefile.
#
# Usage:
#   make              # build every module listed below
#   make clean        # clean every module listed below
#   make Heap         # build a single module
#
# Modules excluded because they are incomplete or unsupported on this
# platform (see README.md): Binary Dictionary, Dictionary, Ctrie,
# Nana Ctrie, Bloom Filter, Matrix, Undirected Graph, Alloc.

MODULES := \
    Algorithms/Bellman-Ford \
    Algorithms/Floyd-Warshall \
    Algorithms/Johnson \
    Dijkstra \
    Disjoint\ Sets \
    Functions \
    Graph \
    Hashtable \
    Heap \
    Karger \
    Kruskal \
    Mergesort \
    Prim \
    Priority\ queue \
    Queue \
    Quicksort

.PHONY: all clean $(MODULES)

all: $(MODULES)

$(MODULES):
	@echo "==> Building $@"
	@$(MAKE) -C "$@"

clean:
	@for d in $(MODULES); do \
	    echo "==> Cleaning $$d"; \
	    $(MAKE) -C "$$d" clean || exit $$?; \
	done
