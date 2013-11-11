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
* Description:  Video configuration implementation
*
*/

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
// CVideoConfigCommand::SetObserverL
// This API allows the user to specify separate observers for the extension interface.
// -----------------------------------------------------------------------------
//
void CVideoConfigCommand::SetObserverL(MVtProtocolHandler* aHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::SetObserverL<")));	
    iVideoConfigHandler = new (ELeave)TVideoConfigHandler(aHandler);
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
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVideoConfigCommand::RequestNextIntraFrame
// Requests the encoder to encode the next frame as an I-Frame.
// -----------------------------------------------------------------------------
//
TInt CVideoConfigCommand::RequestNextIntraFrame()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoConfigCommand::RequestNextIntraFrame")));
    return KErrNone;
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
    return KErrNone;
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
