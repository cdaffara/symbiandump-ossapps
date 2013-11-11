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
 *      The header file of the backup engine from bkup engine wrapper
 */


#ifndef FMBKUPENGINE_H
#define FMBKUPENGINE_H

// INCLUDES
#include <QObject>
#include <QList>
#include "fmbkupengine_global.h"

#include "fmdefine.h"

// FORWARD DECLARATIONS
class FmBkupEnginePrivate;


class FmBkupDrivesAndOperation;
class FmBkupBackupCategory;
class FmRestoreInfo;

class FmBackupSettings;
class FmRestoreSettings;

// CLASS DECLARATION
class FMBKUPENGINE_EXPORT FmBkupEngine : public QObject
{
Q_OBJECT
public:

    enum MAEngineBackupCategories
        {
        EBUCatSettings = 0x1,
        EBUCatMessages = 0x2,
        EBUCatContacts = 0x4,
        EBUCatCalendar = 0x8,
        EBUCatBookmarks = 0x10,
        EBUCatUserFiles = 0x20,
        EBUCatAllInOne = 0x8000000,
        EBUCatAllSeparately = 0x7FFFFFF 
        // Set as 0xFFFFFFF (EBUCatAllInOne+EBUCatAllSeparately) if archive containing 
        // data from all data owners needs to be created.
        };

    
    enum Process{
		ProcessNone = 0,
		ProcessBackup,
		ProcessRestore
	};
    
    FmBkupEngine(QObject* parent = 0);
    ~FmBkupEngine();
 
    
    bool startBackup(    QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
    QList<FmBkupBackupCategory*> backupCategoryList,
    QString drive, quint32 content);
    
    void cancelBackup();

    bool startRestore( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );
    int deleteBackup( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );
    int error() const;

    FmBackupSettings* BackupSettingsL();
    FmRestoreSettings* RestoreSettingsL();
    
    void GetRestoreInfoArray( QList<FmBkupDrivesAndOperation* > &drivesAndOperationList,
            QList< FmRestoreInfo > &restoreInfoList,
            const QString& aDrive );
    
    void getBackupDriveList( QStringList &driveList );
    
signals:
    void notifyPreparing( bool cancelable );
	void notifyStart( bool cancelable, int totalCount );
	void notifyUpdate( int count );
	void notifyFinish( int err );
	void notifyMemoryLow( int memoryValue, int &userError );
	void notifyBackupFilesExist( bool &isContinue );
    
private:
    friend class FmBkupEnginePrivate;
    FmBkupEnginePrivate* d;
    
    FmBackupSettings *iBackupSettings;
    FmRestoreSettings *iRestoreSettings;
};

#endif /*FMBKUPENGINE_H*/

// End of file
