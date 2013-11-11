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

#ifndef CPPROFILEMONITOR_H
#define CPPROFILEMONITOR_H

#include <QObject>
#include <cpprofilewrappermacro.h>

class CpProfileMonitorPrivate;

class PROFILE_WRAPPER_EXPORT CpProfileMonitor: public QObject
{
    Q_OBJECT
public:
    explicit CpProfileMonitor(QObject *parent = 0);
    ~CpProfileMonitor();
signals:
    void profileActivated(int activeProfileId);
    void avtiveProfileModified();
    void profileModified(int profileId);
    void profileNameModified();
private:
    CpProfileMonitorPrivate *const d_ptr;
   
private:    
    Q_DISABLE_COPY(CpProfileMonitor)
    Q_DECLARE_PRIVATE_D(d_ptr,CpProfileMonitor)
};

#endif /* CPPROFILEMONITOR_H */
