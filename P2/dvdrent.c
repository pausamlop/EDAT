#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int new(SQLHDBC dbc, int customer_id, int film_id, int staff_id, int store_id, double amount);
int remove_rental(SQLHDBC dbc, int rent_id);



int main(int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret; /* ODBC API return status */

    /* Chek number of arguments */
    if (argc < 2){
      fprintf(stderr, "Error in the input parameters\n");
      exit(1);
    }

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }


    /* Check arguments */
    if (strcmp(argv[1], "new") == 0){
      /* Check number of input parameters */
      if (argc != 7){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      /* assign parameters to variables */
      int customer_id, film_id,staff_id, store_id;
      double amount;
      customer_id = atoi(argv[2]);
      film_id = atoi(argv[3]);
      staff_id = atoi(argv[4]);
      store_id = atoi(argv[5]);
      amount = atof(argv[6]);

      new(dbc, customer_id, film_id,staff_id, store_id, amount);
    }

    else if (strcmp(argv[1], "remove") == 0){
      if (argc != 3){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      int rent_id;
      rent_id = atoi(argv[2]);;
      remove_rental (dbc, rent_id);
    }


    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int check_customer(SQLHDBC dbc, int customer_id){
  SQLHSTMT stmt;
  char query[1000];

  sprintf (query, "SELECT customer_id FROM customer WHERE  customer_id = %d", customer_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  if (SQL_SUCCEEDED(SQLFetch(stmt))) {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 1;
  }
  else {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 0;
  }
}

int check_film(SQLHDBC dbc, int film_id){
  SQLHSTMT stmt;
  char query[1000];

  sprintf (query, "SELECT film_id FROM film WHERE film_id = %d", film_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  if (SQL_SUCCEEDED(SQLFetch(stmt))) {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 1;
  }
  else {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 0;
  }
}

int check_staff(SQLHDBC dbc, int staff_id){
  SQLHSTMT stmt;
  char query[1000];

  sprintf (query, "SELECT staff_id FROM staff WHERE staff_id = %d", staff_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  if (SQL_SUCCEEDED(SQLFetch(stmt))) {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 1;
  }
  else {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 0;
  }
}

int check_store(SQLHDBC dbc, int store_id){
  SQLHSTMT stmt;
  char query[1000];

  sprintf (query, "SELECT store_id FROM store WHERE store_id = %d", store_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  if (SQL_SUCCEEDED(SQLFetch(stmt))) {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 1;
  }
  else {
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return 0;
  }
}


int new(SQLHDBC dbc, int customer_id, int film_id, int staff_id, int store_id, double amount){
  SQLHSTMT stmt;
  SQLRETURN ret;

  int unrented_dvd;
  char query_dvd[512];
  int new_rental_id;
  char query_rental[512];
  int new_payment_id;
  char query_payment[512];

  char query_insert_rental[512];
  char query_insert_payment[512];


  /* Check if the arguments exists in our database */
  if (check_customer(dbc, customer_id) == 0){
    fprintf(stderr, "Cutomer_id %d does not exist\n", customer_id);
    return(0);
  }
  if (check_film(dbc, film_id) == 0){
    fprintf(stderr, "Film_id %d does not exist\n", film_id);
    return(0);
  }
  if (check_store(dbc, store_id) == 0){
    fprintf(stderr, "Store_id %d does not exist\n", store_id);
    return(0);
  }
  if (check_staff(dbc, staff_id) == 0){
    fprintf(stderr, "Staff_id %d does not exist\n", staff_id);
    return(0);
  }

  /* Find unrented dvd from inventory */
  sprintf(query_dvd, "SELECT DISTINCT inventory.inventory_id FROM inventory, rental WHERE inventory.film_id = %d AND inventory.inventory_id NOT IN (SELECT inventory.inventory_id FROM inventory, rental WHERE inventory.film_id = %d AND inventory.inventory_id = rental.inventory_id AND rental.return_date IS NULL) ORDER BY inventory.inventory_id LIMIT 1;", film_id, film_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_dvd, SQL_NTS);

  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_SLONG, &unrented_dvd, sizeof(unrented_dvd), NULL);
  }
  printf("unrented_dvd = %d\n", unrented_dvd);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  /* Find last rental_id */
  sprintf(query_rental, "SELECT Max(rental_id) + 1 FROM rental;");
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_rental, SQL_NTS);

  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_SLONG, &new_rental_id, sizeof(new_rental_id), NULL);
  }
  printf("new_rental_id = %d\n", new_rental_id);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  /* Find last payment_id */
  sprintf(query_payment, "SELECT Max(payment_id) + 1 FROM payment;");
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_payment, SQL_NTS);

  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_SLONG, &new_payment_id, sizeof(new_payment_id), NULL);
  }
  printf("new_payment_id = %d\n", new_payment_id);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  /* Insert rental */
  sprintf(query_insert_rental, "INSERT INTO rental VALUES (%d, Now(), %d, %d, NULL, %d, Now());", new_rental_id, unrented_dvd, customer_id, staff_id);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_insert_rental, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  /* Insert payment */
  sprintf(query_insert_payment, "INSERT INTO payment VALUES (%d, %d, %d, %d, %f, Now()) ", new_payment_id, customer_id, staff_id, new_rental_id, amount);
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_insert_payment, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  printf("%s\n", query_insert_payment);

  return 1;
}


int remove_rental(SQLHDBC dbc, int rent_id){
  SQLHSTMT stmt;

  char payment[512];
  char rental[512];

  sprintf(payment, "DELETE FROM payment WHERE rental_id = %d;", rent_id);
  sprintf(rental, "DELETE FROM rental WHERE  rental_id = %d;", rent_id);

  /* Delete payment */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) payment, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  /* Delete rental */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) rental, SQL_NTS);
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);


  return 1;
}
