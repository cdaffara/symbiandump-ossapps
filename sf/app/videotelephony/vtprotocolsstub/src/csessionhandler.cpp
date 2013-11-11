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
* Description:  Session command implementation
*
*/

#include "mvtprotocolhandler.h"
#include "cdatasinkproxy.h"
#include "cdatasourceproxy.h"
#include "csessionhandler.h"
#include "ccommserver.h"
#include "ch324confighandler.h"
#include "cvideoconfighandler.h"
#include "caudioconfighandler.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// TSessionHandler::TSessionHandler
// Constructor.
// -----------------------------------------------------------------------------
//
TSessionHandler::TSessionHandler(MVtProtocolHandler* aProtocolHandler) : iProtocolHandler(aProtocolHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::TSessionHandler<")));	
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::TSessionHandler>")));	
    }

   
// -----------------------------------------------------------------------------
// CSessionCommand::CSessionCommand
// Constructor.
// -----------------------------------------------------------------------------
//
CSessionCommand::CSessionCommand(TSessionHandler* aSessionHandler) 
  : iSessionHandler(aSessionHandler) 
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::CSessionCommand<")));
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::CSessionCommand>")));
    }

// -----------------------------------------------------------------------------
// CSessionCommand::~CSessionComman
// Destructor.
// -----------------------------------------------------------------------------
//
CSessionCommand::~CSessionCommand()
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::~CSessionCommand<")));	
    delete iVideoSource;
    delete iDisplaySink;
    delete iSessionHandler;
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::~CSessionCommand>")));
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ConnectToProtocolL
// This function can be invoked only in the ESetup state.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ConnectToProtocolL(MCommServer* aComm)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL")));		
    CCommServer* commServer = static_cast<CCommServer*>(aComm);
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL commServer %d"), commServer));
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSessionCommand::DisconnectFromProtocolL
// The Disconnect call is valid only when invoked in the EConnecting, and
// EConnected states.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::DisconnectFromProtocolL()
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::DisconnectFromProtocolL")));			
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ResetProtocolL
// This function is valid only in the ESetup and EInitializing state.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResetProtocolL()
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResetProtocolL")));	
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::InitProtocolL
// This function is valid only in the EIdle state.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::InitProtocolL(TVtInitInfo& aInitInfo)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::InitProtocolL")));	
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::AddVideoSinkL
// Allows an application to provide a media sink for rendering an incoming media bitstream in a
// logical channel of communication with the peer.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::AddVideoSinkL(const TUint aChannelId, MVTVideoSink& aDataSink)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddVideoSinkL")));	
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddVideoSinkL aChannelId %d"), aChannelId));
    iDisplaySink =CDisplaySinkProxy::NewL(&aDataSink);
    return KErrNone;        
    }

// -----------------------------------------------------------------------------
// CSessionCommand::AddVideoSourceL
// Allows an application to provide a media source to be associated with a logical channel
// of communication with the peer.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::AddVideoSourceL(const TUint aChannelId, MVTVideoSource& aDataSource)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddVideoSourceL")));	
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddVideoSourceL aChannelId %d"), aChannelId));
    iVideoSource = CVideoSourceProxy::NewL(&aDataSource);
    return KErrNone;        
    }

// -----------------------------------------------------------------------------
// CSessionCommand::AddAudioSourceL
// Allows an application to provide a media source to be associated with a logical channel
// of communication with the peer.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::AddAudioSourceL(const TUint aChannelId, MVTAudioSource &aDataSource)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddAudioSourceL")));	
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddAudioSourceL aChannelId %d"), aChannelId));	
    CAudioSourceProxy* audioSourceProxy = static_cast<CAudioSourceProxy*>(&aDataSource);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::AddAudioSinkL
// Allows an application to provide a media sink for rendering an incoming media bitstream in a
// logical channel of communication with the peer.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::AddAudioSinkL(const TUint aChannelId, MVTAudioSink &aDataSink)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddAudioSinkL")));	
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::AddAudioSinkL aChannelId %d"), aChannelId));	
    CAudioSinkProxy* audioSinkProxy = static_cast<CAudioSinkProxy*>(&aDataSink);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::PauseVideoL
// For an incoming track (MVTVideoSink) this function  pauses sending
// media to the sink (output device) and stops the sink.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::PauseVideoL(MVTVideoSource& aTrack)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::PauseVideoL")));		
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSessionCommand::PauseVideoL
// For an incoming track (MVtVideoSink) this function  pauses sending
// media to the sink (output device) and stops the sink.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::PauseVideoL(MVTVideoSink& aTrack) 
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::PauseVideoL")));		
    return KErrNone;     
    }        

// -----------------------------------------------------------------------------
// CSessionCommand::PauseAudioL
// For an incoming track (MVTAudioSource) this function  pauses sending
// media to the sink (output device) and stops the sink.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::PauseAudioL(MVTAudioSource& aTrack)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::PauseAudioL")));		
    CAudioSourceProxy* audioSourceProxy = static_cast<CAudioSourceProxy*>(&aTrack);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ResumeVideoL
// Resume a previously paused incoming or outgoing track.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResumeVideoL(MVTVideoSource& aTrack)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResumeVideoL")));		
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ResumeVideoL
// Resume a previously paused incoming or outgoing track.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResumeVideoL(MVTVideoSink& aTrack) 
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResumeVideoL")));		
    return KErrNone;     
    } 

// -----------------------------------------------------------------------------
// CSessionCommand::ResumeAudioL
// Resume a previously paused incoming or outgoing track.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResumeAudioL(MVTAudioSource& aTrack)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResumeAudioL")));		
    CAudioSourceProxy* audioSourceProxy = static_cast<CAudioSourceProxy*>(&aTrack);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::CancelAllCommandsL
// This API is to allow the user to cancel all pending requests.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::CancelAllCommandsL( )
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::CancelAllCommandsL")));	
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::GetProtocolInterfaceL
// This API is to allow for extensibility of the protocol interface.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::GetProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand*& aProtocolCommand)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::GetProtocolInterfaceL<")));		
    TInt commandId = 0;
    if (aType == EVtH324Config)
        {
        CH324ConfigCommand* H324ConfigCommand = new (ELeave)CH324ConfigCommand();
                
        aProtocolCommand = H324ConfigCommand;
        }
    else if (aType == EVtVideoEncoderConfig)
        {
        CVideoConfigCommand* videoConfigCommand = new (ELeave)CVideoConfigCommand();
                
        aProtocolCommand = videoConfigCommand;
        }
    else
        {
        ;
        }
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::GetProtocolInterfaceL aType %d, aProtocolCommand %d>"), aType, aProtocolCommand));    
    return commandId;
    }

// -----------------------------------------------------------------------------
// CSessionCommand::DeleteProtocolInterfaceL
// This APIis to be used to release an interface that was previously obtained using
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::DeleteProtocolInterfaceL(TVtConfigType aType, MVtProtocolCommand* aProtocolCommand)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::DeleteProtocolInterfaceL<")));
    TInt commandId = 0;
    if (aType == EVtH324Config)
            {
            CH324ConfigCommand* H324ConfigCommand = static_cast<CH324ConfigCommand*>(aProtocolCommand);
            
            delete H324ConfigCommand;
            }
        else if (aType == EVtVideoEncoderConfig)
            {
            CVideoConfigCommand* videoConfigCommand = static_cast<CVideoConfigCommand*>(aProtocolCommand);
            
            delete videoConfigCommand;
            }
        else
            {
            ;
            }
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::DeleteProtocolInterfaceL aType, aProtocolCommand %d>"), aType, aProtocolCommand));        
    return commandId;
    }

// end of file
