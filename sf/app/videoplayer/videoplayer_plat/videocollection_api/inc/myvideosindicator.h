/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  My Videos indicator ECom plugin interface.*
*/



#ifndef MYVIDEOSINDICATOR_H
#define MYVIDEOSINDICATOR_H

#include <e32base.h>
#include "myvideosindicator.hrh"

class CGulIcon;
class CMPXMedia;

/**
 * ECom plugin interface definition.
 */
class CMyVideosIndicator : public CBase
    {
    public:

        /**
         * Wraps ECom object instantitation.
         */
        static CMyVideosIndicator* NewL();
        
        /**
         * Wraps ECom object destruction.
         */
        virtual ~CMyVideosIndicator();
        
        /**
         * Returns overlay indicator. Ownership is transferred.
         *
         * @return Pointer to an icon.
         */
        virtual CGulIcon* VideoOverlayIndicatorL() = 0;

        /**
         * Tells whether to show the overlay indicator for a given media object.
         * 
         * @param aMedia The media object.
         * @return Whether to show the indicator (true/false).
         */
        virtual TBool IsIndicatorShown( const CMPXMedia& aMedia ) = 0;

    private:

        /** 
         * Instance identifier key.
         */
        TUid iDtor_ID_Key;

    };

#include <ecom/ecom.h>
#include "myvideosindicator.inl" // Our own base implementations for ECOM

#endif // MYVIDEOSINDICATOR_H
	
