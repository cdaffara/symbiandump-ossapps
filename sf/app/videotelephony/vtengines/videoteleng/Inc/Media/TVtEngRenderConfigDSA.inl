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
// TVtEngRenderConfigDSA::TVtEngRenderConfigDSA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDSA::TVtEngRenderConfigDSA()
    {
    Mem::FillZ( this, sizeof( TVtEngRenderConfigDSA ) );
    iType = EDsaRender;
    }

// -----------------------------------------------------------------------------
// TVtEngRenderConfigDSA::TVtEngRenderConfigDSA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDSA::TVtEngRenderConfigDSA(
    const TVtEngRenderingOptionsDSA& aOptions )
    {
    iType = EDsaRender;
    iWsSession = &aOptions.iWs;
    iWsSreenDevice = &aOptions.iWsSD;
    iWindow = &aOptions.iWindow;
    iObserver = aOptions.iObserver;
    iRect = aOptions.iRect;
    iClipRect = aOptions.iClipRect;
    }


// -----------------------------------------------------------------------------
// TVtEngRenderConfigDSA::operator=
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderConfigDSA& TVtEngRenderConfigDSA::operator=(
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
