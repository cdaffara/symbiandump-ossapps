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
* Description: 
*     Contains pop specified sync logic
*
*/

#include "ipssosaopluginheaders.h"

// from settings
//<QMail>

//</QMail>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPopAgent::CIpsSosAOPopAgent(             
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId ) : CIpsSosAOBaseAgent(), 
    iSession(aSession), iOpResponse(aOpResponse),
    iServiceId( aServiceId ), 
    iSyncStartedSignaled( EFalse ),
    iState( EStateIdle ), iDoNotDisconnect( EFalse )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPopAgent::~CIpsSosAOPopAgent()
    {
    FUNC_LOG;
    Cancel();
    SignalSyncCompleted( iServiceId, iError );
    ClearSignaledFlags();
    delete iPopSettings;
    delete iOngoingOp;
    delete iSelection;
    delete iPopClientMtm;
    delete iMtmReg;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOPopAgent* CIpsSosAOPopAgent::NewL( 
    CMsvSession& aSession,
    MIpsSosAOAgentOperationResponse& aOpResponse,
    TMsvId aServiceId  )
    {
    FUNC_LOG;
    CIpsSosAOPopAgent* self = new (ELeave) CIpsSosAOPopAgent( 
            aSession,  aOpResponse, aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    
    iMtmReg = CClientMtmRegistry::NewL( iSession );
    CBaseMtm* bmtm = iMtmReg->NewMtmL( KUidMsgTypePOP3 );
    iPopClientMtm = static_cast<CPop3ClientMtm*>(bmtm);
    iSelection = new ( ELeave ) CMsvEntrySelection();
    iState = EStateIdle; 
    }
 
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::DoCancel()
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
void CIpsSosAOPopAgent::RunL()
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
             iState = EStatePopulateAll;
             SetActiveAndCompleteThis();
             break;
         case EStateConnectAndSyncOnHold:
             StartSyncL();
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
             NM_COMMENT("CIpsSosAOPopAgent: disconnecting");
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
             ClearSignaledFlags();
             iError = KErrNone;
             iState = EStateIdle;
             iSyncStartedSignaled = EFalse;
             break;
         case EStateIdle:
             break;
         default:
         break;
  
         }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOPopAgent::RunError( TInt aError )
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
        SetActiveAndCompleteThis();
        }
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::GetServerAddress( 
        TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const
    {
    FUNC_LOG;
    if ( !iPopSettings )
        {
        return;
        }
    TPtrC addr = iPopSettings->ServerAddress();
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
void CIpsSosAOPopAgent::GetUsername( 
        TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const
    {
    FUNC_LOG;
    if ( !iPopSettings )
        {
        return;
        }
    TPtrC8 usrn = iPopSettings->LoginName();
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
TBool CIpsSosAOPopAgent::IsConnected() const
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    if (iSession.GetEntry( iServiceId, service, tentry ) == KErrNone)
        {
        return tentry.Connected();
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent::TAgentState CIpsSosAOPopAgent::GetState() const
    {
    FUNC_LOG;
    return iState;
    }

// ----------------------------------------------------------------------------
// ---------------------------------------------------------------------------- 
void CIpsSosAOPopAgent::StartSyncL()
    {
    FUNC_LOG;
    if ( iOngoingOp )
        {
        User::Leave( KErrNotReady );
        }
    LoadSettingsL( );
    if ( !IsConnected() )
        {
        NM_COMMENT("CIpsSosAOPopAgent: starting sync");
        TBuf8<1> dummy;
        // connect and synchronise starts background sync or idle
        iSelection->ResizeL(0);
        iSelection->AppendL( iServiceId );
        iPopClientMtm->SwitchCurrentEntryL( iServiceId );
        iOngoingOp = iPopClientMtm->InvokeAsyncFunctionL(
                KPOP3MTMConnect, *iSelection, dummy, iStatus);
        SetActive();
        iState = EStateConnectAndSync;
        }
    else
        {
        NM_COMMENT("CIpsSosAOPopAgent: already connected do not sync");
        iState = EStateCompleted;
        iError = KErrCancel;
        SetActiveAndCompleteThis();
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::StartFetchMessagesL( 
        const RArray<TMsvId>& /*aFetchMsgArray*/ )
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::CancelAllAndDisconnectL()
    {
    FUNC_LOG;
    // if we are already idle state, do nothing
    // completing in idle state might cause unvanted events to ui
    if (iState == EStateIdle) 
        {
        return;
        }
    
    iDoNotDisconnect = EFalse;
    iState = EStateCompleted;
    if ( IsActive() )
        {
        Cancel();
        }

    if ( IsConnected() )
        {
        TBuf8<1> dummy;
        iSelection->ResizeL(0);
        iSelection->AppendL( iServiceId );
        iPopClientMtm->SwitchCurrentEntryL( iServiceId );
        TRAPD( error, iOngoingOp = iPopClientMtm->InvokeAsyncFunctionL(
            KPOP3MTMDisconnect, 
            *iSelection, dummy, iStatus) );
        
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
void CIpsSosAOPopAgent::CancelAllAndDoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = ETrue;
    if ( IsActive() )
        {
        Cancel();
        }
    SignalSyncCompleted( iServiceId, iError );
    iState = EStateCompleted;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::DoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = ETrue;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::ClearDoNotDisconnect()
    {
    FUNC_LOG;
    iDoNotDisconnect = EFalse;
    iSyncStartedSignaled = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::HoldOperations()
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
void CIpsSosAOPopAgent::ContinueHoldOperations()
    {
    FUNC_LOG;
    SetActiveAndCompleteThis();
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::PopulateAllL()
    {
    FUNC_LOG;
    if ( !IsConnected() )
        {
        SignalSyncCompleted( iServiceId, iError );
        CancelAllAndDisconnectL();
        }
    else if ( iPopulateLimit > 0 )
        {
        TImPop3PopulateOptions pop3GetMailInfo;
        // NOTE: SetMaxEmailSize and PopulationLimit are basicly same thing.
        // Populate limit using TOP and size is just bytes,
        pop3GetMailInfo.SetMaxEmailSize( KMaxTInt32 ); 
        pop3GetMailInfo.SetPopulationLimit( iPopulateLimit );
        TPckg<TImPop3PopulateOptions> params( pop3GetMailInfo );
    
        iSelection->ResizeL(0);
        iSelection->AppendL(iServiceId);
        iPopClientMtm->SwitchCurrentEntryL( iServiceId );
        iOngoingOp = iPopClientMtm->InvokeAsyncFunctionL(
                KPOP3MTMPopulateAll, *iSelection, params, iStatus);
        SetActive();
        iState = EStateDisconnect;
        SignalSyncStarted( iServiceId );
        }
    else
        {
        SetActiveAndCompleteThis();
        iState = EStateDisconnect;
        SignalSyncStarted( iServiceId );
        }

    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::LoadSettingsL( )
    {
    FUNC_LOG;
    delete iPopSettings;
    iPopSettings = NULL;
    iPopSettings = new ( ELeave ) CImPop3Settings();
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TPopAccount popAcc;
    accounts->GetPopAccountL(iServiceId, popAcc );
    accounts->LoadPopSettingsL( popAcc, *iPopSettings );
    CleanupStack::PopAndDestroy( accounts );
    
    iPopulateLimit = iPopSettings->PopulationLimit();
    if ( iPopulateLimit > 0 )
        {
        iPopulateLimit = ( iPopulateLimit * 1024 ) / 75;
        }
	//<QMail>
    
	//</QMail>
    else
        {
        iPopulateLimit = KMaxTInt;
        }
    
    
    }

// ----------------------------------------------------------------------------
// inline
// ----------------------------------------------------------------------------
void CIpsSosAOPopAgent::SetActiveAndCompleteThis()
    {
    FUNC_LOG;
    if ( !IsActive() )
        {
        SetActive();
        }
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

