/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     An abstract class for an object implementing the state.
*
*/


#ifndef MPHONESTATE_H
#define MPHONESTATE_H

// INCLUDES
#include "mphoneengineevents.h"
#include "mphonekeyevents.h"
#include "mphonesystemevents.h"
#include "mphonemenuandcbaevents.h"
#include "mphoneremotecontrolevents.h"
#include "mphoneneclearedhandler.h"
#include "mphonenumberentryevents.h"
#include "mphonelockinfo.h"
#include "mphonestateproperties.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  An abstract class for an object implementing the state.
*
*/
class MPhoneState : 
    public MPhoneEngineEvents, 
    public MPhoneKeyEvents, 
    public MPhoneSystemEvents,
    public MPhoneMenuAndCbaEvents,
    public MPhoneRemoteControlEvents,
    public MPhoneNEClearedHandler,
    public MPhoneLockInfo,
    public MPhoneStateProperties
    {
    public:

        /**
        * Destructor.
        */
        virtual ~MPhoneState() {};
        
    };

#endif // MPHONESTATE_H
            
// End of File
