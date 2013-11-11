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
* Description:  Class for NGA Rendering.
*
*/

#ifndef TVTENGRENDERINGNGA_H
#define TVTENGRENDERINGNGA_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWindow;
class RWsSession;
// CLASS DECLARATION

/**
*  Direct screen access rendering definitions.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( TVtEngRenderingOptionsNGA )
    {
    public: // constructor

        /**
        * C++ constructor
        */
        inline TVtEngRenderingOptionsNGA(
            RWindow& aWindow, RWsSession& aWs );

        /**
        * C++ copy constructor.
        * @param aOptions options to be copied to this instance.
        */
        inline TVtEngRenderingOptionsNGA(
            const TVtEngRenderingOptionsNGA& aOptions );

    private:
        TVtEngRenderingOptionsNGA& operator=(
            const TVtEngRenderingOptionsNGA& );

    public: // data members
        RWindow&    iWindow;
        
        RWsSession&      iWs;
    };

#include <tvtengrenderingnga.inl>

#endif      // TVTENGRENDERINGNGA_H

// End of File
