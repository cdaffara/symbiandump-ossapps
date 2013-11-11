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



#ifndef TVTENGRENDERCONFIGDSA_H
#define TVTENGRENDERCONFIGDSA_H

//  INCLUDES
#include <e32std.h>
#include "TVtEngRenderConfig.h"
#include "TVtEngRenderingDSA.h"

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
NONSHARABLE_CLASS( TVtEngRenderConfigDSA ) : public TVtEngRenderConfig
    {
public: // new functions

    TVtEngRenderConfigDSA();

    TVtEngRenderConfigDSA( const TVtEngRenderingOptionsDSA& aOptions );

    TVtEngRenderConfigDSA& operator=( const TVtEngRenderConfig& aConfig );

public: // data
    };

#include "TVtEngRenderConfigDSA.inl"

#endif      // TVTENGRENDERCONFIGDSA_H

// End of File
