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
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 *          Shy Shalom <shooshX@gmail.com>
 * Portions created by the Initial Developer are Copyright (C) 2005
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

#include "nsHebrewProber.h"
#include <stdio.h>

// windows-1255 / ISO-8859-8 code points of interest
#define FINAL_KAF ('\xea')
#define NORMAL_KAF ('\xeb')
#define FINAL_MEM ('\xed')
#define NORMAL_MEM ('\xee')
#define FINAL_NUN ('\xef')
#define NORMAL_NUN ('\xf0')
#define FINAL_PE ('\xf3')
#define NORMAL_PE ('\xf4')
#define FINAL_TSADI ('\xf5')
#define NORMAL_TSADI ('\xf6')

// Minimum Visual vs Logical final letter score difference.
// If the difference is below this, don't rely solely on the final letter score distance.
#define MIN_FINAL_CHAR_DISTANCE (5)

// Minimum Visual vs Logical model score difference.
// If the difference is below this, don't rely at all on the model score distance.
#define MIN_MODEL_DISTANCE (0.01)

#define VISUAL_HEBREW_NAME ("ISO-8859-8")
#define LOGICAL_HEBREW_NAME ("WINDOWS-1255")

PRBool nsHebrewProber::isFinal(char c)
{
  return ((c == FINAL_KAF) || (c == FINAL_MEM) || (c == FINAL_NUN) || (c == FINAL_PE) || (c == FINAL_TSADI));
}

PRBool nsHebrewProber::isNonFinal(char c)
{
  return ((c == NORMAL_KAF) || (c == NORMAL_MEM) || (c == NORMAL_NUN) || (c == NORMAL_PE));
  // The normal Tsadi is not a good Non-Final letter due to words like 
  // 'lechotet' (to chat) containing an apostrophe after the tsadi. This 
  // apostrophe is converted to a space in FilterWithoutEnglishLetters causing 
  // the Non-Final tsadi to appear at an end of a word even though this is not 
  // the case in the original text.
  // The letters Pe and Kaf rarely display a related behavior of not being a 
  // good Non-Final letter. Words like 'Pop', 'Winamp' and 'Mubarak' for 
  // example legally end with a Non-Final Pe or Kaf. However, the benefit of 
  // these letters as Non-Final letters outweighs the damage since these words 
  // are quite rare.
}

/** HandleData
 * Final letter analysis for logical-visual decision.
 * Look for evidence that the received buffer is either logical Hebrew or 
 * visual Hebrew.
 * The following cases are checked:
 * 1) A word longer than 1 letter, ending with a final letter. This is an 
 *    indication that the text is laid out "naturally" since the final letter 
 *    really appears at the end. +1 for logical score.
 * 2) A word longer than 1 letter, ending with a Non-Final letter. In normal
 *    Hebrew, words ending with Kaf, Mem, Nun, Pe or Tsadi, should not end with
 *    the Non-Final form of that letter. Exceptions to this rule are mentioned
 *    above in isNonFinal(). This is an in