/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <pv_plugin_interfaces.h>
#include <pv_2way_symbian_interface.h>
#include <pv_2way_h324m_interface.h>
#include <pv_mp4_h263_enc_extension_interface_symbian.h>
#include <pv_audio_output_control_interface.h>
#include <pv_2way_factory.h>
#include "mvtprotocolhandler.h"
#include "cdatasinkproxy.h"
#include "cdatasourceproxy.h"
#include "csessionhandler.h"
#include "ccommserver.h"
#include "ch324confighandler.h"
#include "cvideoconfighandler.h"
#include "caudioconfighandler.h"

#define DEBUG_CFG_BB_LOOPBACK 0
#define DEBUG_CFG_PV_LOOPBACK 0
#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

const TInt KVtProtoMultiplexingDelay = 150;
const TInt EVtGet324CtrlInterface = 15;
const TInt EVtGetH263EncInterface = 24;
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
// TSessionHandler::CommandCompletedL
// Handle an event that has been generated.
// -----------------------------------------------------------------------------
//
void TSessionHandler::CommandCompletedL(const CPVCmdResponse& aResponse)
    {
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::CommandCompletedL<")));	
    TVtCommandType cmdType = (TVtCommandType)aResponse.GetCmdType();
    TInt cmdId = (TInt)aResponse.GetCmdId();
    TInt cmdResponse = aResponse.GetCmdStatus();
    TVtCommandResponse commandRsp(cmdType, cmdId, cmdResponse);
    iProtocolHandler->HandleSessionCommandEventL(commandRsp);
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::CommandCompletedL>")));
    }

// -----------------------------------------------------------------------------
// TSessionHandler::HandleInformationalEventL
// Handle an informational event that has been generated.
// -----------------------------------------------------------------------------
//
void TSessionHandler::HandleInformationalEventL(const CPVAsyncInformationalEvent& aEvent)
    {
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::HandleInformationalEventL<")));		
    const TUint8* buffer = aEvent.GetLocalBuffer();
    TInt eventType = aEvent.GetEventType();
    TVtIndicationEvent vtEvent(eventType, buffer);
    iProtocolHandler->HandleSessionInformationalEventL(vtEvent);
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::HandleInformationalEventL>")));	
    }

// -----------------------------------------------------------------------------
// TSessionHandler::HandleErrorEventL
// Handle an error event that has been generated.
// -----------------------------------------------------------------------------
//
void TSessionHandler::HandleErrorEventL(const CPVAsyncErrorEvent& aEvent)
    {
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::HandleErrorEventL<")));		
    TVtErrorEvent event(aEvent.GetEventType());
    iProtocolHandler->HandleSessionErrorEventL(event);
    __IF_DEBUG(Print(_L("VTProto: TSessionHandler::HandleErrorEventL>")));	
    }
    
// -----------------------------------------------------------------------------
// CSessionCommand::CSessionCommand
// Constructor.
// -----------------------------------------------------------------------------
//
CSessionCommand::CSessionCommand(TSessionHandler* aSessionHandler, MPV2WaySymbianInterface* aPv2Way) 
  : iPv2Way(aPv2Way), iSessionHandler(aSessionHandler) 
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
    TPVConnectOptions connectOptions;
    connectOptions.iLoopbackMode = PV_LOOPBACK_NONE;
#ifdef _DEBUG
    if ( DEBUG_CFG_BB_LOOPBACK )
        {
        connectOptions.iLoopbackMode = PV_LOOPBACK_COMM;
        __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL Loopback COMM")));        
        }
    else if ( DEBUG_CFG_PV_LOOPBACK )
        {
        connectOptions.iLoopbackMode = PV_LOOPBACK_MUX;        
        __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL Loopback MUX")));
        }
    else
        {        
        __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL Loopback NONE")));
        }
#endif
    CCommServer* commServer = static_cast<CCommServer*>(aComm);
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ConnectToProtocolL commServer %d"), commServer));
    return iPv2Way->ConnectL(connectOptions, commServer->iComm);    
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
    return iPv2Way->DisconnectL();
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ResetProtocolL
// This function is valid only in the ESetup and EInitializing state.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResetProtocolL()
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResetProtocolL")));	
    return iPv2Way->ResetL();
    }

