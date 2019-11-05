#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int customer(SQLHDBC dbc, char* fname, char* lname);
int film(SQLHDBC dbc, char* title){
  return 0;
}
int rent(SQLHDBC dbc, int customer_id, char* init_date, char* end_date){
  return 0;
}
int recommend(SQLHDBC dbc, int customer_id){
  return 0;
}


int main(int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret; /* ODBC API return status */
    SQLUSMALLINT i;

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
    if (strcmp(argv[1], "customer") == 0){
      /* Check number of input parameters */
      if (argc != 6){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      /* assign parameters to variables */
      char fname[512], lname[512];
      for (i = 2; i < argc; i++){
        if (strcmp(argv[i], "-n") == 0) strcpy(fname, argv[++i]);
        else if (strcmp(argv[i], "-a") == 0) strcpy(lname,argv[++i]);
        else {
          fprintf(stderr, "Parameter %d is incorrect\n", i);
          exit(1);
        }
    }
      customer(dbc, fname, lname);
    }

    else if (strcmp(argv[1], "film") == 0){
      if (argc != 3){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      char title[512];
      strcpy(title, argv[2]);
      film (dbc, title);
    }

    else if (strcmp(argv[1], "rent") == 0){
      if (argc != 5){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      int customer_id;
      char init_date[100], end_date[100];
      customer_id = atoi(argv[2]);
      strcpy(init_date, argv[3]);
      strcpy(end_date, argv[4]);
      rent(dbc, customer_id, init_date, end_date);
    }

    else if (strcmp(argv[1], "recommend") == 0){
      if (argc != 3){
        fprintf(stderr, "Error in the input parameters\n");
        exit(1);
      }

      int customer_id;
      customer_id = atoi(argv[2]);
      recommend (dbc, customer_id);
     }





    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


int customer(SQLHDBC dbc, char* fname, char* lname) {
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char query[1000];
  /* columns */
  int customer_id;
  char customer_first_name[512];
  char customer_last_name[512];
  char registration_date[512];
  char address[512];
  char district[512];
  char city[512];
  int postal_code;


  sprintf(query, "SELECT ct.customer_id, ct.first_name, ct.last_name, ct.create_date AS registration_date, address, district, city, postal_code FROM address, city, (SELECT customer_id, first_name, last_name, create_date, address_id FROM customer WHERE first_name = '%s' OR last_name = '%s') AS ct WHERE  ct.address_id = address.address_id AND address.city_id = city.city_id;", fname, lname);
  /* printf("%s\n", query); */

  /* Allocate a statement handle */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /* simple query */
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  /* How many columns are there */

  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      ret = SQLGetData(stmt, 1, SQL_C_SLONG, &customer_id, sizeof(customer_id), NULL);
      ret = SQLGetData(stmt, 2, SQL_C_CHAR, customer_first_name, sizeof(customer_first_name), NULL);
      ret = SQLGetData(stmt, 3, SQL_C_CHAR, customer_last_name, sizeof(customer_last_name), NULL);
      ret = SQLGetData(stmt, 4, SQL_C_CHAR, registration_date, sizeof(registration_date), NULL);
      ret = SQLGetData(stmt, 5, SQL_C_CHAR, address, sizeof(address), NULL);
      ret = SQLGetData(stmt, 6, SQL_C_CHAR, district, sizeof(district), NULL);
      ret = SQLGetData(stmt, 7, SQL_C_CHAR, city, sizeof(city), NULL);
      ret = SQLGetData(stmt, 8, SQL_C_SLONG, &postal_code, sizeof(postal_code), NULL);
      printf("%d\t%s\t%s\t%s\t%s\t%s\t%s\t%d\n", customer_id, customer_first_name,customer_last_name, registration_date, address, district, city,postal_code);
  }
  return 1;
}
