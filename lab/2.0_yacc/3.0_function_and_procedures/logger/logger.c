#include <stdio.h>		// standard input output ANSI C
#include <stdlib.h>		// standar libraries ANSI C
#include <string.h>		// libreria per manipolare le stringhe
				// ANSI C
#include <unistd.h>		// per interfacciarsi alle chiamate di
				// sistema POSIX
#include <sys/types.h>		// questa libreria e la prossime 2 servono 
				// a definire lo stato dei file e
				// l'accesso ai file
#include <sys/stat.h>		// per individuare le caratteristiche di
				// un file e poterle manipolare
#include <time.h>		// gestione del tempo nello standard
				// POSIX, times.h è nello standard ANSI


#include "logger.h"


char           *
logString(char *pattern, char *p_value)
{
    char           *res = (char *) calloc(SIZE, sizeof(char));	// nota
								// grandezza 
								// fissa
    sprintf(res, pattern, p_value);
    return res;
}
char           *
logString2(char *pattern, char *p_value, char *p_value2)
{				// nota si può fare dinamica
    char           *res = (char *) calloc(SIZE, sizeof(char));	// nota
								// grandezza 
								// fissa
    sprintf(res, pattern, p_value, p_value2);
    return res;
}

char           *
logInt(char *pattern, int p_value)
{
    char           *res = (char *) calloc(SIZE, sizeof(char));	// nota
								// grandezza 
								// fissa
    sprintf(res, pattern, p_value);
    return res;
}

char           *
logIndent(int indent, char *string)
{
    // printf("%s%d\n","indent",indent);

    char           *res = (char *) calloc(SIZE, sizeof(char));	// nota
								// grandezza 
								// fissa
    char            temp[indent + 1];
    for (int i = 0; i < indent; i++) {
	temp[i] = '\t';
    }
    temp[indent] = '\0';
    sprintf(res, "%s %s", temp, string);
    // return string;
    return res;
}
void
write_log(char *nome_log, char *buffer)
{
    if (nome_log == NULL) {
	nome_log = "interpreter";
    }
    // const char * dirpath = "/var/log/airport";
    // const char * temp = "/var/log/airport/";
    const char     *dirpath =
	"/Users/Lorenzo/Documents/lavoro unitn/LFC/exercizes/LFC2015-2016/5.2 - function and procedures/log";
    const char     *temp =
	"/Users/Lorenzo/Documents/lavoro unitn/LFC/exercizes/LFC2015-2016/5.2 - function and procedures/log/";
    // salvo il percorso attuale
    char           *path = NULL;
    path = getcwd(NULL, 0);

    char           *primacreazione =
	"creo la directory /log nella cartella di riferimento";
    int             firstime = 0;

    // creo il path del file.log richiesto dal primo parametro della
    // funzione
    // char * pathnome=(char *)calloc(SIZE,sizeof(char));
    char            pathnome[SIZE];
    memset(pathnome, 0, SIZE);
    strcpy(pathnome, temp);
    strcat(pathnome, nome_log);
    strcat(pathnome, ".log");

    // mi sposto nel percorso richiesto
    if (chdir(dirpath) == -1) {
	if (mkdir
	    ("/Users/Lorenzo/Documents/lavoro unitn/LFC/exercizes/LFC2015-2016/5.2 - function and procedures/log",
	     S_IRWXU) == -1) {
	    printf("errore creazione cartella, try with sudo\n");
	    exit(-1);
	} else {
	    // se la cartella non esisteva precedentemente imposto
	    // firstime a 1
	    firstime = 1;
	}
    }
    // apro il file in append specificato dal percorso che mi sono creato
    // precedentemente
    FILE           *file;
    file = fopen(pathnome, "at");

    time_t          mytime;
    struct tm      *timeinfo;
    time(&mytime);
    // ottengo dalla struttura time l'ora attuale
    timeinfo = localtime(&mytime);
    char           *buffer_data = (char *) calloc(SIZE, sizeof(char));
    ;

    // converto la struttura in una stringa leggibile dall'utente
    buffer_data = asctime(timeinfo);
    char            data[SIZE];
    memset(data, 0, SIZE);
    // char * data = (char *)calloc(SIZE,sizeof(char));
    strncpy(data, buffer_data, (strlen(buffer_data) - 1));

    if (firstime == 1) {
	// se è stata creata la cartella airport stampo nel file.log
	// anche la stringa predefinita di prima creazione
	fprintf(file, "%s : ", data);
	fprintf(file, "%s\n", primacreazione);
    }
    // stampo la data
    fprintf(file, "%s : ", data);
    chmod(pathnome, S_IRWXU);
    // stampo il messaggio passato come secondo parametro
    fprintf(file, "%s\n", buffer);
    // chiudo lo stream sul file
    fclose(file);
    // ritorno alla path di partenza
    chdir(path);

}
