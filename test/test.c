/*
 * This file is part of the l4l Library.
 *
 * Copyright (C) 2015 by Jose Antonio Ortiz Bascuas <jaortizb@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
 */

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "l4l.h"

//text.exe read from stdin text lines and write a filelog with the compiled chosen configuration and log.cfg

//Example: L4L_LOG_PATH=. L4L_KEY=password L4L_LOG=/tmp/log.cfg history -a >(tee -a ~/.bash_history | test.exe password)
//The commandline execution logs the user commands from de bash history service.

int main(int argc, char **argv) {
	int i=0,rc;
	char linea[L4L_MAX_TRZ_LEN]="";
	FILE * fp;
	
	setenv("L4L_KEY",argv[1],0);
	rc = l4l_init("log.cfg","test",argc,argv);
	
//	while (fgets ( linea, sizeof linea, stdin ) != NULL) {
	for(int x=0; x<atoi(argv[1]);x++) {
		l4l_printf(0xFFFFFFFF,L4L_MSJ_Comentar,argv[2]);
	}
	printf ("fin");
	l4l_end();
	return(0);
}

