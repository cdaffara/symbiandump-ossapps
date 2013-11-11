/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef TVTENGDTMFTONE_H
#define TVTENGDTMFTONE_H

//  INCLUDES
#include "TVtEngType.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Abstraction for DTMF tone
*
*  @lib videoteleng
*  @since 2.6
*/
class TVtEngDtmfTone : public TVtEngtType
    {
    public: // constructor

        /**
        * C++ constructor
        */
        TVtEngDtmfTone( const TChar& aTone );

    public:
        // Tone
        TChar iTone;
    };
#include "TVtEngDtmfTone.inl"

#endif      // TVTENGDTMFTONE_H

// End of File
