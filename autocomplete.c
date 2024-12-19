# include "autocomplete.h"
# include <stdio.h>
#include <string.h>
#include <stdlib.h>

// part 1
int my_comp(const void *a, const void *b){
    const term *term_a = (const term *)a;
    const term *term_b = (const term *)b;
    return strcmp(term_a->term, term_b->term);
}

void read_in_terms(term **terms, int *pnterms, char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("Open the file fialed: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    int num = -1;
    char buffer[1024];

    while(fgets(buffer, sizeof(buffer), file) != NULL){
        num++;
    }
    *pnterms = num;
    rewind(file);

    *terms = (term *)malloc(num * sizeof(term));
    if(*terms == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Skip the first line
    if(fgets(buffer, sizeof(buffer), file) == NULL){ // position the file pointer one step forward to skip the first line
        printf("Error reading the first line\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < num; i++){
        fgets(buffer, sizeof(buffer), file);
        buffer[strcspn(buffer, "\n")] = '\0'; // strcspn finds the index of the first occurance of "\n" and 
                                              // this line replaces it with the empty string \0 to prevent
                                              // including the new line character in the city names
        char *token = strtok(buffer, "\t");// strtok takes the string from the beginning of the buffer to the empty space "\t"
        if(token != NULL){
            // Store population as weight
            (*terms)[i].weight = atof(token);
            // Extract city name
            token = strtok(NULL, "\t");
            if(token != NULL){
                strcpy((*terms)[i].term, token);
            }else{
                printf("Error reading city from line %d\n", num + 1);
                exit(EXIT_FAILURE);
            }
        }else{
            printf("Error reading population from line %d\n", num + 1);
            exit(EXIT_FAILURE);
        }
    }

    qsort(*terms, num, sizeof(term), my_comp);

    // Close the file
    fclose(file);
}

// part 2
int match_comp_lowest(char *a, char *b){// return 1 if a comes before b with a = mid, b = substr
    int match = 0;// originally return false
    int k = strlen(b);
    for(int j = 0; j<k; j++){
        if(a[j] != b[j]){
            for(int i = 0;i<k;i++){
                if(strcmp(&a[i], &b[i])< 0){
                    match = 1;// return true for a comes before b, or a < b
                    break;
                }else{
                    break;// no further comparisons after is setted to be 1
                }
            }
        break;
        }
    }
    return match;
}

int lowest_match(term *terms, int nterms, char *substr){
    int left = 0;
    int right = nterms - 1;
    while(left <= right){
        int mid = (left + right)/2;
        if (match_comp_lowest((terms)[mid].term, substr)){// if mid < target
            left = mid + 1;
        }else{
            right = mid - 1;
        }
    }
    return left;
}

//part 3
int match_comp_highest(char *a, char *b){// return 1 if a comes before b with a = substr, b = mid
    int match = 0;// originally return false
    int k = strlen(a);
    for(int j = 0; j<k; j++){
        if(b[j] != a[j]){
            for(int i = 0;i<k;i++){
                if(strcmp(&a[i], &b[i])< 0){
                    match = 1;// return true for a comes before b, or a < b
                    break;
                }else{
                    break;// no further comparisons after is setted to be 1
                }
            }
        break;
        }
    }
    return match;
}

int highest_match(term *terms, int nterms, char *substr){
    int left = 0;
    int right = nterms - 1;
    while(right >= left){
        int mid = (left + right)/2;
        if(match_comp_highest(substr, (terms)[mid].term)){// if mid > target
            right = mid - 1;
        }else{
            left = mid + 1;
        }
    }
    return right;
}

// part 4
int my_auto_comp(const void *a, const void *b){
    const term *term_a = (const term *)a;
    const term *term_b = (const term *)b;
    return ((term_b->weight)-(term_a->weight));

}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr){
    int lowest_ind = lowest_match(terms, nterms, substr);
    int highest_ind = highest_match(terms, nterms, substr) + 1;

    int num = highest_ind - lowest_ind;
    *n_answer = num;

    *answer = (term *)malloc(num * sizeof(term));
    if (*answer == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for(int i = lowest_ind; i < highest_ind; i++){
        (*answer)[i-lowest_ind].weight = terms[i].weight;
        strcpy((*answer)[i-lowest_ind].term, terms[i].term);
    }

    qsort(*answer, num, sizeof(term), my_auto_comp);
}