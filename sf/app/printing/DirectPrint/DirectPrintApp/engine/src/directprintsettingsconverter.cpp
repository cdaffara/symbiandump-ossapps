/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// System include files
#include <stringloader.h>

// User include files
#include "directprintsettingsconverter.h"
#include "directprinttintcapability.h"
#include "directprintlistcapability.h"
#include "directprintfloatcapability.h"
#include "directprinttextcapability.h"
#include "directprintstringids.h"
#include "directprintcapability.h"
#include "directprintvariant.h"
#include "clog.h"

const TInt KEngineLimitValue( -1 );
const TInt KMenuItemsRequired( 2 );

// Converts TDirectPrintCapability to CDirectPrintTIntCapability
CDirectPrintTIntCapability* DirectPrintSettingsConverter::AddTIntCapabilityLC(
    TDirectPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("DirectPrintSettingsConverter::AddTIntCapabilityLC Begin");
    CDirectPrintTIntCapability* intC = new ( ELeave ) CDirectPrintTIntCapability;
    CleanupStack::PushL( intC );

    // If engine returns KEngineLimitValue as low/high limit then it means
    // that there is no minimum or maximum limit
    if ( aCapability.iLow == KEngineLimitValue )
        {
        intC->iMin = KMinTInt32;
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
    intC->iTitle = DirectPrintSettingsConverter::TextStringLC( aCapabUid )->Des();
    CleanupStack::PopAndDestroy();  // SettingsConverter text string
    intC->iIndexOnList = aIndexOnList;
    intC->iUid = aCapabUid;
    intC->iValue = aCapability.iDefaultValue;
	
	LOG("DirectPrintSettingsConverter::AddTIntCapabilityLC End");
    return intC;
    }

// Converts TDirectPrintCapability to CDirectPrintListCapability
CDirectPrintListCapability* DirectPrintSettingsConverter::AddListCapabilityLC(
    TDirectPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("DirectPrintSettingsConverter::AddListCapabilityLC Begin");
    CDirectPrintListCapability* listC = new ( ELeave ) CDirectPrintListCapability();
    CleanupStack::PushL( listC );
    listC->iIndexOnList = aIndexOnList;
    listC->iUid = aCapabUid;
 
	/* There's a problem with some usb printers:
	   They offer "normal" print quality twice and nothing else.
	   This has to be handled so that user is not shown the option
	   as he cannot change it anyway in this case */
	TBool qualityDuplicates = EFalse;
	if ( aCapabUid == EDirectPrintCapabQuality && 
		 aCapability.iEnumCount == KMenuItemsRequired )
		{
		if ( aCapability.iEnumCodes[0] == aCapability.iEnumCodes[1] )
			{
			LOG("DirectPrintSettingsConverter::AddListCapabilityLC duplicate quality detected!");
			qualityDuplicates = ETrue;
			}
		} 
		 
    // If there is only one value for certain capability, empty
    // title is added so the capability is not visible on the
    // settings menu.
    if( aCapability.iEnumCount < KMenuItemsRequired || qualityDuplicates )
        {
        listC->iTitle = DirectPrintSettingsConverter::TextStringLC( 0 )->Des();
        }
    else
        {
        listC->iTitle = DirectPrintSettingsConverter::TextStringLC( aCapabUid )->Des();
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
#ifdef DIRECT_PRINT_DYNAMIC_PRINT_SETTINGS        
        if ( enumLabels || aCapability.iEnumCount < KMenuItemsRequired )
#else
		// Not for layout if no dynamic print settings
        if (( enumLabels || aCapability.iEnumCount < KMenuItemsRequired ) &&
            aCapabUid != EDirectPrintCapabLayout )
#endif
            {
            LOG("DirectPrintSettingsConverter::AddListCapabilityLC Adding real value");
            User::LeaveIfError( listC->iTexts.Append(
                DirectPrintSettingsConverter::TextStringLC( aCapability.iEnumCodes[i] )));
            CleanupStack::Pop();    // SettingsConverter text string 
            }
        else
            {
            LOG("DirectPrintSettingsConverter::AddListCapabilityLC Adding EMPTY value");
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

	LOG("DirectPrintSettingsConverter::AddListCapabilityLC End");
    return listC;
    }

// Converts TDirectPrintCapability to CDirectPrintFloatCapability
CDirectPrintFloatCapability* DirectPrintSettingsConverter::AddFloatCapabilityLC(
    TDirectPrintCapability& aCapability,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("DirectPrintSettingsConverter::AddFloatCapabilityLC Begin");
    CDirectPrintFloatCapability* floatC = new ( ELeave ) CDirectPrintFloatCapability;
    CleanupStack::PushL( floatC );

    floatC->iIndexOnList = aIndexOnList;
    floatC->iUid = aCapabUid;
    floatC->iTitle = DirectPrintSettingsConverter::TextStringLC( aCapabUid )->Des();
    CleanupStack::PopAndDestroy();  // SettingsConverter text string

    floatC->SetValues( aCapability.iLow,
                       aCapability.iDefaultValue, aCapability.iHigh );
    
    LOG("DirectPrintSettingsConverter::AddFloatCapabilityLC End");
    return floatC;
    }

// Converts TDirectPrintCapability to CDirectPrintTextCapability
CDirectPrintTextCapability* DirectPrintSettingsConverter::AddTextCapabilityLC(
    TDirectPrintCapability& /*aCapability*/,
    TInt aIndexOnList,
    TInt aCapabUid )
    {
    LOG("DirectPrintSettingsConverter::AddTextCapabilityLC Begin");
    CDirectPrintTextCapability* textC = new ( ELeave ) CDirectPrintTextCapability;
    CleanupStack::PushL( textC );

    textC->iIndexOnList = aIndexOnList;
    textC->iUid = aCapabUid;
    textC->iTitle = DirectPrintSettingsConverter::TextStringLC( aCapabUid )->Des();
    CleanupStack::PopAndDestroy();  // SettingsConverter text string

    LOG("DirectPrintSettingsConverter::AddTextCapabilityLC End");
    return textC;
    }

// Fetches text string based on the enumeration ID
HBufC* DirectPrintSettingsConverter::TextStringLC(
    TInt aId )
    {
    LOG1("DirectPrintSettingsConverter::TextStringLC Begin: %d", aId );
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

	LOG("DirectPrintSettingsConverter::TextStringLC End");
    return buf;
    }

//  End of File
