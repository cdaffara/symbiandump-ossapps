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
 * Description: canotifier.h
 *
 */

#ifndef CANOTIFIER_H
#define CANOTIFIER_H

#include <QObject>
#include "cadefs.h"
#include "caentry.h"

class CaNotifier: public QObject
{
Q_OBJECT

public:

    explicit CaNotifier() {};

    virtual ~CaNotifier() {};

    void issueEntryChanged(int entryId, ChangeType changeType)
    {
        emit entryChanged(entryId, changeType);
    }
signals:

    /*!
        A signal emited when the entry is changed.
        \param entryId entry id.
        \param changeType the type of the change.
    */
    void entryChanged(int entryId, ChangeType changeType);

    /*!
        A signal emited when the entry is changed.
        \param entry changed entry.
        \param changeType the type of the change.
    */
    void entryChanged(const CaEntry &entry, ChangeType changeType);

    /*!
        A signal emited when the entry is touched.
        \param id entry id.
    */
    void entryTouched(int id);

    /*!
        A signal emited when group content is changed.
        \param groupId group id.
    */
    void groupContentChanged(int groupId);

protected:

    virtual void connectNotify(const char * signal) { Q_UNUSED(signal); }

    virtual void disconnectNotify(const char * signal) { Q_UNUSED(signal); }

};


#endif // CANOTIFIER_H
