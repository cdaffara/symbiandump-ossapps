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
 *      The header file of the backup engine implement for symbian
 */

#ifndef FMBKUPENGINEPRIVATE_H
#define FMBKUPENGINEPRIVATE_H

// INCLUDES
#include <e32base.h>
#include "CMMCScBkupOperationParameters.h"
#include "fmbkupengine.h"
#include "fmbkupcommon.h"

#include "fmbackupsettings.h"
#include "fmrestoresettings.h"

#include "MMMCScBkupEngineObserver.h"
#include <f32file.h>

#include <QObject>
#include <QStringList>

class CMMCScBkupEngine;

NONSHARABLE_CLASS(TMaskLookup)
    {
    public:
        TUint32 iBkupMask;
        TUint32 iFmgrMask;
    };

const TMaskLookup KMaskLookup[] = {
    { FmBkupEngineCommon::EBUCatSettings,  FmBkupEngineCommon::EFileManagerBackupContentSettings  },
    { FmBkupEngineCommon::EBUCatMessages,  FmBkupEngineCommon::EFileManagerBackupContentMessages  },
    { FmBkupEngineCommon::EBUCatContacts,  FmBkupEngineCommon::EFileManagerBackupContentContacts  },
    { FmBkupEngineCommon::EBUCatCalendar,  FmBkupEngineCommon::EFileManagerBackupContentCalendar  },
    { FmBkupEngineCommon::EBUCatBookmarks, FmBkupEngineCommon::EFileManagerBackupContentBookmarks },
    { FmBkupEngineCommon::EBUCatUserFiles, FmBkupEngineCommon::EFileManagerBackupContentUserFiles }
};
const TUint32 KMaskLookupLen =
    sizeof( KMaskLookup ) / sizeof( KMaskLookup[ 0 ] );

// CLASS DECLARATION
class FmBkupEnginePrivate: public QObject, public MMMCScBkupEngineObserver
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
    

    TUint32 FmgrToBkupMask( const TUint32 aFmrgMask );
    int error();
    
    //
    bool StartRestoreL( QList<FmBkupDrivesAndOperation* > drivesAndOperationList );
    void GetRestoreInfoArray( QList<FmBkupDrivesAndOperation* > drivesAndOperationList,
            QList< FmRestoreInfo > &restoreInfoList,
            const QString& aDrive );
    
    void getBackupDriveList( QStringList &driveList );

signals:
    void notifyPreparing( bool cancelable );
	void notifyStart( bool cancelable, int totalCount );
	void notifyUpdate( int totalCount );
	void notifyFinish( int err );
	void notifyMemoryLow( int memoryValue, int &userError );
	void notifyBackupFilesExist( bool &isContinue );
private:
    void notifyPreparingInternal();
	void notifyStartInternal( int aTotalCount );
	void notifyUpdateInternal( int aCount );
	void notifyFinishInternal();
	void notifyMemoryLowInternal( int memoryValue, int &userError );
	void notifyBackupFilesExistInternal( bool &isContinue );

private: // From MMMCScBkupEngineObserver
    TInt HandleBkupEngineEventL(
        MMMCScBkupEngineObserver::TEvent aEvent, TInt aAssociatedData );
    
    static void ResetAndDestroyArchives( TAny* aPtr );
    TUint32 BkupToFmgrMask( const TUint32 aBkupMask );
    TUint32 AllowedDriveAttMatchMask() const;
    TInt DriverNameToNumber( QString driverName );
    QString NumberToDriverName( TInt driver );
    void PublishBurStatus( TInt aType );
    
private:
    FmBkupEngine* q;
    
    BkupDrivesAndOperationList    *iDrvAndOpList;
    BkupCategoryList           *iBkupCategoryList;    
    
    CMMCScBkupEngine* iBkupEngine;
    
    RFs iFs;
    
    
    FmBkupEngine::Process mProcess;
    int iError;
    int iFinalValue;
};

#endif /*FMBKUPENGINEPRIVATE_H*/

// End of file
