#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <libpq-fe.h>


#define x 300
#define y 300
#define z 300


int outLen = 0, inLen = 0;


void read(char ***table, char* data)
{
  FILE *file = fopen(data, "r");

  int i, j;
  char *c = (char*) malloc(y * sizeof(char)),
       **helpTable = (char**) malloc(x * sizeof(char*)),
       *pch;

  for (i = 0; i < z; i++)
  {
    if (fgets(c, x, file) == NULL) break;
       
    table[i] = (char**) malloc(x * sizeof(char*));
    helpTable[i] = (char*) malloc((strlen(c) + 1) * sizeof(char));

    strcpy(helpTable[i], c);

    pch = strtok(helpTable[i], ";");


    for (j = 0; pch != NULL; j++)
    {
      table[i][j] = (char*) malloc((strlen(helpTable[i]) + 1) * sizeof(char));
      strcpy(table[i][j], pch);
      pch = strtok(NULL, ";");
    }
  }

  outLen = i;
  inLen = j;

  fclose(file);

  free(c);
}


/*void doSQL(PGconn *conn, char *command)
{
  PGresult *result;

  printf("%s\n", command);

  result = PQexec(conn, command);
  printf("status is     : %s\n", PQresStatus(PQresultStatus(result)));
  printf("#rows affected: %s\n", PQcmdTuples(result));
  printf("result message: %s\n", PQresultErrorMessage(result));

  switch(PQresultStatus(result))
  {
    case PGRES_TUPLES_OK:
    {
      int n = 0, m = 0;
      int nrows   = PQntuples(result);
      int nfields = PQnfields(result);

      printf("number of rows returned   = %d\n", nrows);
      printf("number of fields returned = %d\n", nfields);

      for(m = 0; m < nrows; m++)
      {
        for(n = 0; n < nfields; n++)  printf(" %s = %s", PQfname(result, n),PQgetvalue(result,m,n));
        printf("\n");
      }
    }
  }

  PQclear(result);
}*/


int main(int argc, char* argv[])
{
  if (argc == 1)
  {
    printf("Podaj jako argument plik z przykładową tabelą\n");
    return 0;
  }
  else if (argc > 2)
  {
    printf("Program obsługuje tylko jeden argument, reszta argumentów zoastanie pominięta\n");
  }

  int i, j;
  char ***tab = (char***) malloc(z * sizeof(char**)),
       *pch = strrchr(argv[1], '.'),
       *nazwa = argv[1];

  read(tab, argv[1]);

  for (i = 0; i < outLen; i++)
  {
    for (j = 0; j < inLen; j++)
    {
      printf("%s\n", tab[i][j]);
    }
    printf("\n");
  }

  for (i = strlen(argv[1]) - 4; i <= strlen(argv[1]); i++) nazwa[i] = 0;

  printf("%s\n", nazwa); 

  return 0;
}
