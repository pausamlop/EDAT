/*
    Project: EDAT Lab 3 test program
    File:    table.h
    Author:  Laura de Paz and Paula Samper
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
  FILE *f;
  int ncols;
  type_t *types;
  long first_pos;
  char *record;
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
   if(!path || ncols<=0 || *types<0 || *types>1) return 0;

   FILE *f = fopen(path, "w+");
   if(!f) return 0;
   fwrite(&ncols, sizeof(int), 1, f);
   fwrite(types, sizeof(type_t), ncols, f);
   fclose(f);
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
  if(!path) return NULL;

  table_t *table = NULL;
  if (!(table = (table_t*)malloc(sizeof(table_t)))) return NULL;

  table->types = 0;
  table->record = NULL;

  table->f = fopen(path, "r+");
  if(!(table->f)){
    table_close(table); //free(table);
    return NULL;
  }
  fread(&(table->ncols), sizeof(int), 1, table->f);
  table->types = (type_t *)malloc(table->ncols*sizeof(type_t));
  fread(table->types, sizeof(table_t), table->ncols, table->f);
  table->first_pos = ftell(table->f);
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
void table_close(table_t* table){
  if(!table) return;

  // for (int i = 0; i < table->ncols; i++){
  //   free(&table->types[i]);
  // }
  if (table->record) free(table->record);
  free(table->types);
  fclose(table->f);
  free(table);

  return;
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
  if(!table) return -1;
  return table->ncols;
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
  if(!table) return NULL;
  return table->types;
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
  if(!table) return -1L;
  return table->first_pos;
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
  if(!table) return -1L;
  fseek(table->f,0,SEEK_END);
  return ftell(table->f);
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
  int len;
  if(!t || pos < 0) return -1L;

  fseek(table->f, pos, SEEK_SET);
  fread(&len, sizeof(int), 1, table->f);

  table->record = (char *)malloc(len);
  if (!table->record) return -1L;
  fread(table->record, sizeof(char), len, table->f);

  return ftell(table->f);
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
  void *aux = NULL;
  int i;

  if(!table || col < 0) return NULL;
  if(col >= table->ncols || !table->record) return NULL;

  for (i = 0; i < col; i++){
    aux += value_length(table->types[i], aux + table->record);
  }

  return aux;
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
  long last_pos;
  int len, i;

  if(!t || !values) return 0;
  size_t aux[t->ncols];

  last_pos = table_last_pos(t);
  for (i = 0; i < t->ncols; i++){
    aux[i] = value_lenght(t->types[i], (void *)values[i]);
    len += aux[i];
  }
  fwrite(&len, sizeof(int), 1, t->f);
  for (i = 0; i < t->ncols; i++){
    fwrite(values[i], sizeof(types[i]), aux[i], t->f);
  }

  return 1;
}

