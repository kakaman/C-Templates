#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "hashtable.h"

static hashtable_t* hashtable;

void const* insert(char const* key, int score)
{
    int key_len = strlen(key) + 1;
    int size = sizeof(int);
    return hashtable_insert(hashtable, key, key_len, &score, size);
}

bool remove_key(char const* key)
{
    int key_len = strlen(key) + 1;
    return hashtable_remove(hashtable, key, key_len);
}

bool lookup(char const* key)
{
    int key_len = strlen(key) + 1;
    return hashtable_lookup(hashtable, key, key_len);
}

void const* find_value(char const* key)
{
    int key_len = strlen(key) + 1;
    return hashtable_find(hashtable, key, key_len);
}

int main()
{
    hashtable = hashtable_create();

    char** string_array = NULL;

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* file = fopen("string_list.txt", "r");

    read = getline(&line, &len, file);
    int num_strings = 0;
    int count = 0;
    sscanf(line, "%d", &num_strings);

    string_array = malloc(sizeof(char*) * num_strings);
    while ((read = getline(&line, &len, file)) != -1)
    {
        string_array[count] = strdup(line);
        count++;
    }

    free(line);
    fclose(file);

    for(int i = 0; i < num_strings; i++)
    {
        printf("Score: %d \nRanking: %d\n", i, i);
        char const* ret = insert(string_array[i], i);
        printf("%s\n\n", ret);
    }

    for(int i = 0; i < num_strings; i++)
    {
        bool found = false;

        found = lookup(string_array[i]);
        //char* is_found = (found == 1) ? "true" : "false";
        printf("Key found: %d\n", found);
        int* find = find_value(string_array[i]);
        printf("Key: %d\n",*find);
    }

//    for(int i = num_strings - 1; i >= 0; i--)
//    {
//        bool return_val = false;
//        if( i % 2 == 0)
//            return_val = remove_key(string_array[i]);
//
//        return_val = lookup(string_array[i]);
//        printf("Key found: %d\n", return_val);
//
//        int const* test_find = find(string_array[i]);
//        if(test_find != NULL)
//            printf("%d\n", *test_find);
//    }

    return 0;
}
