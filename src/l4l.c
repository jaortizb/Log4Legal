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

#include <unistd.h>
#include <sys/resource.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "version.h"
#include "l4l.h"
#include "l4lcompcfg.h"
#if defined(L4L_TIMESTAMP) && L4L_TIMESTAMP == YES
#include "timestamp.h"
#endif

L4L __l4l;

//Configuration read from file and datastructres load

static int l4l_LeerCfg(const char *cfg, const char *cat,char *bufftype_str) {
	char item[512],value[256],aux1[256]="log4legal.";
	FILE *fp;
	int ret=0,i;

	char key[512]="";
	char * pEnd;

	if((fp = fopen(cfg, "r")) == NULL) {
		fprintf(stderr,"L4L: Unable to open file (%s)",__l4l.logFileName);
		return(2);
	}

	while (1==1) {
		ret = fscanf(fp, "%s", item);
		if (ret == EOF) break;
		else {
			i=0;for(i=0;item[i]!= '=' && i<(int)strlen(item);i++);
			strncpy(key,item,(size_t)i);key[i]='\0';
			strcpy(value,item+i+1);
//			printf("linea (%s), key (%s), value (%s)\n",item,key,value);

			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".rotation.size");if(0==strcmp(key,aux1)) {
				__l4l.cfg.max_size=(size_t)atoi(value);
				continue;
			}
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".file.path");if(0==strcmp(key,aux1)) {
				strcpy(__l4l.data.logFilePath,value);continue;
			} 
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".print_mask");if(0==strcmp(key,aux1)) {
				__l4l.cfg.print_mask = strtoul (value,&pEnd,16);continue;
			} 
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".flush_mask");if(0==strcmp(key,aux1)) {
				__l4l.cfg.flush_mask = strtoul (value,&pEnd,16);continue;
			} 
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".sync_mask");if(0==strcmp(key,aux1)) {
				__l4l.cfg.sync_mask = strtoul (value,&pEnd,16);continue;
			} 
#ifdef L4L_HASH_METHOD
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".hash_mask");if(0==strcmp(key,aux1)) {
				__l4l.cfg.hash_mask = strtoul (value,&pEnd,16);continue;
			} 
#endif
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".buffsize");if(0==strcmp(key,aux1)) {
				__l4l.cfg.buffsize = (size_t)atoi (value);continue;
			}
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".bufftype");if(0==strcmp(key,aux1)) {
				strcpy(bufftype_str,value);
				if(0==strcmp(value,"_IOFBF")) __l4l.cfg.bufftype=_IOFBF;
				else if(0==strcmp(value,"_IOLBF")) __l4l.cfg.bufftype=_IOLBF;
				else if(0==strcmp(value,"_IONBF")) __l4l.cfg.bufftype=_IONBF;
				continue;
			}
			strcpy(aux1,"log4legal.");strcat(aux1,cat);strcat(aux1,".rotation.seconds");if(0==strcmp(key,aux1)) {
				__l4l.cfg.max_seconds=atoi(value);continue;
			}
		}
	}
	return 0;
	printf("__l4l.cfg.max_size (%d) __l4l.cfg.max_seconds (%d) print_mask (%x)\n",__l4l.cfg.max_size,__l4l.cfg.max_seconds, __l4l.cfg.print_mask);
}


#define MAX_BUF_LEN 1024*128

#ifdef L4L_HASH_METHOD

