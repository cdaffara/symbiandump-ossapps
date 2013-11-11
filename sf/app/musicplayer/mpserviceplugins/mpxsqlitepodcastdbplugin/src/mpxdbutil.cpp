/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility functions for the podcast plugin
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <caf/data.h>
#include <caf/virtualpathptr.h>
#include <hash.h>
#include <apgcli.h>
#include <apmstd.h>

#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <mpxmediacollectiondetaildefs.h>
#include <mpxmedia.h>
#include <mpxmediaarray.h>
#include <mpxlog.h>

#include "mpxdbcommonstd.h"
#include "mpxdbcommonutil.h"
#include "mpxdbcommondef.h"

#include "mpxdbutil.h"
#include "mpxpodcastcollectiondbdef.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Append an item into the media array
// ----------------------------------------------------------------------------
//
void MPXDbUtil::AppendMediaL(
    CMPXMediaArray& aArray,
    const TDesC& aTitle,
    TMPXGeneralType aType,
    TMPXGeneralCategory aCat,
    TMPXPodcastType aPodcastType,
    TMPXPodcastCategory aPodcastCat,
    TMPXItemId aId,
    TInt aNonPermissibleActions)
    {
    MPX_FUNC("MPXDbUtil::AppendMediaL");

    RArray<TInt> supportedIds;
    CleanupClosePushL (supportedIds);
    supportedIds.AppendL (KMPXMediaIdGeneral);
    supportedIds.AppendL (KMPXMediaIdPodcast);
    CMPXMedia* entry = CMPXMedia::NewL(supportedIds.Array());
    CleanupStack::PushL (entry);
    entry->SetTextValueL (KMPXMediaGeneralTitle, aTitle);
    entry->SetTObjectValueL(KMPXMediaPodcastType, aPodcastType);
    entry->SetTObjectValueL(KMPXMediaPodcastCategoryGroup, aPodcastCat);
    entry->SetTObjectValueL(KMPXMediaGeneralId, aId);
    entry->SetTObjectValueL(KMPXMediaGeneralType, aType);
    entry->SetTObjectValueL(KMPXMediaGeneralCategory, aCat);

    // temp fix: Beryl BEGIN
    if ( aNonPermissibleActions != 0 )
        {
        // set non-permissible actions
        entry->SetTObjectValueL(KMPXMediaGeneralNonPermissibleActions,
            static_cast<TMPXGeneralNonPermissibleActions>(aNonPermissibleActions));
        }
    // temp fix: Beryl END
    aArray.AppendL (entry);
    CleanupStack::Pop (entry);
    CleanupStack::PopAndDestroy (&supportedIds);
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::PodcastFieldNameForCategoryL
// ----------------------------------------------------------------------------
//
TPtrC MPXDbUtil::PodcastFieldNameForCategoryL(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("MPXDbUtil::PodcastFieldNameForCategoryL");

    TPtrC ptr;
    switch (aCategory)
        {
        case EMPXArtist:
            ptr.Set(KMCPodcastAuthor);
            break;

        case EMPXAlbum:
            ptr.Set(KMCPodcastTitle);
            break;

        case EMPXGenre:
            ptr.Set(KMCPodcastGenre);
            break;

        case EMPXComposer:
            ptr.Set(KMCPodcastComposer);
            break;

        default:
            User::Leave(KErrNotSupported);
        }

    return ptr;
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::Today
// ----------------------------------------------------------------------------
//
TTime MPXDbUtil::Today()
    {
    MPX_FUNC("MPXDbUtil::Today");

    TTime now;  // in microseconds
    now.HomeTime();
    TDateTime todayDate = now.DateTime();

    todayDate.SetHour(0);
    todayDate.SetMinute(0);
    todayDate.SetSecond(0);
    todayDate.SetMicroSecond(0);

    return TTime(todayDate);
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::MonthStart
// ----------------------------------------------------------------------------
//
TTime MPXDbUtil::MonthStart()
    {
    MPX_FUNC("MPXDbUtil::MonthStart");

    TTime now;  // in microseconds
    now.HomeTime();
    TDateTime lastMonthStartDate = now.DateTime();

    lastMonthStartDate.SetDay(0);  // Day range from 0 to 30
    lastMonthStartDate.SetHour(0);
    lastMonthStartDate.SetMinute(0);
    lastMonthStartDate.SetSecond(0);
    lastMonthStartDate.SetMicroSecond(0);

    return TTime(lastMonthStartDate);
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::DayNoInWeek
// ----------------------------------------------------------------------------
//
TInt MPXDbUtil::DayNoInWeek()
    {
    MPX_FUNC("MPXDbUtil::DayNoInWeek");

    TTime now;  // in microseconds
    now.HomeTime();

    // Calculate number of days in this week
    TDay dayNo = now.DayNoInWeek();
    TDay firstDay = TLocale().StartOfWeek();
    TInt numDay(0);
    if (firstDay == ESunday)
        {
        numDay = dayNo + 1;
        }
    else
        {
        numDay = dayNo - firstDay;
        }

    return numDay;
    }

// ----------------------------------------------------------------------------
// MPXDbUtil::TableNameForCategoryL
// ----------------------------------------------------------------------------
//
TPtrC MPXDbUtil::TableNameForCategoryL(
    TMPXGeneralCategory aCategory)
    {
    MPX_FUNC("MPXDbUtil::TableNameForCategoryL");

    TPtrC ptr;
    switch (aCategory)
        {
        case EMPXArtist:
            ptr.Set(KMCAuthorTable);
            break;
        case EMPXAlbum:
            ptr.Set(KMCTitleTable);
            break;
        case EMPXGenre:
            ptr.Set(KMCCategoryTable);
            break;
        case EMPXComposer:
            ptr.Set(KMCComposerTable);
            break;
        default:
            User::Leave(KErrNotSupported);
        }

    return ptr;
    }

// End of File
