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
*     The header file of filemanager view manager
*
*/

#ifndef FMVIEWMANAGER_H
#define FMVIEWMANAGER_H

#include <QObject>
#include <QList>
#include "fmcommon.h"
#include <QPixmap>

class FmMainWindow;
class HbView;
class FmViewManager;
class FmOperationService;
class FmOperationBase;
class QFileSystemWatcher;
class FmDriveWatcher;
class FmDialog;
class FmServiceUtils;

/*
 * this class is used to record relationship between dialog and drive name 
 * FmViewManager will auto-close dialog which related drive is not available again.
 * for example. set name to MMC will pop a dialog, if add the dialog to FmViewManager::addDlgCloseUnit 
 * then after eject MMC, the dialog will be auto-closed by FmViewManager::checkDlgCloseUnit()
 */
class FmDlgCloseUnit
{
public:
	FmDlgCloseUnit( FmDialog *dialog );
	~FmDlgCloseUnit();

	void addAssociatedDrives( QString drives );
	void removeAssociatedDrives( QString drives );
	QString associatedDrives();
	FmDialog *dialog();

private:
	FmDialog *mDialog;
	QString mAssociatedDrives;
};

class FmViewManager : public QObject
{
    Q_OBJECT

public:
    int viewCount();
    static FmViewManager *CreateViewManager( FmMainWindow* mainWindow );
    static void RemoveViewManager();
    static FmViewManager *viewManager();
    FmOperationService *operationService();
    FmServiceUtils *serviceUtils();
    
    Qt::Orientation orientation(); 

    void createFileView( const QString &path,
    bool enableBackPathCheck = true, bool disableFind = false );
    void createFindView( const QString &keyword, const QStringList &pathList );
    void createDriverView();
    void createSplitView();
	void createBackupView();
	void createRestoreView();
    void createDeleteBackupView();
    void saveActivity();    
	// add a close unit. this is used to auto-close dialog which related drive is not available again.
	void addDlgCloseUnit( FmDlgCloseUnit* unit )
	{
		FM_LOG( "FmViewManager::addDlgCloseUnit_" + unit->associatedDrives() );
		mDlgCloseUnitList.append( unit );
	}
	
	// remove a close unit.
	void removeDlgCloseUnit( FmDlgCloseUnit* unit )
	{
		FM_LOG( "FmViewManager::removeDlgCloseUnit_" + unit->associatedDrives() );
		mDlgCloseUnitList.removeOne( unit );
	}

protected:
    explicit FmViewManager( FmMainWindow* mainWindow );
    ~FmViewManager();
    
public slots:
   /// popViewAndShow will delete current view and pop view from stack, then show it.
    void popViewAndShow();
    
    // triggered when drive space is changed
    // this slots is used to watch filemanager internal events
    // drive space event from other applictaion will not be observed here
    void on_operationService_driveSpaceChanged( FmOperationBase* operationBase );
    
    // triggered when drive is ejected/inserted, or other drive event. it will impact available property
    void on_driveWatcher_driveAddedOrChanged();
    
    // if operation finished, some addition works( such as refresh... ) should be done in some views
    // so this is the central controller function
    void on_operationService_notifyFinish( FmOperationBase *operationBase );
    
    void onAboutToChangeView(HbView * oldView, HbView *newView);
    
signals:
    // emit when drive space is changed by some operation inside filemanager.
    void driveSpaceChanged();
    
    // emit when drive is ejected/inserted, this is watched by FmDriveWatcher
    void driveChanged();
    
    // when backup is created, backup date should be updated.
    void refreshBackupDate();

private:
    // when drive is ejected, this function will be called and exam dialogs in mDlgCloseUnitList
    // it will colse dialogs if related drive is not available
	void checkDlgCloseUnit();
	    
private:
     static FmViewManager *mViewManager;

private:
    // main window
    FmMainWindow        *mMainWindow;
    
    // operation service, used to start operations such as copy, move, delete, format...
    FmOperationService  *mOperationService;
    
    // used to watch drive event, for example, eject/insert drive
    FmDriveWatcher      *mDriveWatcher;

    // used to record some dialogs that related to drive
    // the dialog should be closed in checkDlgCloseUnit if drive is not available 
	QList<FmDlgCloseUnit*> mDlgCloseUnitList;
	
	QPixmap mScreenShot;

    // non-static service utils
    FmServiceUtils *mServiceUtils;
};





#endif


