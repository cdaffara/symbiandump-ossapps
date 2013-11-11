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

#ifndef NMAPIMAILBOXSETTINGS_P_H_
#define NMAPIMAILBOXSETTINGS_P_H_

#include <QObject>
#include <qpointer.h>
#include <nmapimailboxsettings.h>
#include <nmoperation.h>

class NmApiDataPluginFactory;
class NmOperation;

namespace EmailClientApi
{
class NmApiMailboxSettingsData;

class NmApiMailboxSettingsPrivate : public QObject
{
    Q_OBJECT
public:
    NmApiMailboxSettingsPrivate(QObject *parent = 0);
    ~NmApiMailboxSettingsPrivate();

    bool listMailboxIds(QList<quint64> &idList);
    QString getMailboxType(quint64 mailboxId);
    bool loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data);
    bool saveSettings(const NmApiMailboxSettingsData &data);
    bool createMailbox(const QString &mailboxType, NmApiMailboxSettingsData &data);
    bool deleteMailbox(quint64 mailboxId);
    bool populateDefaultSettings(const QString &mailboxType, NmApiMailboxSettingsData &data);

signals:
    void mailboxDeleted(int result = 0);
    
private:
    void createPopImapMailbox(const QString &type, NmApiMailboxSettingsData &data);
    QString resolveMailboxTypeL(quint64 mailboxId);

    
private:
    NmApiDataPluginFactory *mFactory;
    QPointer<NmOperation> mDeleteMailboxOperation; // not owned
};

}

#endif /* NMAPIMAILBOXSETTINGS_P_H_ */
