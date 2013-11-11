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

#ifndef CNTPRESENCELISTENER_H
#define CNTPRESENCELISTENER_H

#include <QObject>
#include <qtcontacts.h>
#include <QMap>

class PrcPresenceReader;
class PrcPresenceBuddyInfoQt;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/**
    Presence listener for contact card. It provides aggregated
    presence information for a contact and separately for each
    valid QContactOnlineAccount.
*/
class CntPresenceListener : public QObject
{
    friend class TestCntPresenceListener;
    
    Q_OBJECT
    
public:
    CntPresenceListener(QObject* parent = NULL);
    ~CntPresenceListener();
    
    QMap<QString, bool> initialPresences(const QContact& contact, bool &combinedOnlineStatus);
    
private slots:
    void handlePresenceUpdate(bool aSuccess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo);
    
private:
    bool parsePresence(QList<PrcPresenceBuddyInfoQt*> buddyList);
    
signals:
    /// account-specific presence status
    void accountPresenceUpdated(const QString& accountUri, bool online);
    /// aggregated presence status for this contact
    void fullPresenceUpdated(bool online);
    
private:
    PrcPresenceReader*  mReader; // own
    QStringList         mAccountList;
};

#endif // CNTPRESENCELISTENER_H
