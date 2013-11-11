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


#include "CalenLunarVietnameseLocalizer.h"

#include "CalenLunarLocalizer.h"
#include "CalenLunarInfo.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenExtraRowFormatter.h"

#include <calenlunarvietnameseplugindata.rsg>
#include <StringLoader.h>


// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::NewL
// -----------------------------------------------------------------------------
//
CCalenLunarVietnameseLocalizer* CCalenLunarVietnameseLocalizer::NewL()
    {
    CCalenLunarVietnameseLocalizer* self = new (ELeave) CCalenLunarVietnameseLocalizer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::~CCalenLunarVietnameseLocalizer
// -----------------------------------------------------------------------------
//
CCalenLunarVietnameseLocalizer::~CCalenLunarVietnameseLocalizer()
    {
    delete iYearOfCat;
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::CCalenLunarVietnameseLocalizer
// -----------------------------------------------------------------------------
//
CCalenLunarVietnameseLocalizer::CCalenLunarVietnameseLocalizer()
    {
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnameseLocalizer::ConstructL()
    {
    CCalenLunarLocalizer::ConstructL();
    iYearOfCat = StringLoader::LoadL( R_CALE_YEAR_OF_CAT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::LocalizeL
// -----------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo* CCalenLunarVietnameseLocalizer::LocalizeL( TCalenLunarInfo& aInfo )
    {    
    // 1) In Vietnamese variant, two festivals are not used
    // Seventh Night Festival
    // Eight Day of Twelfth Month Festival
    if ( aInfo.iFestival == TCalenLunarInfo::EFestivalSeventhNight || 
         aInfo.iFestival == TCalenLunarInfo::EFestivalTwelfthMonth )         
        {
        aInfo.iFestival = TCalenLunarInfo::ENoFestival;
        }
    
    // Use upper class to localize standard stuff
    CCalenLunarLocalizedInfo* localized = CCalenLunarLocalizer::LocalizeL( aInfo );

    // 2) Replace year of rabbit with year of cat. different logical strings.
    if ( aInfo.AnimalYear() == TCalenLunarInfo::EYearOfRabbit )
        {
        localized->iAnimalYear.Set( *iYearOfCat );
        }
    return localized; 
    }


// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::GetExtraRowTextL
// -----------------------------------------------------------------------------
//
TPtrC CCalenLunarVietnameseLocalizer::GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, TInt aMaxWidth, 
														const CFont& aFont, TBool aTwoLines)
    {
    // In Vietnamese, we don't show animal year in extra row at all, 
    // because it's in essence same name as lunar years terrestial branch. 
    // It's shown in details dialog though.
    RArray<CCalenLunarLocalizedInfo::TField> fields;
    CleanupClosePushL( fields );
    fields.AppendL( CCalenLunarLocalizedInfo::EFestival );
    fields.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );
    //fields.AppendL( CCalenLunarLocalizedInfo::EAnimalYear );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarYear );
    
    TPtrC text = iRowFormatter->FormatExtraRowInformationL( aLocInfo, fields, aMaxWidth, aFont

                                                            , aTwoLines 

                                                            );
    CleanupStack::PopAndDestroy( &fields );
    return text;
    }

// -----------------------------------------------------------------------------
// CCalenLunarVietnameseLocalizer::LocalizeMonthAndDayL
// -----------------------------------------------------------------------------
//
void CCalenLunarVietnameseLocalizer::LocalizeMonthAndDayL(
    CCalenLunarLocalizedInfo* aLocInfo,
    TCalenLunarInfo& aInfo)
    {
    // Lunar Month and Day
    TInt dateResource = aInfo.iLunarDate.iLeapMonth ? R_CALE_LUNAR_LEAP_DATE : R_CALE_LUNAR_DATE;
    TBuf<10> month;
    TBuf<10> day;
    month.Num( TInt64(aInfo.iLunarDate.iMonth) );
    day.Num( TInt64(aInfo.iLunarDate.iDay ));
    CPtrCArray* monthAndDaySubs = new (ELeave) CPtrCArray(2);
    CleanupStack::PushL( monthAndDaySubs );
    monthAndDaySubs->AppendL(month);
    monthAndDaySubs->AppendL(day);
    
    HBufC* tmp = StringLoader::LoadLC( dateResource, *monthAndDaySubs );
    aLocInfo->iLunarMonthAndDay = *tmp;
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( monthAndDaySubs );
    }

//EOF
