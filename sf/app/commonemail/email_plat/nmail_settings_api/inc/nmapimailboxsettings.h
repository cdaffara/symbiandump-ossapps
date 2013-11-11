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

#ifndef NMAPIMAILBOXSETTINGS_H_
#define NMAPIMAILBOXSETTINGS_H_

#include <QList>
#include <QObject>

#include <nmapidef.h>

namespace EmailClientApi
{

class NmApiMailboxSettingsData;
class NmApiMailboxSettingsPrivate;
class NMAPI_EXPORT NmApiMailboxSettings : public QObject
{
    Q_OBJECT
public:
    NmApiMailboxSettings(QObject *parent = 0);
    ~NmApiMailboxSettings();

    bool listMailboxIds(QList<quint64> &idList);
    QString getMailboxType(quint64 mailboxId);
    bool loadSettings(quint64 mailboxId, NmApiMailboxSettingsData &data);
    bool saveSettings(const NmApiMailboxSettingsData &data);
    bool createMailbox(const QString &mailboxType, NmApiMailboxSettingsData &data);
    bool deleteMailbox(quint64 mailboxId);
    bool populateDefaultSettings(const QString &mailboxType, NmApiMailboxSettingsData &data);

signals:
    void mailboxDeleted(int result = 0);
    
protected:    
    NmApiMailboxSettingsPrivate *d;
 
};

}

#endif /* NMAPIMAILBOXSETTINGS_H_ */
