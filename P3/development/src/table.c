/*
    Project: EDAT Lab 3 test program
    File:    table.h
    Author:  Simone Santini
    Rev.     1.0
    Date:    10/13/2019  (check it out: it's Sunday!
                          and I am here, working for you.
                          Praise to the commmitted professor.)

    File in which YOU (Yes: you!) have to implement the functions defined in
    the file table.h. The functions are defined in this file, but they are
    elft empty or return dummy values. It is up to you to implement them
    to guarantee the functionality expressed in table.h
*/
#include "table.h"
#include "type.h"



/*
    This is the structure that contains the data relative to a table. You
    have to implement it. Keep in mind that all the information about the table
    that the functions have is in this structure, so you must put in there
    all that is needed for the correct work of the functions
*/

struct table_ {
  type_t *type;
  int n_columns;
  FILE* file;
  void* buffer;
};

/*
    void table_create(char* path, int ncols, type_t* types);

    Stores an empty table in a newly created file.

    Note that this function does not return any value nor does it do anything
    in memory. It creates a new file, stores in it a header that indicates
    the number of columns, the types of these columns, and that the table
    has 0 records. Then closes the file and returns.

    Parameters:
    path:    path name (referred to the current directory) of the file
             where the table is to be stored.
    ncols:   number of columns of the table.
    types:   array of ncols elements of type type_t with the type of each
             one of the columns (see type.h and type.c for details on the
             types recognized by the table).

    Returns:
        1: table created
        0: error in creation

    WARNING: if the file specified in path already exists, this function
    erases it and creates a new one. That is, all the data contained in the
    file will be lost.
*/
int table_create(char* path, int ncols, type_t* types) {
  FILE *file;

  file = fopen(path, "w");

  if(file == NULL || types == NULL)
    return 0;

  fwrite(&ncols, sizeof(int), 1, file);
  fwrite(types, sizeof(type_t), ncols, file);

  fclose(file);

  return 1;
}

/*
    table_t* table_open(char* path)

    Opens a table and returns the structure necessary to use it. The file
    <path> must exist for this function to succeeds. This functions
    allocates a table_t structure and fills in the necessary fields so that
    the other functions defined here can operate on the table.

    Parameters:
    path:   path name (referred to the current directory) of the file
            where the table is stored. The file must exist.

    Returns:
    A pointer to a newly allocated table_t structure with the information
    necessary to operate on the table (the table is NOT read in memory), or
    NULL is the file <path> does not exist.

    NOTE: The calling program should not release the structure returned
    by this function. Use table_close instead.
*/
table_t* table_open(char* path) {
  table_t *table;
  FILE *file = fopen(path, "a+");

  if(file == NULL)
    return NULL;

  fseek(file, 0, SEEK_SET);

  table = (table_t *) malloc(sizeof(table_t));

  if(table == NULL)
    return NULL;

  fread(&(table->n_columns), sizeof(int), 1, file);


  table->type = (type_t*)malloc((table->n_columns)*sizeof(type_t));

  if(table->type == NULL)
    return NULL;

  fread(table->type, sizeof(type_t), table->n_columns, file);

  table->buffer = NULL;
  table->file = file;

  return table;
}

/*
    void table_close(table_t* table);

    Closes a table freeing all the resources allocated. This function must
    leave the whole system in the state it was before the table was opened:
    all files closed, all memory released.

    Note that after calling this function, it will no longer be possible
    to operate on the table <table>.

    Parameters:
    table:  The table that we eant to close.

    Returns:
        Nothing
*/
void table_close(table_t* table) {
  if(table == NULL){
    printf("ERROR AL ELIMINAR TABLA EN table_close\n");
    return;
  }

  fclose(table->file);
  free(table->type);
  free(table->buffer);
  free(table);

}

/*
    int table_ncols(table_t* table);

    Returns the number of columns of the table <table>

    Parameters:
    table:  The table on which we want to operate.

    Returns:
        n>0:    The table has n columns
        n<0:    Incorrect parameter
*/
int table_ncols(table_t* table) {
  if(table == NULL)
    return -1;

  return table->n_columns;
}


/*
    type_t *table_types(table_t* table);

    Returns an array containing the types of the columns of the table
    <table>.

    Parameters:
    table:  The table on which we want to operate.

    Returns:
    An array of table_ncols(table) element. Each element is of type type_t,
    and contains the type of the corresponding column. For the definition
    of type_t, see the file type.h/type.c. Returns NULL if the parameter
    is invalid.

    WARNING: The array that is returned is not a copy of the one used
    internally by these functions, but a pointer to the same array. The
    caller should not free the pointer returned by this function nor
    should it modify it in any way.
*/
type_t *table_types(table_t* table) {
  if(table == NULL)
    return NULL;

  return table->type;
}

