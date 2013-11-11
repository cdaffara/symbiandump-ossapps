/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/

#ifndef IRSYSTEMEVENTOBSERVERINTERFACE_H
#define IRSYSTEMEVENTOBSERVERINTERFACE_H

#include <qglobal.h>

class MIRAlarmObserverInterface
{
public:
    virtual void alarmStarted()= 0;    
    virtual void alarmStopped() = 0;
};

 
class MIRDiskSpaceObserverInterface
{
public:

    /**
     * notifyLowDiskSpace Called when the observed disk's free disk space is
     * less than the critical level.
     */
    virtual void notifyLowDiskSpace(qint64 aCriticalLevel) = 0;
};

class MIRPropertyObserverInterface
{
public:
    
    virtual void callIsActivated() = 0;
    virtual void callIsDeactivated() = 0;    
    //this function is a general error call back, no matter which propertyChanged, 
    //we will call this to notify the upper layer. 
    virtual void errorCallback(int aError) = 0;
};

class MIRHeadsetObserverInterface
{
public:
    
    virtual void headsetIsConnected() = 0;
    virtual void headsetIsDisconnected() = 0;
};

#endif // IRSYSTEMEVENTOBSERVERINTERFACE_H
