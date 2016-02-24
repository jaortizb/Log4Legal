#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "l4l.h"
#include "compcfg.h"
#include "timestamp.h"

//PROMPT_COMMAND='history -a >(tee -a ~/.bash_history | sudo test.exe )'

int main(int argc, char **argv) {
	int i=0,rc;
	char linea[L4L_MAX_TRZ_LEN]="",L2[L4L_MAX_TRZ_LEN]="";
	FILE * fp;

	rc = l4l_init("log.cfg","test",argc,argv);
	fp = fopen (argv[1], "r");fscanf(fp, "%[^\n]",linea);fclose(fp);setenv("L4L_KEY",linea,0);
	while (fgets ( linea, sizeof linea, stdin ) != NULL) {
		linea[strlen(linea)-1]=0;
		strcpy(L2,(char*)timestamp(linea,linea));
		l4l_printf(0xFFFFFFFF,L4L_MSJ_Comentar,L2);
	}
	printf ("fin");
	l4l_end();
	return(0);
}

