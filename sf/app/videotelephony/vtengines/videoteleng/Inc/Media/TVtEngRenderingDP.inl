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
// TVtEngRenderingOptionsDP::TVtEngRenderingOptionsDP
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsDP::TVtEngRenderingOptionsDP(
    MVtEngFrameObserver& aObserver,
    RWsSession& aWs,
    CWsScreenDevice& aScreenDevice,
    RWindowBase& aWindow,
    const TRect& aScreenRect,
    const TRect& aClipRect,
    const TPoint& aOffset ) :
    iWs( aWs ),
    iWsSD( aScreenDevice ),
    iWindow( aWindow ),
    iRect( aScreenRect ),
    iClipRect( aClipRect ),
    iOffset( aOffset ),
    iObserver( &aObserver )
    {
    }

// -----------------------------------------------------------------------------
// TVtEngRenderingOptionsDSA::TVtEngRenderingOptionsDP
// C++ copy constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsDP::TVtEngRenderingOptionsDP(
    const TVtEngRenderingOptionsDP& aOptions )
    : iWs( aOptions.iWs ),
      iWsSD( aOptions.iWsSD ),
      iWindow( aOptions.iWindow ),
      iRect( aOptions.iRect ),
      iClipRect( aOptions.iClipRect ),
      iOffset( aOptions.iOffset ),
      iObserver( aOptions.iObserver )
    {
    }

// End of File
