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
#ifndef __l4l_h
#define __l4l_h

#include "l4lcompcfg.h"
#ifdef L4L_HASH_METHOD
#include <openssl/hmac.h>
#endif

char* timestamp(char *,char *);

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__func__)
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# if defined __GNUC__ && __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif
#endif

//Data structures for configuration issues
typedef struct L4Laux
  {
	FILE *logFile;
#if defined(_PTHREAD_H)
	pthread_mutex_t mutex;
#endif
	char *s_trz_aux;
	char *s_trz_aux2;
	char *s_trz;
	int l_trz;
#ifdef L4L_HASH_METHOD
	unsigned char hash_bin[256];
#endif
	char *none;
	unsigned long currMask;
	int i;
	unsigned int len;
	char logFilePath[256];
	char *hash_str;
#ifdef L4L_HASH_METHOD
	HMAC_CTX ctx;
#endif
} aux;
 
typedef struct L4LCfgStruct
  {
	unsigned long print_mask;
	unsigned long hash_mask;
	unsigned long sync_mask;
	unsigned long flush_mask;
	size_t buffsize;
	size_t bufftype;
	size_t max_size;
	long max_seconds;
  } L4LCfg;
  
typedef struct L4Lstruct
  {
	char logFileName[256];
	char cfgFile[256];
	char category[256];
#ifdef L4L_HASH_METHOD
	const EVP_MD *hash_method;
#endif
	long size;
	long max_timestamp;
	aux data;
	L4LCfg cfg;
  } L4L;

extern L4L __l4l;

static int	l4l_LeerCfg(const char *cfg, const char *cat,char *bufftype_str);
char *L4Lhostname(char *res);
char *calcFileHash(char *path,char *digest);
void l4l_private_key();
char *l4l_now(const char *format);
void l4l_end();
int  l4l_init(const char *cfg,const char *cat,int argc, char**argv);
void l4l_Rotate();
void l4l_rest();

// If <pthread.h> defined news lock control instrunctios injected in compilation fase.

#if defined(_PTHREAD_H)
#	define L4L_CMD_THREADSAFE_LOCK pthread_mutex_lock(&__l4l.data.mutex)
#else
#	define L4L_CMD_THREADSAFE_LOCK
#endif

//Version header print log
#define L4L_MSJ_Version  ">%s< %ld MSJ-Version: L4L v.%%s Copyright (c) 2015 Jose Antonio Ortiz <jaortizb@gmail.com>\nThis software is released under the LGPL 2.1 license, see the LICENSE file",L4L_HASH,L4L_EPOCH

//Principal print log macro
#define l4l_printf(Mask,T,F,args...)\
	if (__l4l.cfg.print_mask & Mask) {\
		L4L_CMD_THREADSAFE_LOCK;\
			if(#args[0]) snprintf(__l4l.data.s_trz_aux,L4L_MAX_TRZ_LEN-1,F,##args);\
			else strcpy(__l4l.data.s_trz_aux,F);\
			__l4l.data.currMask=Mask;\
			snprintf(__l4l.data.s_trz_aux2,L4L_MAX_TRZ_LEN-1,T);\
			l4l_rest();\
	}

#define QUOTE(name) #name

#ifdef __cplusplus
}
#endif
#endif

#include "l4lcompcfg.h"
