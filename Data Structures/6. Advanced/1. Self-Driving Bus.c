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
 * Complete the 'solve' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts 2D_INTEGER_ARRAY tree as parameter.
 */

#define MAXN 200200
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

// Arrays for storing parent relationships and range values
long long int par[MAXN], upr[MAXN], dnr[MAXN];
long long int run_upr[MAXN], run_dnr[MAXN], grand[MAXN];

// Graph representation
long long int** G;
long long int* G_size;

// Function to set parent relationships
void set_parents(long long int m, long long int a, long long int b, long long int p) {
    upr[m] = max(m, upr[p]);
    dnr[m] = min(m, dnr[p]);
    par[m] = p;
    grand[m] = grand[p];
    
    for (long long int i = 0; i < G_size[m]; i++) {
        long long int next = G[m][i];
        if (next < a || next >= b || next == p) continue;
        set_parents(next, a, b, m);
    }
}

// Function to handle the middle case
long long int middle_case(long long int m, long long int a, long long int b) {
    run_upr[m] = run_dnr[m] = m;
    long long int aux;
    
    for (aux = m; aux < b && grand[aux] == m; aux++);
    b = aux;
    
    for (aux = m; aux >= a && grand[aux] == m; aux--);
    a = aux + 1;

    for (long long int i = m + 1; i < b; i++) {
        run_upr[i] = max(run_upr[i-1], upr[i]);
        run_dnr[i] = min(run_dnr[i-1], dnr[i]);
    }

    for (long long int i = m - 1; i >= a; i--) {
        run_upr[i] = max(run_upr[i+1], upr[i]);
        run_dnr[i] = min(run_dnr[i+1], dnr[i]);
    }
    
    long long int total = 0; // {m}
    // Count [i,d] with i <= m/2, d >= m/2
    long long int ct = 0;
    for (long long int d = m, l = m + 1, r = m + 1; d < b; d++) {
        if (d != run_upr[d]) continue;
        
        for (; l - 1 >= a && d >= run_upr[l - 1]; l--) {
            if (l - 1 == run_dnr[l - 1]) ct++;
        }
        
        for (; r - 1 > run_dnr[d] && r > l; r--) {
            if (r - 1 == run_dnr[r - 1]) ct--;
        }
        
        total += ct;
    }
    
    return total;
}

// Main solve function using divide and conquer
long long int solve_tree(long long int a, long long int b) {
    if (a == b) {
        return 0;
    }
    if (a + 1 == b) {
        return 1;
    }
    
    long long int m = (a + b) / 2;
    upr[m] = par[m] = dnr[m] = grand[m] = m;
    
    set_parents(m, a, b, m);
    
    long long int x = middle_case(m, a, b);
    long long int y = solve_tree(a, m);
    long long int z = solve_tree(m + 1, b);
    
    return (x + y + z);
}

long long int solve(int tree_rows, int tree_columns, int** tree) {
    int n = tree_rows + 1; // Number of nodes
    
    // Allocate memory for graph representation
    G = (long long int**)malloc(n * sizeof(long long int*));
    G_size = (long long int*)calloc(n, sizeof(long long int));
    
    // First, count edges for each node
    for (int i = 0; i < tree_rows; i++) {
        int u = tree[i][0] - 1; // Convert to 0-based indexing
        int v = tree[i][1] - 1;
        G_size[u]++;
        G_size[v]++;
    }
    
    // Allocate memory for adjacency lists
    for (int i = 0; i < n; i++) {
        G[i] = (long long int*)malloc(G_size[i] * sizeof(long long int));
        G_size[i] = 0; // Reset to use as counter
    }
    
    // Fill adjacency lists
    for (int i = 0; i < tree_rows; i++) {
        int u = tree[i][0] - 1;
        int v = tree[i][1] - 1;
        G[u][G_size[u]++] = v;
        G[v][G_size[v]++] = u;
    }
    
    // Solve the problem
    long long int result = solve_tree(0, n);
    
    // Free allocated memory
    for (int i = 0; i < n; i++) {
        free(G[i]);
    }
    free(G);
    free(G_size);
    
    return (int)result;
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    int n = parse_int(ltrim(rtrim(readline())));

    int** tree = malloc((n - 1) * sizeof(int*));

    for (int i = 0; i < n - 1; i++) {
        *(tree + i) = malloc(2 * (sizeof(int)));

        char** tree_item_temp = split_string(rtrim(readline()));

        for (int j = 0; j < 2; j++) {
            int tree_item = parse_int(*(tree_item_temp + j));

            *(*(tree + i) + j) = tree_item;
        }
    }

    long long int result = solve(n - 1, 2, tree);
    fprintf(fptr, "%lld\n", result);  // Use %lld format specifier for long long

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