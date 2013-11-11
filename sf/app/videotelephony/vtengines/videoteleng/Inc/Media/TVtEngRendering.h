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



#ifndef TVTENGRENDERING_H
#define TVTENGRENDERING_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;

// CLASS DECLARATION

/**
*  Configuration for representation of local or remote video
*  via window server.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( TVtEngRenderingOptions )
    {
    public: // constructor

        /**
        * C++ constructor
        */
        inline TVtEngRenderingOptions(
            MVtEngFrameObserver& aObserver,
            TSize aRect );

        /**
        * copy constructor
        */
        inline TVtEngRenderingOptions( const TVtEngRenderingOptions& aOptions );

    private:
        TVtEngRenderingOptions& operator=( const TVtEngRenderingOptions& );

    public:
        // Frame observer
        MVtEngFrameObserver* iObserver;

        // Bitmap size
        TSize iSize;
    };

#include <tvtengrendering.inl>

#endif      // TVTENGRENDERING_H

// End of File
