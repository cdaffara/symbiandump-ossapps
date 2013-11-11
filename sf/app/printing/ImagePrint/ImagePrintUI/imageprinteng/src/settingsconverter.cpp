/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <StringLoader.h>

#include "settingsconverter.h"
#include "ctintcapability.h"
#include "clistcapability.h"
#include "cfloatcapability.h"
#include "stringids.h"
#include "tprintcapability.h"
#include "imageprintvariant.h"
#include "clog.h"
#include "crealfactory.h"

const TInt KEngineLimitValue( -1 );
const TInt KMenuItemsRequired( 2 );

// Converts TCapability to CTIntCapability
CTIntCapability* SettingsConverter::AddTIntCapabilityLC(
    TPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("SettingsConverter::AddTIntCapabilityLC Begin");
    CTIntCapability* intC = new ( ELeave ) CTIntCapability;
    CleanupStack::PushL( intC );

    // If engine returns KEngineLimitValue as low/high limit then it means
    // that there is no minimum or maximum limit
    if ( aCapability.iLow == KEngineLimitValue )
        {
        intC->iMax = KMinTInt32;
        }
    else
        {
        intC->iMin = aCapability.iLow;
        }

    if ( aCapability.iHigh == KEngineLimitValue )
        {
        intC->iMax = KMaxTInt32;
        }
    else
        {
        intC->iMax = aCapability.iHigh;
        }

    // Initialize TInt capability and return it
    intC->iTitle = SettingsConverter::TextStringLC( aCapabUid )->Des();
    CleanupStack::PopAndDestroy();  // SettingsConverter text string
    intC->iIndexOnList = aIndexOnList;
    intC->iUid = aCapabUid;
    intC->iValue = aCapability.iDefaultValue;
	
	LOG("SettingsConverter::AddTIntCapabilityLC End");
    return intC;
    }

// Converts TCapability to CListCapability
CListCapability* SettingsConverter::AddListCapabilityLC(
    TPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid,
    CRealFactory* aFactory )
    {
    LOG("SettingsConverter::AddListCapabilityLC Begin");
    CListCapability* listC = new ( ELeave ) CListCapability( aFactory );
    CleanupStack::PushL( listC );
    listC->iIndexOnList = aIndexOnList;
    listC->iUid = aCapabUid;
 
	/* There's a problem with some usb printers:
	   They offer "normal" print quality twice and nothing else.
	   This has to be handled so that user is not shown the option
	   as he cannot change it anyway in this case */
	TBool qualityDuplicates = EFalse;
	if ( aCapabUid == EPrintCapabQuality && 
		 aCapability.iEnumCount == KMenuItemsRequired )
		{
		if ( aCapability.iEnumCodes[0] == aCapability.iEnumCodes[1] )
			{
			LOG("SettingsConverter::AddListCapabilityLC duplicate quality detected!");
			qualityDuplicates = ETrue;
			}
		} 
		 
    // If there is only one value for certain capability, empty
    // title is added so the capability is not visible on the
    // settings menu.
    if( aCapability.iEnumCount < KMenuItemsRequired || qualityDuplicates )
        {
        listC->iTitle = SettingsConverter::TextStringLC( 0 )->Des();
        }
    else
        {
        listC->iTitle = SettingsConverter::TextStringLC( aCapabUid )->Des();        
        }    

    CleanupStack::PopAndDestroy();  // SettingsConverter text string
    listC->iValue = aCapability.iDefaultValue;

    TBool enumLabels = listC->iTitle.CompareF( KNullDesC() ) !=  0;
    TInt i( 0 );
    TInt num( aCapability.iEnumCount );
    for ( i = 0; i < num; i++ )
        {
        User::LeaveIfError(
            listC->iEnumIDs.Append( aCapability.iEnumCodes[i] ) );

        // Find matching text for the enumeration if the capability is
        // visible on the settings view
#ifdef IMG_PRINT_DYNAMIC_PRINT_SETTINGS        
        if ( enumLabels || aCapability.iEnumCount < KMenuItemsRequired )
#else
		// Not for layout if no dynamic print settings
        if (( enumLabels || aCapability.iEnumCount < KMenuItemsRequired ) &&
            aCapabUid != EPrintCapabLayout )
#endif
            {
            LOG("SettingsConverter::AddListCapabilityLC Adding real value");
            User::LeaveIfError( listC->iTexts.Append(
                SettingsConverter::TextStringLC( aCapability.iEnumCodes[i] )));
            CleanupStack::Pop();    // SettingsConverter text string                   	
            }
        else
            {
            LOG("SettingsConverter::AddListCapabilityLC Adding EMPTY value");
            User::LeaveIfError( listC->iTexts.Append(
                StringLoader::LoadLC( R_EMPTY ) ) );
            CleanupStack::Pop();	// SettingsConverter text string
            }
        }

    if ( aCapability.iEnumCount > 0 )
        {
        i = 0;
        // Check that current active capability code is found from the
        // list of codes and activate first one if not found
        TInt num( aCapability.iEnumCount );
        while ( i < num &&
                listC->iValue != aCapability.iEnumCodes[i] )
            {
            i++;
            }
        if ( i >= aCapability.iEnumCount )
            {
            listC->iValue = aCapability.iEnumCodes[0];
            }
        }

	LOG("SettingsConverter::AddListCapabilityLC End");
    return listC;
    }

// Converts TCapability to CFloatCapability
CFloatCapability* SettingsConverter::AddFloatCapabilityLC(
    TPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("SettingsConverter::AddFloatCapabilityLC Begin");
    CFloatCapability* floatC = new ( ELeave ) CFloatCapability;
    CleanupStack::PushL( floatC );

    floatC->iIndexOnList = aIndexOnList;
    floatC->iUid = aCapabUid;
    floatC->iTitle = SettingsConverter::TextStringLC( aCapabUid )->Des();
    CleanupStack::PopAndDestroy();  // SettingsConverter text string

    floatC->SetValues( aCapability.iLow,
                       aCapability.iDefaultValue, aCapability.iHigh );
    
    LOG("SettingsConverter::AddFloatCapabilityLC End");
    return floatC;
    }

// Fetches text string based on the enumeration ID
HBufC* SettingsConverter::TextStringLC(
    TInt aId )
    {
    LOG1("SettingsConverter::TextStringLC Begin: %d", aId );
    HBufC* buf = 0;
    TBool found( EFalse );

    TInt i( 0 );
    for ( i = 0; i < KConverterTableCount && !found; i++ )
        {
        if ( aId == KIDConverterTable[i][0] )
            {
            buf = StringLoader::LoadLC( KIDConverterTable[i][1] );
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

	LOG("SettingsConverter::TextStringLC End");
    return buf;
    }

//  End of File
