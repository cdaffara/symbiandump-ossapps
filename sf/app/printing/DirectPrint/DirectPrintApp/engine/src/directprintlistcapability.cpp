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
#include <aknqueryvaluetext.h>
#include <aknpopupsettingpage.h>
#include <badesca.h>
#include <DirectPrintApp.rsg>

#include "directprintlistcapability.h"
#include "tprinter.h"
#include "directprintcapabilitycodes.h"

const TInt KArrayGranularity( 4 );

CDirectPrintListCapability::CDirectPrintListCapability()
	{
	}
	
// Destructor
CDirectPrintListCapability::~CDirectPrintListCapability()
    {
    iEnumIDs.Close();
    iTexts.ResetAndDestroy();
    }

// Creates text string for the settings list box
HBufC* CDirectPrintListCapability::ListBoxTextL()
    {        
    CDesCArrayFlat* strings = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( iTitle );

    TInt i( 0 );
    TInt num( iEnumIDs.Count() );
    TBool found( EFalse );
    // Find the matching text based on the enumeration value
    for ( i = 0; i < num && !found; i++ )
        {
        if ( iEnumIDs[i] == iValue )
            {
            strings->AppendL( iTexts[i]->Des() );
            found = ETrue;
            }
        }

    if ( !found )
        {
        User::Leave( KErrNotFound );
        }

    HBufC* buf = StringLoader::LoadL( R_QTN_LBOX_FORMAT, *strings ); 
    CleanupStack::PopAndDestroy();  // strings

    return buf;
    }

// Clones itself
CDirectPrintBaseCapability* CDirectPrintListCapability::CloneL()
	{
	CDirectPrintListCapability* clone = new ( ELeave ) CDirectPrintListCapability();
	CleanupStack::PushL( clone );

	clone->iValue = iValue;
	clone->iIndexOnList = iIndexOnList;
	clone->iTitle = iTitle;
	clone->iUid = iUid;

	TInt num( iEnumIDs.Count() );
	if ( iUid != EDirectPrintCapabLayout )
		{
		for ( TInt j = 0; j < num; j++ )
			{
			User::LeaveIfError( clone->iEnumIDs.Append( iEnumIDs[j] ) );
			User::LeaveIfError( clone->iTexts.Append( iTexts[j]->AllocLC() ) );
			CleanupStack::Pop();      // clone->iTexts
			}
		}

	CleanupStack::Pop(); // clone
	return clone;
	}

// Creates setting item for the settings list box
CAknSettingItem* CDirectPrintListCapability::SettingItemLC(TInt aIndex)
	{
	CAknEnumeratedTextPopupSettingItem* item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aIndex, iEnumIndex);
	CleanupStack::PushL(item);
	item->ConstructL(EFalse, aIndex, iTitle, NULL, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_PAGE, -1, 0, R_DIRECTPRINT_PRINTERSETTING_POPUP_SETTING_TEXTS);

	TInt count = iTexts.Count();
	if (count > 0)
		{
		item->EnumeratedTextArray()->ResetAndDestroy();
		}
	for (TInt i=0; i<count; i++ )
		{
		CAknEnumeratedText* enumText;
		enumText = new (ELeave) CAknEnumeratedText(i, iTexts[i]->AllocLC());
		CleanupStack::Pop();
		CleanupStack::PushL(enumText);
		item->EnumeratedTextArray()->AppendL(enumText);
		CleanupStack::Pop();

		if (iValue == iEnumIDs[i])
			{
			iEnumIndex = i;
			}
		}

	return item;
	}

//  End of File
