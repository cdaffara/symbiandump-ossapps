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

#ifndef NMMAILBOXSETTINGSMANAGER_H
#define NMMAILBOXSETTINGSMANAGER_H

#include <QList>
#include <QObject>
#include <QVariant>

#include "nmsettingscommon.h"

class NmMailbox;
class NmDataManager;
class HbIcon;
class NmId;
class HbDataFormModel;
class HbDataForm;
class QPluginLoader;


class NmMailboxSettingsManager : public QObject
{
    Q_OBJECT

public:

    NmMailboxSettingsManager();

    ~NmMailboxSettingsManager();

    void listMailboxes(QList<NmMailbox *> &mailboxList);

    HbIcon &mailboxIcon(const NmId &mailboxId) const;

    void populateModel(HbDataFormModel &model,
                       HbDataForm &form,
                       const NmId &mailboxId) const;


signals:

    void mailboxListChanged(const NmId &mailboxId,
                            NmSettings::MailboxEventType type);

    void mailboxPropertyChanged(const NmId &mailboxId,
                                QVariant property,
                                QVariant value);

    void goOnline(const NmId &mailboxId);

    void goOffline(const NmId &mailboxId);

    void aboutToClose();


private:

    Q_DISABLE_COPY(NmMailboxSettingsManager)

    void loadPlugins();

    void unloadPlugins();


private: // Data

    // Owned.
    NmDataManager* mDataManager;

    QList<QPluginLoader *> mPluginLoaders;
};

#endif // NMMAILBOXSETTINGSMANAGER_H

// End of file.
