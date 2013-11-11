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

#ifndef NMIPSSETTINGS_H
#define NMIPSSETTINGS_H

#include <QObject>
#include <QVariant>
#include <AlwaysOnlineManagerClient.h>

#include "nmsettingsplugininterface.h"
#include "nmsettingscommon.h"

class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class NmId;
class NmIpsSettingsHelper;
class NmIpsSettingsManagerBase;
class HbAction;

class NmIpsSettingsPlugin : public QObject, public NmSettingsPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(NmSettingsPluginInterface)

public:

    NmIpsSettingsPlugin();
    virtual ~NmIpsSettingsPlugin();

    bool populateModel(HbDataFormModel &model, HbDataForm &form, const NmId &mailboxId);

signals:

    void mailboxListChanged(const NmId &mailboxId, NmSettings::MailboxEventType type);
    void mailboxPropertyChanged(const NmId &mailboxId, QVariant property, QVariant value);
    void goOnline(const NmId &mailboxId);
    void goOffline(const NmId &mailboxId);

public slots:

    void aboutToClose();

private slots:

    void createUserDefinedMode();
    void showMailInInboxModified(int index);
    
private:

    void initGroupItems();
    void initPreferenceItems(HbDataFormModelItem &item) const;
    void initReceivingScheduleItems(HbDataFormModelItem &item);
    void initUserInfoItems(HbDataFormModelItem &item) const;
    void initServerInfoItems(HbDataFormModelItem &item);
    void setAlwaysOnlineState(TAlwaysOnlineServerAPICommands command, NmId mailboxId) const;

    Q_DISABLE_COPY(NmIpsSettingsPlugin)

private: // data.

    NmIpsSettingsHelper *mSettingsHelper;  // Owned.
    NmIpsSettingsManagerBase *mSettingsManager; // Owned.
    HbDataFormModel *mModel; // Not owned.
    HbDataForm *mForm; // Not owned.
    bool mHiddenItem;
};

#endif // NMIPSSETTINGS_H
