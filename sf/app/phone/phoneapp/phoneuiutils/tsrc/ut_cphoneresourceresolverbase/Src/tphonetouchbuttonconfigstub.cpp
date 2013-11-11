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
* Description: Touch button configuration stub.
*
*/

// INCLUDE FILES
#include <centralrepository.h>
#include <barsread.h>
#include <coemain.h>

#include "phoneresourceids.h"
#include "tphonetouchbuttonconfig.h"
#include "PhoneUI.pan"

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
    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( 
            KCRUidTelTouchButtonsVariation ) );

    if ( err == KErrNone )
        {
        for ( TInt i = 0; i < KTelButtonsConfigKeyCount; i ++ )
            {
            TInt key = MapToCenRepKey(i);
            if ( key != KErrNotFound )
                {
                err = repository->Get( key, aKeyValues[i] );
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

    delete repository;
    }

// ---------------------------------------------------------------------------
// ReadConfigResourceL
// ---------------------------------------------------------------------------
//
void TPhoneTouchButtonConfig::ReadConfigResourceL( 
    const TFixedArray<TInt,KTelButtonsConfigKeyCount>& aKeyValues )
    {
    if( aKeyValues[0] == 1 )
        {
     //   iConfiguration[0] = R_PHONEUI_INCOMING_CALL_SILENCE_BUTTONS;    
        }
    else if ( aKeyValues[0] == 0 )
        {
        iConfiguration[0] = R_PHONEUI_INCOMING_CALL_BUTTONS;
        }
    else
        {
        iConfiguration[0] = KErrNotFound;
        }
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
