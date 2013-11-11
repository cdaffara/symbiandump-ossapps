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
* Description:  H324 configuration implementation
*
*/


#include "mvtprotocolhandler.h"
#include "ch324confighandler.h"
#include "cvtuserinput.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif


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
// CH324ConfigCommand::SetVendorId
// Sets the vendor identification data.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetVendorId(TUint8 cc, TUint8 ext, TUint32 mc, const TDesC8* aProduct, const TDesC8* aVersion)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetVendorId")));		
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand
// This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand(TUint aLogicalChannel, TUint8 aTradeoff)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendVideoTemporalSpatialTradeoffCommand")));			
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication
// This API allows the user to send a videoTemporalSpatialTradeOff command to the peer.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication(TUint aLogicalChannel, TUint8 aTradeoff)
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SendVideoTemporalSpatialTradeoffIndication")));		
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetVideoResolutions
// This API allows the user to specify the supported resolutions for video for transmit and receive.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetSupportedResolutions()
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetSupportedResolutions")));	
        
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CH324ConfigCommand::SetFastCsupOptions
// This API allows the user to set options for fast call setup procedures.
// -----------------------------------------------------------------------------
//
TInt CH324ConfigCommand::SetFastCsupOptions( )
    {
    __IF_DEBUG(Print(_L("VTProto: CH324ConfigCommand::SetFastCsupOptions")));	
    return KErrNone;
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
