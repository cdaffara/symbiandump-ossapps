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
* Description:  Inline implementation for TVtEngRenderConfigNGA.
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVtEngRenderConfigNGA::TVtEngRenderConfigNGA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigNGA::TVtEngRenderConfigNGA()
    {
    Mem::FillZ( this, sizeof( TVtEngRenderConfigNGA ) );
    iType = ENgaRender;
    }

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDP::TVtEngRenderConfigNGA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigNGA::TVtEngRenderConfigNGA(
    const TVtEngRenderingOptionsNGA& aOptions )
    {
    iType = ENgaRender;
    iWsSession = &aOptions.iWs;
    //iWsSreenDevice = &aOptions.iWsSD;
    //iWindow = &aOptions.iWindow;
    //iObserver = aOptions.iObserver;
    //iRect = aOptions.iRect;
    //iClipRect = aOptions.iClipRect;
    //iOffset = aOptions.iOffset;
    iRemoteWindow = &aOptions.iWindow;
    }

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDP::operator=
//
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigNGA& TVtEngRenderConfigNGA::operator=(
    const TVtEngRenderConfig& aConfig )
    {
    iWsSession = aConfig.iWsSession;
   // iWsSreenDevice = aConfig.iWsSreenDevice;
   // iWindow = aConfig.iWindow;
    iObserver = aConfig.iObserver;
   // iRect = aConfig.iRect;
    //iClipRect = aConfig.iClipRect;
   // iOffset = aConfig.iOffset;
    iType = aConfig.iType;
    iRemoteWindow = aConfig.iRemoteWindow;
    return *this;
    }

// End of File
