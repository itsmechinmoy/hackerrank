#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* ltrim(char*);
char* rtrim(char*);
char** split_string(char*);

int parse_int(char*);

/*
 * Complete the 'dynamicArray' function below.
 *
 * The function is expected to return an INTEGER_ARRAY.
 * The function accepts following parameters:
 *  1. INTEGER n
 *  2. 2D_INTEGER_ARRAY queries
 */

/*
 * To return the integer array from the function, you should:
 *     - Store the size of the array to be returned in the result_count variable
 *     - Allocate the array statically or dynamically
 *
 * For example,
 * int* return_integer_array_using_static_allocation(int* result_count) {
 *     *result_count = 5;
 *
 *     static int a[5] = {1, 2, 3, 4, 5};
 *
 *     return a;
 * }
 *
 * int* return_integer_array_using_dynamic_allocation(int* result_count) {
 *     *result_count = 5;
 *
 *     int *a = malloc(5 * sizeof(int));
 *
 *     for (int i = 0; i < 5; i++) {
 *         *(a + i) = i + 1;
 *     }
 *
 *     return a;
 * }
 *
 */
int* dynamicArray(int n, int queries_rows, int queries_columns, int** queries, int* result_count) {
    // Create n empty sequences (arrays)
    int** seqList = (int**)malloc(n * sizeof(int*));
    int* seqSizes = (int*)calloc(n, sizeof(int));  // Track the size of each sequence
    int* seqCapacities = (int*)calloc(n, sizeof(int));  // Track the capacity of each sequence
    
    // Initialize arrays to track type 2 query results
    int* answers = NULL;
    int answerCount = 0;
    int answerCapacity = 0;
    
    // Variable to keep track of the last answer
    int lastAnswer = 0;
    
    // Process each query
    for (int i = 0; i < queries_rows; i++) {
        int queryType = queries[i][0];
        int x = queries[i][1];
        int y = queries[i][2];
        
        // Calculate the sequence index
        int idx = (x ^ lastAnswer) % n;
        
        if (queryType == 1) {
            // Type 1 query: Append y to sequence at index idx
            
            // If sequence doesn't exist or needs more space, allocate/reallocate
            if (seqCapacities[idx] == 0) {
                // Initialize with small capacity
                seqCapacities[idx] = 4;
                seqList[idx] = (int*)malloc(seqCapacities[idx] * sizeof(int));
            } else if (seqSizes[idx] >= seqCapacities[idx]) {
                // Double capacity if needed
                seqCapacities[idx] *= 2;
                seqList[idx] = (int*)realloc(seqList[idx], seqCapacities[idx] * sizeof(int));
            }
            
            // Append y to the sequence
            seqList[idx][seqSizes[idx]] = y;
            seqSizes[idx]++;
            
        } else if (queryType == 2) {
            // Type 2 query: Find value at index y % size in sequence at index idx, update lastAnswer
            int size = seqSizes[idx];
            if (size > 0) {
                lastAnswer = seqList[idx][y % size];
                
                // Allocate/reallocate space for answers if needed
                if (answerCapacity == 0) {
                    // Initial allocation
                    answerCapacity = 4;
                    answers = (int*)malloc(answerCapacity * sizeof(int));
                } else if (answerCount >= answerCapacity) {
                    // Double capacity if needed
                    answerCapacity *= 2;
                    answers = (int*)realloc(answers, answerCapacity * sizeof(int));
                }
                
                // Store lastAnswer in the results
                answers[answerCount++] = lastAnswer;
            }
        }
    }
    
    // Set the result count
    *result_count = answerCount;
    
    // Clean up allocated memory for sequences
    for (int i = 0; i < n; i++) {
        if (seqCapacities[i] > 0) {
            free(seqList[i]);
        }
    }
    free(seqList);
    free(seqSizes);
    free(seqCapacities);
    
    // Return the array of answers
    return answers;
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char** first_multiple_input = split_string(rtrim(readline()));

    int n = parse_int(*(first_multiple_input + 0));

    int q = parse_int(*(first_multiple_input + 1));

    int** queries = malloc(q * sizeof(int*));

    for (int i = 0; i < q; i++) {
        *(queries + i) = malloc(3 * (sizeof(int)));

        char** queries_item_temp = split_string(rtrim(readline()));

        for (int j = 0; j < 3; j++) {
            int queries_item = parse_int(*(queries_item_temp + j));

            *(*(queries + i) + j) = queries_item;
        }
    }

    int result_count;
    int* result = dynamicArray(n, q, 3, queries, &result_count);

    for (int i = 0; i < result_count; i++) {
        fprintf(fptr, "%d", *(result + i));

        if (i != result_count - 1) {
            fprintf(fptr, "\n");
        }
    }

    fprintf(fptr, "\n");

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}

char* ltrim(char* str) {
    if (!str) {
        return '\0';
    }

    if (!*str) {
        return str;
    }

    while (*str != '\0' && isspace(*str)) {
        str++;
    }

    return str;
}

char* rtrim(char* str) {
    if (!str) {
        return '\0';
    }

    if (!*str) {
        return str;
    }

    char* end = str + strlen(str) - 1;

    while (end >= str && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

char** split_string(char* str) {
    char** splits = NULL;
    char* token = strtok(str, " ");

    int spaces = 0;

    while (token) {
        splits = realloc(splits, sizeof(char*) * ++spaces);

        if (!splits) {
            return splits;
        }

        splits[spaces - 1] = token;

        token = strtok(NULL, " ");
    }

    return splits;
}

int parse_int(char* str) {
    char* endptr;
    int value = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        exit(EXIT_FAILURE);
    }

    return value;
}