FILE* table_file(table_t* table){

  if(table == NULL)
    return NULL;

  return table->file;
}

/*
    long table_first_pos(table_t* table);

    Returns the position of the file where the first record begin. Calling
    table_read_record with this value as position will result in reading
    the first record of the table (see the example at the beginning of this
    file.

    Parameters:
    table:  The table on which we want to operate.

    Returns:
    n>0:    the first record begins at position n in the file
    n<0:    error in the parameter
*/
long table_first_pos(table_t* table) {
  int cols = 0;

  if(table == NULL)
    return -1L;

  fseek(table->file, 0, SEEK_SET);

  fread(&cols, sizeof(int), 1, table->file);

  fseek(table->file, cols * sizeof(int), SEEK_CUR);

  return ftell(table->file);
}

/*
    long table_last_pos(table_t* table);

    Returns the last position of the file, that is, the position where a new
    record will be inserted upon calling table_insert_record. Note that
    table_insert_record does not use this function, which is used simply to
    give information to the calling program.

    Parameters:
    table:  The table on which we want to operate.

    Returns:
    n>0:    the new record begins at position n in the file
    n<=0:   error in the parameter
*/
long table_last_pos(table_t* table) {
  if(table == NULL)
    return -1L;

  fseek(table->file, 0, SEEK_END);

  return ftell(table->file);
}

/*
   Returns the position in the file in which the table is currently
   positioned.
*/
long table_cur_pos(table_t* table) {

  if(table == NULL)
    return -1L;

  fseek(table->file, 0, SEEK_CUR);

  return ftell(table->file);
}

/*
    long table_read_record(table_t* table, long pos);

    Reads a record that begins at a given position in the table file.

    Parameters:
    table:  The table on which we want to operate.
    pos:    Position in the file where the record begins. The pos-th byte
            in the file must be the beginning of a record; if it is not, the
            result of the call will be unpredictable.

    Returns:
    n>0:     The next record in the file begins at position n
    n<0:     No record read, we had already reached the end of the file

    Note: this function reads the record, but it returns no data from that
    record. Use the function table_get_col to read the data of the record
    after it has been read.
*/

long table_read_record(table_t* t, long pos) {
  int len = 0;

  if(t == NULL)
    return -1L;

  if(t->buffer != NULL)
    free(t->buffer);

  fseek(t->file, pos, SEEK_SET);
  if (fread(&len, sizeof(int), 1, t->file) < 1)
    return -1;

  t->buffer = (char*)malloc(len);

  if(t->buffer == NULL)
    return -1L;

  fread(t->buffer, sizeof(char), len, t->file);

  return ftell(t->file);
}


/*
    void *table_get_col(table_t* table, int col)

    Returns the pointer to the data contained in the col-th column of the
    record currently in memory. The record must have been previpusly read
    using table_read_record. If no record was read in memory, the result
    will be unpredictable.

    Parameters:
    table:  The table on which we want to operate.
    col:    The column that we want to read, 0<=col<ncol,

    Returns:
    A pointer to the data that is contained in the column, or MULL if the
    column number is out of range. The way the data are interpreted
    depends on the type of the column, as specified by the col-th element
    of the array returned by table_types (see the example at the beginning
    of the file).
*/
void *table_get_col(table_t* table, int col) {
  char* tmp = table->buffer;
  int i;

  if(table == NULL)
    return NULL;

  for(i = 0; i < col; i++)
    tmp += value_length(table->type[i], tmp);

  return (void*)tmp;
}

/*  void table_insert_record(table_t* table, void** values);

    Inserts a record at the end of the file given the pointers to the
    values of each column.

    Parameters:
    table:  The table on which we want to operate.
    values: Array of ncol pointers to the data that are to be stored in the
            record. The element values[i] must be a pointer to a datum of the
            same type as the i-th column of the file. If this constraint is
            not respected, the results will be unpredictable.

    Returns:
    1: inserted OK
    0: error
*/
int table_insert_record(table_t* t, void** values) {
  int tam = 0;
  size_t tam2[64];
  int i;

  if(t == NULL || values == NULL)
    return 0;

  for(i = 0; i < t->n_columns; i++){
      tam2[i] = value_length(t->type[i], values[i]);
      tam += tam2[i];
  }

  fseek(t->file, 0, SEEK_END);
  fwrite(&tam, sizeof(tam), 1, t->file);

  for(i = 0; i < t->n_columns; i++)
    fwrite(values[i], tam2[i], 1, t->file);

  return 1;
}
