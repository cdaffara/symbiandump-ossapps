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

#include "fmoperationremove.h"
#include "fmcommon.h"
#include "fmoperationbase.h"
#include "fmdrivedetailstype.h"
#include "fmutils.h"

#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QStack>

/* \fn  void driveSpaceChanged()
 * This signal is emitted when copy or move is completed, and used to update the drive size.
 */

/*
 * Constructs a remove operation with
 * \a parent parent
 * \a pathList the file or path to be removed.
 */
FmOperationRemove::FmOperationRemove(QObject *parent, const QStringList &pathList ) :
        FmOperationBase( parent, FmOperationService::EOperationTypeRemove ),        
        mPathList( pathList ), 
        mStop( 0 ),
        mTotalCount( 0 ), mRemovedCount( 0 ), mTotalSteps( 100 ), mCurrentStep( 0 )
{
    connect( this, SIGNAL( driveSpaceChanged() ),
                parent, SLOT( on_operation_driveSpaceChanged() ) );
}

/*
 * Destructs the operation.
 */
FmOperationRemove::~FmOperationRemove()
{
}

/*
 * Returns the path list
 */
QStringList FmOperationRemove::pathList()
{
    return mPathList;
}

/*
 * Starts the operation.
 * \a isStopped flag the outside stop operation
 */
void FmOperationRemove::start( volatile bool *isStopped )
{
    mStop = isStopped;
    mTotalCount   = 0;
    mRemovedCount  = 0;
    mCurrentStep = 0;

    if( mPathList.empty() ) {
        emit notifyError( FmErrWrongParam, mErrString );    
        return ;
    }

    emit notifyPreparing( true );

    quint64 totalSize= 0; 
    int numofFolders = 0;
    int numofFiles      = 0;

    int ret = FmFolderDetails::queryDetailOfContentList( mPathList, numofFolders, 
        numofFiles, totalSize, mStop, true );
    if( ret != FmErrNone ) {
        emit notifyError( ret, mErrString );
        return;
    }
    mTotalCount = numofFolders + numofFiles;

    emit notifyStart( true, mTotalSteps );

    foreach( const QString& srcPath, mPathList ) {
        int ret = remove( srcPath );
        if( ret != FmErrNone ) {
            // refresh drive space no care if cancel, error or finished.
            // as filemanger cannot notify drive space changed
            // do not refresh path as QFileSystemModel will do auto-refresh
            emit driveSpaceChanged();
            emit notifyError( ret, mErrString );
            return;
        }
    }    
    emit driveSpaceChanged();
    emit notifyFinish();
}

/*
 * Removes the file or dir with name \a fileName
 */
int FmOperationRemove::remove( const QString &fileName )
{
    if( *mStop ) {
        return FmErrCancel;
    }

    int ret = FmErrNone;
    QFileInfo fi( fileName );
    if (fi.isFile()) {
        if( !QFile::remove( fileName ) ) {
            mErrString = fileName;
            ret = FmErrCannotRemove;
        }
        IncreaseProgressOnce();
    } else if (fi.isDir()) {
       if( FmUtils::isDefaultFolder( fileName ) ){
           ret = FmErrRemoveDefaultFolder;
       }
       else{
           ret = recursiveRemoveDir( fileName );
       }


    } else {
        qWarning( "Things other than file and directory are not copied" );
        ret = FmErrIsNotFileOrFolder;
    }
    return ret;
}

/*
 * Remove the dir with name \a pathName
 */
int FmOperationRemove::recursiveRemoveDir( const QString &pathName )
{
    QFileInfo fi( pathName );
    if (!fi.exists() || !fi.isDir())
        return FmErrSrcPathDoNotExist;

    QStack<QDir> dirs;
    dirs.push( QDir( pathName ) );

    while (!dirs.isEmpty()) {
        QFileInfoList infoList = dirs.top().entryInfoList( QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System );
        if (infoList.size() == 0) {
            QDir dirToRemove( dirs.pop() );
            if ( !dirToRemove.rmdir( dirToRemove.absolutePath() ) ) {
                mErrString = dirToRemove.absolutePath();
                return FmErrCannotRemove;
            }
            IncreaseProgressOnce();

        } else {
            QList<QDir> dirList;
            for (QFileInfoList::Iterator it = infoList.begin(); it != infoList.end(); ++it) {
                if( *mStop ) {
                    return FmErrCancel;
                }

                if (it->isDir()) {
                    dirList.push_front( QDir( it->absoluteFilePath() ) );
                } else {
                    if ( !QFile::remove( it->absoluteFilePath() ) ) {
                        mErrString = it->absoluteFilePath();
                        return FmErrCannotRemove;
                    }
                    IncreaseProgressOnce();
                }
            }
            foreach( const QDir& dir, dirList ) {
                dirs.push( dir );
            }
        }
    }
    return FmErrNone;
}

/*
 * Increase the progress bar
 */
void FmOperationRemove::IncreaseProgressOnce()
{
    if( mTotalCount <= 0 )
        return;
    mRemovedCount++;
    int step = ( mRemovedCount * 100 ) / mTotalCount;
    if( step > mCurrentStep ) {
        mCurrentStep = step;
        emit notifyProgress( mCurrentStep );
    }
}
