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
 *     The source file of the operation service of file manager
 */

#include "fmoperationservice.h"
#include "fmoperationthread.h"
#include "fmbackupconfigloader.h"
#include "fmbkupengine.h"
#include "fmbackupsettings.h"
#include "fmviewdetailsdialog.h"
#include "fmoperationresultprocesser.h"
#include "fmoperationcopyormove.h"
#include "fmoperationremove.h"
#include "fmoperationformat.h"
#include "fmoperationviewdetails.h"
#include "fmbackuprestorehandler.h"
#include <hbaction.h>

/* \fn void driveSpaceChanged( FmOperationBase* operationBase )
 * This signal is emitted when disk size changed.
 */

/* \fn void notifyWaiting( FmOperationBase* operationBase, bool cancelable )
 * This signal is emitted when the operation emits notifyWaiting.
 */

/* \fn void notifyPreparing( FmOperationBase* operationBase, bool cancelable )
 * This signal is emitted when the operation emits notifyPreparing.
 */

/* \fn void notifyStart( FmOperationBase* operationBase, bool cancelable, int maxSteps )
 * This signal is emitted when the operation emits notifyStart.
 */

/* \fn void notifyProgress( FmOperationBase* operationBase, int currentStep )
 * This signal is emitted when the operation emits notifyProgress.
 */
    
/* \fn void notifyFinish( FmOperationBase* operationBase )
 * This signal is emitted when the operation emits notifyFinish.
 */ 
    
/* \fn void notifyError( FmOperationBase* operationBase, int error, QString errString )
 * This signal is emitted when the operation emits notifyError.
 */ 
 
/* \fn void notifyCanceled( FmOperationBase* operationBase )
 * This signal is emitted when the operation emits notifyCanceled.
 */ 

/*
 * Constructs one operation Service with \a parent.
 */
FmOperationService::FmOperationService( QObject *parent ) : QObject( parent ),
        mBackupRestoreHandler( 0 ), mCurrentOperation( 0 )
{
    mThread = new FmOperationThread( this );
    mThread->setObjectName( "operationThread" );
    mOperationResultProcesser = new FmOperationResultProcesser( this );
    
    QMetaObject::connectSlotsByName( this );
}

/*
 * Destructs the operation service.
 */
FmOperationService::~FmOperationService()
{
    delete mThread;    
    mThread = 0;
    delete mBackupRestoreHandler;
    mBackupRestoreHandler = 0;
}

/*
 * Returns true if the thread is running, false if not.
 */
bool FmOperationService::isRunning()
{
    return mThread->isRunning();    
}

/*
 * Copies the file or foler \a targetPath asynchronously. 
 */
