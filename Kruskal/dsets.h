#pragma once

// This struct is meant to be inserted into the object using disjoint sets.
typedef struct _disjoint_set_t_
{
    void* parent;
    int rank;
    void* next;
    void* prev;
} disjoint_set_t;

typedef struct _disjoint_sets_t_
{
    int offset;
    void (*print)(void const*);
    int num_sets;
    int allocated_size;
    void** disjoint_sets;
} disjoint_sets_t;

disjoint_sets_t* create_disjoint_sets(int offset, void (*print)(void const*));
void delete_disjoint_sets(disjoint_sets_t* sets);

disjoint_set_t* disjoint_set_init();
void add_set_to_disjoint_sets(disjoint_sets_t* sets, void* node);
void delete_set_from_disjoint_sets(disjoint_sets_t* sets, void* set);

void* find_disjoint_set(disjoint_sets_t* sets, void* node);
void* union_disjoint_sets(disjoint_sets_t* sets, void* left, void* right);

int get_num_disjoint_sets(disjoint_sets_t const* sets);
void* get_disjoint_set(disjoint_sets_t const* sets, int set_number);
void print_disjoint_set(disjoint_sets_t* sets, int set_number);
void enumerate_disjoint_set(disjoint_sets_t* sets, int set_number, void (*callback)(void *));
