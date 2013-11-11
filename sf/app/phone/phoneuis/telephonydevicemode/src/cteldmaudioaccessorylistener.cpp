/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Accessory mode.
*
*/


//  INCLUDE FILES
#include    "cteldmaudioaccessorylistener.h"               // this
#include    "cteldmdebug.h"
#include    <TelephonyAudioRouting.h>

//  CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::NewL
//
// Two phase constructor
// -----------------------------------------------------------------------------
//
CTelDMAudioAccessoryListener* CTelDMAudioAccessoryListener::NewL()
    {    
    CTelDMAudioAccessoryListener* self = new ( ELeave ) CTelDMAudioAccessoryListener( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::~CTelDMAudioAccessoryListener
//
// -----------------------------------------------------------------------------
//
CTelDMAudioAccessoryListener::~CTelDMAudioAccessoryListener( )
    {
    FLOG( _L( "CTelDMAudioAccessoryListener::~CTelDMAudioAccessoryListener()" ) )
    delete iAudioRouting;
    iOutputArray.Reset();
    iOutputArray.Close();
    }
    
// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::CTelDMAudioAccessoryListener
//
// -----------------------------------------------------------------------------
//
CTelDMAudioAccessoryListener::CTelDMAudioAccessoryListener( ):
                    iIsActiveAccessory ( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::ConstructL
//
// -----------------------------------------------------------------------------
//
void CTelDMAudioAccessoryListener::ConstructL()
    {
    FLOG( _L( "CTelDMAudioAccessoryListener::ConstructL<" ) )
    // Audio route handler 
    iAudioRouting = CTelephonyAudioRouting::NewL( *this );
    FLOG( _L( "CTelDMAudioAccessoryListener::ConstructL>" ) )
    }

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::AvailableOutputsChanged
// Receives event on accessory mode change.
//
// -----------------------------------------------------------------------------
//
void CTelDMAudioAccessoryListener::AvailableOutputsChanged( CTelephonyAudioRouting& 
                                            aTelephonyAudioRouting )
    {
    FLOG( _L( "CTelDMAudioAccessoryListener::AvailableOutputsChanged<" ) )
    iOutputArray.Reset();
        
    TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputs 
                            = aTelephonyAudioRouting.AvailableOutputs();
    
    // Stores available output
    for ( TInt i=0; i< availableOutputs.Count();i++ )
        {
        TRAPD(err, iOutputArray.AppendL( availableOutputs[i] ));
        if(err != KErrNone) 
            {
            FTRACE( FPrint( _L( "CTelDMAudioAccessoryListener::AvailableOutputsChanged failed =%d"),
                err ))
            }
        }
    
    FLOG( _L( "CTelDMAudioAccessoryListener::AvailableOutputsChanged>" ) )  
    }
// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::OutputChanged
//
// -----------------------------------------------------------------------------
//  
void CTelDMAudioAccessoryListener::OutputChanged( CTelephonyAudioRouting& 
                                    aTelephonyAudioRouting)
    {
    FLOG( _L( "CTelDMAudioAccessoryListener::OutputsChanged<" ) )
    CTelephonyAudioRouting::TAudioOutput output = aTelephonyAudioRouting.Output();
    
        switch ( output )
        {
        // No active outputs available
        case CTelephonyAudioRouting::ENone:
            iIsActiveAccessory = EFalse;
            break;
        case CTelephonyAudioRouting::EHandset:
            iIsActiveAccessory = EFalse;
            break;
        case CTelephonyAudioRouting::ELoudspeaker:
            iIsActiveAccessory = EFalse;
            break;
        // Active outputs available
        case CTelephonyAudioRouting::EWiredAudioAccessory:
            iIsActiveAccessory = ETrue;
            break;
        case CTelephonyAudioRouting::EBTAudioAccessory:
            iIsActiveAccessory = ETrue;
            break;
        case CTelephonyAudioRouting::ETTY:
            iIsActiveAccessory = ETrue;
            break;          
        default:
            break;     
        }
    FTRACE( FPrint( _L( "CTelDMAudioAccessoryListener::OutputsChanged =%d"), 
                                                            iIsActiveAccessory ))
    FLOG( _L( "CTelDMAudioAccessoryListener::OutputsChanged>" ) )
    }
// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::SetOutputComplete
//
// -----------------------------------------------------------------------------
//  
void CTelDMAudioAccessoryListener::SetOutputComplete( CTelephonyAudioRouting& 
                                        /*aTelephonyAudioRouting*/, 
                                        TInt /*aError*/)
    {
    
    }

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::IsAnyActiveAccessory
//
// -----------------------------------------------------------------------------
//
TBool CTelDMAudioAccessoryListener::IsAnyActiveAccessory()
    {
    FTRACE( FPrint( _L( "CTelDMAudioAccessoryListener::IsAnyActiveAccessory =%d"), 
                                                            iIsActiveAccessory ))
    return iIsActiveAccessory;
    }

// -----------------------------------------------------------------------------
// CTelDMAudioAccessoryListener::IsAccessoryAttached
//
// -----------------------------------------------------------------------------
//
TBool CTelDMAudioAccessoryListener::IsAccessoryAttached()
    {
                
    TBool accessroryAttached = EFalse;
    
    for ( TInt i=0; i < iOutputArray.Count();i++ )
        {
        if ( iOutputArray[i] == CTelephonyAudioRouting::EWiredAudioAccessory ||
             iOutputArray[i] == CTelephonyAudioRouting::EBTAudioAccessory ||
             iOutputArray[i] == CTelephonyAudioRouting::ETTY )
            {
            accessroryAttached = ETrue;
            break;                
            } 
        }
    FTRACE( FPrint( _L( "CTelDMAudioAccessoryListener::IsAccessoryAttached =%d"), 
                                                            accessroryAttached ))
    return accessroryAttached;
    }
    
// End of file
