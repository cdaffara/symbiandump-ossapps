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



#ifndef TVTENGRENDERCONFIG_H
#define TVTENGRENDERCONFIG_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class RWindow;


// CLASS DECLARATION

/**
*  Configuration for representation of local or remote video.
*  Used only internally.
*
*  @lib videoteleng
*  @since 2.6
*/
struct TVtEngRenderConfig
    {
    public: // enumerations

        /**
        * Indication of type of rendering specified by configuration.
        * EWsRender fields applicable for window server are valid
        * EDsaRender fields applicable for DSA are valid
        */
        enum TRenderType
            {
            EWsRender,
            EDsaRender,
            EDpRender,
            ENgaRender
            };

    public: // data
        RWindow*            iRemoteWindow;

        RWsSession*          iWsSession;
        CWsScreenDevice*     iWsSreenDevice;
        RWindowBase*         iWindow;

        MVtEngFrameObserver* iObserver;
        TRect                iRect;
        TRect                iClipRect;
        TPoint               iOffset;
        TRenderType          iType;
    };

#endif      // TVTENGRENDERCONFIG_H

// End of File
