/*
* Copyright (c) 2004 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Call control interface
*
*/



#ifndef MVTCTLCALLCONTROL_H
#define MVTCTLCALLCONTROL_H

//  INCLUDES
#include "VtCtlDefs.h"
#include "TVtCtlType.h"

// CLASS DECLARATION

/**
*  Call control class
*  
*  @lib CSCallCtrl
*  @since 2.6
*/
class MVtCtlCallControl
    {
    public:  // new functions

        /**
        * Session state
        * EIdle          idle, no connection
        * EInitializing  initializing MO session
        * EAlerting      alerting at remote end 
        * ERinging       incoming session ringing
        * EConnected     session is established
        * EDisconnecting disconnecting session
        */
        enum TVtCtlState {
            EUnknown,       // 0
            EIdle,          // 1
            EInitializing,  // 2
            EAlerting,      // 3
            ERinging,       // 4
            EConnected,     // 5
            EDisconnecting  // 6
            };        

        /**
        * Session direction
        * EDirectionUnknown          unknown direction
        * EDirectionMobileOriginated MO session
        * DirectionMobileTerminated  MT session
        */
        enum TVtCtlDirection
            {
            EDirectionUnknown,
            EDirectionMobileOriginated,
            DirectionMobileTerminated
            };

        /**
        * Gets session duration
        * @param aId session id
        * @param aDuration session duration
        * @return Symbian OS error code
        */
        virtual TInt GetDuration( 
            TVtCtlSessionId aId,
            TVtCtlDuration& aDuration ) = 0;

        /**
        * Gets information on a session. 
        * @param aId session id
        * @param aInfo packetized object derived from TVtCtlType
        */
        virtual void GetSessionInfoL( 
            TVtCtlSessionId aId,
            TDes8& aInfo ) = 0;

        /**
        * Returns session state
        * @param aId session id
        * @return session state
        */
        virtual TVtCtlState GetSessionState( 
            TVtCtlSessionId aId ) const = 0;

        /**
        * Returns previous session state
        * @param aId session id
        * @return session state
        */
        virtual TVtCtlState GetPreviousPreviousState( 
            TVtCtlSessionId aId ) const = 0;

    };

#endif      // MVTCTLCALLCONTROL_H
 
// End of File
