#include <iostream>
#include "dsets.h"

using namespace std;
void DisjointSets::addelements ( int num )
{
	/* Push to the back of the vector the number of elements specified,
	 * each with a size of -1.
	 */
	for (int i = 0; i < num; i++)
	{
		uptrees.push_back( -1 );
	}
}

int DisjointSets::find ( int elem )
{
	/* Base Case
	 * Returns the index of the current element if the current element
	 * is a root node.
	 */
	if (uptrees[elem] < 0)
	{
		return elem;
	}

	/* Recursive Definition
	 * Applies path compression by pointing the parent of the current
	 * element direct to the root node of the set.
	 * returns the parent of the element, which is the root node.
	 */
	else
	{
		uptrees[elem] = find ( uptrees[elem] );
		return uptrees[elem];
	}
}

void DisjointSets::setunion ( int a, int b )
{
	/* Finds the root nodes of the two elements. */
	int root1 = find ( a );
	int root2 = find ( b );

	/* If the root nodes are the same node, then the two elements
	 * are already in the same set and there is no work to be done.
	 */
	if ( root1 == root2 )
	{
		return;
	}

	/* The two elements are in different sets and the two sets have to
	 * be combined.
	 */
	else
	{
		/* Find the new size of the combined set. */
		int newSize = uptrees[root1] + uptrees[root2];

		/* Check if the size of set represented by root1 is bigger than the
		 * size of set represented by root2. If it is, assign the parent of
		 * root2 to be root1 and increase the size of the set accordingly.
		 *
		 * In case the two sets are of equal sizes, root2 will be combined
		 * into root1.
		 */
		if ( isBigger ( root1, root2 ) )
		{
			uptrees[root2] = root1;
			uptrees[root1] = newSize;
		}

		/* If the size of set represented by root2 is bigger than the size of
		 * set represented bby root1, assign the parent of root1 to be root2
		 * and increase the size of the set accoridngly.
		 */
		else
		{
			uptrees[root1] = root2;
			uptrees[root2] = newSize;
		}
	}
}

bool DisjointSets::isBigger ( int root1, int root2)
{
	/* If the size of set represented by root1 is bigger or equal to the
	 * size of set represented by root2, return true. Otherwise, return false.
	 */
	if (uptrees[root1] <= uptrees[root2])
	{
		return true;
	}

	else
	{
		return false;
	}
}

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

disjoint_set_t*new_disjoint_set(int maxindex)
{
    disjoint_set_t*dj;
    int i;

    dj = talloc(disjoint_set_t, 1);
    if (dj == NULL)
    {
        perror("malloc");
        exit(1);
    }

    dj->maxindex = maxindex;
    dj->nsets = 0;

    dj->links = talloc(int, maxindex);
    if (dj->links == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (i = 0; i < maxindex; i++)
        dj->links[i] = -2;

    dj->sizes = talloc(int, maxindex);
    if (dj->sizes == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (i = 0; i < maxindex; i++)
        dj->sizes[i] = -1;

    dj->ranks = talloc(int, maxindex);
    if (dj->ranks == NULL)
    {
        perror("malloc");
        exit(1);
    }
    for (i = 0; i < maxindex; i++)
        dj->ranks[i] = -1;
    return dj;
}

void free_disjoint_set(disjoint_set_t*dj)
{
    free(dj->links);
    free(dj->sizes);
    free(dj->ranks);
    free(dj);
}

void disjoint_makeset(disjoint_set_t*dj, int index)
{
    if (dj->links[index] != -2)
    {
        fprintf(stderr, "disjoint_makeset called on an already made set %d\n", index);
        exit(1);
    }
    dj->links[index] = -1;
    dj->nsets++;
    dj->sizes[index] = 1;
    dj->ranks[index] = 1;
}

int disjoint_union(disjoint_set_t*dj, int s1, int s2)
{
    if (dj->links[s1] != -1)
    {
        fprintf(stderr, "disjoint_union called on a non-root node %d\n", s1);
        exit(1);
    }
    if (dj->links[s2] != -1)
    {
        fprintf(stderr, "disjoint_union called on a non-root node %d\n", s2);
        exit(1);
    }
    if (s1 == s2)
        return 0;

    dj->nsets--;

    if (dj->ranks[s1] < dj->ranks[s2])
    {
        dj->links[s1] = s2;
        dj->sizes[s2] += dj->sizes[s1];
        return s2;
    }
    else if (dj->ranks[s1] > dj->ranks[s2])
    {
        dj->links[s2] = s1;
        dj->sizes[s1] += dj->sizes[s2];
        return s1;
    }
    else
    {
        dj->links[s2] = s1;
        dj->sizes[s1] += dj->sizes[s2];
        dj->ranks[s1]++;
        return s1;
    }
}

int disjoint_find(disjoint_set_t*dj, int index)
{
    int id;

    if (dj->links[index] == -1)
        return index;

    if (dj->links[dj->links[index]] == -1)
        return dj->links[index];

    id = disjoint_find(dj, dj->links[index]);
    dj->links[index] = id;
    return id;
}
