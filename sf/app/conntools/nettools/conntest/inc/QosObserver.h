/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Receiver for QoS events.
*
*/

#ifndef __QOSOBSERVER_H__
#define __QOSOBSERVER_H__

// INCLUDES
#include <qoslib.h>

// FORWARD DECLARATIONS
class MUINotify;

/**
* Class that receives QoS events. 
*/
class CQosObserver : public CBase, public MQoSObserver
{
public: // new methods
    
    /**
    * Constructor
    */
    CQosObserver(MUINotify& aConsole);

    /**
    * Destructor
    */
    ~CQosObserver();
    
    /**
    * This method is called when QoS event has been generated.
    * Prints the event on the console.
    */
    void Event(const CQoSEventBase& aQosEvent);
        
private:
    // Member variables
    MUINotify&            iConsole; 
};

#endif // __QOSOBSERVER_H__

