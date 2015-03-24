typedef struct _int_data_t
{
    int value;
} heap_data_t;

typedef struct _heap_t
{
    int size;
    int allocated_size;
    int leaf;
    heap_data_t** data;
} heap_t;

// Node functions
heap_data_t* node_init(int data);
void node_delete(heap_data_t* node);

// Constructor and destructor for heap.
heap_t* heap_init();
void clear_heap(heap_t* heap);
int get_heap_top(heap_t* heap); // Returns top of heap.
int get_heap_size(heap_t* heap); // Returns size of heap.
void print_heap(heap_t* heap);

// Max Heap
void create_max_heap(heap_t* heap, int* arr, int size); // Create max heap from input array

// Max Heap basic functions
int heap_remove_top(heap_t* heap); // Removes the top from the Max heap.
void insert_max_heap(heap_t* heap, int data); // Inserts the data into the Max heap.
int get_max(heap_t* heap); // Returns the top from min heap.

// Internal Helper functions
void heapify_down_max(heap_t* heap, int i);
heap_data_t* get_min_node_max_heap(heap_t* heap, int i);

// Min heap
void create_min_heap(heap_t* heap, int* arr, int size);

// Min heap basic functions
int remove_min(heap_t* heap); // Removes the top from the min heap.
void insert_min_heap(heap_t* heap, int data); // Inserts the data into the min heap.
int get_min(heap_t* heap); // Returns the top from min heap.

// Internal Helper functions
void heapify_down_min(heap_t* heap, int i);
heap_data_t* get_max_node_min_heap(heap_t* heap, int i);
