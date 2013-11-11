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
* Description:  Inline implementation for TVtEngRenderingOptionsNGA
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVtEngRenderingOptionsNGA::TVtEngRenderingOptionsNGA
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsNGA::TVtEngRenderingOptionsNGA(
    RWindow& aWindow, RWsSession& aWs) :
    iWindow( aWindow ),
    iWs( aWs )
    {
    }

// -----------------------------------------------------------------------------
// TVtEngRenderingOptionsNGA::TVtEngRenderingOptionsNGA
// C++ copy constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptionsNGA::TVtEngRenderingOptionsNGA(
    const TVtEngRenderingOptionsNGA& aOptions )
    : iWindow( aOptions.iWindow ),
      iWs( aOptions.iWs)
    {
    }
// End of File
