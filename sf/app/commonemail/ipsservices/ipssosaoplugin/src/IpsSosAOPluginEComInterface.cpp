/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Main plugin class
*
*/

#include "ipssosaopluginheaders.h"

const TInt KIpsSosAOPluginCommandBufferGra = 1;


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOPluginEComInterface::CIpsSosAOPluginEComInterface( ) : 
    CAlwaysOnlineEComInterface(), 
    iBufferedAOCommands( KIpsSosAOPluginCommandBufferGra )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
EXPORT_C CIpsSosAOPluginEComInterface* CIpsSosAOPluginEComInterface::NewL( )
    {
    FUNC_LOG;
    CIpsSosAOPluginEComInterface* self = new (
            ELeave ) CIpsSosAOPluginEComInterface();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOPluginEComInterface::ConstructL()
    {
    FUNC_LOG;
    iSession = CMsvSession::OpenAsyncL( *this );
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsSosAOPluginEComInterface::CompleteConstructL()
    {
    FUNC_LOG;
    iSendWatcher = CIpsSosAOSmtpSendWatcher::NewL( 
            CActive::EPriorityStandard, *iSession );
    iImapPopLogic = CIpsSosAOImapPopLogic::NewL( *iSession );
    
    // send buffered command at this point when logic objects is created
    if ( iBufferedAOCommands.Count() > 0 )
        {
        for ( TInt i = 0; i < iBufferedAOCommands.Count(); i++ )
            {
            HandleServerCommandL( iBufferedAOCommands[i], NULL );
            }
        }
    iBufferedAOCommands.Reset();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
CIpsSosAOPluginEComInterface::~CIpsSosAOPluginEComInterface()
    {
    FUNC_LOG;
    delete iSendWatcher;
    delete iImapPopLogic;
    delete iSession;
    iBufferedAOCommands.Close();
    }
    
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
EXPORT_C TAny* CIpsSosAOPluginEComInterface::HandleServerCommandL( 
    TInt aCommand, 
    TDesC8* aParameters )
    {
    FUNC_LOG;
    TInt result = KErrNotSupported;
    if ( iSendWatcher && iImapPopLogic )
        {
        TInt imapPopResult = KErrNone;
        imapPopResult = iImapPopLogic->HandleAOServerCommandL( aCommand, aParameters );
        
        //don't care about smtp problems
        result = imapPopResult;
        }
    else
        {
        // buffer important commands if iSmtpLogic and iImapPopLogic 
        // is no created. This needed because plugin is created in 
        // boot and there might be situation where AO server is created
        // before MSV server
        if ( aCommand == EAOManagerPluginStart
             || aCommand == EAOManagerPluginStop
             || aCommand == EAOManagerNWOpsNotAllowed
             || aCommand == EAOManagerNWOpsAllowed
             || aCommand == EAOManagerStartedRoaming
             || aCommand == EAOManagerStoppedRoaming
             || aCommand == EAOManagerDiskSpaceAboveCritical
             || aCommand == EAOManagerDiskSpaceBelowCritical )
            {
            iBufferedAOCommands.AppendL( aCommand );
            result = KErrNone;
            }
        }
    return reinterpret_cast<TAny*>( result );
    }


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
void CIpsSosAOPluginEComInterface::HandleSessionEventL(    
    MMsvSessionObserver::TMsvSessionEvent aEvent,
    TAny* aArg1, 
    TAny* aArg2, 
    TAny* aArg3 )
    {
    FUNC_LOG;
    switch( aEvent )
        {
        case EMsvServerReady:
            CompleteConstructL();
            break;
        case EMsvEntriesCreated:
        case EMsvEntriesChanged:
        case EMsvEntriesDeleted:
        case EMsvEntriesMoved:
            // send only to ImapPopLogic, because
            // SmtpLogic not need these events
            iImapPopLogic->HandleMsvSessionEventL( 
                    aEvent, aArg1, aArg2, aArg3 );
            break;
        // handle these so that this process not panic if
        // CMsvServer panics and restarts
        case EMsvGeneralError:
        case EMsvCloseSession:
        case EMsvServerFailedToStart:
        case EMsvCorruptedIndexRebuilt:
        case EMsvServerTerminated:
            break;
        // are we intrest about these events at the moment
        // FSEmail not support msv store changes?
        case EMsvMtmGroupInstalled:
        case EMsvMtmGroupDeInstalled:
        case EMsvMediaChanged:
        case EMsvMediaUnavailable:
        case EMsvMediaAvailable:
        case EMsvMediaIncorrect:
        case EMsvCorruptedIndexRebuilding:
        default:
            break;
        };
    }

//EOF

