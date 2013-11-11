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



#ifndef TVTENGRENDERCONFIGDP_H
#define TVTENGRENDERCONFIGDP_H

//  INCLUDES
#include "TVtEngRenderConfig.h"
#include "TVtEngRenderingDP.h"
#include <Posting_Surface.h>

// FORWARD DECLARATIONS

class MVtEngFrameObserver;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;

// CLASS DECLARATION

/**
*  Configuration for representation of local or remote video.
*  Used only internally.
*
*  @lib videoteleng
*  @since 2.6
*/
NONSHARABLE_CLASS( TVtEngRenderConfigDP ) : public TVtEngRenderConfig
    {
public: // new functions

    TVtEngRenderConfigDP();

    TVtEngRenderConfigDP( const TVtEngRenderingOptionsDP& aOptions );

    TVtEngRenderConfigDP& operator=( const TVtEngRenderConfig& aConfig );

public: // data
    // Background color value. This color is filled to areas that are not
    // covered by the image.
    TUint                           iBackGround;
    };

#include "TVtEngRenderConfigDP.inl"

#endif      // TVTENGRENDERCONFIGDP_H

// End of File