// -----------------------------------------------------------------------------
// CSessionCommand::InitProtocolL
// This function is valid only in the EIdle state.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::InitProtocolL(TVtInitInfo& aInitInfo)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::InitProtocolL")));	
    CPV2WayInitInfo initInfo;
    initInfo.iMultiplexingDelayMs = KVtProtoMultiplexingDelay;
    
    if (aInitInfo.iAMRWBOn)
        {
        initInfo.iIncomingAudioFormats.
        Append((TDesC8 *)&KVtAudioMIMETypeAMRWBIF2);

        initInfo.iOutgoingAudioFormats.
        Append((TDesC8 *)&KVtAudioMIMETypeAMRWBIF2);
        }
        
    initInfo.iIncomingAudioFormats.
    Append((TDesC8 *)&KVtAudioMIMETypeAMRIF2);

    initInfo.iOutgoingAudioFormats.
    Append((TDesC8 *)&KVtAudioMIMETypeAMRIF2);

    initInfo.iIncomingVideoFormats.
    Append( (TDesC8 *) &KVtVideoMIMETypeH264 );

    initInfo.iOutgoingVideoFormats.
    Append( (TDesC8 *) &KVtVideoMIMETypeH264 );

    if (!aInitInfo.iDisableMpeg4)
        {
        initInfo.iIncomingVideoFormats.
        Append((TDesC8 *)&KVtVideoMIMETypeM4V);
        
        initInfo.iOutgoingVideoFormats.
        Append((TDesC8 *)&KVtVideoMIMETypeM4V);
        }
        
    initInfo.iIncomingVideoFormats.
    Append((TDesC8 *)&KVtVideoMIMETypeH263);

    initInfo.iOutgoingVideoFormats.
    Append((TDesC8 *)&KVtVideoMIMETypeH263);

    return iPv2Way->InitL(initInfo);
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
    return iPv2Way->AddDataSinkL(aChannelId, *iDisplaySink);        
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
    return iPv2Way->AddDataSourceL(aChannelId, *iVideoSource);        
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
    return iPv2Way->AddDataSourceL(aChannelId, *(audioSourceProxy->iAudioSource));
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
    return iPv2Way->AddDataSinkL(aChannelId, *(audioSinkProxy->iAudioSink));
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
    if (&aTrack == iVideoSource->VideoSource())
        {
        return iPv2Way->PauseL(*iVideoSource);        
        }
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
    if (&aTrack == iDisplaySink->DisplaySink())
        {
        return iPv2Way->PauseL(*iDisplaySink);        
        }
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
    return iPv2Way->PauseL(*(audioSourceProxy->iAudioSource));
    }

// -----------------------------------------------------------------------------
// CSessionCommand::ResumeVideoL
// Resume a previously paused incoming or outgoing track.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::ResumeVideoL(MVTVideoSource& aTrack)
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::ResumeVideoL")));		
    if (&aTrack == iVideoSource->VideoSource())
        {
        return iPv2Way->ResumeL(*iVideoSource);        
        }
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
    if (&aTrack == iDisplaySink->DisplaySink())
        {
        return iPv2Way->ResumeL(*iDisplaySink);        
        }
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
    return iPv2Way->ResumeL(*(audioSourceProxy->iAudioSource));
    }

// -----------------------------------------------------------------------------
// CSessionCommand::CancelAllCommandsL
// This API is to allow the user to cancel all pending requests.
// -----------------------------------------------------------------------------
//
TInt CSessionCommand::CancelAllCommandsL( )
    {
    __IF_DEBUG(Print(_L("VTProto: CSessionCommand::CancelAllCommandsL")));	
    return iPv2Way->CancelAllCommandsL();
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
        commandId = iPv2Way->QueryInterfaceL(
                     KPVUidH324ConfigInterface,
                     ( MPVInterface*& ) H324ConfigCommand->PVH324ConfigInterface(),
                     ( void* ) EVtGet324CtrlInterface );
        
        aProtocolCommand = H324ConfigCommand;
        }
    else if (aType == EVtVideoEncoderConfig)
        {
        CVideoConfigCommand* videoConfigCommand = new (ELeave)CVideoConfigCommand();
        commandId = iPv2Way->QueryInterfaceL(
                        KPVUidVidEncConfigInterface,
                        ( MPVInterface*& ) videoConfigCommand->PVVideoEncoderInterface(),
                        ( void* ) EVtGetH263EncInterface  );
        
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
            commandId = iPv2Way->ReleaseInterfaceL(H324ConfigCommand->PVH324ConfigInterface());
            delete H324ConfigCommand;
            }
        else if (aType == EVtVideoEncoderConfig)
            {
            CVideoConfigCommand* videoConfigCommand = static_cast<CVideoConfigCommand*>(aProtocolCommand);
            commandId = iPv2Way->ReleaseInterfaceL(videoConfigCommand->PVVideoEncoderInterface());
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
