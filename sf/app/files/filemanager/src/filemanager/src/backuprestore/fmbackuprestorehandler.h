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
 * 
 * Description:
 *     The header file of backup and restore handler of file manager
 */

#ifndef FMBACKUPRESTOREHANDLER_H
#define FMBACKUPRESTOREHANDLER_H

#include <QObject>

#include "fmoperationservice.h"

class FmBkupEngine;
class FmOperationBackup;
class FmOperationRestore;
class FmBackupConfigLoader;

class FmBackupRestoreHandler : public QObject
{
Q_OBJECT

public:

    FmBackupRestoreHandler( QObject *parent );
    ~FmBackupRestoreHandler();

    int error();
    
    bool startBackup( FmOperationBackup *operationBackup );
    void cancelBackup();
    
    bool startRestore( FmOperationRestore *operationRestore );

    int deleteBackup( quint64 selection );
    FmBkupEngine *bkupEngine();
    FmBackupConfigLoader *backupConfigLoader();
    void getBackupDriveList( QStringList &driveList );

signals:
    void notifyPreparing( bool cancelable );
    void notifyStart( bool cancelable, int maxSteps );
    void notifyProgress( int currentStep );
    void notifyFinish();
    void notifyError( int error, QString errString ); 
    void notifyCanceled(); 

    
public slots:
    void onNotifyPreparing( bool cancelable );
    void onNotifyMemoryLow( int memoryValue, int &userError );
    
    void onNotifyStart( bool cancelable, int maxSteps );
    void onNotifyUpdate( int currentStep );
    void onNotifyFinish( int err );
    
    void onNotifyBackupFilesExist( bool &isContinue );
private:
    enum Process{
		ProcessNone = 0,
		ProcessBackup,
		ProcessRestore
	};
private:
    FmBkupEngine  *mBkupEngine;
    FmBackupConfigLoader        *mBackupConfigLoader;
    Process       mCurrentProcess;
};

#endif
