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
 *      The header file of the backup engine implement for win32
 */

#ifndef FMBKUPENGINEPRIVATE_H
#define FMBKUPENGINEPRIVATE_H

// INCLUDES
#include "fmbkupengine.h"

// FORWARD DECLARATIONS



#include <QString>
#include <QStringList>

#include <QThread>

class FmBkupThread;


// CLASS DECLARATION
class FmBkupEnginePrivate : public QObject
{
Q_OBJECT
public:
    FmBkupEnginePrivate(FmBkupEngine* bkupEngine);
    ~FmBkupEnginePrivate();
    
    bool startBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
    QList<FmBkupBackupCategory*> backupCategoryList,
    QString drive, quint32 content);
    
    void cancelBackup();
    int deleteBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );

//    TUint32 FmgrToBkupMask( const TUint32 aFmrgMask );
    int error();
    
    //
    bool StartRestoreL( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );
    void GetRestoreInfoArray( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
            QList< FmRestoreInfo > &restoreInfoList,
            const QString& aDrive );
    void getBackupDriveList( QStringList &driveList );
private:
    int DriverNameToNumber( QString driverName );
    QString NumberToDriverName( int driver );
    
//    TBool HasMultipleBackupTargets();

signals:
    void notifyPreparing( bool cancelable );
    void notifyStart( bool cancelable, int totalCount );
    void notifyUpdate( int count );
    void notifyFinish( int err );
    void notifyMemoryLow( int memoryValue, int &userError );

    // win32 source code is used to simulator backup action.
    // add this signal in win32 to avoid warning when run test case
    // though it is not used in win32 source code currently
    void notifyBackupFilesExist( bool &isContinue );
	
public slots:
    void onNotifyPreparing( bool cancelable );
    void onNotifyStart( bool cancelable, int totalCount );
	void onNotifyUpdate( int count );
	void onNotifyFinish( int err );
	void onNotifyMemoryLow( int memoryValue, int &userError );

private:
    int mError;
    FmBkupEngine* q;
    FmBkupThread* mBkupThread;

    QList< FmRestoreInfo > selection;
};



class FmBkupThread : public QThread
{
    Q_OBJECT
public:
    enum RunType
    {
        RunTypeNone,
        RunTypeBackup,
        RunTypeRestore
    };

    FmBkupThread( QObject *parent );
    ~FmBkupThread();

    int asyncBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
QList<FmBkupBackupCategory*> backupCategoryList,QString drive, quint32 content );
    int asyncRestore( QList<FmBkupDrivesAndOperation* > drivesAndOperationList, QList< FmRestoreInfo > selection );
    void stop();

signals:
    void notifyPreparing( bool cancelable );
    void notifyStart( bool cancelable, int totalCount );
	void notifyUpdate( int totalCount );
	void notifyFinish( int err );
	void notifyMemoryLow( int memoryValue, int &userError );

protected:
    void run();

private:
    volatile bool mStop;
    RunType mRunType;

    QList<FmBkupDrivesAndOperation* > mDrivesAndOperationList;
    QList<FmBkupBackupCategory*> mBackupCategoryList;
    QString mDrive;
    quint32 mContent;
    QList< FmRestoreInfo > mSelection;

};


#endif /*FMBKUPENGINEPRIVATE_H*/

// End of file
