#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <libpq-fe.h>


#define X 500
#define Y 500
#define Z 500


int outLen = 0, inLen = 0;


void read(char ***table, char* data)
{
  FILE *file = fopen(data, "r");

  int i, j;
  char *c = (char*) malloc(Y * sizeof(char)),
       **helpTable = (char**) malloc(X * sizeof(char*)),
       *pch;

  for (i = 0; i < Z; i++)
  {
    if (fgets(c, X, file) == NULL) break;
       
    table[i] = (char**) malloc(X * sizeof(char*));
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
  char ***tab = (char***) malloc(Z * sizeof(char**)),
       **commands = (char**) malloc((Y + 2) * sizeof(char*)),
       *pch = strrchr(argv[1], '.'),
       *nazwa = argv[1];

  read(tab, argv[1]);

  for (i = strlen(argv[1]) - 4; i <= strlen(argv[1]); i++) nazwa[i] = 0;

  printf("%s\n", nazwa);

  commands[0] = (char*) malloc((X + 1) * sizeof(char));
  strcpy(commands[0], "DROP TABLE ");
  strcat(commands[0], nazwa);

  commands[outLen + 1] = (char*) malloc((X + 1) * sizeof(char));
  strcpy(commands[outLen + 1], "SELECT * FROM ");
  strcat(commands[outLen + 1], nazwa);

  for (i = 0; i < outLen; i++)
  {
    commands[i + 1] = (char*) malloc((X + 1) * sizeof(char));

    for (j = 0; j < inLen; j++)
    {
      if (i == 0)
      {
        if (j == 0)
        {
          strcpy(commands[i + 1], "CREATE TABLE ");
          strcat(commands[i + 1], nazwa);
          strcat(commands[i + 1], "(");
          strcat(commands[i + 1], tab[i][j]);
          strcat(commands[i + 1], " INTEGER PRIMARY KEY, ");
        }
        else if (j == inLen - 1)
        {
          strncat(commands[i + 1], tab[i][j], strlen(tab[i][j]) - 1);
          strcat(commands[i + 1], " VARCHAR(30))");
        }
        else
        {
          strcat(commands[i + 1], tab[i][j]);
          strcat(commands[i + 1], " VARCHAR(30), ");
        }
      }
      else
      {
        if (j == 0)
        {
          strcpy(commands[i + 1], "INSERT INTO ");
          strcat(commands[i + 1], nazwa);
          strcat(commands[i + 1], " values(");
          strcat(commands[i + 1], tab[i][j]);
          strcat(commands[i + 1], ", '");
        }
        else if (j == inLen - 1)
        {
          strncat(commands[i + 1], tab[i][j], strlen(tab[i][j]) - 1);
          strcat(commands[i + 1], "')");
        }
        else
        {
          strcat(commands[i + 1], tab[i][j]);
          strcat(commands[i + 1], "', '");
        }
      }
    }
  }

  for (i = 0; i < outLen + 2; i++) printf("%s\n", commands[i]);
  
  return 0;
}
