#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


int customer(SQLHDBC dbc, char* fname, char* lname);
int film_actors(SQLHDBC dbc, int film_id);
int film(SQLHDBC dbc, char* title);
int rent(SQLHDBC dbc, int customer_id, char* init_date, char* end_date);
int recommend(SQLHDBC dbc, int customer_id);


int main(int argc, char** argv) {
    SQLHENV env;
    SQLHDBC dbc;
    SQLRETURN ret; /* ODBC API return status */
    SQLUSMALLINT i;

    /* Chek number of arguments */
    if (argc < 2){
      fprintf(stderr, "Error in the input parameters: too few arguments\n");
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
        fprintf(stderr, "Error in the input parameters: dvdreq customer -n <first_name> -a <last_name>\n");
        exit(1);
      }

      /* assign parameters to variables */
      char fname[512], lname[512];
      for (i = 2; i < argc; i++){
        if (strcmp(argv[i], "-n") == 0) strcpy(fname, argv[++i]);
        else if (strcmp(argv[i], "-a") == 0) strcpy(lname,argv[++i]);
        else {
          fprintf(stderr, "Parameter %d is incorrect: dvdreq customer -n <first_name> -a <last_name>\n", i);
          exit(1);
        }
    }
      customer(dbc, fname, lname);
    }

    else if (strcmp(argv[1], "film") == 0){
      if (argc != 3){
        fprintf(stderr, "Error in the input parameters: dvdreq film <title>\n");
        exit(1);
      }

      char title[512];
      strcpy(title, argv[2]);
      film (dbc, title);
    }

    else if (strcmp(argv[1], "rent") == 0){
      if (argc != 5){
        fprintf(stderr, "Error in the input parameters: dvdreq rent <customer_id> <init_date> <end_date>\n");
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
        fprintf(stderr, "Error in the input parameters: dvdreq recommend <customer_id>\n");
        exit(1);
      }

      int customer_id;
      customer_id = atoi(argv[2]);
      recommend (dbc, customer_id);
     }

     else{
       fprintf(stderr, "Error in the input parameters: second argument has to be customer, film, rent or recommend\n");
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
  printf("%10s %10s %10s %20s %20s %13s %10s %15s\n","customer_id","first_name","last_name","registration_date","address","district","city","postal_code");
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      ret = SQLGetData(stmt, 1, SQL_C_SLONG, &customer_id, sizeof(customer_id), NULL);
      ret = SQLGetData(stmt, 2, SQL_C_CHAR, customer_first_name, sizeof(customer_first_name), NULL);
      ret = SQLGetData(stmt, 3, SQL_C_CHAR, customer_last_name, sizeof(customer_last_name), NULL);
      ret = SQLGetData(stmt, 4, SQL_C_CHAR, registration_date, sizeof(registration_date), NULL);
      ret = SQLGetData(stmt, 5, SQL_C_CHAR, address, sizeof(address), NULL);
      ret = SQLGetData(stmt, 6, SQL_C_CHAR, district, sizeof(district), NULL);
      ret = SQLGetData(stmt, 7, SQL_C_CHAR, city, sizeof(city), NULL);
      ret = SQLGetData(stmt, 8, SQL_C_SLONG, &postal_code, sizeof(postal_code), NULL);
      printf(" %10d %10s %10s %20s %20s %13s %10s %15d\n", customer_id, customer_first_name,customer_last_name, registration_date, address, district, city,postal_code);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  return 1;
}

int film_actors(SQLHDBC dbc, int film_id){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  char fname[100];
  char lname[100];

  sprintf(query, "SELECT DISTINCT actor.first_name, actor.last_name FROM actor, film, film_actor WHERE film.film_id = film_actor.film_id AND actor.actor_id = film_actor.actor_id AND film.film_id = %d;", film_id);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  printf("Actors:\n");
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_CHAR, fname, sizeof(fname), NULL);
    ret = SQLGetData(stmt, 2, SQL_C_CHAR, lname, sizeof(lname), NULL);
    printf("   - %s %s\n", fname, lname);
  }
  printf("\n");

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  return 1;
}


int film(SQLHDBC dbc, char* title){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];
  int film_id;
  char film_title[100];
  char release_year[100];
  int lenght;
  char language[100];
  char description[512];

  sprintf(query, "SELECT film.film_id, film.title, film.release_year, film.length, language.NAME AS language_name, film.description FROM film, language WHERE  film.title LIKE '%c%s%c' AND film.language_id = language.language_id;",'%', title, '%');
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);


  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_SLONG, &film_id, sizeof(film_id), NULL);
    ret = SQLGetData(stmt, 2, SQL_C_CHAR, film_title, sizeof(film_title), NULL);
    ret = SQLGetData(stmt, 3, SQL_C_CHAR, release_year, sizeof(release_year), NULL);
    ret = SQLGetData(stmt, 4, SQL_C_SLONG, &lenght, sizeof(lenght), NULL);
    ret = SQLGetData(stmt, 5, SQL_C_CHAR, language, sizeof(language), NULL);
    ret = SQLGetData(stmt, 6, SQL_C_CHAR, description, sizeof(description), NULL);
    printf("%10s %20s %15s %10s %12s\t%-1s\n", "film_id", "title", "release_year", "lenght", "language", "description");
    printf("%10d %20s %15s %10d %12s\t%-1s\n", film_id, film_title, release_year, lenght, language, description);
    film_actors(dbc, film_id);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  return 1;
}

