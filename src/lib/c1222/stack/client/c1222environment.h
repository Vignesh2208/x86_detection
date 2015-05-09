// CDDL HEADER START
//*************************************************************************************************
//                                     Copyright � 2006-2009
//                                           Itron, Inc.
//                                      All Rights Reserved.
//
//
// The contents of this file, and the files included with this file, are subject to the current 
// version of Common Development and Distribution License, Version 1.0 only (the �License�), which 
// can be obtained at http://www.sun.com/cddl/cddl.html. You may not use this file except in 
// compliance with the License. 
//
// The original code, and all software distributed under the License, are distributed and made 
// available on an �AS IS� basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED. ITRON, 
// INC. HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR PARTICULAR PURPOSE, OR NON INFRINGEMENT. Please see the License 
// for the specific language governing rights and limitations under the License.
//
// When distributing Covered Software, include this CDDL Header in each file and include the 
// License file at http://www.sun.com/cddl/cddl.html.  If applicable, add the following below this 
// CDDL HEADER, with the fields enclosed by brackets �[   ]� replaced with your own identifying 
// information: 
//		Portions Copyright [yyyy]  [name of copyright owner]
//
//*************************************************************************************************
// CDDL HEADER END


#ifndef C1222ENVIRONMENT_H
#define C1222ENVIRONMENT_H

#ifndef AVOID_PORTAB_H
#include "portab.h"
#else
// portab introduces a bitfield that causes warnings in borland, and I just need
// true and false defined, so let's just define them for the moment
// will need to use portab eventually
#define TRUE 1
#define FALSE 0
#endif

#ifndef AVOID_SLB_H
#include "slb.h"
#endif

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#ifdef __BCPLUSPLUS__
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

#ifdef __ICCAVR__
#define C1222CONST __flash const
void *UTIL_Safe_memcpy(void *, const void *, size_t);
#define memcpy                  UTIL_Safe_memcpy
#else
#define C1222CONST const
#endif


//void logHex(char* text, unsigned char* buffer, unsigned short length);
//void logTxt(char* text);
//void logInt(char* text, int ivalue);

#endif
