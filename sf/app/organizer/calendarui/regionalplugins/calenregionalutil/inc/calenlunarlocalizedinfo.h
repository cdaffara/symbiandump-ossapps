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


 
#ifndef __CALENLUNARLOCALIZEDINFO_H__
#define __CALENLUNARLOCALIZEDINFO_H__

#include <e32base.h>

class CCalenLunarLocalizer;

class CCalenLunarLocalizedInfo : public CBase
    {
public:
    enum TField
        {
        EFestival,
        ESolarTerm,
        EFullLunarDate,
        ELunarYear,
        ELunarMonthAndDay,
        EAnimalYear,
        EGregorianDate
        };
        

public: // public API 
    IMPORT_C static CCalenLunarLocalizedInfo* NewL();

    IMPORT_C virtual ~CCalenLunarLocalizedInfo();

    /**
     * Get specific field. This is useful, when you have ordered array of TFields 
     * and you want to process them in a loop.
     * If day didn't had e.g. festival or solar term, KNullDesC is returned for those.
     */
    IMPORT_C const TDesC& GetField( TField aField );
    
    IMPORT_C const TDesC& Festival();
    IMPORT_C const TDesC& SolarTerm();
    IMPORT_C const TDesC& FullLunarDate();
    IMPORT_C const TDesC& LunarYear();    
    IMPORT_C const TDesC& LunarMonthAndDay();    
    IMPORT_C const TDesC& AnimalYear();    
    IMPORT_C const TDesC& GregorianDate();

private:
    CCalenLunarLocalizedInfo();

    void ConstructL();

private:
    
    TPtrC iFestival;
    TPtrC iSolarTerm;
    TPtrC iAnimalYear;
    
    TBuf<50> iLunarYear;
    TBuf<50> iLunarMonthAndDay;
    TBuf<50> iFullLunarDate;
    TBuf<50> iGregorianDate;
private:
    friend class CCalenLunarLocalizer;
    friend class CCalenLunarVietnameseLocalizer;
    };

#endif // __CALENLUNARLOCALIZEDINFO_H__
