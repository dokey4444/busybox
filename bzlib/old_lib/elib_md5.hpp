/*
// =====================================================================================
// 
//       Filename:  elib_md5.hpp
// 
//    Description:  copy from: http://www.ietf.org/rfc/rfc1321.txt
// 
//        Version:  1.0
//        Created:  07/17/2013 12:17:39 AM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/

#ifndef  _ELIB_MD5_H_
#define  _ELIB_MD5_H_

#include "elib_util.hpp"

/* GLOBAL.H - RSAREF types and constants
 */

/* PROTOTYPES should be set to one if and only if the compiler supports
   function argument prototyping.
   The following makes PROTOTYPES default to 0 if it has not already
   been defined with C compiler flags.
 */
// #ifndef PROTOTYPES
// #define PROTOTYPES 0
// #endif

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
// typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
// typedef unsigned long int UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
   If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
   returns an empty list.
 */
// #if PROTOTYPES
// #define PROTO_LIST(list) list
// #else
// #define PROTO_LIST(list) ()
// #endif

/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

/* MD5 context. */
typedef struct {
	uint32_t state[4];                                   /* state (ABCD) */
	uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

// void MD5Init PROTO_LIST ((MD5_CTX *));
// void MD5Update PROTO_LIST
// ((MD5_CTX *, unsigned char *, unsigned int));
// void MD5Final PROTO_LIST ((unsigned char [16], MD5_CTX *));

void MD5Init(MD5_CTX *context);
void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
void MD5Final (unsigned char digest[16], MD5_CTX *context);

void md5(unsigned char *input_buf, unsigned int buf_len, unsigned char *output_buf);
uint32_t md5_32(unsigned char *input_buf, unsigned int buf_len);
uint64_t md5_64(unsigned char *input_buf, unsigned int buf_len);

#endif   // ----- #ifndef _ELIB_MD5_H_  -----
