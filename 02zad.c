#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>


#define X 500
#define Y 500
#define Z 500


int outLen = 0, inLen = 0, globalGuard = 0;


void readData(char ***table, char *data)
{
  FILE *file = fopen(data, "r");

  int i, j, lenGuard;
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
      lenGuard = j;
      table[i][j] = (char*) malloc((strlen(pch) + 1) * sizeof(char));
      strcpy(table[i][j], pch);
      pch = strtok(NULL, ";");
      if (i == 0) inLen = lenGuard;
      else if (lenGuard > inLen) globalGuard = 1;
    }
    
    if (lenGuard < inLen) globalGuard = 1;
  }

  outLen = i;

  fclose(file);

  free(c);
}


void generateCommands(char **commands, char ***table, char *data)
{
  int i, j;
  char *name = data;

  for (i = strlen(data) - 4; i <= strlen(data); i++) name[i] = 0;

  commands[0] = (char*) malloc((X + 1) * sizeof(char));
  strcpy(commands[0], "DROP TABLE ");
  strcat(commands[0], name);

  commands[outLen + 1] = (char*) malloc((X + 1) * sizeof(char));
  strcpy(commands[outLen + 1], "SELECT * FROM ");
  strcat(commands[outLen + 1], name);

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
          strcat(commands[i + 1], name);
          strcat(commands[i + 1], "(");
          strcat(commands[i + 1], table[i][j]);
          strcat(commands[i + 1], " INTEGER PRIMARY KEY, ");
        }
        else if (j == inLen - 1)
        {
          strncat(commands[i + 1], table[i][j], strlen(table[i][j]) - 1);
          strcat(commands[i + 1], " VARCHAR(30))");
        }
        else
        {
          strcat(commands[i + 1], table[i][j]);
          strcat(commands[i + 1], " VARCHAR(30), ");
        }
      }
      else
      {
        if (j == 0)
        {
          strcpy(commands[i + 1], "INSERT INTO ");
          strcat(commands[i + 1], name);
          strcat(commands[i + 1], " values(");
          strcat(commands[i + 1], table[i][j]);
          strcat(commands[i + 1], ", '");
        }
        else if (j == inLen - 1)
        {
          strncat(commands[i + 1], table[i][j], strlen(table[i][j]) - 1);
          strcat(commands[i + 1], "')");
        }
        else
        {
          strcat(commands[i + 1], table[i][j]);
          strcat(commands[i + 1], "', '");
        }
      }
    }
  }
}


void doSQL(PGconn *conn, char *command)
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
      int n = 0,
          m = 0,
          nrows   = PQntuples(result),
          nfields = PQnfields(result);

      printf("number of rows returned   = %d\n", nrows);
      printf("number of fields returned = %d\n", nfields);

      for(m = 0; m < nrows; m++)
      {
        for(n = 0; n < nfields; n++)
        {
          printf(" %s = %s", PQfname(result, n),PQgetvalue(result,m,n));
        }

        printf("\n");
      }
    }
  }

  PQclear(result);
}


int main(int argc, char* argv[])
{
  if (argc == 1)
  {
    printf("Podaj jako argument plik z przykładową tabelą\n");
    return 0;
  }
  else if (argc > 2)
  {
    printf("Program obsługuje tylko jeden argument, reszta argumentów zostanie pominięta\n");
  }

  int i;
  char ***table = (char***) malloc(Z * sizeof(char**)),
       **commands = (char**) malloc((Y + 2) * sizeof(char*));

  readData(table, argv[1]);

  if (globalGuard == 1)
  {
    printf("\n\n   !!  ERROR  !!   \n\n\n");
    return 0;
  }

  generateCommands(commands, table, argv[1]);

  PGresult *result;
  PGconn   *conn;

  conn = PQconnectdb("host=localhost port=5432 dbname=jbelcik user=jbelcik password=?");
  if (PQstatus(conn) == CONNECTION_OK)
  {
    printf("connection made\n");

    for (i = 0; i < outLen + 2; i++) doSQL(conn, commands[i]);
  }
  else printf("connection failed: %s\n", PQerrorMessage(conn));

  PQfinish(conn);
  
  return 0;
}

