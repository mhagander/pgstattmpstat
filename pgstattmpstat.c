#include "postgres.h"
#include "pgstat.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FILE *f;
	Oid currdb;
	int64 tabcount=0, funccount=0;
	int32		format_id;
	PgStat_GlobalStats globalStats;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: pgstattmpstat <filename>\n");
		exit(1);
	}

	f = fopen(argv[1], "rb");
	if (!f)
	{
		perror("could not open file");
		exit(1);
	}

	if (fread(&format_id, 1, sizeof(format_id), f) != sizeof(format_id))
	{
		perror("could not read format id!");
		exit(1);
	}
	if (format_id != PGSTAT_FILE_FORMAT_ID)
	{
		fprintf(stderr, "File format id does not match\n");
		exit(1);
	}

	if (fread(&globalStats, 1, sizeof(globalStats), f) != sizeof(globalStats))
	{
		perror("could not read global stats!");
		exit(1);
	}

	for (;;)
	{
		PgStat_StatDBEntry db;
		PgStat_StatTabEntry tab;
		PgStat_StatFuncEntry func;

		switch (fgetc(f))
		{
			case 'D': /* new database */
				if (fread(&db, 1, offsetof(PgStat_StatDBEntry, tables), f)
					!= offsetof(PgStat_StatDBEntry, tables))
				{
					perror("could not read database start record");
					exit(1);
				}
				currdb = db.databaseid;
				break;
			case 'd': /*end of database */
				printf("%d,%ld,%ld,%ld,%ld\n",
					   currdb,
					   tabcount,
					   tabcount * sizeof(PgStat_StatTabEntry),
					   funccount,
					   funccount * sizeof(PgStat_StatFuncEntry));
				currdb = -1;
				tabcount = 0;
				funccount = 0;
				break;
			case 'T': /* table entry */
				if (fread(&tab, 1, sizeof(PgStat_StatTabEntry), f) != sizeof(PgStat_StatTabEntry))
				{
					perror("could not read table record");
					exit(1);
				}
				tabcount++;
				break;
			case 'F': /* function entry */
				if (fread(&tab, 1, sizeof(PgStat_StatFuncEntry), f) != sizeof(PgStat_StatFuncEntry))
				{
					perror("could not read function record");
					exit(1);
				}
				funccount++;
				break;
			case 'E': /* end of file */
				goto done;
		}
	}
done:
	fclose(f);
}
