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
* Description:  Class for NGA render configuration.
*
*/


#ifndef TVTENGRENDERCONFIGNGA_H
#define TVTENGRENDERCONFIGNGA_H

//  INCLUDES
#include "TVtEngRenderConfig.h"
#include "TVtEngRenderingNGA.h"

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
NONSHARABLE_CLASS( TVtEngRenderConfigNGA ) : public TVtEngRenderConfig
    {
public: // new functions

    TVtEngRenderConfigNGA();

    TVtEngRenderConfigNGA( const TVtEngRenderingOptionsNGA& aOptions );

    TVtEngRenderConfigNGA& operator=( const TVtEngRenderConfig& aConfig );

public: // data    
    };

#include "TVtEngRenderConfigNGA.inl"

#endif      // TVTENGRENDERCONFIGNGA_H

// End of File
