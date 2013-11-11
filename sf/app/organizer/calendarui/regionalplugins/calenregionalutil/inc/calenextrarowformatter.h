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


#ifndef __CALENEXTRAROWFORMATTER_H__
#define __CALENEXTRAROWFORMATTER_H__

#include <e32base.h>

#include "CalenLunarLocalizedInfo.h"
#include "CalendarVariant.hrh"
#include "calendarui_debug.h"

class CFont;

class CCalenExtraRowFormatter : public CBase
    {
public: // public API 
    IMPORT_C static CCalenExtraRowFormatter* NewL();

    IMPORT_C virtual ~CCalenExtraRowFormatter();

    IMPORT_C TPtrC FormatExtraRowInformationL( 
					CCalenLunarLocalizedInfo& aLocInfo, 
					RArray<CCalenLunarLocalizedInfo::TField>& aPrioritizedFields 
					);

private: // own methods
    CCalenExtraRowFormatter();

    void ConstructL();


private: // data
    TBuf<500> iText;
    };

#endif // __CALENEXTRAROWFORMATTER_H__
