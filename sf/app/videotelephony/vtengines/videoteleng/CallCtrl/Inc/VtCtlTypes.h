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
* Description:  Data types for call control
*
*/


#ifndef VTCTLTYPES_H
#define VTCTLTYPES_H

//  INCLUDES
#include "VtCtlDefs.h"
#include "TVtCtlType.h"

/**
*  CS Session information.
*
*  @lib CSCallCtrl
*  @since 2.6
*/
class TVtCtlCSSessionInfoV1 : public TVtCtlType
    {
    public: // constructors
        /**
        * C++ default contructor.
        */
        inline TVtCtlCSSessionInfoV1();

    public: // data members

        // Session duration
        TVtCtlDuration  iDuration;

        // Address
        TVtCtlTelNumber iTelNumber;

        // Session state
        MVtCtlCallControl::TVtCtlState iState;

        // Previous session state
        MVtCtlCallControl::TVtCtlState iPreviousState;

        // Session direction MO/MT
        MVtCtlCallControl::TVtCtlDirection iDirection;
    };

/**
* Data passed in MVtCtlEventObserver::HandleVtSessionEventL
* when event is KVtCtlEventSessionStatusChanged.
*/
struct TVtCtlSessionStateEvent
    {
    /**
    * Session state
    */
    MVtCtlCallControl::TVtCtlState iState;
    
    /**
    * Previous session state
    */
    MVtCtlCallControl::TVtCtlState iPreviousState;
    
    /**
    * Session id
    */
    TVtCtlSessionId                iSessionId;
    };

#include "VtCtlTypes.inl"

#endif // VTCTLTYPES_H

// End of File
