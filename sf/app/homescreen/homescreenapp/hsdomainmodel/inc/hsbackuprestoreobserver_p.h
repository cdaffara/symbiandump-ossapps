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
* Description:  Active object for Backup/Restore observer.
*
*/

#ifndef HSBACKUPRESTOREOBSERVER_P_H
#define HSBACKUPRESTOREOBSERVER_P_H

#include <connect/abclient.h>
#include <XQSettingsKey>
#include <QObject>

class CHsBURActiveCallback;
class HsBackupRestoreObserver;

using namespace conn;

class HsBackupRestoreObserverPrivate : public QObject
{
    Q_OBJECT

public:

    /**
     * Creates an instance of CCPServerBURListener implementation.
     * @param aCallBack Pointer to observer interface.
     */
    HsBackupRestoreObserverPrivate (HsBackupRestoreObserver* observer,
                                    QObject *parent = 0);

    /**
     * Destructor.
     */
    virtual ~HsBackupRestoreObserverPrivate();
    
    void readyForBUR();

public slots:    
     void handleKeyChange(XQSettingsKey key, const QVariant &value);

private:
	/**
     * mQ pointer to HsBackupRestoreObserver
     * Owned by HsBackupRestoreObserver object, instantiated in
     * constructor.
     */
    HsBackupRestoreObserver *mQ;

    /**
     * Callback interface for widget's active data owner.
     * Own.
     */
    CHsBURActiveCallback *mCallBack;

    /**
     * Active backup client.
     * Own.
     */
	conn::CActiveBackupClient *mActiveBackupClient;
    
    /**
     * Last state of BUR: either backup or restore or normal or unset
     */
    conn::TBURPartType mLastType;
};

#endif //HSBACKUPRESTOREOBSERVER_P_H
