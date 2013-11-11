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
#include <akntextsettingpage.h>
#include <imageprintengine.rsg>

#include "ctintcapability.h"

const TInt KMaxTIntCharNum( 11 );

// Destructor
CTIntCapability::~CTIntCapability()
    {
    }

// Creates text string for the settings list box
HBufC* CTIntCapability::ListBoxTextL()
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

// Displays pop-up list for changing the active TInt value
TBool CTIntCapability::LaunchPopupListL()
    {    
    CAknIntegerSettingPage* page = 
        new ( ELeave ) CAknIntegerSettingPage( 
            &iTitle, EAknSettingPageNoOrdinalDisplayed, EAknCtIntegerEdwin,
            R_DEF_INTEGER_EDITOR, 0, iValue );    
    CleanupStack::PushL( page );
    page->ConstructL();        
    CleanupStack::Pop();    // page
    page->IntegerEditorControl()->SetMinimumIntegerValue( iMin );
    page->IntegerEditorControl()->SetMaximumIntegerValue( iMax );
    TBool changed = page->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );

    return changed;
    }

// Clones itself
CBaseCapability* CTIntCapability::CloneL()
    {
    CTIntCapability* clone = new ( ELeave ) CTIntCapability;

    clone->iIndexOnList = iIndexOnList;
    clone->iMax = iMax;
    clone->iMin = iMin;
    clone->iTitle = iTitle;
    clone->iUid = iUid;
    clone->iValue = iValue;
    
    return clone;
    }

//  End of File
