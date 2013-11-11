/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for MCSPLineStatusObserver class
*
*/



#ifndef MCSPLINESTATUSOBSERVER_H
#define MCSPLINESTATUSOBSERVER_H

//INCLUDES
#include <etelmm.h>
#include "rcsplinecontainer.h"

// CLASS DECLARATION

/**
*  Virtual interface for monitoring line status changes.
*
*  @since S60 5.0
*/
NONSHARABLE_CLASS(MCSPLineStatusObserver)
    {
    public:

        /**
        * Informs about line status changes.
        * @since S60 5.0
        * @param aLine the line that is monitored for events
        * @param aLineId the line id 
        * @param aCallStatus the line status
        */    
        virtual void LineStatusChanged( RMobileLine& aLine,
                RCSPLineContainer::TCSPLineId aLineId, 
                RMobileCall::TMobileCallStatus& aCallStatus ) = 0;

    };

#endif // MCSPLINESTATUSOBSERVER_H

//End of file
