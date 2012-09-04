=============
pgstattmpstat
=============

Tiny utility to drump some per-database statistics about your
`pg_stat_tmp/pgstat.stat` file in PostgreSQL. Will show which databases
have how many records in the stats file and how big they are.

It's a quick and dirty hack, so there is really not much of an interface.
It generates a comma separated file that can be loaded into the
database for trivial analysis. Since it only contains oid, if it's
loaded into the same cluster the stats file is from it can be used to
map oids to database names.

Note! The tool (obviously) only counts the number of objects actually
in the statistics files, which is not the same as the number of
objects in the database!


Usage
-----
To generate the statistics file, simply run::

   $ pgstattmpstat /some/where/pgstat.tmp > stat.csv

This data can be loaded into a table defined like::

  postgres=# CREATE TABLE pgstat_stat (dbid oid, numtables int8, tablesize int8, numfunctions int8, functionsize int8);
  postgres=# \copy pgstat_stat FROM stat.csv WITH CSV

The data gets one row per database, and can of course be queried
like any other table, for example::

  postgres=# select sum(numtables),pg_size_pretty(sum(tablesize)::bigint) from pgstat_stat;
     sum   | pg_size_pretty 
  ---------+----------------
   1027892 | 165 MB
  (1 row)

  postgres=# select * from pgstat_stat order by tablesize desc limit 1;
    dbid   | numtables | tablesize | numfunctions | functionsize 
  ---------+-----------+-----------+--------------+--------------
   5984022 |     26237 |   4407816 |            0 |            0
  (1 row)
