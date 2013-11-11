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

#include "fmoperationviewdetails.h"
#include "fmdrivedetailstype.h"
#include <QtAlgorithms>

/*
 * Constructs one view drive details operation with
 * \a parent parent.
 * \a driverName the drive name.
 */
FmOperationDriveDetails::FmOperationDriveDetails( QObject *parent, const QString &driverName ) :
        FmOperationBase( parent, FmOperationService::EOperationTypeDriveDetails ),
        mDriverName( driverName ), mStop( 0 )
{
}

/*
 * Destructs the operation.
 */
FmOperationDriveDetails::~FmOperationDriveDetails()
{
    qDeleteAll(mDetailsSizeList);
}

/*
 * Returns the drive name.
 */
QString FmOperationDriveDetails::driverName()
{
    return mDriverName;
}

/*
 * reimp.
 */
void FmOperationDriveDetails::start( volatile bool *isStopped )
{
    mStop = isStopped;
    emit notifyWaiting( true );
    //FmOperationDriveDetails *operationDriverDetails = static_cast<FmOperationDriveDetails*>( mOperationBase );
    int ret = FmDriveDetailsContent::querySizeofContent( driverName(), detailsSizeList(), mStop );
    if( ret == FmErrNone ) {       
        emit notifyFinish();
    } else {
        emit notifyError( ret, QString() );
    }
}

/*
 * Returns detail items size list.
 */
QList<FmDriveDetailsSize*> &FmOperationDriveDetails::detailsSizeList()
{
    return mDetailsSizeList;
}

/*
 * Constructs one view folder details operation with
 * \a parent parent.
 * \a driverName the drive name.
 */
FmOperationFolderDetails::FmOperationFolderDetails( QObject *parent, const QString &folderPath ) :
        FmOperationBase( parent, FmOperationService::EOperationTypeFolderDetails ),
        mFolderPath( folderPath ),
        mNumofSubFolders( 0 ),
        mNumofFiles( 0 ),
        mSizeofFolder( 0 )

{
}

/*
 * Destructs the operation.
 */
FmOperationFolderDetails::~FmOperationFolderDetails()
{

}

/*
 * Returns the folder path.
 */
QString FmOperationFolderDetails::folderPath()
{
    return mFolderPath;
}

/*
 * Returns the number of sub folders.
 */
int &FmOperationFolderDetails::numofSubFolders()
{
    return mNumofSubFolders;
}

/*
 * Returns the number of files
 */
int &FmOperationFolderDetails::numofFiles()
{
    return mNumofFiles;
}

/*
 * Returns the size of folder.
 */
quint64 &FmOperationFolderDetails::sizeofFolder()
{
    return mSizeofFolder;
}

/*
 * Reimp.
 */
void FmOperationFolderDetails::start( volatile bool *isStopped )
{
    mStop = isStopped;
    emit notifyWaiting( true );   
    int ret = FmFolderDetails::getNumofSubfolders( folderPath(), numofSubFolders(), 
                                                   numofFiles(), sizeofFolder(), mStop );
    if( ret == FmErrNone ) {
        emit notifyFinish();
    } else {
        emit notifyError( ret, QString() );
    }    
}
