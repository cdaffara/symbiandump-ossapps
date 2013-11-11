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

#include "fmoperationcopyormove.h"
#include "fmcommon.h"
#include "fmoperationbase.h"
#include "fmdrivedetailstype.h"
#include "fmutils.h"

#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QStack>

/* \fn void askForReplace( const QString &srcFile, const QString &destFile, bool *isAccepted )
 * This signal is emitted when the same file as \a srcFile exists.
 * \a destFile the destination file.
 * \a isAccepted whether to replace the destination file.
 */

/* \fn void askForRename( const QString &srcFile, QString *destFile )
 * This signal is emitted when \a srcFile needs to be renamed.
 * \a destFile return the new name.
 */

/* \fn  void driveSpaceChanged()
 * This signal is emitted when copy or move is completed, and used to update the drive size.
 */

//the size of one time copy
const int FmOneTimeCopyOrMoveSize = 1024;
//the total steps of progress bar
const int FmTotalProgressBarSteps = 100;
/*
 * Constructs a copy or move operation with
 * \a operationService parent
 * \a type the type of operation(EOperationTypeCopy or EOperationTypeMove)
 * \a sourceList the source file or path lists that will be copied
 * \a targetPath the target path where the source file will be copied
 */
FmOperationCopyOrMove::FmOperationCopyOrMove( FmOperationService *operationService, FmOperationService::TOperationType type, const QStringList &sourceList, const QString &targetPath )
        : FmOperationBase( operationService, type ),
          mOperationService( operationService ), mSourceList( sourceList ), mTargetPath( targetPath ),
          mStop( 0 ), mTotalSize( 0 ), mCopiedOrMovedSize( 0 ), mTotalSteps( FmTotalProgressBarSteps ), mCurrentStep( 0 )
{
	mTargetPath = FmUtils::fillPathWithSplash( mTargetPath );
    connect( this, SIGNAL( askForRename( QString, QString* ) ),
                mOperationService, SLOT( on_operation_askForRename( QString, QString* )), Qt::BlockingQueuedConnection );
    connect( this, SIGNAL( askForReplace( QString, QString, bool* ) ),
                mOperationService, SLOT( on_operation_askForReplace( QString, QString, bool* )), Qt::BlockingQueuedConnection );    
    connect( this, SIGNAL( driveSpaceChanged() ),
                mOperationService, SLOT( on_operation_driveSpaceChanged() ) );
}

/*
 * Destructs the copy or move operation.
 */
FmOperationCopyOrMove::~FmOperationCopyOrMove()
{
}

/*
 * Returns the source file or path list
 */
QStringList FmOperationCopyOrMove::sourceList()
{
    return mSourceList;
}

/*
 * Returns the target path
 */
QString FmOperationCopyOrMove::targetPath()
{
    return mTargetPath;
}

/*
 * Starts the operation.
 * \a isStopped flag the outside stop operation
 */
void FmOperationCopyOrMove::start( volatile bool *isStopped )
{
    mStop = isStopped; 
    mTotalSize   = 0;
    mCopiedOrMovedSize  = 0;
    mCurrentStep = 0;

    if ( mSourceList.empty() ) {
        emit notifyError( FmErrWrongParam, mErrString );    
        return ;
    }
	emit notifyPreparing( true );

    int numofFolders = 0;
    int numofFiles      = 0;

    int ret = FmFolderDetails::queryDetailOfContentList( mSourceList, numofFolders, 
        numofFiles, mTotalSize, mStop, true );
    if( ret != FmErrNone ) {
        emit notifyError( ret, mErrString );
        return;        
    }    
    if ( !targetHasEnoughSpace() ) {
        emit notifyError( FmErrDiskFull, mErrString );
        return;
        }    
    emit notifyStart( true, mTotalSteps );

    foreach( const QString& source, mSourceList ) {
        // formatPath, but do not append splash in the end
        // Otherwise could not get fileName in QFileInfo::fileName
        QString checkedSource( FmUtils::formatPath( source ) );
        QFileInfo fi( checkedSource );
        if( !fi.exists() ) {
            mErrString = checkedSource;   
            emit driveSpaceChanged();
            emit notifyError( FmErrSrcPathDoNotExist, mErrString );
            return;
        }
        QString newName;
        bool    isAcceptReplace = false;
        QFileInfo destFi( mTargetPath + fi.fileName() );

        // while for duplicated file/dir
        while( destFi.exists() ) {
            if( destFi.isFile() && destFi.absoluteFilePath().compare( fi.absoluteFilePath(), Qt::CaseInsensitive ) != 0 ) {
                emit askForReplace( destFi.absoluteFilePath(), fi.absoluteFilePath(), &isAcceptReplace );
                if( isAcceptReplace ) {
                    //delete src file
                    if( !QFile::remove( destFi.absoluteFilePath() ) ) {
                        mErrString = destFi.absoluteFilePath();
                        ret = FmErrCannotRemove;
                        break;
                    }
                    destFi.setFile( destFi.absoluteFilePath() );
                } else {
                    queryForRename( destFi.absoluteFilePath(), &newName );
                    if( newName.isEmpty() ) {
                        ret = FmErrCancel;
                        break;
                    }
                    QString targetName = mTargetPath + newName;
                    destFi.setFile( targetName );
                }
            } else{
                // destination is dir
                queryForRename( destFi.absoluteFilePath(), &newName );
                if( newName.isEmpty() ) {
                    ret = FmErrCancel;
                    break;
                }
                QString targetName = mTargetPath + newName;
                destFi.setFile( targetName );
            }
        }
        if( ret != FmErrNone ) {
            // refresh drive space no care if cancel, error or finished.
            // as filemanger cannot notify drive space changed
            // do not refresh path as QFileSystemModel will do auto-refresh
            emit driveSpaceChanged();
            emit notifyError( ret, mErrString );
            return;
        }
        ret = copyOrMove( checkedSource, mTargetPath, newName );
        if( ret != FmErrNone ) {
            emit driveSpaceChanged();
            emit notifyError( ret, mErrString );            
            return;
        }
    }    
    emit driveSpaceChanged();
    emit notifyFinish();
}

