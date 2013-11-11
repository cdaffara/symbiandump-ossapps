/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class handles mailbox sync state queries
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgSyncStateHandler* CIpsPlgSyncStateHandler::NewL( 
    CMsvSession& aSession, 
    CIpsPlgSosBasePlugin& aPlugin,
    RPointerArray<CIpsPlgSingleOpWatcher>& aOperationsRef )
    {
    FUNC_LOG;
    CIpsPlgSyncStateHandler* self = new( ELeave ) CIpsPlgSyncStateHandler( 
        aSession, aPlugin, aOperationsRef );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgSyncStateHandler::~CIpsPlgSyncStateHandler()
    {
    FUNC_LOG;
    iSyncingMailboxes.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgSyncStateHandler::CIpsPlgSyncStateHandler( 
    CMsvSession& aSession, 
    CIpsPlgSosBasePlugin& aPlugin,
    RPointerArray<CIpsPlgSingleOpWatcher>& aOperationsRef ) : 
    iSession( aSession ), iPlugin( aPlugin ), iOperationsRef( aOperationsRef )
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSyncStateHandler::ConstructL()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSyncStateHandler::HandlePropertyEventL( 
        TInt aEvent, TInt aMailbox, TInt aPluginId ,TInt /*aError*/ )
    {
    FUNC_LOG;
    // <qmail> removed sync-on-hold state
    if ( ( iPlugin.PluginId() == aPluginId ) &&
        ( aEvent == KIpsSosEmailSyncStarted || aEvent == KIpsSosEmailSyncCompleted ) )
        {
        AppendMailboxToSyncingMailboxL( aMailbox, aEvent );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TSSMailSyncState CIpsPlgSyncStateHandler::GetCurrentSyncState(
    const TFSMailMsgId& aMailboxId )
    {
    FUNC_LOG;
    TMsvEntry tEntry;
    TMsvId service;
    
    TInt err = iSession.GetEntry( aMailboxId.Id(), service, tEntry );

    if ( err != KErrNone )
        {
        return Idle;
        }
    
    RAlwaysOnlineClientSession aosession;
    
    err = aosession.Connect();
    if ( err == KErrNone )
        {
        
        TPckgBuf<TInt> idBuf( aMailboxId.Id() );
        TInt status = KErrNotFound;
        TPckgBuf<TInt> statusBuf( status );
        TInt error = aosession.SendReceiveSync(
            EServerAPIEmailQueryState, idBuf, statusBuf );
        status = statusBuf();
        if ( error == KErrNone && 
                status == EIpsAOPluginStatusSyncStarted )
            {
            aosession.Close();
            return EmailSyncing;
            }
        }
    aosession.Close();

    // If the mailbox is not online but it has some connection operation
    // already running, it means that it will be synchronized when the mailbox
    // goes online. So we need to check is there any connection operation
    // ongoing and return StartingSync in that case.
    if( !tEntry.Connected() )
        {
        if( ConnOpRunning( aMailboxId ) )
            {
            // Some connection operation already processing, so the sync
            // will start soon
            return StartingSync;
            }
        else
            {
            // no sync ongoing if not connected and no connection operations
            // started
            return Idle;
            }
        }

//#ifndef RD_101_EMAIL    
// <cmail> RD_IPS_AO_PLUGIN flaf removed

//#endif
    
// </cmail> 
   
   // found correct operation
   for ( TInt i = 0; i < iOperationsRef.Count(); i++ )
       {
       const CIpsPlgBaseOperation* baseOp = iOperationsRef[i]->BaseOperation();
       
       if ( baseOp && baseOp->FSMailboxId() == aMailboxId &&
            ( baseOp->IpsOpType() == EIpsOpTypePop3SyncOp ||
              baseOp->IpsOpType() == EIpsOpTypeImap4SyncOp ||
              baseOp->IpsOpType() == EIpsOpTypeImap4PopulateOp ) )
           {
           // Due to timing problems we might in some rare cases report
           // EmailSyncing here even if the actual syncing is already
           // finsihed, because the operation is removed from the array
           // with an async function call. HandlePropertyEventL events
           // seem to be even more unreliable (sync start event comes with
           // big delay), so we can't trust those either. Some kind of
           // redesign is needed to get this sync state query more reliable.
           return EmailSyncing;
           }
       }
    return Idle;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgSyncStateHandler::ConnOpRunning( const TFSMailMsgId& aMailBoxId  )
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iOperationsRef.Count(); i++ )
       {
       const CIpsPlgBaseOperation* baseOp = iOperationsRef[i]->BaseOperation();

       if ( baseOp && baseOp->FSMailboxId() == aMailBoxId &&
              ( baseOp->IpsOpType() == EIpsOpTypePop3SyncOp
               || baseOp->IpsOpType() == EIpsOpTypeImap4SyncOp
               || baseOp->IpsOpType() == EIpsOpTypeOnlineOp
               || baseOp->IpsOpType() == EIpsOpTypeImap4PopulateOp ) )
           {
           // Due to timing problems we might in some rare cases report
           // EmailSyncing here even if the actual syncing is already
           // finsihed, because the operation is removed from the array
           // with an async function call. HandlePropertyEventL events
           // seem to be even more unreliable (sync start event comes with
           // big delay), so we can't trust those either. Some kind of
           // redesign is needed to get this sync state query more reliable.
           return ETrue;
           }
       }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------    
TInt CIpsPlgSyncStateHandler::FindSyncingMailbox( TMsvId aMailbox )
    {
    FUNC_LOG;
    TInt index = KErrNotFound;
    index = FindMailbox( aMailbox );
    
    if ( index == KErrNotFound )
        {
        TInt ipsState = KErrNotFound;
        TSSMailSyncState state = GetCurrentSyncState(
            TFSMailMsgId( iPlugin.PluginId(), aMailbox ) );
        if ( state == EmailSyncing )
            {
            ipsState = KIpsSosEmailSyncStarted;
            }
        else
            {
            ipsState = KIpsSosEmailSyncCompleted;
            }
        // the next call, FindMailbox, handles the case
        // if .Append is failing, thus the return value can
        // be ignored
        TInt ignore = iSyncingMailboxes.Append( 
                TIpsMailboxState( aMailbox, ipsState ) );
        index = FindMailbox( aMailbox );
        }

    return index;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------  
TInt CIpsPlgSyncStateHandler::FindMailbox( TMsvId aMailbox )
    {
    FUNC_LOG;
    TInt index = KErrNotFound;
    for ( TInt i = iSyncingMailboxes.Count()-1; i >= 0; i-- )
        {
        if ( iSyncingMailboxes[i].iMailbox == aMailbox )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// ---------------------------------------------------------------------------
// --------------------------------------------------------------------------- 
void CIpsPlgSyncStateHandler::AppendMailboxToSyncingMailboxL( 
        TMsvId aMailbox, TInt aState )
    {
    FUNC_LOG;
    TInt index = FindMailbox( aMailbox );
    if ( index == KErrNotFound )
        {
        iSyncingMailboxes.AppendL( TIpsMailboxState( aMailbox, aState ) );
        }
    else
        {
        iSyncingMailboxes[index].iState = aState; // faulty CS warning
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgSyncStateHandler::GetMailboxIpsState( TMsvId aMailbox )
    {
    FUNC_LOG;
    TInt index = FindSyncingMailbox( aMailbox );
    TInt state = KErrNotFound;
    if ( index != KErrNotFound )
        {
        state = iSyncingMailboxes[index].iState; // faulty CS warning
        }
    return state;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSyncStateHandler::NotifyMailboxRemove( TMsvId aMailbox )
    {
    FUNC_LOG;
    TInt syncindex = FindMailbox( aMailbox );
    if ( syncindex != KErrNotFound )
        {
        iSyncingMailboxes.Remove(syncindex);
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgSyncStateHandler::SolveOpProtocolType( )
    {
    FUNC_LOG;
    TInt ret = EIpsOpTypeUnknown;
    TUint pluginId = iPlugin.PluginId();
    if (  pluginId == IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID )
        {
        ret = EIpsOpTypeImap4SyncOp;
        }
    else if ( pluginId == IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID )
        {
        ret = EIpsOpTypePop3SyncOp;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgSyncStateHandler::SaveSuccessfulSyncTimeL( 
        CMsvSession& /*aSession*/, TMsvId /*aService*/ )
    {
    FUNC_LOG;
    TTime now;
    now.HomeTime();
// <qmail>
    //CIpsSetDataExtension* extendedSettings = CIpsSetDataExtension::NewLC();
    //CIpsSetDataApi* dataApi = CIpsSetDataApi::NewL( aSession );
    //CleanupStack::PushL( dataApi );
    //dataApi->LoadExtendedSettingsL( aService, *extendedSettings );
    //TAOInfo info;
    //info.iLastSuccessfulUpdate = now;
    //info.iUpdateSuccessfulWithCurSettings = ETrue;
    //extendedSettings->SetLastUpdateInfo( info );
    // clear flag
    //extendedSettings->SetEmnReceivedButNotSyncedFlag( EFalse );
    //dataApi->SaveExtendedSettingsL( *extendedSettings );
    //CleanupStack::PopAndDestroy( 2, extendedSettings );
// </qmail>
    }


// End of file

