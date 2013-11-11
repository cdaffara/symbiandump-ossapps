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
* Description:  Header file for MCSPIncomingCallObserver class
*
*/



#ifndef MCSPINCOMINGCALLOBSERVER_H
#define MCSPINCOMINGCALLOBSERVER_H

//INCLUDES
#include <etelmm.h>

#include "rcsplinecontainer.h"

// CLASS DECLARATION

/**
*  Virtual interface for monitoring incoming call.
*
*  @since S60 3.2
*/
class MCSPIncomingCallObserver
    {
    public:

        /**
        * Informs incoming call.
        * @since S60 3.2
        * @param aLine the line that corresponds to the call
        * @param aCallName the name of the call
        */    
        virtual void IncomingCallArrived( RMobileLine& aLine, 
                                  const TName& aCallName,
                                  RCSPLineContainer::TCSPLineId aLineId ) = 0;


    };

#endif // MINCOMINGCALLOBSERVER_H

//End of file
