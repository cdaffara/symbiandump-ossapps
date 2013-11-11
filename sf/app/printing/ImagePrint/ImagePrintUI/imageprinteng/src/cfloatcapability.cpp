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


#include <AknQueryDialog.h>
#include <StringLoader.h>
#include <imageprintengine.rsg>

#include "cfloatcapability.h"

const TInt KMaxTRealCharNum( 30 );
const TInt KDecimals( 2 );

// Destructor
CFloatCapability::~CFloatCapability()
    { 
    }

// Creates text string for the settings list box
HBufC* CFloatCapability::ListBoxTextL()
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

// Displays pop-up list for changing the active TInt value
TBool CFloatCapability::LaunchPopupListL()
    {    
    TReal aMin = 0;
    TReal aMax = TReal( iMaxNumerator )/TReal( iDenominator );

    CAknFloatingPointQueryDialog* dlg = 
        CAknFloatingPointQueryDialog::NewL( iRealValue );    
    CleanupStack::PushL( dlg );
    dlg->SetPromptL( iTitle );
    dlg->PrepareLC( R_GENERIC_FLOAT_QUERY_DLG );
    CleanupStack::Pop( dlg );
    dlg->SetMinimumAndMaximum( aMin, aMax );
	TBool valueChanged = dlg->RunLD();

    if ( valueChanged )
        {
        iValue = TInt( TReal ( iRealValue / TReal ( iDenominator ) ) );
        }

    return valueChanged;
    }

// Clones itself
CBaseCapability* CFloatCapability::CloneL()
    {
    CFloatCapability* clone = new ( ELeave ) CFloatCapability;    

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
TInt CFloatCapability::SetValues( 
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
TInt CFloatCapability::Numerator() const
    {
    return iValue;
    }

// Denominator
TInt CFloatCapability::Denominator() const
    {
    return iDenominator;
    }

//  End of File