int FmOperationService::asyncCopy( const QStringList &sourceList, const QString &targetPath )
{    
    Q_ASSERT( !mCurrentOperation ); 
    mCurrentOperation  = new FmOperationCopyOrMove( this, FmOperationService::EOperationTypeCopy, sourceList, targetPath );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Moves the file or foler \a sourceList to \a targetPath asynchronously. 
 */
int FmOperationService::asyncMove( const QStringList &sourceList, const QString &targetPath )
{
    Q_ASSERT( !mCurrentOperation );

    mCurrentOperation  = new FmOperationCopyOrMove( this, FmOperationService::EOperationTypeMove, sourceList, targetPath );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Removes the file or dir \a pathList asynchronously. 
 */
int FmOperationService::asyncRemove( const QStringList &pathList )
{
    Q_ASSERT( !mCurrentOperation ); 

    mCurrentOperation = new FmOperationRemove( this, pathList );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Formats the drive \a driverName asynchronously. 
 */
int FmOperationService::asyncFormat( const QString &driverName )
{
    Q_ASSERT( !mCurrentOperation );

    mCurrentOperation = new FmOperationFormat( this, driverName );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Views drive \a driverName details asynchronously.
 */
int FmOperationService::asyncViewDriveDetails( const QString &driverName )
{
    Q_ASSERT( !mCurrentOperation );
    
    mCurrentOperation = new FmOperationDriveDetails( this, driverName );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Views folder \a folderPath details asynchronously. 
 */
int FmOperationService::asyncViewFolderDetails( const QString &folderPath )
{
    Q_ASSERT( !mCurrentOperation );
    
    mCurrentOperation = new FmOperationFolderDetails( this, folderPath );
    connectSignalsAndSlots( mCurrentOperation );
    int ret = mThread->prepareOperationAndStart( mCurrentOperation );    
    if ( ret!= FmErrNone ) {
        resetOperation();
        return ret; 
    }        
    return FmErrNone;
}

/*
 * Backups asynchronously. 
 */
int FmOperationService::asyncBackup()
{   
    if ( isRunning() )
        return FmErrAlreadyStarted;
    Q_ASSERT( !mCurrentOperation );

    // BackupSettingsL will not leave, coding convention will be improvied in another task.
    QString targetDrive( backupRestoreHandler()->bkupEngine()->BackupSettingsL()->availableTargetDrive() );
    quint32 content( backupRestoreHandler()->bkupEngine()->BackupSettingsL()->content() );
    FmOperationBackup *operationBackup = 
            new FmOperationBackup( backupRestoreHandler(), targetDrive, content );
    mCurrentOperation = operationBackup;
    int ret = backupRestoreHandler()->startBackup( operationBackup );
    if( ret ){
        return FmErrNone;
    } else {
        resetOperation();
        return backupRestoreHandler()->error();
    }
}

/*
 * Restores asynchronously. 
 * \a selection selected restore items
 */
int FmOperationService::asyncRestore( quint64 selection )
{
    if ( isRunning() )
        return FmErrAlreadyStarted;
    Q_ASSERT( !mCurrentOperation );

    FmOperationRestore* operationRestore = new FmOperationRestore( mBackupRestoreHandler, selection );
    mCurrentOperation = operationRestore;
    int ret = backupRestoreHandler()->startRestore( operationRestore );
    if( ret ){
        return FmErrNone;
    } else {
        resetOperation();
        return backupRestoreHandler()->error();
    }
}

/*
 * Delete backup synchronously. 
 * \a selection selected backup items
 */
int FmOperationService::syncDeleteBackup( quint64 selection )
{
    return backupRestoreHandler()->deleteBackup( selection );
}

/*
 * Cancels current operation.
 */
void FmOperationService::cancelOperation()
{
    switch( mCurrentOperation->operationType() )
    {
    case  EOperationTypeBackup:
        backupRestoreHandler()->cancelBackup();
        break;
    case EOperationTypeDriveDetails:
        mThread->stop();
        break;
    case EOperationTypeFolderDetails:
        mThread->stop();
        break;
    case EOperationTypeFormat:
        //can not cancel format
        break;
    case EOperationTypeCopy:
         mThread->stop();
        break;
    case EOperationTypeMove:
         mThread->stop();
        break;
    case EOperationTypeRemove:
         mThread->stop();
        break;
    default:
        Q_ASSERT( false );
    }    
}

/*
 * Set valume synchronously. not used.
 */
int FmOperationService::syncSetVolume( const QString &driverName, const QString &volume )
{
    Q_UNUSED( driverName );
    Q_UNUSED( volume );
    return FmErrNone;
}

/*
 * Set drive password synchronously. not used.
 */
int FmOperationService::syncSetdDriverPassword( const QString &driverName,
                                               const QString &oldPassword, 
                                               const QString &newPassword )
{
    Q_UNUSED( driverName );
    Q_UNUSED( oldPassword );
    Q_UNUSED( newPassword );
    return FmErrNone;
}

/*
 * Rename synchronously. not used.
 */
int FmOperationService::syncRename( const QString &oldPath, const QString &newName )
{
    Q_UNUSED( oldPath );
    Q_UNUSED( newName );
    return FmErrNone;
}

/*
 * Launches the file in synchronous way.
 */
int FmOperationService::syncLaunchFileOpen( const QString &filePath )
{
    return FmUtils::launchFile( filePath );
}

/*
 * Returns the backup handler.
 */
FmBackupRestoreHandler *FmOperationService::backupRestoreHandler()
{
    if( !mBackupRestoreHandler ) {
        mBackupRestoreHandler = new FmBackupRestoreHandler( this );        
        mBackupRestoreHandler->setObjectName( "backupRestore" ) ;
        QMetaObject::connectSlotsByName( this );
    }
    return mBackupRestoreHandler;
}

/*
 * Deletes the operation and set it to be 0.
 */
void FmOperationService::resetOperation()
{
   if( mCurrentOperation ) {
        delete mCurrentOperation;
        mCurrentOperation = 0;
    }
}

/*
 * Connects \a operation's sinals to slots
 */
void FmOperationService::connectSignalsAndSlots( FmOperationBase *operation )
{
    
    connect( operation, SIGNAL( showNote( QString ) ),           // blocking because need wait for show note
            this, SLOT( on_operation_showNote( QString )),       Qt::BlockingQueuedConnection );
    connect( operation, SIGNAL( notifyPreparing( bool ) ),       // blocking to show waiting note immediately
            this, SLOT( on_operation_notifyPreparing( bool )),   Qt::BlockingQueuedConnection );
    connect( operation, SIGNAL( notifyProgress( int ) ),         // blocking to make progress more accurate
            this, SLOT( on_operation_notifyProgress( int ) ),    Qt::BlockingQueuedConnection );
    connect( operation, SIGNAL( notifyStart( bool, int ) ),      // blocking to show waiting note immediately
            this, SLOT( on_operation_notifyStart( bool, int ) ), Qt::BlockingQueuedConnection );    
    connect( operation, SIGNAL( notifyWaiting( bool ) ),         // blocking to show waiting note immediately
            this, SLOT( on_operation_notifyWaiting( bool )),     Qt::BlockingQueuedConnection );    
    connect( operation, SIGNAL( notifyFinish() ),
            this, SLOT( on_operation_notifyFinish()) );
    connect( operation, SIGNAL( notifyError( int, QString ) ),
            this, SLOT( on_operation_notifyError( int, QString ) ) );
}

/*
 * Responds to mCurrentOperation's askForRename signal.
 * \a srcFile the source file.
 * \a destFile the new file name.
 */
void FmOperationService::on_operation_askForRename( const QString &srcFile, QString *destFile )
{
    mOperationResultProcesser->onAskForRename(
        mCurrentOperation, srcFile, destFile );
}

/*
 * Responds to mCurrentOperation's askForReplace signal.
 * \a srcFile the source file.
 * \a destFile the target file.
 * \a isAccepted whether to replace the target file.
 */
void FmOperationService::on_operation_askForReplace( const QString &srcFile, const QString &destFile, bool *isAccepted )
{
    mOperationResultProcesser->onAskForReplace(
        mCurrentOperation, srcFile, destFile, isAccepted );
}

/*
 * Responds to mCurrentOperation's showNote signal.
 * \a noteString the note content.
 */
void FmOperationService::on_operation_showNote( const char *noteString )
{
    mOperationResultProcesser->onShowNote( mCurrentOperation, noteString );
}

/*
 * Responds to mCurrentOperation's showNote signal.
 * \a noteString the note content.
 */
void FmOperationService::on_operation_notifyWaiting( bool cancelable )
{
    mOperationResultProcesser->onNotifyWaiting(
        mCurrentOperation, cancelable );
    emit notifyWaiting( mCurrentOperation, cancelable );
}

/*
 * Responds to mCurrentOperation's notifyPreparing signal.
 * \a cancelable indicates whether the progress bar could be cancelled.
 */
void FmOperationService::on_operation_notifyPreparing( bool cancelable )
{
    mOperationResultProcesser->onNotifyPreparing(
        mCurrentOperation, cancelable );
    emit notifyPreparing( mCurrentOperation, cancelable );
}

/*
 * Responds to mCurrentOperation's notifyPreparing signal.
 * \a cancelable indicates whether the progress bar could be cancelled.
 * \maxSteps the length of progress bar.
 */
void FmOperationService::on_operation_notifyStart( bool cancelable, int maxSteps )
{
    mOperationResultProcesser->onNotifyStart(
        mCurrentOperation, cancelable, maxSteps );
    emit notifyStart( mCurrentOperation, cancelable, maxSteps );
}

/*
 * Responds to mCurrentOperation's notifyPreparing signal.
 * \a currentStep indicates the current length of progress bar.
 */
void FmOperationService::on_operation_notifyProgress( int currentStep )
{
    mOperationResultProcesser->onNotifyProgress(
        mCurrentOperation, currentStep );
    emit notifyProgress( mCurrentOperation, currentStep );
}

/*
 * Responds to mCurrentOperation's notifyFinish signal, indicate the
 * progress is over.
 */
void FmOperationService::on_operation_notifyFinish()
{
    mOperationResultProcesser->onNotifyFinish( mCurrentOperation );
    emit notifyFinish( mCurrentOperation );
    resetOperation();
}

/*
 * Responds to mCurrentOperation's notifyError signal.
 * \a error error id.
 * \a errString the error string.
 */
void FmOperationService::on_operation_notifyError(int error, QString errString )
{
    mOperationResultProcesser->onNotifyError(
        mCurrentOperation, error, errString );
    emit notifyError( mCurrentOperation, error, errString );
    resetOperation();
}

/*
 * Responds to mCurrentOperation's driveSpaceChanged 
 */
void FmOperationService::on_operation_driveSpaceChanged()
{
    emit driveSpaceChanged( mCurrentOperation );
}

/*
 * Responds to mBackupRestoreHandler's notifyPreparing 
 * \a cancelable indicates whether it could be cancelled.
 */
void FmOperationService::on_backupRestore_notifyPreparing( bool cancelable )
{
    mOperationResultProcesser->onNotifyPreparing(
        mCurrentOperation, cancelable );
     emit notifyPreparing( mCurrentOperation, cancelable );
}

/*
 * Responds to mBackupRestoreHandler's notifyStart 
 * \a cancelable indicates whether it could be cancelled.
 * \a maxSteps the lenth of progress bar.
 */
void FmOperationService::on_backupRestore_notifyStart( bool cancelable, int maxSteps )
{
    mOperationResultProcesser->onNotifyStart(
        mCurrentOperation, cancelable, maxSteps );
    emit notifyStart( mCurrentOperation, cancelable, maxSteps );
}

/*
 * Responds to mBackupRestoreHandler's notifyProgress 
 * \a currentStep the current progress bar's step.
 */
void FmOperationService::on_backupRestore_notifyProgress( int currentStep )
{
    mOperationResultProcesser->onNotifyProgress(
        mCurrentOperation, currentStep );
    emit notifyProgress( mCurrentOperation, currentStep );
}

/*
 * Responds to mBackupRestoreHandler's notifyFinish 
 */
void FmOperationService::on_backupRestore_notifyFinish()
{
    mOperationResultProcesser->onNotifyFinish( mCurrentOperation );
    emit notifyFinish( mCurrentOperation );
    resetOperation();
}

/*
 * Responds to mBackupRestoreHandler's notifyError
 * \a error the error id.
 * \a errString the error string.
 */
void FmOperationService::on_backupRestore_notifyError(int error, const QString &errString )
{
    mOperationResultProcesser->onNotifyError(
        mCurrentOperation, error, errString );
    emit notifyError( mCurrentOperation, error, errString );
    resetOperation();
}

/*
 * Responds to mBackupRestoreHandler's notifyCanceled 
 */
void FmOperationService::on_backupRestore_notifyCanceled()
{
    mOperationResultProcesser->onNotifyError(
        mCurrentOperation, FmErrCancel, QString() );
    emit notifyError( mCurrentOperation, FmErrCancel, QString() );
    resetOperation();
}

///
/////////////////////////////////////////////////////
