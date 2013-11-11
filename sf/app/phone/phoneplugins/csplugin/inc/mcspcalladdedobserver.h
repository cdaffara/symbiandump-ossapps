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
* Description:  Header file for MCSPCallAddedObserver class
*
*/



#ifndef MCSPCALLADDEDOBSERVER_H
#define MCSPCALLADDEDOBSERVER_H

//INCLUDES
#include <etelmm.h>

#include "rcsplinecontainer.h"

// CLASS DECLARATION

/**
*  Virtual interface for monitoring incoming call.
*
*  @since S60 3.2
*/
class MCSPCallAddedObserver
    {
    public:

        /**
        * Informs about added call.
        * @since S60 3.2
        * @param aLine the line that corresponds to the call
        * @param aCallName the name of the call
        */    
        virtual void CallAdded( RMobileLine& aLine, 
                                TName aCallName,
                                RCSPLineContainer::TCSPLineId aLineId ) = 0;


    };

#endif // MINCOMINGCALLOBSERVER_H

//End of file
