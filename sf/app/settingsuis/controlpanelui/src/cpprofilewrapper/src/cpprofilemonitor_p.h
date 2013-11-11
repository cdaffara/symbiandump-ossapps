/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#ifndef CPPROFILEMONITORPRIVATE_H
#define CPPROFILEMONITORPRIVATE_H

#include <MProEngNotifyHandler.h>
#include <MProEngProfileActivationObserver.h>
#include <MProEngProfileObserver.h>
#include <MProEngProfileNameArrayObserver.h>
#include <qglobal.h>

class CpProfileModel;
class CpProfileMonitor;

class CpProfileMonitorPrivate:public MProEngProfileActivationObserver,
                              public MProEngProfileObserver,
                              public MProEngProfileNameArrayObserver
{
    Q_DECLARE_PUBLIC(CpProfileMonitor)
public:
    CpProfileMonitorPrivate();
    ~CpProfileMonitorPrivate();
    void initialize(CpProfileMonitor *parent);
    void stopMonitoring();
private:
    //from MProEngProfileObserver
    void HandleProfileModifiedL( TInt aProfileId );
    //from MProEngProfileActivationObserver
    void HandleProfileActivatedL( TInt aProfileId );
    //from MProEngProfileNameArrayObserver
    void HandleProfileNameArrayModificationL();
private:
    CpProfileModel *mProfileModel;
    MProEngNotifyHandler *mProfileNotifier;
private:
    CpProfileMonitor *q_ptr;
};

#endif