/*
 * Copy or move the \a source to \a targetPath
 * with \a newTargetName
 */
int FmOperationCopyOrMove::copyOrMove( const QString &source, const QString &targetPath,
                         const QString &newTargetName )
{        
    if( *mStop ) {
        return FmErrCancel;
    }

    QFileInfo fi( source );
    if( !fi.exists() ) {
        mErrString = source;
        return FmErrSrcPathDoNotExist;
    }
    QString newName;
    if( !newTargetName.isEmpty() ) {
        newName = targetPath + newTargetName;
    } else {
        newName = targetPath + fi.fileName();
    }

    int ret = FmErrNone;
    
    if ( fi.isFile() ) {
        if ( FmUtils::getDriveLetterFromPath( source ) == 
                FmUtils::getDriveLetterFromPath( targetPath ) && 
                operationType() == FmOperationService::EOperationTypeMove ) 
            {
            return FmUtils::moveInsideDrive( source, newName );
            
            }
        quint64 fileSize = fi.size();
        ret = copyOneFile( source, newName );
        if (ret != FmErrNone) {
            mErrString = source;
            return ret;
        }        
        if ( operationType() == FmOperationService::EOperationTypeMove 
                && !QFile::remove( source ) ) {
            mErrString = source;
            return FmErrCannotRemove;           
        }
    } else if ( fi.isDir() ) {
        if( operationType() == FmOperationService::EOperationTypeMove 
                && FmUtils::isDefaultFolder( source ) ){
            ret = FmErrRemoveDefaultFolder;
        }
        else{
            ret = copyOrMoveDirInsideContent( source, newName );
        }        
        if( ret!= FmErrNone ) {
            return ret;
        }
        if ( operationType() == FmOperationService::EOperationTypeMove 
                && !fi.dir().rmdir( fi.absoluteFilePath() ) ) {
            mErrString = fi.absolutePath();
            return FmErrCannotRemove;
        }
    } else {
        qWarning( "Things other than file and directory are not copied" );
        ret = FmErrIsNotFileOrFolder;
    }

    return ret;
}