//End file log hash calculation for rotation issues
char *calcFileHash(char *path,char *digest) {
  BIO *b=NULL;
  EVP_MD_CTX c;
  const EVP_MD *md;
  char *res;
  static char sres[256];
  
  unsigned char buf[MAX_BUF_LEN]="";
  unsigned int len,i=0;

  unsigned char hash[EVP_MAX_MD_SIZE];

  b=BIO_new_file(path, "r");
  if( b == NULL ){
      return NULL;
  }

  OpenSSL_add_all_digests();

  md = EVP_get_digestbyname(digest);

  EVP_MD_CTX_init(&c);
  if(0==EVP_DigestInit_ex(&c, md, NULL)) return NULL;
  while ( len=(unsigned int)BIO_read(b,buf,MAX_BUF_LEN) )
  if(0==EVP_DigestUpdate(&c,buf,(size_t)len)) return NULL;

  if(0==EVP_DigestFinal_ex(&c, hash, &len)) return NULL;
  if(0==EVP_MD_CTX_cleanup(&c)) return NULL;
  if(0==BIO_free(b)) return NULL;
 
  if(NULL==(res = (char*)malloc(len*sizeof(char)*2))) return NULL;
  *res='\0';
  while ( i<len) {
     sprintf(res,"%s%02x",res,(unsigned int)hash[i]);
     i++;
  }
	strncpy(sres,res,256);
  free(res);
  return sres;
}
#endif

#ifdef L4L_READ_KEY
const char lut[] = "0123456789abcdef";

//Read private key for https comm
void l4l_private_key() {
	int i=0;

		HMAC_Update(&__l4l.data.ctx, (unsigned char*)&__l4l.data.s_trz, (size_t)(__l4l.data.l_trz-1));
		HMAC_Final(&__l4l.data.ctx, __l4l.data.hash_bin, &__l4l.data.len);
		for(i=0;i < L4L_MAX_HASH_LEN;i+=2) {
			__l4l.data.hash_str[i]=lut[(int)(__l4l.data.hash_bin[i] >>4)];
			__l4l.data.hash_str[i+1]=lut[(int)(__l4l.data.hash_bin[i] & 0x0f)]; 
		}
		__l4l.data.hash_str[i]='\0';
}
#endif

#define L4L_FILE_NAME_SIZE 256

#define L4L_MAKE_ROTATION \
	if (__l4l.cfg.max_size && __l4l.cfg.max_size < __l4l.size) {\
			l4l_LeerCfg(__l4l.cfgFile,__l4l.category,__l4l.data.none);\
			snprintf(__l4l.logFileName,L4L_FILE_NAME_SIZE,L4L_FILE_NAME);\
			__l4l.size=0;\
			__l4l.max_timestamp=time(NULL)+__l4l.cfg.max_seconds;\
			if (NULL == (__l4l.data.logFile = freopen (__l4l.logFileName, "w",__l4l.data.logFile))) {\
				fprintf(stderr,"L4L: Unable to REopen file (%s)",__l4l.logFileName);\
				exit(-1);\
			}\
	}

void l4l_rest() {
			__l4l.data.l_trz=snprintf(__l4l.data.s_trz,L4L_MAX_TRZ_LEN-1,__l4l.data.s_trz_aux2,__l4l.data.s_trz_aux);
			__l4l.data.s_trz[__l4l.data.l_trz++]='\n';__l4l.data.s_trz[__l4l.data.l_trz]='\0';
			__l4l.size+=fwrite(__l4l.data.s_trz,1,__l4l.data.l_trz,__l4l.data.logFile);
			strcpy(__l4l.data.s_trz_aux,__l4l.data.s_trz);
			if(__l4l.data.currMask & __l4l.cfg.flush_mask) {
				fflush (__l4l.data.logFile);\
				fsync (fileno (__l4l.data.logFile));
			}

#ifdef L4L_HASH_METHOD
			if (__l4l.data.currMask & __l4l.cfg.hash_mask) {
				int i=0;

				HMAC_Update(&__l4l.data.ctx, (unsigned char*)&__l4l.data.s_trz, (size_t)(__l4l.data.l_trz-1));
				HMAC_Final(&__l4l.data.ctx, __l4l.data.hash_bin, &__l4l.data.len);
				for(i=0;i < L4L_MAX_HASH_LEN;i+=2) {
					__l4l.data.hash_str[i]=lut[(int)(__l4l.data.hash_bin[i] >>4)];
					__l4l.data.hash_str[i+1]=lut[(int)(__l4l.data.hash_bin[i] & 0x0f)]; 
				}
				__l4l.data.hash_str[i]='\0';
			}
#endif

#if defined(L4L_ROTATION) && L4L_ROTATION == YES
L4L_MAKE_ROTATION
#endif

#if defined(_PTHREAD_H)
		pthread_mutex_unlock(&__l4l.data.mutex);
#endif

}

