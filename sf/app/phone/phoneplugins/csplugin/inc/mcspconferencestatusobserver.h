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
* Description:  Header file for MCSPConferenceStatusObserver class
*
*/



#ifndef MCSPCONFERENCESTATUSOBSERVER_H
#define MCSPCONFERENCESTATUSOBSERVER_H


// CLASS DECLARATION

/**
*  Virtual interface for monitoring conference status.
*
*  @since S60 5.0 
*/
NONSHARABLE_CLASS(MCSPConferenceStatusObserver)
    {
    public:

        /* Security setting event value */  
        enum TCSPConferenceState
            {
            /** The conference call is in the idle state. */
            ECSPConferenceIdle,
            /** The conference call is in the active, connected state. */
            ECSPConferenceActive,
            /** The conference call is in the held, connected state. */
            ECSPConferenceHold,
            };
        
        /**
        * Informs conference call state change.
        * @since S60 5.0 
        */    
        virtual void NotifyStateChange(
                MCSPConferenceStatusObserver::TCSPConferenceState aStatus ) = 0;

    };

#endif // MCSPCONFERENCESTATUSOBSERVER_H
