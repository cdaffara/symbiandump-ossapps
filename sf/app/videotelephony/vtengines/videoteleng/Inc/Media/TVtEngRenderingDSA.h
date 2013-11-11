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


#ifndef TVTENGRENDERINGDSA_H
#define TVTENGRENDERINGDSA_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;

// CLASS DECLARATION

/**
*  Direct screen access rendering definitions.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( TVtEngRenderingOptionsDSA )
    {
    public: // constructor

        /**
        * C++ constructor
        */
        inline TVtEngRenderingOptionsDSA(
            MVtEngFrameObserver& aObserver,
            RWsSession& aWs,
            CWsScreenDevice& aScreenDevice,
            RWindowBase& aWindow,
            const TRect& aScreenRect,
            const TRect& aClipRect );

        /**
        * C++ copy constructor.
        * @param aOptions options to be copied to this instance.
        */
        inline TVtEngRenderingOptionsDSA(
            const TVtEngRenderingOptionsDSA& aOptions );

    private:
        TVtEngRenderingOptionsDSA& operator=(
            const TVtEngRenderingOptionsDSA& );

    public: // data members
        RWsSession&      iWs;
        CWsScreenDevice& iWsSD;
        RWindowBase&     iWindow;
        TRect            iRect;
        TRect            iClipRect;

        // Frame observer
        MVtEngFrameObserver* iObserver;
    };

#include <tvtengrenderingdsa.inl>

#endif      // TVTENGRENDERINGDSA_H

// End of File
