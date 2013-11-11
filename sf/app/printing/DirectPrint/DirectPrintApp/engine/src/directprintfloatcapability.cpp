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
#include <aknquerydialog.h>
#include <stringloader.h>
#include <DirectPrintApp.rsg>

#include "directprintfloatcapability.h"

const TInt KMaxTRealCharNum( 30 );
const TInt KDecimals( 2 );

// Destructor
CDirectPrintFloatCapability::~CDirectPrintFloatCapability()
    {
    }

// Creates text string for the settings list box
HBufC* CDirectPrintFloatCapability::ListBoxTextL()
    {   
    const TInt KGranularity( 4 );
    TRealFormat format( KMaxTRealCharNum, KDecimals );
    TBuf<KMaxTRealCharNum> numStr;
    
    numStr.Zero();
    numStr.AppendNum( iRealValue, format );

    CDesCArrayFlat* strings = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( strings );
    strings->AppendL( iTitle );
    strings->AppendL( numStr );

    HBufC* buf = StringLoader::LoadL( R_QTN_LBOX_FORMAT, *strings ); 
    CleanupStack::PopAndDestroy();  // strings
    
    return buf;
    }

// Clones itself
CDirectPrintBaseCapability* CDirectPrintFloatCapability::CloneL()
    {
    CDirectPrintFloatCapability* clone = new ( ELeave ) CDirectPrintFloatCapability;

    clone->iDenominator = iDenominator;
    clone->iIndexOnList = iIndexOnList;
    clone->iMaxNumerator = iMaxNumerator;
    clone->iValue = iValue;
    clone->iTitle = iTitle;
    clone->iUid = iUid;
    clone->iRealValue = iRealValue;

    return clone;
    }

// Sets the new values and checks the boundaries
TInt CDirectPrintFloatCapability::SetValues( 
    TInt aDenom, 
    TInt aNumerator, 
    TInt aMaxNumerator )
    {
    TInt err( KErrNone );

    if ( aNumerator > aMaxNumerator )
        {
        err = KErrArgument;
        }
    else
        {
        iDenominator = aDenom;
        iValue = aNumerator;
        iMaxNumerator = aMaxNumerator;
        if( iDenominator != 0 )
            {
            iRealValue = TReal( iValue )/TReal( iDenominator );
            }
        }

    return err;
    }

// Numerator
TInt CDirectPrintFloatCapability::Numerator() const
    {
    return iValue;
    }

// Denominator
TInt CDirectPrintFloatCapability::Denominator() const
    {
    return iDenominator;
    }

// Creates setting item for the settings list box
CAknSettingItem* CDirectPrintFloatCapability::SettingItemLC(TInt aIndex)
	{
	CAknIntegerEdwinSettingItem* item = new (ELeave) CAknIntegerEdwinSettingItem(aIndex, iValue); // Not Float Edwin
	CleanupStack::PushL(item);
	item->ConstructL(EFalse, aIndex, iTitle, NULL, R_DIRECTPRINT_PROPERTYSETTING_INTEGER_SETTING_PAGE, -1);

	return item;
	}

//  End of File
