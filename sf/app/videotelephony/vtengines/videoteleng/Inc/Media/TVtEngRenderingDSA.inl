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
// TVtEngRenderingOptionsDSA::TVtEngRenderingOptionsDSA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsDSA::TVtEngRenderingOptionsDSA(
    MVtEngFrameObserver& aObserver,
    RWsSession& aWs,
    CWsScreenDevice& aScreenDevice,
    RWindowBase& aWindow,
    const TRect& aScreenRect,
    const TRect& aClipRect ) :
    iWs( aWs ),
    iWsSD( aScreenDevice ),
    iWindow( aWindow ),
    iRect( aScreenRect ),
    iClipRect( aClipRect ),
    iObserver( &aObserver )
    {
    }

// -----------------------------------------------------------------------------
// TVtEngRenderingOptionsDSA::TVtEngRenderingOptionsDSA
// C++ copy constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsDSA::TVtEngRenderingOptionsDSA(
    const TVtEngRenderingOptionsDSA& aOptions )
    : iWs( aOptions.iWs ),
      iWsSD( aOptions.iWsSD ),
      iWindow( aOptions.iWindow ),
      iRect( aOptions.iRect ),
      iClipRect( aOptions.iClipRect ),
      iObserver( aOptions.iObserver )
    {
    }

// End of File
