/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Lunar Plugin 
 *
*/


 
#ifndef __CALENLUNARINFOPROVIDER_H__
#define __CALENLUNARINFOPROVIDER_H__

#include "CalenLunarInfo.h"
#include <e32base.h>


class CChineseCalendarConverter;
class CCalenSolarTerms;
class RFs;

class CCalenLunarInfoProvider : public CBase
    {
public: // public API 
    IMPORT_C static CCalenLunarInfoProvider* NewL(RFs& aFs);

    IMPORT_C virtual ~CCalenLunarInfoProvider();

    IMPORT_C TCalenLunarInfo GetLunarInfoL( const TTime& aDay );

private: // own methods
    CCalenLunarInfoProvider();
    void ConstructL(RFs& aFs);

private: // data
    CChineseCalendarConverter* iConverter;
    CCalenSolarTerms* iSolarTerms;

    };

#endif // __CALENLUNARINFOPROVIDER_H__
