
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#include "nsCodingStateMachine.h"

/*
Modification from frank tang's original work:
. 0x00 is allowed as a legal character. Since some web pages contains this char in 
  text stream.
*/

// BIG5 

static const PRUint32 BIG5_cls [ 256 / 8 ] = {
//PCK4BITS(0,1,1,1,1,1,1,1),  // 00 - 07 
PCK4BITS(1,1,1,1,1,1,1,1),  // 00 - 07    //allow 0x00 as legal value
PCK4BITS(1,1,1,1,1,1,0,0),  // 08 - 0f 
PCK4BITS(1,1,1,1,1,1,1,1),  // 10 - 17 
PCK4BITS(1,1,1,0,1,1,1,1),  // 18 - 1f 
PCK4BITS(1,1,1,1,1,1,1,1),  // 20 - 27 
PCK4BITS(1,1,1,1,1,1,1,1),  // 28 - 2f 
PCK4BITS(1,1,1,1,1,1,1,1),  // 30 - 37 
PCK4BITS(1,1,1,1,1,1,1,1),  // 38 - 3f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 40 - 47 
PCK4BITS(2,2,2,2,2,2,2,2),  // 48 - 4f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 50 - 57 
PCK4BITS(2,2,2,2,2,2,2,2),  // 58 - 5f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 60 - 67 
PCK4BITS(2,2,2,2,2,2,2,2),  // 68 - 6f 
PCK4BITS(2,2,2,2,2,2,2,2),  // 70 - 77 
PCK4BITS(2,2,2,2,2,2,2,1),  // 78 - 7f 
PCK4BITS(4,4,4,4,4,4,4,4),  // 80 - 87 
PCK4BITS(4,4,4,4,4,4,4,4),  // 88 - 8f 
PCK4BITS(4,4,4,4,4,4,4,4),  // 90 - 97 
PCK4BITS(4,4,4,4,4,4,4,4),  // 98 - 9f 
PCK4BITS(4,3,3,3,3,3,3,3),  // a0 - a7 
PCK4BITS(3,3,3,3,3,3,3,3),  // a8 - af 
PCK4BITS(3,3,3,3,3,3,3,3),  // b0 - b7 
PCK4BITS(3,3,3,3,3,3,3,3),  // b8 - bf 
PCK4BITS(3,3,3,3,3,3,3,3),  // c0 - c7 
PCK4BITS(3,3,3,3,3,3,3,3),  // c8 - cf 
PCK4BITS(3,3,3,3,3,3,3,3),  // d0 - d7 
PCK4BITS(3,3,3,3,3,3,3,3),  // d8 - df 
PCK4BITS(3,3,3,3,3,3,3,3),  // e0 - e7 
PCK4BITS(3,3,3,3,3,3,3,3),  // e8 - ef 
PCK4BITS(3,3,3,3,3,3,3,3),  // f0 - f7 
PCK4BITS(3,3,3,3,3,3,3,0)   // f8 - ff 
};


static const PRUint32 BIG5_st [ 3] = {
PCK4BITS(eError,eStart,eStart,     3,eError,eError,eError,eError),//00-07 
PCK4BITS(eError,eError,eItsMe,eItsMe,eItsMe,eItsMe,eItsMe,eError),//08-0f 
PCK4BITS(eError,eStart,eStart,eStart,eStart,eStart,eStart,eStart) //10-17 
};

static const PRUint32 Big5CharLenTable[] = {0, 1, 1, 2, 0};

SMModel const Big5SMModel = {
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, BIG5_cls },
    5,
  {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, BIG5_st },
  Big5CharLenTable,
  "BIG5",
};

static const PRUint32 EUCJP_cls [ 256 / 8 ] = {
//PCK4BITS(5,4,4,4,4,4,4,4),  // 00 - 07 
PCK4BITS(4,4,4,4,4,4,4,4),  // 00 - 07 