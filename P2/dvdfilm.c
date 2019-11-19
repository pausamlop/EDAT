#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int filmremove(SQLHDBC dbc, int film_id);

int main(int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret; /* ODBC API return status */

    /* Chek number of arguments */
    if (argc != 3){
      fprintf(stderr, "Error in the input parameters: dvdfilm remove <film_id>\n");
      exit(1);
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Check arguments */
    if (strcmp(argv[1], "remove") == 0){
      /* assign parameters to variables */
      int film_id = atoi(argv[2]) ;
      filmremove(dbc, film_id);
    }
    else{
      fprintf(stderr, "Error in the input parameters: second argument has to be remove\n");
    }

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int filmremove(SQLHDBC dbc, int film_id){
  SQLHSTMT stmt;

  char query1[1000];
  char query2[1000];
  char query3[1000];
  char query4[1000];
  char query5[1000];
  char query6[1000];

  sprintf(query1, "DELETE FROM film_category WHERE film_id = %d; ", film_id);
  sprintf(query2, "DELETE FROM film_actor WHERE film_id = %d;", film_id);
  sprintf(query3, "DELETE FROM payment WHERE rental_id IN (SELECT rental_id FROM rental, inventory, film WHERE film.film_id = inventory.film_id AND inventory.inventory_id = rental.inventory_id AND film.film_id = %d); ", film_id);
  sprintf(query4, "DELETE FROM rental WHERE rental_id IN (SELECT rental_id FROM rental, inventory, film WHERE film.film_id = inventory.film_id AND inventory.inventory_id = rental.inventory_id AND film.film_id = %d); ", film_id);
  sprintf(query5, "DELETE FROM inventory WHERE film_id = %d; ", film_id);
  sprintf(query6, "DELETE FROM film WHERE film_id = %d; ", film_id);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query1, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query2, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query3, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query4, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query5, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query6, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  printf("Film %d has been removed\n", film_id);

  return 1;





}
