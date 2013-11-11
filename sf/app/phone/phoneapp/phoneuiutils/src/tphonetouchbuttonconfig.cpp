/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Touch button configuration.
*
*/

// INCLUDE FILES
#include "cphonecenrepproxy.h"
#include <barsread.h>
#include <coemain.h>

#include "tphonetouchbuttonconfig.h"
#include "phoneui.pan"

// CenRep keys
const TUid KCRUidTelTouchButtonsVariation = {0x2001B2E6};
const TUint32 KTelButtonsIncomingCall = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
TPhoneTouchButtonConfig::TPhoneTouchButtonConfig()
    {
    for ( TInt i = 0; i < iConfiguration.Count(); i++ )
        {
        iConfiguration[i] = KErrNotFound;
        }
    }

// ---------------------------------------------------------------------------
// ReadConfiguration
// ---------------------------------------------------------------------------
//
TInt TPhoneTouchButtonConfig::ReadConfiguration()
    {
    TFixedArray<TInt,EPhoneButtonConfigCount> keys;
            
    TRAPD( err,
        {
        // read configuration key values
        ReadCenRepKeysL( keys );
        // read button set resource ids
        ReadConfigResourceL( keys );        
        } );
    
    return err;
    }

// ---------------------------------------------------------------------------
// ReadCenRepKeysL
// ---------------------------------------------------------------------------
//
void TPhoneTouchButtonConfig::ReadCenRepKeysL( 
    TFixedArray<TInt,KTelButtonsConfigKeyCount>& aKeyValues ) const
    {
    for ( TInt i = 0; i < KTelButtonsConfigKeyCount; i ++ )
        {
        TInt key = MapToCenRepKey(i);
        if ( key != KErrNotFound )
            {
            const TInt err = CPhoneCenRepProxy::Instance()->GetInt(
                    KCRUidTelTouchButtonsVariation, key, aKeyValues[i] );
            aKeyValues[i] -= 1; // map to range 0..n
            if ( err )
                {
                aKeyValues[i] = 0; // default
                }
            }
        else
            {
            aKeyValues[i] = 0; // default
            }
        }
    }

// ---------------------------------------------------------------------------
// ReadConfigResourceL
// ---------------------------------------------------------------------------
//
void TPhoneTouchButtonConfig::ReadConfigResourceL( 
    const TFixedArray<TInt,KTelButtonsConfigKeyCount>& /*aKeyValues*/ )
    {
    }

// ---------------------------------------------------------------------------
// MapToCenRepKey
// ---------------------------------------------------------------------------
//
TInt TPhoneTouchButtonConfig::MapToCenRepKey( TInt aButtons ) const
    {
    switch ( aButtons )
        {
        case EPhoneConfigIncomingCallButtons:
            return KTelButtonsIncomingCall;
        default:
            return KErrNotFound;
        }
    }

// ---------------------------------------------------------------------------
// ResourceId
// ---------------------------------------------------------------------------
//
TInt TPhoneTouchButtonConfig::ResourceId(
    TPhoneUIConfigurableButtons aButtons,
    TInt aDefaultResourceId )  const
    {
    if ( iConfiguration[aButtons] != KErrNotFound )
        {
        return iConfiguration[aButtons];    
        }
    else
        {
        return aDefaultResourceId;
        }
    
    }

//  End of File  
