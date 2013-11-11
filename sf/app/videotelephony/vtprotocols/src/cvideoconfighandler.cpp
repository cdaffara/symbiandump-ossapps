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


#include <pv_2way_symbian_interface.h>
#include <pv_mp4_h263_enc_extension_interface_symbian.h>
#include "mvtprotocolhandler.h"
#include "cvideoconfighandler.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// TVideoConfigHandler::TVideoConfigHandler
// Constructor.
// -----------------------------------------------------------------------------
//
TVideoConfigHandler::TVideoConfigHandler(MVtProtocolHandler* aProtocolHandler) : iProtocolHandler(aProtocolHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::TVideoConfigHandler<")));
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::TVideoConfigHandler>")));	
    }
    
// -----------------------------------------------------------------------------
// TVideoConfigHandler::MPvMp4H263EncExtensionCommandCompletedL
// Handle an event that has been generated.
// -----------------------------------------------------------------------------
//
void TVideoConfigHandler::MPvMp4H263EncExtensionCommandCompletedL(const CPVCmdResponse& aResponse)
    {
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::MPvMp4H263EncExtensionCommandCompletedL<")));	
    TVtCommandType cmdType = (TVtCommandType)aResponse.GetCmdType();
    TInt cmdId = aResponse.GetCmdId();
    TInt cmdResponse = aResponse.GetCmdStatus();
    TVtCommandResponse commandRsp(cmdType, cmdId, cmdResponse);
    iProtocolHandler->HandleVideoEncoderCommandCompletedL(commandRsp);
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::MPvMp4H263EncExtensionCommandCompletedL>")));	
    }

// -----------------------------------------------------------------------------
// TVideoConfigHandler::HandleMPvMp4H263EncExtensionInformationalEventL
// Handle an information event.
// -----------------------------------------------------------------------------
//
void TVideoConfigHandler::HandleMPvMp4H263EncExtensionInformationalEventL(const CPVAsyncInformationalEvent& aEvent)
    {
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::HandleMPvMp4H263EncExtensionInformationalEventL<")));		
    const TUint8* buffer = aEvent.GetLocalBuffer();
    TInt eventType = aEvent.GetEventType();
    TVtIndicationEvent vtEvent(eventType, buffer);
    iProtocolHandler->HandleVideoEncoderInformationalEventL(vtEvent);
    __IF_DEBUG(Print(_L("VTProto: TVideoConfigHandler::HandleMPvMp4H263EncExtensionInformationalEventL>")));	
    }
 
// -----------------------------------------------------------------------------
// CVideoConfigCommand::SetObserverL
// This API allows the user to specify separate observers for the extension interface.
// -----------------------------------------------------------------------------
//
void CVideoConfigCommand::SetObserverL(MVtProtocolHandler* aHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetObserverL<")));	
    iVideoConfigHandler = new (ELeave)TVideoConfigHandler(aHandler);
    iH263Encoder->SetObserver(iVideoConfigHandler);
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetObserverL>")));
    }

// -----------------------------------------------------------------------------
// CVideoConfigCommand::SetIntraFrameInterval
// Sets the I-Frame refresh rate of the encoded output.
// -----------------------------------------------------------------------------
//
TInt CVideoConfigCommand::SetIntraFrameInterval(TUint32 aIFrameInterval)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetIntraFrameInterval")));
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetIntraFrameInterval aIFrameInterval %d"), aIFrameInterval));			
    return iH263Encoder->SetIFrameInterval(aIFrameInterval);
    }

// -----------------------------------------------------------------------------
// CVideoConfigCommand::RequestNextIntraFrame
// Requests the encoder to encode the next frame as an I-Frame.
// -----------------------------------------------------------------------------
//
TInt CVideoConfigCommand::RequestNextIntraFrame()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::RequestNextIntraFrame")));
    return iH263Encoder->RequestIFrame();
    }

// -----------------------------------------------------------------------------
// CVideoConfigCommand::SetVideoFrameRate
// Sets the frame rate of encoded output for the specified layer.
// -----------------------------------------------------------------------------
//
TInt CVideoConfigCommand::SetVideoFrameRate(TUint32 aFrameRate)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetVideoFrameRate")));
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetVideoFrameRate aFrameRate %d"), aFrameRate));		
    return iH263Encoder->SetOutputFrameRate(aFrameRate);
    }
    

// -----------------------------------------------------------------------------
// CVideoConfigCommand::~CVideoConfigCommand
// Destructor.
// -----------------------------------------------------------------------------
//
CVideoConfigCommand::~CVideoConfigCommand()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::~CVideoConfigCommand")));	
    delete iVideoConfigHandler;
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::~CVideoConfigCommand")));
    }

// end of file
