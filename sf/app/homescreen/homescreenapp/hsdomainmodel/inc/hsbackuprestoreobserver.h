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
* Description:  Backup/Restore observer.
*
*/

#ifndef HSBACKUPRESTOREOBSERVER_H
#define HSBACKUPRESTOREOBSERVER_H

#include <QObject>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

#ifdef Q_OS_SYMBIAN
class HsBackupRestoreObserverPrivate;
#endif //Q_OS_SYMBIAN

class HSDOMAINMODEL_EXPORT HsBackupRestoreObserver : public QObject
{
	Q_OBJECT

public:
	static HsBackupRestoreObserver *instance();	
	~HsBackupRestoreObserver();
	
	void backupRestoreStarted();
    void backupRestoreCompleted();
    
    void readyForBUR();
    bool checkBUR();

signals:	
    void event_backupRestoreStarted();
    void event_backupRestoreCompleted();	

private:
	HsBackupRestoreObserver(QObject *parent = 0);
    Q_DISABLE_COPY(HsBackupRestoreObserver)

private:

#ifdef Q_OS_SYMBIAN
    /**
     * mD pointer to HsBackupRestoreObserverPrivate
     * Owned by HsBackupRestoreObserver object, instantiated in
     * constructor.
     */
    HsBackupRestoreObserverPrivate *mD;
#endif //Q_OS_SYMBIAN
    
    bool mBUROngoing;

    static HsBackupRestoreObserver *mInstance;
#ifdef Q_OS_SYMBIAN
    friend class HsBackupRestoreObserverPrivate;
#endif //Q_OS_SYMBIAN
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSBACKUPRESTOREOBSERVER_H

// End of File
