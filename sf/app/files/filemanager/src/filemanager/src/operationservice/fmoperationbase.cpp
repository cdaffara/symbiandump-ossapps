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
*     The source file of the operation param of file manager
*
*/

#include "fmoperationbase.h"
#include "fmdrivedetailstype.h"
#include <QtAlgorithms>
/*
 * \fn void showNote( const QString &note )
 * This signal is emitted when operation needs operation service to show some note.
 * \a note indicates the note to show. 
 */

/* \fn  void notifyWaiting( bool cancelable )
 * This signal is emitted when operation needs operation service to wait.
 * \a cancelable indicates whether it could be canceled.
 */

/* \fn  void notifyPreparing( bool cancelable )
 * This signal is emitted when operation needs operation service to prepare.
 * \a cancelable indicates whether it could be canceled.
 */

/* \fn  void notifyStart( bool cancelable, int maxSteps )
 * This signal is emitted when operation starts and operation service shows progress bar.
 * \a cancelable indicates whether it could be canceled.
 * \a maxStep indicates the max of progress bar.
 */
    
/* \fn  void notifyProgress( int currentStep )
 * This signal is emitted when operation needs operation service update progress bar.
 * \a currentStep the current step.
 */

/* \fn  void notifyFinish()
 * This signal is emitted when operation finished.
 */

/* \fn  void notifyError( int error, const QString &errString )
 * This signal is used to notify error.
 * \a error the error id.
 * \a errString the error string.
 */
    
/*
  Constructs the base operation with
  \a parent parent
  \a operationType the type of current operation.
*/
FmOperationBase::FmOperationBase( QObject *parent, FmOperationService::TOperationType operationType ) : 
    QObject( parent ), mOperationType( operationType )
{
}

/*
  Destructs the operation
*/
FmOperationBase::~FmOperationBase()
{

}

/*
  Returns the current operation type
*/
FmOperationService::TOperationType FmOperationBase::operationType()
{
    return mOperationType;
}

/*
  Starts the operation, called by FmOperationThread.
  Use empty implementation because Backup and Restore 
  are not done within thread, so they will not reimplement
  this function.
  \sa FmOperationThread::run
*/
void FmOperationBase::start( volatile bool */*isStopped*/ )
{

}

/*
  Does some additional work before starting the operation.
  Returns the error number.
*/
int FmOperationBase::prepare()
{
    return FmErrNone;
}


/*
  Constructs a backup operation with
  \a parent parent
  \a targetDrive backup target drive
  backup \a content is mask for contents want to backup. Refer to mContent in FmBackupSettings
*/
FmOperationBackup::FmOperationBackup( QObject *parent, const QString &targetDrive, quint32 content )
    : FmOperationBase( parent, FmOperationService::EOperationTypeBackup ),
      mTargetDrive( targetDrive ), mContent( content )
{
}


/*
  Destructs the backup operation
*/
FmOperationBackup::~FmOperationBackup()
{
}

/*
  Get target backup drive
*/
QString FmOperationBackup::targetDrive() const 
{
    return mTargetDrive;
}

/*
  Get backup content mask. Refer to mContent in FmBackupSettings
*/
quint32 FmOperationBackup::content() const
{
    return mContent;
}

/*
  Constructs a restore operation with
  \a parent parent
  \a selection the selected backup items
*/
FmOperationRestore::FmOperationRestore( QObject *parent, quint64 selection )
    : FmOperationBase( parent, FmOperationService::EOperationTypeRestore ), mSelection( selection )
{
}

/*
  Destructs the restore operation
*/
FmOperationRestore::~FmOperationRestore()
{
}

/*
  Returns the backup items
*/
quint64 FmOperationRestore::selection() const
{
    return mSelection;
}
