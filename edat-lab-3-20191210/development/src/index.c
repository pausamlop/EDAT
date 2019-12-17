#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"

#define NO_ENCONTRADO -2

struct irecord_ {
    int key;
    int n_pos;
    long *pos;
};

struct index_ {
    irecord_t **records;
    int n_records;
    int type;
    char *path;
};


/*AUXILIARY FUNCTIONS*/
int bbin(irecord_t **tabla, int P, int U, int clave, int *ppos){

    int medio;

    if(tabla == NULL || P > U || clave < 0)
    return -1;

    if(P > U){
        *ppos = NO_ENCONTRADO;
        return -1;
    }

    medio = (P + U) / 2;

    if(tabla[medio]->key == clave){
        *ppos = medio;
        return 1;
    }

    if(clave < tabla[medio]->key)
        return bbin(tabla, P, medio - 1, clave, ppos) + 1;

    else
        return bbin(tabla, medio + 1, U, clave, ppos) + 1;

}
/*
Function: int index_create(char *path, int type)

Creates a file for saving an empty index. The index is initialized
to be of the specific tpe (in the basic version this is always INT)
and to contain 0 entries.

Parameters:
path:  the file where the index is to be created
type:  the type of the index (always INT in this version)

Returns:
1:   index created
0:   parameter error or file creation problem. Index not created.
*/
int index_create(char *path, type_t type) {

    FILE *file = NULL;
    int n_records = 0;

    file = fopen(path, "w");

    if(file == NULL)
        return 0;

    fwrite(&type, sizeof(int), 1, file);
    fwrite(&n_records, sizeof(int),1,file);

    fclose(file);

    return 1;
}



/*
Opens a previously created index: reads the contents of the index
in an index_t structure that it allocates, and returns a pointer to
it (or NULL if the files doesn't exist or there is an error).

NOTE: the index is stored in memory, so you can open and close the
file in this function. However, when you are asked to save the
index, you will not be given the path name again, so you must store
in the structure either the FILE * (and in this case you must keep
the file open) or the path (and in this case you will open the file
again).

Parameters:
path:  the file where the index is

Returns:
pt:   index opened
NULL: parameter error or file opening problem. Index not opened.

*/
index_t* index_open(char* path) {

    index_t *idx = NULL;
    FILE *file = NULL;
    int i;
    int key = 0;
    int end;
    long pos = 0;

    file = fopen(path, "a+");

    if(file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END);
    end = ftell(file);
    fseek(file, 0, SEEK_SET);

    idx = (index_t*)malloc(sizeof(index_t));

    if(idx == NULL)
        return NULL;

    idx->path = path;
    idx->records = NULL;

    fread(&(idx->type), sizeof(int), 1, file);
    fread(&(idx->n_records), sizeof(int),1, file);

    if(idx->n_records == 0)
        return idx;

    idx->records = (irecord_t**)malloc(idx->n_records * sizeof(irecord_t*));

    for(i = 0; i < idx->n_records; i++)
      idx->records[i] = NULL;

    for(i = ftell(file); i < end; i = ftell(file)){
        fread(&key, sizeof(int), 1, file);
        fread(&pos, sizeof(long), 1, file);
        index_put(idx, key, pos);
    }

    fclose(file);

    return idx;
}


/*
int index_save(index_t* index);

Saves the current state of index in the file it came from. Note
that the name of the file in which the index is to be saved is not
given.  See the NOTE to index_open.

Parameters:
index:  the index the function operates upon

Returns:
1:  index saved
0:  error saving the index (cound not open file)

*/
int index_save(index_t* idx) {

    FILE *file = NULL;
    int i,j;

    if(index_create(idx->path, idx->type) == 0) return 0;

    file = fopen(idx->path, "a+");

    if(file == NULL)
        return 0;

    for(i = 0; i < idx->n_records; i++){
        fwrite(&(idx->records[i]->key), sizeof(int), 1, file);
        fwrite(&(idx->records[i]->pos), sizeof(int), 1, file);

        for(j = 0; j < idx->records[i]->n_pos; j++){
            fwrite(&(idx->records[i]->pos[j]),sizeof(long), 1, file);
        }
    }

    fclose(file);

    return 1;
}


