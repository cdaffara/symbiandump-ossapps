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

#include "directprinttextcapability.h"

// Destructor
CDirectPrintTextCapability::~CDirectPrintTextCapability()
	{
	}

HBufC* CDirectPrintTextCapability::ListBoxTextL()
	{
	const TInt KGranularity( 4 );

	CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( KGranularity );
	CleanupStack::PushL( strings );
	strings->AppendL( iTitle );
	strings->AppendL( iText );

	HBufC* buf = StringLoader::LoadL( R_QTN_LBOX_FORMAT, *strings );
	CleanupStack::PopAndDestroy();  // strings

	return buf;
	}

CDirectPrintBaseCapability* CDirectPrintTextCapability::CloneL()
	{
	CDirectPrintTextCapability* clone = new ( ELeave ) CDirectPrintTextCapability;

	clone->iIndexOnList = iIndexOnList;
	clone->iTitle = iTitle;
	clone->iUid = iUid;
	clone->iValue = iValue;
	clone->iText = iText;

	return clone;
	}

CAknSettingItem* CDirectPrintTextCapability::SettingItemLC(TInt aIndex)
	{
	CAknTextSettingItem* item = new (ELeave) CAknTextSettingItem(aIndex, iText);
	CleanupStack::PushL(item);
	item->SetEmptyItemTextL( _L("") );
	item->ConstructL(EFalse, aIndex, iTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_TEXT_SETTING_PAGE, -1);

	return item;
	}

//  End of File
