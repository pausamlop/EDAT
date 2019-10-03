# Por favor especifique su grupo y numero de pareja en las dos
# líneas siguientes.
GRUPO=1201
PAREJA=06
# Por lo general no se requiere modificar el fichero
# a parir de esta línea
# export PGDATABASE:=$(GRUPO)_$(PAREJA)
export PGDATABASE:=dvdrental
export PGUSER :=alumnodb
export PGPASSWORD :=alumnodb
export PGCLIENTENCODING:=LATIN9
export PGHOST:=localhost

DBNAME =$(PGDATABASE)
PSQL = psql
CREATEDB = createdb
DROPDB = dropdb --if-exists
PG_DUMP = pg_dump
PG_RESTORE = pg_restore

all: dropdb createdb restore
createdb:
	@echo Creando BBDD
	@$(CREATEDB)
dropdb:
	@echo Eliminando BBDD
	@$(DROPDB) $(DBNAME)
	rm -f *.log
dump:
	@echo creando dumpfile
	@$(PG_DUMP) > $(DBNAME).sql
restore:
	@echo restore data base
	@cat $(DBNAME).sql | $(PSQL)  
shell:
	@echo create psql shell
	@$(PSQL)  

query1:
	@echo query-1: "customer who has rented more movies"
	@cat query1.sql | $(PSQL) | tee query1.log
query2:
	@echo query-2: "rentals per year"
	@cat query2.sql | $(PSQL) | tee query2.log
query3:
	@echo query3 "Ciudades alquiler Bob Fawcett"
	@cat query3.sql | $(PSQL) | tee query3.log
query4:
	@echo query4: "lenguage en el que se producen mas peliculas"
	@cat query4.sql | $(PSQL) | tee query4.log
query5:
	@echo query5: "lenguage en el que se alquilan mas peliculas"
	@cat query5.sql | $(PSQL) | tee query5.log
query6:
	@echo query6: "categoria favorita del cliente que alquila más  peliculas"
	@cat query6.sql | $(PSQL) | tee query6.log

