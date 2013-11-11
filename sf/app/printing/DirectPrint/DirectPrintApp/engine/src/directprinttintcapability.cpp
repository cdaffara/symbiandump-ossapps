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
#include <akntextsettingpage.h>
#include <DirectPrintApp.rsg>

#include "directprinttintcapability.h"

const TInt KMaxTIntCharNum( 11 );

// Destructor
CDirectPrintTIntCapability::~CDirectPrintTIntCapability()
    {
    }

// Creates text string for the settings list box
HBufC* CDirectPrintTIntCapability::ListBoxTextL()
    {
    const TInt KGranularity( 4 );
    TBuf<KMaxTIntCharNum> numStr;
    numStr.Zero();
    numStr.AppendNum( iValue );

    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( iTitle );
    strings->AppendL( numStr );

    HBufC* buf = StringLoader::LoadL( R_QTN_LBOX_FORMAT, *strings );
    CleanupStack::PopAndDestroy();  // strings

    return buf;
    }

// Clones itself
CDirectPrintBaseCapability* CDirectPrintTIntCapability::CloneL()
    {
    CDirectPrintTIntCapability* clone = new ( ELeave ) CDirectPrintTIntCapability;

    clone->iIndexOnList = iIndexOnList;
    clone->iMax = iMax;
    clone->iMin = iMin;
    clone->iTitle = iTitle;
    clone->iUid = iUid;
    clone->iValue = iValue;

    return clone;
    }

// Creates setting item for the settings list box
CAknSettingItem* CDirectPrintTIntCapability::SettingItemLC(TInt aIndex)
	{
	CAknIntegerEdwinSettingItem* item = new (ELeave) CAknIntegerEdwinSettingItem(aIndex, iValue);
	CleanupStack::PushL(item);
	item->ConstructL(EFalse, aIndex, iTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);

	return item;
	}

//  End of File
