/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Local and remote video rendering configuration.
*
*/



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDP::TVtEngRenderConfigDP
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDP::TVtEngRenderConfigDP()
    {
    Mem::FillZ( this, sizeof( TVtEngRenderConfigDP ) );
    iType = EDpRender;
    }

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDP::TVtEngRenderConfigDP
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDP::TVtEngRenderConfigDP(
    const TVtEngRenderingOptionsDP& aOptions )
    {
    iType = EDpRender;
    iWsSession = &aOptions.iWs;
    iWsSreenDevice = &aOptions.iWsSD;
    iWindow = &aOptions.iWindow;
    iObserver = aOptions.iObserver;
    iRect = aOptions.iRect;
    iClipRect = aOptions.iClipRect;
    iOffset = aOptions.iOffset;
    iBackGround = 0;
    }

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDP::operator=
//
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDP& TVtEngRenderConfigDP::operator=(
    const TVtEngRenderConfig& aConfig )
    {
    iWsSession = aConfig.iWsSession;
    iWsSreenDevice = aConfig.iWsSreenDevice;
    iWindow = aConfig.iWindow;
    iObserver = aConfig.iObserver;
    iRect = aConfig.iRect;
    iClipRect = aConfig.iClipRect;
    iOffset = aConfig.iOffset;
    iType = aConfig.iType;
    return *this;
    }

// End of File
