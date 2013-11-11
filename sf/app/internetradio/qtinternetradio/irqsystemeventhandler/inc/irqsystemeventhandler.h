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
#ifndef IRQSYSTEMEVENTHANDLER_H
#define IRQSYSTEMEVENTHANDLER_H

#include <QObject>
#include "irqsystemeventhandlerexport.h"

class IRQSystemEventHandlerPrivate;

class IRQSYSTEMEVENTHANDLER_DLL_EXPORT IRQSystemEventHandler : public QObject  
{    
    Q_OBJECT
    
public:
    
    IRQSystemEventHandler();
    ~IRQSystemEventHandler();

    //for disk monitor
    bool   isBelowCriticalLevel(const qint64 aCriticalLevel = 0);
    qint64 diskCriticalLevel() const;
    //to cancel all the system events
    void cancel();    
    void start();

signals:

    void alarmStarted();
    void alarmStopped();
    void diskSpaceLowNotification(qint64 aCriticalLevel);
    void callActivated();
    void callDeactivated();
    void headsetConnected();
    void headsetDisconnected();
    
private:
    
    IRQSystemEventHandlerPrivate * const d_ptr;
    
    Q_DISABLE_COPY(IRQSystemEventHandler)
    friend class IRQSystemEventHandlerPrivate;
};

#endif // IRQSYSTEMEVENTHANDLER_H
