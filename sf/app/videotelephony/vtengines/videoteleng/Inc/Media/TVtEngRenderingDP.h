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



#ifndef TVTENGRENDERINGDP_H
#define TVTENGRENDERINGDP_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;

// CLASS DECLARATION

/**
*  Display posting access rendering definitions.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( TVtEngRenderingOptionsDP )
    {
    public: // constructor

        /**
        * C++ constructor
        */
        inline TVtEngRenderingOptionsDP(
            MVtEngFrameObserver& aObserver,
            RWsSession& aWs,
            CWsScreenDevice& aScreenDevice,
            RWindowBase& aWindow,
            const TRect& aScreenRect,
            const TRect& aClipRect,
            const TPoint& aOffset );

        /**
        * C++ copy constructor.
        * @param aOptions options to be copied to this instance.
        */
        inline TVtEngRenderingOptionsDP(
            const TVtEngRenderingOptionsDP& aOptions );

    private:
        TVtEngRenderingOptionsDP& operator=(
            const TVtEngRenderingOptionsDP& );

    public: // data members
        RWsSession&      iWs;
        CWsScreenDevice& iWsSD;
        RWindowBase&     iWindow;
        TRect            iRect;
        TRect            iClipRect;
        TPoint           iOffset;

        // Frame observer
        MVtEngFrameObserver* iObserver;
    };

#include <tvtengrenderingdp.inl>

#endif      // TVTENGRENDERINGDP_H

// End of File
