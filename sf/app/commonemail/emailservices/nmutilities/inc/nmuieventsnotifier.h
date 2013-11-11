/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMUIEVENTSNOTIFIER_H
#define NMUIEVENTSNOTIFIER_H

#include <QObject>
#include <XQSettingsKey>
#include <XQSettingsManager>
#include <QVariant>
#include <nmcommon.h> // For NmId.
#include <nmuiviewids.h>

#include "emailutilitiesdef.h"



class EMAIL_UTILITIES_EXPORT NmUiEventsNotifier : public QObject
{
    Q_OBJECT
    Q_ENUMS(NmUiEventType)

public: // Data types

    enum NmUiEventType {
        NmInvalidUiEvent = 0,
        NmViewShownEvent,
        NmViewHiddenEvent
    };


public:

    NmUiEventsNotifier(QObject *parent = NULL);

    ~NmUiEventsNotifier();


public:

    static bool notifyViewStateChanged(const NmUiEventType eventType,
                                       const NmUiViewId viewId,
                                       const NmId &mailboxId);


private:

    bool keysEqual(const XQSettingsKey& settingsKey,
                   const XQPublishAndSubscribeSettingsKey& psSettingsKey) const;

    bool parseKeyValue(const QByteArray &array,
                       NmUiEventType &eventType,
                       NmUiViewId &viewId,
                       NmId &mailboxId) const;


private slots:

    void valueChanged(const XQSettingsKey &key, const QVariant &value);


Q_SIGNALS:

    void viewStateChanged(NmUiEventsNotifier::NmUiEventType eventType,
                          NmUiViewId viewId,
                          NmId mailboxId);


private: // Data

    XQSettingsManager mSettingsManager;
};


#endif /* NMUIEVENTSNOTIFIER_H */

// End of file.
