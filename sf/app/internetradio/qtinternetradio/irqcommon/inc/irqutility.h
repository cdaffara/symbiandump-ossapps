/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef _IRQUTILITY_H
#define _IRQUTILITY_H

#include "irqenums.h"
#include "irqcommonexport.h"

class IRQPreset;
class CIRPreset;
class CIRIsdsPreset; 
class QString;

class IRQCOMMMON_DLL_EXPORT IRQUtility 
{
public:
  
  /* convert the CIRIsdsPreset to the IRQPreset. 
   */
  static void convertCIRIsdsPreset2IRQPrest(const CIRIsdsPreset& aCIRIsdsPreset, IRQPreset& aQIRPreset);
  /* convert the IRQPreset to the CIRIsdsPreset 
   */
  static void convertIRQPreset2CIRIsdsPreset(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset);
  /* convert CIRPreset to the IRQPreset 
   */
  static void convertCIRPreset2IRQPreset(const CIRPreset& aCIRPreset, IRQPreset& aIRQPreset);
  /* switch from the symbian error code to the IRQ error code 
   */
  static void convertSError2QError(const int aSError, int& aQError);

  static bool isValidUrl(const QString& aUrl);

  static bool openAdvLink(const QString& aUrl);
  
  static bool findAppByUid(const int aUid);
  
  static bool launchAppByUid(const int aUid);
  
private:
  
  static void appendURLL(const IRQPreset& aQIRPreset, CIRIsdsPreset& aCIRIsdsPreset);  
};

#endif
