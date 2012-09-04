CFLAGS=-I$(shell pg_config --includedir-server)

all: pgstattmpstat

pgstattmpstat: pgstattmpstat.c
