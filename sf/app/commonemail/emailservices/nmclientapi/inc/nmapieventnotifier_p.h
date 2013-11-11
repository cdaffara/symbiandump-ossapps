/*
 * Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef NMAPIEVENTNOTIFIERPRIVATE_H
#define NMAPIEVENTNOTIFIERPRIVATE_H

#include <QObject>
#include "nmapiprivateclasses.h"
#include <nmapicommon.h>

class QTimer;

class NmApiEngine;

namespace EmailClientApi
{
class NmApiEventNotifierPrivate : public QObject
{
    Q_OBJECT
public:
    NmApiEventNotifierPrivate(QObject *parent = 0);
    virtual ~NmApiEventNotifierPrivate();

    void cancel();
    bool isRunning() const;
    bool start();
    void stop();
    void events(QList<NmApiEvent> &events);
public slots:
    void emailStoreEvent(const NmApiEvent &events);

signals:
    void timedOut();

private:
    QTimer *mEmitSignals;
    NmApiEngine *mEngine;
    bool mIsRunning;
    QList<NmApiEvent> mBufferOfEvents;
};

}

#endif /* NMAPIEVENTNOTIFIERPRIVATE_H */