/*
  copy \a srcPath as \a destPath
  both \a srcPath and \a destPath are Directory
*/
int FmOperationCopyOrMove::copyOrMoveDirInsideContent( const QString &srcPath, const QString &destPath )
{
    QFileInfo srcInfo( srcPath );
    QFileInfo destInfo( destPath );
    
    if( FmUtils::isSubLevelPath( srcPath, destPath ) ) {
        mErrString = destPath;
        if ( operationType() == FmOperationService::EOperationTypeMove ) {
            return FmErrMoveDestToSubFolderInSrc;
        } else {
            return FmErrCopyDestToSubFolderInSrc;
        }
        
    }
    
    if( !srcInfo.isDir() || !srcInfo.exists() ) {
        mErrString = srcPath;
        return FmErrSrcPathDoNotExist;
    }
    
    if( !destInfo.exists() ) {
        if( !destInfo.dir().mkdir( destInfo.absoluteFilePath() ) ) {
            mErrString = destPath;
            return FmErrCannotMakeDir;
        }
    }    
    destInfo.setFile( destPath );
    if( !destInfo.isDir() ) {
        mErrString = destPath;
        return FmErrCannotMakeDir;
    }

    //start to copy
    QFileInfoList infoList = QDir( srcPath ).entryInfoList( QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System );
    while( !infoList.isEmpty() ) {
        if( *mStop ) {
            return FmErrCancel;
        }

        QFileInfo fileInfo = infoList.takeFirst();
        if( fileInfo.isFile() ){
            //copy file
            QString newFilePath = destPath + fileInfo.absoluteFilePath().mid( srcPath.length() );
            int ret = copyOneFile( fileInfo.absoluteFilePath(), newFilePath );
            if ( ret != FmErrNone ) {
                mErrString = fileInfo.absoluteFilePath();
                return ret;
            }
            if( operationType() == FmOperationService::EOperationTypeMove 
                    && !QFile::remove( fileInfo.absoluteFilePath() ) ) {
                mErrString = fileInfo.absoluteFilePath();
                return FmErrCannotRemove;
            }   
        } else if( fileInfo.isDir() ) {
            //makedir
            QString newDirPath = destPath + fileInfo.absoluteFilePath().mid( srcPath.length() );
            if( !QDir( newDirPath ).exists() && !QDir( destPath ).mkdir( newDirPath ) ) {
                mErrString = newDirPath;
                return FmErrCannotMakeDir;
            }
            // add dir content to list.
            QFileInfoList infoListDir = QDir( fileInfo.absoluteFilePath() ).entryInfoList(
                QDir::NoDotAndDotDot | QDir::AllEntries );
            if ( operationType() == FmOperationService::EOperationTypeMove ) {
                if( infoListDir.isEmpty() ) {
                    if ( !fileInfo.dir().rmdir( fileInfo.absoluteFilePath() ) ) {
                        mErrString = fileInfo.absolutePath();
                        return FmErrCannotRemove;
                    }
                } else {
                    infoList.push_front( fileInfo );
                }
            }
            while( !infoListDir.isEmpty() ) {
                infoList.push_front( infoListDir.takeLast() );
            }

        } else {
            mErrString = fileInfo.absoluteFilePath();
            return FmErrIsNotFileOrFolder;
        }

    }

    return FmErrNone;
}

/*
 * Increase the progress bar
 * \a size the current copy or moved size
 */
void FmOperationCopyOrMove::increaseProgress( quint64 size )
{
    if( mTotalSize <=0 ) {
        return;
    }
    mCopiedOrMovedSize += size;
    int step = ( mCopiedOrMovedSize * FmTotalProgressBarSteps ) / mTotalSize;
    if( step > mCurrentStep ) {
        mCurrentStep = step;
        emit notifyProgress( mCurrentStep );
    }
}

/*
 * Emits askForRename signal.
 * \a srcFile the source file path.
 * \a destFile get the new name from user input
 */
void FmOperationCopyOrMove::queryForRename( const QString &srcFile, QString *destFile )
{
    emit askForRename( srcFile, destFile );
}

/*
 * Copies one file from \a srcFile to \a desFile
 */
int FmOperationCopyOrMove::copyOneFile( const QString &srcFile, const QString &desFile )
{
    QFile src( srcFile );
    QFile des( desFile );
    if ( !src.open( QIODevice::ReadOnly ) || !des.open( QIODevice::WriteOnly ) ) {
        return FmErrCannotCopy;
    }
    QDataStream  outputStream( &src );
    QDataStream  inputStream( &des );
    //used to cache data from source file to target file during one copy
    QScopedPointer<char> tempString( new char[FmOneTimeCopyOrMoveSize] );
    memset( tempString.data(), 0, FmOneTimeCopyOrMoveSize );
    while ( !outputStream.atEnd() ) {
        if ( *mStop ) {            
            src.close();
            des.close();
            QFile::remove( desFile );
            return FmErrCancel;
        }
        int ret = outputStream.readRawData(tempString.data(), FmOneTimeCopyOrMoveSize );
        if (ret == -1) {
            src.close();
            des.close();
            QFile::remove( desFile );            
            return FmErrCannotCopy;
        }
        ret = inputStream.writeRawData(tempString.data(), ret);
        if (ret == -1) {
            src.close();
            des.close();
            QFile::remove( desFile );            
            return FmErrCannotCopy;
        }        
        memset( tempString.data(), 0, FmOneTimeCopyOrMoveSize );
        increaseProgress( ret );
    }    
    src.close();
    des.close();
    if ( FmUtils::setFileAttributes( srcFile, desFile ) != FmErrNone ) {
        QFile::remove( desFile );        
        return FmErrCannotCopy;
    }
    return FmErrNone;
}

/*
 * Prepare some work before starting operation.
 * Returns error number.
 */
int FmOperationCopyOrMove::prepare()
{
    if( mSourceList.empty() ) {
        return FmErrWrongParam;
    } else {
    return FmErrNone;
    }
}

bool FmOperationCopyOrMove::targetHasEnoughSpace()
{
    QString sourceDrive = FmUtils::getDriveLetterFromPath( mSourceList.front() );
    QString targetDrive = FmUtils::getDriveLetterFromPath( mTargetPath );
    if ( sourceDrive == targetDrive &&
            operationType() == FmOperationService::EOperationTypeMove ) {
        return true;
    } else {
        return FmUtils::hasEnoughSpace( targetDrive, mTotalSize );
    }
}
