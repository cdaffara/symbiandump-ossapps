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
* Description: Presence info provider plugin for CntListModel. It can provide
*              the presence information of a contact (icon2 field).
*
*/

#ifndef CNTPRESENCEINFOPROVIDER_H
#define CNTPRESENCEINFOPROVIDER_H

#include <cntinfoprovider.h>
#include <qcontact.h>

class PrcPresenceReader;
class PrcPresenceBuddyInfoQt;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/**
    Presence info provider plugin for CntListModel. It can provide
    the presence information of a contact (icon2 field).
 */
class CntPresenceInfoProvider : public CntInfoProvider
{
    friend class TestCntPresenceInfoProvider;
    Q_OBJECT
    
public:
    CntPresenceInfoProvider();
    ~CntPresenceInfoProvider();
    
	// From CntInfoProvider
    QString id() const { return "presence"; };
    ContactInfoFields supportedFields() const;
    void requestInfo(const QContact& contact, ContactInfoFields requestedInfo);
    
private slots:
    void handlePresenceUpdate(bool aSuccess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo);
    
private:
    QString parsePresence(const QList<PrcPresenceBuddyInfoQt*>& buddyList);

signals:
    void infoFieldReady(CntInfoProvider* sender, int contactId, ContactInfoField field, const QString& value);
    
private:
    PrcPresenceReader*              iReader; // owned
    QContactManager*                mManager; // owned
    QMap<QString, QContactLocalId>  mBuddyMap;
};

#endif