char *l4l_now(const char *format) {
  time_t rawtime;
  
  time (&rawtime);
  strftime (__l4l.data.none,80,format,localtime (&rawtime));

  return __l4l.data.none;
}

//End notification log line
void l4l_end() {
#if  defined(L4L_TIMESTAMP) && L4L_TIMESTAMP == YES
	l4l_printf(0xFFFFFFFF,L4L_MSJ_Comentar,"Timestamp: %s",timestamp(__l4l.data.s_trz_aux,"L4L_Close"));
#endif
	if (0 != fclose(__l4l.data.logFile)) {
		fprintf(stderr,"L4L: Unable to close file (%s)",__l4l.logFileName);
	}
}

//Init configurantion datastructures and first lines log.

int l4l_init(const char *cfg,const char *cat,int argc,char **argv)  {
#ifdef L4L_HASH_METHOD
	char *key;
	char *hash;
#endif
	int i=0;
	char bufftype_str[20]="";

	*__l4l.data.logFilePath='\0';
	__l4l.cfg.bufftype=_IOFBF;
	strcpy(bufftype_str,"_IOFBF");
	strcpy(__l4l.cfgFile,cfg);
	strcpy(__l4l.category,cat);
	__l4l.cfg.print_mask=__l4l.cfg.hash_mask=__l4l.cfg.sync_mask=__l4l.cfg.flush_mask=0xffffffff;
#ifdef L4L_HASH_METHOD
	OpenSSL_add_all_digests();
//printf("hash method: %s\n",L4L_HASH_METHOD);
	__l4l.hash_method=EVP_get_digestbyname(L4L_HASH_METHOD);
	
#endif
	if(0 != l4l_LeerCfg(cfg,"global",bufftype_str)) return -1;;
	if(0 != l4l_LeerCfg(cfg,cat,bufftype_str)) return -1;
	__l4l.data.none = (char*)malloc(256*sizeof(char));
	__l4l.data.s_trz = (char*)malloc((L4L_MAX_TRZ_LEN+1)*sizeof(char));
	__l4l.data.s_trz_aux = (char*)malloc((L4L_MAX_TRZ_LEN+1)*sizeof(char));
	__l4l.data.s_trz_aux2 = (char*)malloc((L4L_MAX_TRZ_LEN+1)*sizeof(char));
#ifdef L4L_HASH_METHOD
	__l4l.data.len=L4L_MAX_HASH_LEN/2+2;
	__l4l.data.hash_str = (char*)malloc(257*sizeof(char));
	if(__l4l.data.hash_str == NULL) return -3;
#else
	__l4l.data.hash_str = (char*)malloc((1)*sizeof(char));
#endif
	*__l4l.data.hash_str='\0';
	if(__l4l.data.none == NULL || __l4l.data.s_trz == NULL || __l4l.data.s_trz_aux == NULL) 
		return -3;
	__l4l.max_timestamp=time(NULL)+__l4l.cfg.max_seconds;
	*__l4l.data.s_trz=*__l4l.data.none=*__l4l.data.s_trz_aux='\0';

#if defined(_PTHREAD_H)
	__l4l.data.mutex=PTHREAD_MUTEX_INITIALIZER;
#endif
//	sprintf(info_to_autenticate,L4L_CFG_STR_AUTH,__DATE__,__TIME__,L4L_EVP_hash,calcFileHash(argv[0],__l4l.data.none),argv[0],getpid(),getppid(),getgid(),getuid(),geteuid(),time(NULL),L4Lhostname(__l4l.data.none),getenv("PWD"));
//	printf(info_to_autenticate);
	if(0==snprintf(__l4l.logFileName,L4L_FILE_NAME_SIZE,L4L_FILE_NAME)) {
		fprintf(stderr,"L4L: Invalid file name\n");
		return(-5);
	}
	if (NULL == (__l4l.data.logFile = fopen (__l4l.logFileName, "w"))) {
		fprintf(stderr,"L4L: Unable to open file (%s)",__l4l.logFileName);
		return(-6);
	}
	if(0!=setvbuf ( __l4l.data.logFile , NULL , (int)__l4l.cfg.bufftype , __l4l.cfg.buffsize )) {
		fprintf(stderr,"L4L: The file descriptor underlying stream is not valid.\n");
		return(-7);
	}
#ifdef L4L_HASH_METHOD
	if(NULL==(key=L4L_READ_KEY)) 
	{
		return -11;
	}
#endif
	sprintf(__l4l.data.s_trz,"MSJ-Version: L4L v.%s Copyright (c) 2015 Jose Antonio Ortiz <jaortizb@gmail.com>\n",L4L_VERSION);
	sprintf(__l4l.data.s_trz,"%sMSJ-License: This software is released under the LGPL 2.1 license, see the LICENSE file.\n",__l4l.data.s_trz);
	sprintf(__l4l.data.s_trz,"%sMSJ-Log: Log File Name %s\n",__l4l.data.s_trz,__l4l.logFileName);
	if(argv!=NULL) { 
		sprintf(__l4l.data.s_trz,"%sMSJ-Bin: Binary file >%s<\n",__l4l.data.s_trz,argv[0]);
		for(i=1;i<argc;i++) {
			sprintf(__l4l.data.s_trz,"%sMSJ-Bin: Binary file arg[%d]=%s\n",__l4l.data.s_trz,i,argv[i]);
		}
	}
#if defined(L4L_ROTATION) && L4L_ROTATION == YES
	sprintf(__l4l.data.s_trz,"%sMSJ-Def: L4L_ROTATION: YES\n",__l4l.data.s_trz);
#else
	sprintf(__l4l.data.s_trz,"%sMSJ-Def: L4L_ROTATION: NO\n",__l4l.data.s_trz);
#endif

#ifdef L4L_HASH_METHOD
	sprintf(__l4l.data.s_trz,"%sMSJ-Def: L4L_HASH_METHOD: %s\n",__l4l.data.s_trz,L4L_HASH_METHOD);
#endif

	sprintf(__l4l.data.s_trz,"%sMSJ-Cfg: print_mask=0x%x, hash_mask=0x%x, sync_mask=0x%x,flush_mask=0x%x\n",__l4l.data.s_trz,__l4l.cfg.print_mask,__l4l.cfg.hash_mask,__l4l.cfg.sync_mask,__l4l.cfg.flush_mask);
	sprintf(__l4l.data.s_trz,"%sMSJ-Cfg: buffsize=%d, bufftype=%s, max_size=%d, max_seconds=%d\n",__l4l.data.s_trz,__l4l.cfg.buffsize,bufftype_str,__l4l.cfg.max_size,__l4l.cfg.max_seconds);
	__l4l.size=fprintf(__l4l.data.logFile,"%s",__l4l.data.s_trz);

#ifdef L4L_HASH_METHOD
	if(1!=HMAC_Init_ex(&__l4l.data.ctx, key, (int)strlen(key), __l4l.hash_method, NULL)) return -10;
	__l4l.data.l_trz=__l4l.size;
	HMAC_Update(&__l4l.data.ctx, (unsigned char*)&__l4l.data.s_trz, (size_t)(__l4l.data.l_trz-1));
	HMAC_Final(&__l4l.data.ctx, __l4l.data.hash_bin, &__l4l.data.len);
	for(i=0;i < L4L_MAX_HASH_LEN;i+=2) {
		__l4l.data.hash_str[i]=lut[(int)(__l4l.data.hash_bin[i] >>4)];
		__l4l.data.hash_str[i+1]=lut[(int)(__l4l.data.hash_bin[i] & 0x0f)]; 
	}
	__l4l.data.hash_str[i]='\0';
#endif

#if  defined(L4L_TIMESTAMP) && L4L_TIMESTAMP == YES
	l4l_printf(0xFFFFFFFF,L4L_MSJ_Comentar,"Timestamp: %s",timestamp(__l4l.data.s_trz,"L4L_Init"));
#endif

return 0;
}