/*
Function: int index_put(index_t *index, int key, long pos);

Puts a pair key-position in the index. Note that the key may be
present in the index or not... you must manage both situation. Also
remember that the index must be kept ordered at all times.

Parameters:
index:  the index the function operates upon
key: the key of the record to be indexed (may or may not be already
present in the index)
pos: the position of the corresponding record in the table
file. This is the datum that we will want to recover when we
search for the key.

Return:
n>0:  after insertion the file now contains n unique keys
0:    error inserting the key

*/
int index_put(index_t *idx, int key, long pos) {

    int i =0;
    int ppos;

    if(idx->n_records == 0){

        idx->n_records++;
        idx->records = (irecord_t**)malloc(idx->n_records * sizeof(irecord_t*));

        for(i = 0; i < idx->n_records; i++)
          idx->records[i] = NULL;

        idx->records[0] = (irecord_t *)malloc(sizeof(irecord_t));

        if(idx->records[0]->pos == NULL){
            free(idx->records);
            return -1;
        }

        idx->records[0]->key = key;
        idx->records[0]->n_pos = 0;

        idx->records[0]->pos = (long *)malloc(sizeof(long));
        *(idx->records[0]->pos) = pos;
        idx->records[0]->n_pos++;
    }

    else{

        int m = bbin(idx->records, 0, idx->n_records-1, key, &ppos);

        if(ppos == NO_ENCONTRADO){
            idx->n_records++;
            idx->records = realloc(idx->records, idx->n_records * sizeof(irecord_t *));

            idx->records[idx->n_records - 1] = (irecord_t *)malloc(sizeof(irecord_t));

            if(idx->records[idx->n_records - 1] == NULL) return -1;

            for(i = idx->n_records - 1; i > m; i--)
                idx->records[i] = idx->records[i - 1];

            idx->records[m]->pos = (long *)malloc(sizeof(long));

            if(idx->records[m]->pos == NULL){
                free(idx->records[idx->n_records - 1]);
                return -1;
            }

            idx->records[m]->key = key;
            *(idx->records[m]->pos) = pos;
            idx->records[m]->n_pos = 1;

        }

        else if (ppos > 0){
            idx->records[ppos]->n_pos++;
            idx->records[ppos]->pos = (long *)realloc(idx->records[ppos]->pos, idx->records[ppos]->n_pos * sizeof(long));
            idx->records[ppos]->pos[idx->records[ppos]->n_pos-1] = pos;
        }
    }

    return idx->n_records;
}


/*
Function: long *index_get(index_t *index, int key, int* nposs);

Retrieves all the positions associated with the key in the index.

Parameters:
index:  the index the function operates upon
key: the key of the record to be searched
nposs: output paramters: the number of positions associated to this key

Returns:

pos: an array of *nposs long integers with the positions associated
to this key
NULL: the key was not found

NOTE: the parameter nposs is not an array of integers: it is
actually an integer variable that is passed by reference. In it you
must store the number of elements in the array that you return,
that is, the number of positions associated to the key. The call
will be something like this:

int n
long **poss = index_get(index, key, &n);

for (int i=0; i<n; i++) {
Do something with poss[i]
}

ANOTHER NOTE: remember that the search for the key MUST BE DONE
using binary search.

FURTHER NOTE: the pointer returned belongs to this module. The
caller guarantees that the values returned will not be changed.

*/
long *index_get(index_t *idx, int key, int* nposs) {

    int ppos;

    int m = bbin(idx->records, 0, idx->n_records - 1, key, &ppos);

    if(ppos == NO_ENCONTRADO){
        printf("Not found\n");
        return NULL;
    }
    else{
        *nposs = idx->records[ppos]->n_pos;
        return idx->records[ppos]->pos;
    }
}


/*
Closes the index by freeing the allocated resources. No operation
on the index will be possible after calling this function.

Parameters:
index:  the index the function operates upon

Returns:
Nothing

NOTE: This function does NOT save the index on the file: you will
have to call the function index_save for this.
*/
void index_close(index_t *idx) {

    if(idx == NULL){
        printf("ERROR AL LIBERAR INDEX EN index_close\n");
        return;
    }
    if(idx->records){
        for (int i = 0; i < idx->n_records - 1; i++){
            if(idx->records[i]->pos)
                free(idx->records[i]->pos);
            free(idx->records[i]);
        }
        free(idx->records);
    }
    free(idx);
}


/*
Function: long **index_get_order(index_t *index, int n, int* nposs);

Function useful for debugging but that should not be used otherwise:
returns the nth record in the index. DO NOT USE EXCEPT FOR
DEBUGGING. The test program uses it.

Parameters:
index:  the index the function operates upon
n: number of the record to be returned
key: output parameter: the key of the record
nposs: output parameter: the number of positions associated to this key

Returns:

pos: an array of *nposs long integers with the positions associated
to this key
NULL: the key was not found


See index_get for explanation on nposs and pos: they are the same stuff
*/
long *index_get_order(index_t *index, int n, int *key, int* nposs) {

    if (!index || !key || !nposs || n < 0) return NULL;
    if (n > index->n_records) return NULL;

    if (!index->records[n]->pos) return NULL;

    *key = index->records[n]->key;
    *nposs = index->records[n]->n_pos;

    return index->records[n]->pos;
}
