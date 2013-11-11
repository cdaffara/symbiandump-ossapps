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

#ifndef NMAPIMESSAGETASK_H_
#define NMAPIMESSAGETASK_H_

#include <QObject>

#include <nmapidef.h>

/*!
   base class for async tasks
 */
class NMAPI_EXPORT NmApiMessageTask : public QObject
{
    Q_OBJECT
protected:
    NmApiMessageTask(QObject *parent);

public:
    virtual ~NmApiMessageTask();

public slots:
    /*!
       Starts task and returns true if starting succeeded.
     */
    virtual bool start() = 0;

    /*!
       Cancels started but not yet completed task. may not be applicable
       in all tasks.
     */
    virtual void cancel() = 0;

    signals:
    /*!
       task failed, error code is specific to concrete task
     */
    void failed (int error);
    /*!
       task canceled
     */
    void canceled();
};

#endif /*NMAPIMESSAGETASK_H_ */
