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

#ifndef NMSETTINGSVIEWLAUNCHER_H
#define NMSETTINGSVIEWLAUNCHER_H

#include <QObject>
#include <QVariant>

#include "nmsettinguidef.h"
#include "nmsettingscommon.h"

class NmSettingsViewFactory;
class CpItemDataHelper;
class QString;
class NmId;
class HbIcon;


/*!
    \class NmSettingsViewLauncher
    \brief Launches settings view for given mailbox.
*/
class NMAILSETTINGUI_EXPORT NmSettingsViewLauncher : public QObject
{
    Q_OBJECT

public:

    NmSettingsViewLauncher();

    virtual ~NmSettingsViewLauncher();

    void launchSettingsView(const NmId &mailboxId,
                            const QString &mailboxName);


signals:

    void mailboxListChanged(const NmId &mailboxId,
                            NmSettings::MailboxEventType type);

    void mailboxPropertyChanged(const NmId &mailboxId,
                                QVariant property,
                                QVariant value);
    void goOnline(const NmId &mailboxId);
    void goOffline(const NmId &mailboxId);


private:

    Q_DISABLE_COPY(NmSettingsViewLauncher)


private: // data

    NmSettingsViewFactory *mSettingsViewFactory; // Owned
    CpItemDataHelper *mItemDataHelper; // Owned
};

#endif // NMSETTINGSVIEWLAUNCHER_H

// End of file.
