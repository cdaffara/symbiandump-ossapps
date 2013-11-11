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
#include <pv_2way_h324m_interface.h>
#include "mvtprotocolhandler.h"
#include "ch324confighandler.h"
#include "cvtuserinput.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// Define image sizes.
// QCIF.
const TInt KVtProtoResolutionQCIFWidth = 176;
const TInt KVtProtoResolutionQCIFHeight = 144;

// SQCIF.
const TInt KVtProtoResolutionSQCIFWidth = 128;
const TInt KVtProtoResolutionSQCIFHeight = 96;

// -----------------------------------------------------------------------------
// TH324ConfigHandler::TH324ConfigHandler
// Constructor.
// -----------------------------------------------------------------------------
//
TH324ConfigHandler::TH324ConfigHandler(MVtProtocolHandler* aProtocolHandler) : iProtocolHandler(aProtocolHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::TH324ConfigHandler<")));	
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::TH324ConfigHandler>")));	
    }

// -----------------------------------------------------------------------------
// TH324ConfigHandler::MPVH324MConfigCommandCompletedL
// Handle an event that has been generated.
// -----------------------------------------------------------------------------
//
void TH324ConfigHandler::MPVH324MConfigCommandCompletedL(const CPVCmdResponse& aResponse)
    {
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::MPVH324MConfigCommandCompletedL<")));		
    TVtCommandType cmdType = (TVtCommandType)aResponse.GetCmdType();
    TInt cmdId = aResponse.GetCmdId();
    TInt cmdResponse = aResponse.GetCmdStatus();
    TVtCommandResponse commandRsp(cmdType, cmdId, cmdResponse); 
    iProtocolHandler->HandleH324MConfigCommandCompletedL(commandRsp);
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::MPVH324MConfigCommandCompletedL>")));	
    }

// -----------------------------------------------------------------------------
// TH324ConfigHandler::MPVH324MConfigInformationalEventL
// Handle an information event.
// -----------------------------------------------------------------------------
//
void TH324ConfigHandler::MPVH324MConfigInformationalEventL(const CPVAsyncInformationalEvent& aEvent)
    {
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::MPVH324MConfigInformationalEventL<")));	
    const TUint8* buffer = aEvent.GetLocalBuffer();
    TInt eventType = aEvent.GetEventType();
    TVtIndicationEvent vtEvent(eventType, buffer);
    iProtocolHandler->HandleH324MConfigInformationalEventL(vtEvent);
    __IF_DEBUG(Print(_L("VTProto: TH324ConfigHandler::MPVH324MConfigInformationalEventL>")));
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetVendorId
// Sets the vendor identification data.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetVendorId(TUint8 cc, TUint8 ext, TUint32 mc, const TDesC8* aProduct, const TDesC8* aVersion)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetVendorId")));		
    return iH324Config->SetVendor(cc, ext, mc, aProduct, aVersion);
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand
// This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand(TUint aLogicalChannel, TUint8 aTradeoff)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand")));			
    return iH324Config->SendVideoTemporalSpatialTradeoffCommand(aLogicalChannel, aTradeoff);
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication
// This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication(TUint aLogicalChannel, TUint8 aTradeoff)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication")));		
    return iH324Config->SendVideoTemporalSpatialTradeoffIndication(aLogicalChannel, aTradeoff);
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetVideoResolutions
// This API allows the user to specify the supported resolutions for video for transmit and receive.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetSupportedResolutions()
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetSupportedResolutions")));	
    RArray<TPVVideoResolutionRange> resolutions;
    TPVVideoResolutionRange qcif;
    qcif.first.iWidth = KVtProtoResolutionQCIFWidth;
    qcif.first.iHeight = KVtProtoResolutionQCIFHeight;
    qcif.last.iWidth = KVtProtoResolutionQCIFWidth;
    qcif.last.iHeight = KVtProtoResolutionQCIFHeight;
    TInt err( resolutions.Append( qcif ) );
    if ( err == KErrNone )
        {
        TPVVideoResolutionRange sqcif;
        sqcif.first.iWidth = KVtProtoResolutionSQCIFWidth;
        sqcif.first.iHeight = KVtProtoResolutionSQCIFHeight;
        sqcif.last.iWidth = KVtProtoResolutionSQCIFWidth;
        sqcif.last.iHeight = KVtProtoResolutionSQCIFHeight;
        err = resolutions.Append( sqcif );
        if ( err == KErrNone )
            {
            err = iH324Config->SetVideoResolutions( INCOMING, resolutions );
            }
        }
    resolutions.Close();
    
    return err;    
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetFastCsupOptions
// This API allows the user to set options for fast call setup procedures.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetFastCsupOptions( )
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetFastCsupOptions")));	
    TPVH324FastCsupOptions fastCsupOptions;
    fastCsupOptions.iEnableWnsrp = ETrue;
    fastCsupOptions.iEnableMona = EFalse;
    return iH324Config->SetFastCsupOptions(fastCsupOptions);
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SendUserInputMessageL
// Causes the vt2way to send the specified user input to the remote terminal using
// control channel.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SendUserInputMessageL(MVTUserInput& user_input)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendUserInputMessageL<")));	
    if ( user_input.GetType() == EVtUiiDTMF )
        {
        CVTUserInputDtmf* dtmf = static_cast<CVTUserInputDtmf*>(&user_input);
        return iH324Config->SendUserInputL(*dtmf->iUII);
        }
    else if (user_input.GetType() == EVtUiiAlphaNumeric)
        {
        CVTUserInputAlphanumeric* alphanumeric = static_cast<CVTUserInputAlphanumeric*>(&user_input);
        return iH324Config->SendUserInputL(*alphanumeric->iUII);
        }
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendUserInputMessageL>")));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetObserverL
// This API allows the user to specify observers for the 324m interface.
// -----------------------------------------------------------------------------
//
void CH324ConfigCommand::SetObserverL(MVtProtocolHandler* aHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetObserverL<")));	 	
    iH324ConfigHandler = new (ELeave)TH324ConfigHandler(aHandler);
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetObserverL observer %d"), iH324ConfigHandler));
    iH324Config->SetObserver(iH324ConfigHandler);
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetObserverL>")));	 
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::~CH324ConfigCommand
// Destructor.
// -----------------------------------------------------------------------------
//
CH324ConfigCommand::~CH324ConfigCommand()
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::~CH324ConfigCommand<")));		
    delete iH324ConfigHandler;
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::~CH324ConfigCommand>")));	
    }    

// end of file
