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

               


#ifndef CBITARRAY_H
#define CBITARRAY_H

#include "c1222.h"
#include "c1222environment.h"

EXTERN_C  Boolean CMultiByteBitArray__isBitSet( const unsigned char* parray, unsigned long n);
EXTERN_C  void CMultiByteBitArray__setBit( unsigned char* parray, unsigned long n, Boolean value);
EXTERN_C Boolean CMultiByteReverseBitArray__isBitSet(const unsigned char* pArray, unsigned long n);



#endif



//-----------------------------------------------------------------------
// End of Header
//-----------------------------------------------------------------------
