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
* Description: 
*     Contains imap specified sync logic
*
*/

#include "ipssosaopluginheaders.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent::CIpsSosAOImapAgent(             
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId) : CIpsSosAOBaseAgent(), 
    iSession(aSession), iOpResponse(aOpResponse),
    iServiceId( aServiceId ),
    iState( EStateIdle ), iDoNotDisconnect( EFalse )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent::~CIpsSosAOImapAgent()
    {
    FUNC_LOG;
    Cancel();
    SignalSyncCompleted( iServiceId, iError );
    ClearSignaledFlags();
    delete iImapSettings;
    delete iOngoingOp;
    delete iImapClientMtm;
    delete iMtmReg;
	//<QMail>

	//</QMail>
    iFoldersArray.Close();

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOImapAgent* CIpsSosAOImapAgent::NewL( 
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId )
    {
    FUNC_LOG;
    CIpsSosAOImapAgent* self = new (ELeave) CIpsSosAOImapAgent( 
            aSession,  aOpResponse, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    
    iMtmReg = CClientMtmRegistry::NewL( iSession );
    CBaseMtm* bmtm = iMtmReg->NewMtmL( KUidMsgTypeIMAP4 );
    iImapClientMtm = static_cast<CImap4ClientMtm*>(bmtm);
	//<QMail>
    
	//</QMail>
    iState = EStateIdle; 
    }
 
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::DoCancel()
    {
    FUNC_LOG;
    if (iOngoingOp )
        {
        iOngoingOp->Cancel();
        }
    delete iOngoingOp;
    iOngoingOp = NULL;
    
    SignalSyncCompleted( iServiceId, iError );
    ClearSignaledFlags();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::HandleImapConnectionEvent(
    TImapConnectionEvent aConnectionEvent )
    {
    FUNC_LOG;
    switch ( aConnectionEvent )
        {
        case EConnectingToServer:
            break;
        case ESynchronisingFolderList:
        case ESynchronisingInbox:
        case ESynchronisingFolders:
            SignalSyncStarted( iServiceId );
            break;
        case ESynchronisationComplete:
            break;
        case EDisconnecting:
            break;
        case EConnectionCompleted:
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::RequestResponseL( 
        TFSProgress /*aEvent*/, 
        TInt /*aRequestId*/ )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::RunL()
    {
    FUNC_LOG;
    if ( iOngoingOp )
        {
        // errors are not tolerated atm
        if (  iOngoingOp->iStatus.Int() != KErrNone )
            {
            iError = iOngoingOp->iStatus.Int();
            SignalSyncCompleted( iServiceId, iError );
            CancelAllAndDisconnectL();
            }
        delete iOngoingOp;
        iOngoingOp = NULL;
        }
    
    switch( iState )
         {
         case EStateConnectAndSync:
             iState = EStateRefreshFolderArray;
             SetActiveAndCompleteThis();
             break;
         case EStateConnectAndSyncOnHold:
             StartSyncL();
             break;
         case EStateRefreshFolderArray:
			 //<QMail>
             {
             CIpsSosAOSettingsHandler* settings = 
                     CIpsSosAOSettingsHandler::NewL(iSession, iServiceId);
             CleanupStack::PushL(settings);
             settings->GetSubscribedImapFoldersL( iServiceId , iFoldersArray );
             CleanupStack::PopAndDestroy(settings);
             }             
			 //</QMail>
             iState = EStatePopulateAll;
             SetActiveAndCompleteThis();
             break;
         case EStatePopulateAll:
             PopulateAllL();
             break;
         case EStatePopulateOnHold:
             PopulateAllL();
             break;
         case EStateFetchOngoing:
             break;
         case EStateFetchOnHold:
             break;
         case EStateDisconnect:
             NM_COMMENT("CIpsSosAOImapAgent: disconnecting");
             if ( !iDoNotDisconnect )
                 {
                 CancelAllAndDisconnectL();
                 }
             else
                 {
                 iState = EStateCompleted;
                 SetActiveAndCompleteThis();
                 }
             break;
         case EStateCompleted:
             NM_COMMENT("CIpsSosAOImapAgent: completed");
             TRAP_IGNORE( iOpResponse.OperationCompletedL( iError ) );
             SignalSyncCompleted( iServiceId, iError );
             iError = KErrNone;
             ClearSignaledFlags();
             iState = EStateIdle;
             break;
         case EStateIdle:
             break;
         default:
         break;
  
         }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOImapAgent::RunError( TInt aError )
    {
    FUNC_LOG;
    iError = aError;
    if ( IsConnected() && iState != EStateDisconnect )
        {
        TRAP_IGNORE( CancelAllAndDisconnectL() );
        }
    else if ( IsConnected() && iState == EStateDisconnect )
        {
        iSession.StopService( iServiceId );
        iState = EStateCompleted;
        SetActiveAndCompleteThis();
        }
    else
        {
        iState = EStateCompleted;
        iError = KErrCancel;
        SetActiveAndCompleteThis();
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::GetServerAddress( 
        TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const
    {
    FUNC_LOG;
    if ( !iImapSettings )
        {
        return;
        }
    TPtrC addr = iImapSettings->ServerAddress();
    __ASSERT_DEBUG( ( 
            addr.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( addr.Length() <= KIpsSosAOTextBufferSize )
        {
        aIncomingServer.Copy( addr );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::GetUsername( 
        TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const
    {
    FUNC_LOG;
    if ( !iImapSettings )
        {
        return;
        }
    TPtrC8 usrn = iImapSettings->LoginName();
    __ASSERT_DEBUG( ( 
            usrn.Length() <= KIpsSosAOTextBufferSize ), 
            User::Panic( KIpsSosAOPanicLit, KErrGeneral) );
    
    if ( usrn.Length() <= KIpsSosAOTextBufferSize )
        {
        aUsername.Copy( usrn );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOImapAgent::IsConnected() const
    {
    FUNC_LOG;
    TBool ret = EFalse;
    TMsvEntry tentry;
    TMsvId service;
    TInt err = iSession.GetEntry( iServiceId, service, tentry );
    if ( err == KErrNone )
    {
        ret = tentry.Connected();
    }
    return ret;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent::TAgentState CIpsSosAOImapAgent::GetState() const
    {
    FUNC_LOG;
    return iState;
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
void CIpsSosAOImapAgent::StartSyncL()
    {
    FUNC_LOG;
    if ( iOngoingOp )
        {
        User::Leave( KErrNotReady );
        }
    LoadSettingsL( );
    if ( !IsConnected() )
        {
        NM_COMMENT("CIpsSosAOImapAgent: starting sync");
        TPckg<MMsvImapConnectionObserver*> parameter(this);
        // connect and synchronise starts background sync or idle
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( sel );
        sel->AppendL( iServiceId );
        iImapClientMtm->SwitchCurrentEntryL( iServiceId );
        iOngoingOp = iImapClientMtm->InvokeAsyncFunctionL(
                KIMAP4MTMConnectAndSyncCompleteAfterFullSync, 
                *sel, parameter, iStatus);
        CleanupStack::PopAndDestroy( sel );
        SetActive();
        iState = EStateConnectAndSync;
        }
    else
        {
        NM_COMMENT("CIpsSosAOImapAgent: already connected do not sync");
        // do not do anything if we are connected, especially do never
        // try to sync if sync is is already started (ex. from ips plugin)
        // that cause problems with imap flags etc.
        iError = KErrCancel;
        iState = EStateCompleted;
        SetActiveAndCompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::StartFetchMessagesL( 
        const RArray<TMsvId>& /*aFetchMsgArray*/ )
    {
    FUNC_LOG;
    // implementation removed when not needed anywhere
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::CancelAllAndDisconnectL()
    {
    FUNC_LOG;
    // if we are already idle state, do nothing,
    // completing in idle state might cause unvanted events to ui
    if (iState == EStateIdle) 
        {
        return;
        }
    
    iDoNotDisconnect = EFalse;
    iState = EStateCompleted;
    iFoldersArray.Reset();
    if ( IsActive() )
        {
        Cancel();
        }

    if ( IsConnected() )
        {
        TBuf8<1> dummy;
        CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( sel );
        sel->AppendL( iServiceId );
        iImapClientMtm->SwitchCurrentEntryL( iServiceId );
        TRAPD( error, iOngoingOp = iImapClientMtm->InvokeAsyncFunctionL(
            KIMAP4MTMDisconnect, 
            *sel, dummy, iStatus) );
        CleanupStack::PopAndDestroy( sel );
        
        if ( error == KErrNone )
            {
            SetActive();
            }
        else
            {
            iSession.StopService( iServiceId );
            SetActiveAndCompleteThis();
            }
        }
    else
        {
        SetActiveAndCompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::CancelAllAndDoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = ETrue;
    if ( IsActive() )
        {
        Cancel();
        }
    iFoldersArray.Reset();
    SignalSyncCompleted( iServiceId, KErrCancel );
    iState = EStateCompleted;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::DoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = ETrue;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ClearDoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::HoldOperations()
    {
    FUNC_LOG;
    if ( IsActive() )
        {
        Cancel();
        }
    
    if ( iState == EStatePopulateAll )
        {
        iState = EStatePopulateOnHold;
        }
    else if ( iState == EStateFetchOngoing )
        {
        iState = EStateFetchOnHold;
        }
    else if ( iState == EStateConnectAndSync )
        {
        iState = EStateConnectAndSyncOnHold;
        }
    else 
        {
        iState = EStateIdle;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::ContinueHoldOperations()
    {
    FUNC_LOG;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::PopulateAllL()
    {
    FUNC_LOG;
    TImImap4GetPartialMailInfo info;
    //<QMail>
    CIpsSosAOSettingsHandler* settings = 
             CIpsSosAOSettingsHandler::NewL(iSession, iServiceId);
     CleanupStack::PushL(settings);
     settings->ConstructImapPartialFetchInfo( info, *iImapSettings );
     CleanupStack::PopAndDestroy(settings);	
	//</QMail>
    
    if ( !IsConnected() )
        {
        SignalSyncCompleted( iServiceId, iError );
        CancelAllAndDisconnectL();
        }
	//<QMail>
    else if ( iFoldersArray.Count() > 0 && info.iTotalSizeLimit 
            != KIpsSetDataHeadersOnly )
	//</QMail>
         {

         // only inbox is set, do we have to populate other folders also
         TMsvId id = iFoldersArray[0];
         CMsvEntry* cEntry = iSession.GetEntryL( id );
         CleanupStack::PushL( cEntry );
         CMsvEntrySelection* sel = cEntry->ChildrenWithTypeL( 
                 KUidMsvMessageEntry ); 
         CleanupStack::PushL( sel );
         
         info.iDestinationFolder = iFoldersArray[0];
         
         CIpsPlgTimerOperation* dummy = NULL;
         iImapClientMtm->SwitchCurrentEntryL( iServiceId );
         TFSMailMsgId mbox( KIpsPlgImap4PluginUidValue, iServiceId );
         iStatus = KRequestPending;
		 //<Qmail>
         iOngoingOp = CIpsPlgImap4PopulateOp::NewL(
                 iSession,
                 this->iStatus,
                 iServiceId,
                 *dummy,
                 info,
                 *sel,
                 mbox,
                 this,
                 0,
                 NULL );
         //</Qmail>
         iFoldersArray.Remove( 0 );
         SetActive();
         iState = EStatePopulateAll;
         CleanupStack::PopAndDestroy( 2, cEntry );
         
         }
     else
         {
         iState = EStateDisconnect;
         SignalSyncStarted( iServiceId );
         SetActiveAndCompleteThis();
         }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOImapAgent::LoadSettingsL( )
    {
    FUNC_LOG;
    delete iImapSettings;
    iImapSettings = NULL;
    iImapSettings = new ( ELeave ) CImImap4Settings();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TImapAccount imapAcc;
    accounts->GetImapAccountL(iServiceId, imapAcc );
    accounts->LoadImapSettingsL( imapAcc, *iImapSettings );
    CleanupStack::PopAndDestroy( accounts );
    }

// ----------------------------------------------------------------------------
// inline
// ----------------------------------------------------------------------------    
void CIpsSosAOImapAgent::SetActiveAndCompleteThis()
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        SetActive();
        }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }
