#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "type.h"

size_t value_length(type_t type, void* value) {
    switch(type) {

        case INT:
            return sizeof(int);
        case STR:
            return (strlen((char*) value) + 1) * sizeof(char);
        case DBL:
            return sizeof(double);
        case LLNG:
            return sizeof(long);
        default:
            return 0;
    }
}

void print_value(FILE* f, type_t type, void* value) {
    switch(type) {
        case INT:
            fprintf(f, "%d", *((int*) value));
            break;
        case STR:
            fprintf(f, "%s", (char*) value);
            break;
        case DBL:
            fprintf(f, "%lf", *((double*) value));
            break;
        case LLNG:
            fprintf(f, "%li", *((long*) value));
            break;
    }
}


int value_cmp(type_t type, void* value1, void* value2) {
    switch(type) {
        case INT:
            return *((int*) value1) - *((int*) value2);
        case STR:
            return strcmp((char*) value1, (char*) value2);
        case DBL:
            return *((double*) value1) - *((double*) value2);
        case LLNG:
            return *((long*) value1) - *((long*) value2);
        default:
            return 0;
    }
}

type_t type_parse(char* type_name) {
    if (strcmp(type_name, "INT") == 0) {
        return INT;
    } else if (strcmp(type_name, "STR") == 0) {
        return STR;
    } else if (strcmp(type_name, "DBL") == 0){
        return DBL;
    } else if (strcmp(type_name, "LLNG") == 0){
        return LLNG;
    } else {
        return -1;
    }
}

void* value_parse(type_t type, char* literal) {
    void* value;

    switch(type) {
        case INT:
            value = malloc(sizeof(int));
            *((int*) value) = atoi(literal);
            break;
        case STR:
            value = malloc((strlen(literal) + 1) * sizeof(char));
            strcpy(value, literal);
            break;
        case DBL:
            value = malloc(sizeof(double));
            *((double*) value) = atof(literal);
            break;
        case LLNG:
            value = malloc(sizeof(long));
            *((long*) value) = atol(literal);
            break;
        default:
            value = NULL;
            break;
    }

    return value;
}


char *type_to_str(type_t type) {
    switch(type) {
        case INT:
            return strdup("INT");
        case STR:
            return strdup("STR");
        case DBL:
            return strdup("DBL");
        case LLNG:
            return strdup("LLNG");
        default:
            return strdup("Invalid");
    }
}