int rent(SQLHDBC dbc, int customer_id, char* init_date, char* end_date){
  SQLHSTMT stmt;
  SQLRETURN ret;
  char query[1000];

  int customer_customer_id;
  int rental_rental_id;
  char rental_rental_date[512];
  int film_film_id;
  char film_title[512];
  int staff_staff_id;
  char staff_first_name[512];
  char staff_last_name[512];
  int store_store_id;
  double payment_amount;

  sprintf(query, "SELECT customer.customer_id, rental.rental_id, rental.rental_date, film.film_id, film.title, staff.staff_id, staff.first_name, staff.last_name, store.store_id, payment.amount FROM rental, film, staff, store, payment, customer, inventory WHERE customer.customer_id = %i AND customer.customer_id = rental.customer_id AND rental.rental_date > '%s' AND rental.rental_date < '%s' AND rental.inventory_id = inventory.inventory_id AND film.film_id = inventory.film_id AND staff.staff_id = rental.staff_id AND store.manager_staff_id = staff.staff_id AND rental.rental_id = payment.rental_id ORDER BY rental.rental_date ASC; ",customer_id, init_date, end_date );

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  printf("%12s\t%12s\t%20s\t%8s\t%25s\t%9s\t%10s\t%10s\t%9s\t%6s\n","customer_id", "rental_id", "rental_date", "film_id", "title", "staff_id", "first_name", "last_name", "store_id", "amount");
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      ret = SQLGetData(stmt, 1, SQL_C_SLONG, &customer_customer_id, sizeof(customer_customer_id), NULL);
      ret = SQLGetData(stmt, 2, SQL_C_SLONG, &rental_rental_id, sizeof(rental_rental_id), NULL);
      ret = SQLGetData(stmt, 3, SQL_C_CHAR, rental_rental_date, sizeof(rental_rental_date), NULL);
      ret = SQLGetData(stmt, 4, SQL_C_SLONG, &film_film_id, sizeof(film_film_id), NULL);
      ret = SQLGetData(stmt, 5, SQL_C_CHAR, film_title, sizeof(film_title), NULL);
      ret = SQLGetData(stmt, 6, SQL_C_SLONG, &staff_staff_id, sizeof(staff_staff_id), NULL);
      ret = SQLGetData(stmt, 7, SQL_C_CHAR, staff_first_name, sizeof(staff_first_name), NULL);
      ret = SQLGetData(stmt, 8, SQL_C_CHAR, staff_last_name, sizeof(staff_last_name), NULL);
      ret = SQLGetData(stmt, 9, SQL_C_SLONG, &store_store_id, sizeof(store_store_id), NULL);
      ret = SQLGetData(stmt, 10, SQL_C_DOUBLE, &payment_amount, sizeof(payment_amount), NULL);

      printf("%12d\t%12d\t%20s\t%8d\t%25s\t%9d\t%10s\t%10s\t%9d\t%6.2f\n", customer_customer_id,rental_rental_id,rental_rental_date,film_film_id,film_title,staff_staff_id,staff_first_name,staff_last_name,store_store_id,payment_amount );
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  return 1;

}


int recommend(SQLHDBC dbc, int customer_id){
  SQLHSTMT stmt;
  SQLRETURN ret;
  int category_id;
  char category_name[512];
  char query_fav_cat[1000];
  int film_id;
  char title[512];
  char query[1000];

  sprintf(query_fav_cat, "SELECT category.category_id, category.name FROM rental, inventory, film, film_category, category WHERE  rental.inventory_id = inventory.inventory_id AND inventory.film_id = film.film_id AND film.film_id = film_category.film_id AND film_category.category_id = category.category_id AND rental.customer_id = %d GROUP  BY category.category_id ORDER  BY Count(*) DESC LIMIT  1;", customer_id);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query_fav_cat, SQL_NTS);

  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    ret = SQLGetData(stmt, 1, SQL_C_SLONG, &category_id, sizeof(category_id), NULL);
    ret = SQLGetData(stmt, 2, SQL_C_CHAR, category_name, sizeof(category_name), NULL);
  }
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  sprintf(query, "SELECT f.film_id, title FROM film, (SELECT film.film_id, Count(*) AS times_rented FROM rental, inventory, film, film_category WHERE rental.inventory_id = inventory.inventory_id AND inventory.film_id = film.film_id AND film.film_id = film_category.film_id AND category_id = %d GROUP  BY film.film_id) AS f WHERE  f.film_id NOT IN (SELECT film.film_id FROM film, rental, inventory, film_category WHERE  rental.customer_id = %d AND rental.inventory_id = inventory.inventory_id AND inventory.film_id = film.film_id AND film.film_id = film_category.film_id AND category_id = %d) AND f.film_id = film.film_id ORDER  BY times_rented DESC LIMIT  3;", category_id, customer_id, category_id);

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  printf("%10s %20s %20s\n", "film_id", "title", "category");
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
      ret = SQLGetData(stmt, 1, SQL_C_SLONG, &film_id, sizeof(film_id), NULL);
      ret = SQLGetData(stmt, 2, SQL_C_CHAR, title, sizeof(title), NULL);
      printf("%10d %20s %20s\n", film_id, title,category_name);
  }

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);
  return 1;
}
