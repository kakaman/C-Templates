#ifndef _DSETS_H_
#define _DSETS_H_

#include <vector>

using std::vector;

class DisjointSets
{
	public:

	/* Add Elements
	 * Creates n unconnected root nodes at the end of the vector.
	 *
	 * Parameters
	 * num: The number of nodes to create.
	 */
	void addelements ( int num );

	/* Find
	 * This function should compress paths and works as described in lecture.
	 *
	 * Returns
	 * The index of the root of the up-tree in which the parameter element
	 * resides.
	 */
	int find ( int elem );

	/* SetUnion
	 * This function should be implemented as union-by-size.
	 *
	 * That is, when you setunion two disjoint sets, the smaller (in terms
	 * of number of nodes) should point at the larger. This function works
	 * as described in lecutre, except that you should not assume that the
	 * arguments to setunion are roots of existing uptrees.
	 *
	 * Your setunion function SHOULD find the roots of its arguments before
	 * combining the trees. If the two sets are the same size, make the tree
	 * containing the second argument point to the tree containing the first.
	 * (Note that normally we could break this tie arbitrarily, but in this
	 * case we want to control things for grading.)
	 *
	 * Parameters
	 * a: Index of the first element to union.
	 * b: Index of the second element to union.
	 */
	void setunion ( int a, int b );

	private:

	/* This is the Up-Trees Representation
	 * For nodes with parents, the value of the element is the index of
	 * the parent.
	 * For root nodes, the value of of the element is the negative of
	 * the size of the up-tree.
	 */
	vector< int > uptrees;

	/* isBigger helper function.
	 * Returns true if the size of root1 is bigger than the size of root2,
	 * false otherwise.
	 *
	 * Also returns true if the sets are the same size.
	 *
	 * Assumes that the arguments are roots.
	 */
	 bool isBigger ( int root1, int root2);

};

typedef struct _disjoint_set_t
{
    int *links;
    int *sizes;
    int *ranks;
    int maxindex;
    int nsets;
}disjoint_set_t;

disjoint_set_t* new_disjoint_set(int maxindex);
void free_disjoint_set(disjoint_set_t* disjoint_set);
void disjoint_makeset(disjoint_set_t* disjoint_set, int index);
int disjoint_union(disjoint_set_t* disjoint_set, int s1, int s2);
int disjoint_find(disjoint_set_t* disjoint_set, int index);


#endif
