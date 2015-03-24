typedef struct
{
    void* data;
    int priority;
} p_queue_data_t;

typedef struct
{
    p_queue_data_t* array;
    int num_elements;
    int allocated;
} p_queue_t;

// Constructor and Destructor.
p_queue_t* p_queue_init(int size);
void p_queue_destroy(p_queue_t* p_queue);

// Priority Queue functions.
void p_queue_insert(p_queue_t* p_queue, void* data, int priority);
void* p_queue_remove(p_queue_t* p_queue, int* priority);
void* p_queue_peek(p_queue_t* p_queue, int *pri);
