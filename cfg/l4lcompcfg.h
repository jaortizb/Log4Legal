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

#define L4L_MAX_TRZ_LEN 4096
#define L4L_READ_KEY getenv("L4L_KEY")
#define L4L_ROTATION YES

#define L4L_HASH __l4l.data.hash_str
#define L4L_EPOCH (long)time(NULL)
#define L4L_MASK __l4l.data.currMask
#define L4L_LOG_FILE __l4l.logFileName

#ifdef L4L_READ_KEY
#define L4L_MAX_HASH_LEN 10
#define L4L_TIMESTAMP YES
//#define L4L_HASH_METHOD "md2"
//#define L4L_HASH_METHOD "md4"
//#define L4L_HASH_METHOD "md5"
//#define L4L_HASH_METHOD "sha"
//#define L4L_HASH_METHOD "sha1"
//#define L4L_HASH_METHOD "sha224"
#define L4L_HASH_METHOD "sha256"
//#define L4L_HASH_METHOD "sha384"
//#define L4L_HASH_METHOD "sha512"
//#define L4L_HASH_METHOD "dss1"
//#define L4L_HASH_METHOD "mdc2"
//#define L4L_HASH_METHOD "ripemd160"
#define L4L_MSJ_Generic  ">%s< %x %ld %%s",L4L_HASH,L4L_MASK,L4L_EPOCH
#endif

#define L4L_CRITICITY ((L4L_MASK & 0x01000000)?'O':(L4L_MASK & 0x02000000)?'W':(L4L_MASK & 0x04000000)?'C':'F'
#define L4L_ENV __l4l.cfg.environment

#define L4L_FILE_NAME "./tmp_%s_%ld.log",l4l_now("%Y%m%d_%H%M%S"),getpid()

#define L4L_MSJ_Comentar ">%s< MSJ-Comentar: %%s",L4L_HASH
#define L4L_MSJ_Evento  "TEST;%c;%s;;ACU;%%s;AUTOMATIC",L4L_CRITICITY,L4L_ENV

//#define L4L_MSJ_Comentar "%%s"
#define L4L_MSJ_Assert   ">%s< %x %ld [%s:%d] PRO-Cond: Assertion (%s) result: %s",L4L_HASH,L4L_MASK,time(NULL),__FILE__,__LINE__
#define L4L_MSJ_Timestamp ">%s< %x %ld [%s:%d] MSJ-Timestamp: %s",L4L_HASH,L4L_MASK,time(NULL),__FILE__,__LINE__,timestamp(__l4l.data.s_trz,"L4L_Init")

#define MAX_LEN_HASH 20
