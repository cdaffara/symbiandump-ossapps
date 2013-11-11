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
*     Contains mailbox specified always online logic
*
*/

#include "ipssosaopluginheaders.h"

_LIT( KIpsAOPlgPanicLit, "IpsAOPlg");
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent* CIpsSosAOBaseAgent::CreateAgentL( 
        CMsvSession& aSession,
        MIpsSosAOAgentOperationResponse& aOpResponse,
        TMsvId aServiceId )
    {
    FUNC_LOG;
    TMsvEntry tentry;
    TMsvId service;
    aSession.GetEntry( aServiceId, service, tentry );
    
    CIpsSosAOBaseAgent* self = NULL;
    if ( tentry.iMtm.iUid == KSenduiMtmImap4UidValue )
        {
        CIpsSosAOImapAgent* imap = CIpsSosAOImapAgent::NewL( 
                aSession,  aOpResponse, aServiceId );
        self = static_cast<CIpsSosAOBaseAgent*>(imap);
        self->iMtmType = KSenduiMtmImap4Uid;
        }
    else if ( tentry.iMtm.iUid == KSenduiMtmPop3UidValue )
        {
        CIpsSosAOPopAgent* pop = CIpsSosAOPopAgent::NewL( 
                aSession,  aOpResponse, aServiceId );
        self = static_cast<CIpsSosAOBaseAgent*>(pop);
        self->iMtmType = KSenduiMtmPop3Uid;
        }
    else
        {
        User::Panic( KIpsAOPlgPanicLit, KErrGeneral );
        }
    
    return self;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent::CIpsSosAOBaseAgent() : 
    CActive( CActive::EPriorityStandard ), iSyncStartedSignaled( EFalse )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
CIpsSosAOBaseAgent::~CIpsSosAOBaseAgent()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOBaseAgent::ClearSignaledFlags()
    {
    FUNC_LOG;
    iSyncStartedSignaled = EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void CIpsSosAOBaseAgent::SignalSyncStarted( TInt aMailbox )
    {
    FUNC_LOG;
    if ( !iSyncStartedSignaled )
        {
        iSyncStartedSignaled = ETrue;
        SetPropertyEvent( 
                KIpsSosEmailSyncStarted, aMailbox, KErrNone );
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsSosAOBaseAgent::SignalSyncCompleted( TInt aMailbox, TInt aError )
    {
    FUNC_LOG;
    if ( iSyncStartedSignaled )
        {
        SetPropertyEvent( 
                KIpsSosEmailSyncCompleted, aMailbox, aError );
        iSyncStartedSignaled = EFalse;
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------       
void CIpsSosAOBaseAgent::SetPropertyEvent( 
        TInt aEvent, TInt aMailbox, TInt aError )
    {
    FUNC_LOG;
    TInt error = RProperty::Define( 
            KIpsPlgPropertyCatUid, 
        KIPSSosEventPropertyKey, RProperty::EByteArray,
        KAllowAllPolicy,
        KAllowWriteDeviceDataPolicy );
    
    TIpsPlgPropertyEvent pEvent(  aEvent, aMailbox, 
            GetFSPluginId(), aError );
    iPropertyBuf = pEvent;

    // if definition was success
    if ( error == KErrNone || 
         error == KErrAlreadyExists || 
         error == KErrPermissionDenied )
        {
        error = RProperty::Set(
                KIpsPlgPropertyCatUid, 
                KIPSSosEventPropertyKey, iPropertyBuf );
    
        }
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TBool CIpsSosAOBaseAgent::IsTypeImap4() const
    {
    FUNC_LOG;
    if ( iMtmType.iUid == KSenduiMtmImap4UidValue )
        {
        return ETrue;
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
TInt CIpsSosAOBaseAgent::GetFSPluginId() const
    {
    FUNC_LOG;
    TInt pluginId = 0;
    if ( iMtmType.iUid == KSenduiMtmImap4UidValue )
        {
        pluginId = IPSSOSIMAP4PLUGIN_IMPLEMENTATION_UID;
        }
    else if ( iMtmType.iUid == KSenduiMtmPop3UidValue )
        {
        pluginId = IPSSOSPOP3PLUGIN_IMPLEMENTATION_UID;
        }
    return pluginId;
    }

// End of file

