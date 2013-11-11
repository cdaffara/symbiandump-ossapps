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
// TVtEngRenderingOptions::TVtEngRenderingOptions
// C++ constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptions::TVtEngRenderingOptions(
    MVtEngFrameObserver& aObserver,
    TSize aSize ) :
    iObserver( &aObserver ),
    iSize( aSize)
    {
    }

// -----------------------------------------------------------------------------
// TVtEngRenderingOptions::TVtEngRenderingOptions
// C++ copy constructor
// -----------------------------------------------------------------------------
//
inline TVtEngRenderingOptions::TVtEngRenderingOptions(
    const TVtEngRenderingOptions& aOptions ) :
    iObserver( aOptions.iObserver ),
    iSize( aOptions.iSize )
    {
    }

// End of File
