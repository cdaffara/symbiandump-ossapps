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
#ifndef _NMAPI_MAILBOX_SETTINGS_DATA_PRIVATE_
#define _NMAPI_MAILBOX_SETTINGS_DATA_PRIVATE_

#include <QObject>
#include <QScopedPointer>
#include <QHash>

namespace EmailClientApi
{
class NmApiMailboxSettingsDataPrivate
{
public:
    NmApiMailboxSettingsDataPrivate();
    virtual ~NmApiMailboxSettingsDataPrivate();
    void setMailboxId(quint64 mailboxId);
    quint64 mailboxId() const;
    void setValue(int key, const QVariant &settingValue);
    bool getValue(int key, QVariant &settingValue) const;
    bool validateData() const;
    QList<int> listSettings() const;
	void clearSettings();
    
private:
    bool validateString(int key, QVariant& val, bool& validated) const;
    bool validateStringValue(int key, QString& val) const;
    bool validateStringGeneral(int key, QString& val, bool& handled) const;
    bool validateEmailAddress(int key, QString& val, bool& handled) const;
    bool validateUsesAuthentication(int key, QString& val, bool& handled) const;
    bool validateSecurityType(int key, QString& val, bool& handled) const;
    bool validateAOLState(int key, QString& val, bool& handled) const;
    bool validateInteger(int key, QVariant& val, bool& validated) const;
    bool validateIntVal(int key, int val) const;
    bool validateIntGeneral(int key, int val, bool& handled) const;
    bool validateOnOffValue(int key, int val, bool& handled) const;
    bool validateWeekdayMask(int key, int val, bool& handled) const;
    bool validateHoursInDay(int key, int val, bool& handled) const;
    bool validateBool(int key, QVariant& val, bool& validated) const;
    bool validateDateTime(int key, QVariant& val, bool& validated) const;

private:
    quint64 mId;
    QScopedPointer<QHash<int, QVariant> > mSettings;

};
}//end namespace

#endif //_NMAPI_MAILBOX_SETTINGS_DATA_PRIVATE_
