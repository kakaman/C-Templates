#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
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

void const* insert_random_num(int random, int score)
{
    int key_len = sizeof(int) + 1;
    int size = sizeof(int);
    char key = (char) random;
    //key[key_len - 1] = '\0';

    return hashtable_insert(hashtable, &key, key_len, &score, size);
}

void const* find_random_num(int random)
{
    int key_len = sizeof(int) + 1;
    char key = (char) random;
    //key[key_len - 1] = '\0';

    return hashtable_find(hashtable, &key, key_len);
}

int main(int argc, char** argv)
{
    if(argc == 1)
        return 1;

    hashtable = hashtable_create();

    if (strcmp(argv[1], "time") == 0)
    {
        time_t start = time(NULL);

        printf("\n");

        int random = 0;
        srand(start);
        for (int i = 1; i < 101; i++)
        {
            random = rand() % 100;
            insert_random_num(random, i);
            if (i % 10 == 0)
                printf(" (%d, %d)\n", random, i);
            else
                printf(" (%d, %d) ", random, i);
        }
        printf("\n");

        srand(start);
        for (int i = 1; i < 101; i++)
        {
            random = rand() % 100;

            int* ret = find_random_num(random);

            if (i % 10 == 0)
                printf("\n");

            if (ret == NULL)
                printf(" (%d, NULL) ", random);
            else
                printf(" (%d, %d) ", random, *ret);
        }
    }
    else if(strcmp(argv[1], "string") == 0)
    {
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

        for (int i = 0; i < num_strings; i++)
        {
            printf("Score: %d \nRanking: %d\n", i, i);
            char const* ret = insert(string_array[i], i);
            printf("%s\n\n", ret);
        }

        for (int i = num_strings - 1; i >= 0; i--)
        {
            bool return_val = false;
            if (i % 2 == 0)
                return_val = remove_key(string_array[i]);

            return_val = lookup(string_array[i]);
            printf("Key found: %d\n", return_val);

            int* test_find = find_value(string_array[i]);
            if (test_find != NULL)
                printf("%d\n", *test_find);
        }

        for(int i = 0; i < num_strings; i++)
        {
            free(string_array[i]);
        }
        free(string_array);
    }

    hashtable_destroy(hashtable);
    return 0;
}
