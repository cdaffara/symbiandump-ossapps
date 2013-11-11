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
 *     Zhiqiang Yang <zhiqiang.yang@nokia.com>
 * 
 * Description:
 *     The find result model source file of file manager
 */

#include "fmfindresultmodel.h"
#include "fmfindthread.h"
#include "fmfileiconprovider.h"
#include "fmcommon.h"

#include <QDateTime>

#include <hbglobal.h>

// wait \a circularWaitingTimeForStartFind microsecond to try to start find again
// Since find can not be started while last find is not stopped.
const int circularWaitingTimeForStartFind = 200;

// wait \a firstWaitingTimeForStartFind microsecond to start find when accept find from caller.
// Means user can continuously input words for search, so that find will not immediately start after accept each input event.
// Search will be started after \a firstWaitingTimeForStartFind time of last user input
const int firstWaitingTimeForStartFind = 750;

/*!
    \fn void finished()
    This signal is emitted when find is finished.
*/

/*!
    \fn void modelCountChanged( int count )
    This signal is emitted when data count in model is changed
    \a count is current data count in model.
    Currently only start find and get more find result will emit this signal.
*/

FmFindResultModel::FmFindResultModel( QObject *parent )
    : QAbstractListModel( parent )
{
    FM_LOG("FmFindResultModel::FmFindResultModel");
    init();
    connect( mFindThread, SIGNAL( finished()), this, SLOT(onThreadFinished()) );
	connect( mFindThread, SIGNAL(found(QStringList)), this, SLOT(on_findThread_found( QStringList) ), Qt::BlockingQueuedConnection ); 
    connect( &mTimer, SIGNAL(timeout()), this, SLOT(startFind()));
}

FmFindResultModel::~FmFindResultModel()
{
    FM_LOG("FmFindResultModel::~FmFindResultModel START");
    mFindThread->stop();
    if( mFindThread->isRunning() ) {
        mStopEventLoop.exec();
    }
	delete mIconProvider;
    FM_LOG("FmFindResultModel::~FmFindResultModel END");
}

/*!
    Send find \a regExp and \a pathList to find queue
    last un-stopped find will be stopped and start new find in 0 - 2 seconeds.
*/
void FmFindResultModel::find( const QRegExp &regExp, const QStringList &pathList )
{
    mFindPath   = pathList;
    mRegExp     = regExp;

    mTimer.stop();

	if (mFindThread->isRunning()) {
        mFindThread->stop();
    }
    mTimer.start( firstWaitingTimeForStartFind );
}

/*!
    Internal start entrance, will be triggered by timer
    Wait till last find stopped and start new find.
*/
void FmFindResultModel::startFind()
{
    mTimer.stop();
    if (mFindThread->isRunning()) {
        mFindThread->stop();
        mTimer.start( circularWaitingTimeForStartFind );
		return;
    }

    // Find starting. Initialize context.
    mFindThread->setFindPathList( mFindPath );
    mFindThread->setPattern( mRegExp );
	removeRows( 0, rowCount() );
    emit findStarted();
    mFindThread->start();
}

/*!
    Return if find is inprogress
*/
bool FmFindResultModel::isFinding() const
{
    return mFindThread->isRunning();
}

/*!
    Stop find
*/
void FmFindResultModel::stopFind()
{
    mFindThread->stop();
    if( mFindThread->isRunning() ) {
        mStopEventLoop.exec();
    }
}

/*
    Find finish slot.
*/
void FmFindResultModel::onThreadFinished()
{
    FM_LOG("FmFindResultModel::onThreadFinished");

    // close event loop so that blocked destructor and stopFind() can be released.
    mStopEventLoop.exit();

    emit findFinished();
}

/*!
    Returns the number of rows in the model. This value corresponds to the
    number of items in the model's internal string list.

    The optional \a parent argument is in most models used to specify
    the parent of the rows to be counted. Because this is a list if a
    valid parent is specified, the result will always be 0.

    \sa insertRows(), removeRows(), QAbstractItemModel::rowCount()
*/
int FmFindResultModel::rowCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return mFindResult.count();

    return 0;
}

/*!
   Get model column count
*/
int FmFindResultModel::columnCount( const QModelIndex &parent ) const
{
    if ( !parent.isValid() )
        return 4;
    
    return 0;
}

/*!
   Get model data
*/
QVariant FmFindResultModel::data( const QModelIndex &index, int role ) const
{
    if (!indexValid( index ))
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return fileInfo( index ).fileName();
            case 1: return fileInfo( index ).size();
            case 2: return mIconProvider->type( fileInfo( index ) );
            case 3: return fileInfo( index ).lastModified().toString( Qt::LocalDate );
            default:
                qWarning( "FmFindResultData: invalid display value column %d", index.column() );
                return QVariant();
        }
    }

    if (index.column() == 0) {
        if (role == FileIconRole) {
            return mIconProvider->icon( fileInfo( index ) );
        }
    }

    if (index.column() == 1 && role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }

    return QVariant();
}

/*!
   Get filePath by \a index
*/
QString FmFindResultModel::filePath ( const QModelIndex & index ) const
{
   return fileInfo( index ).filePath();
}

/*!
   Get header data by column number \a section
*/
QVariant FmFindResultModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();

        switch (section) {
            case 0: return hbTrId("Name");
            case 1: return hbTrId("Size");
            case 2: return hbTrId("Type");
            case 3: return hbTrId("Date Modified");
            default: return QVariant();
        }
    }

    return QAbstractItemModel::headerData( section, orientation, role );
}

/*!
    Inserts \a dataList into the model, beginning at the given \a row.

    The \a parent index of the rows is optional and is only used for
    consistency with QAbstractItemModel. By default, a null index is
    specified, indicating that the rows are inserted in the top level of
    the model.

    \sa QAbstractItemModel::insertRows()
*/

bool FmFindResultModel::insertRows( int row, const QStringList &dataList, const QModelIndex &parent )
{
    if ( row < 0 || dataList.count() < 1 || row > rowCount(parent) )
		return false;

    beginInsertRows( QModelIndex(), row, row + dataList.count() - 1 );
    mFindResult.append( dataList );
    endInsertRows();

    // emit modelCountChanged could let FmFindWidget switch style between EmptyTipWidget and ResultListview
    // FmFindWidget will show an empty tip widget such as "No found files or folderss" if set 0 as parameter
    emit modelCountChanged( rowCount() );

    return true;
}

/*!
    Removes \a count rows from the model, beginning at the given \a row.

    The \a parent index of the rows is optional and is only used for
    consistency with QAbstractItemModel. By default, a null index is
    specified, indicating that the rows are removed in the top level of
    the model.

    \sa QAbstractItemModel::removeRows()
*/
bool FmFindResultModel::removeRows( int row, int count, const QModelIndex &parent )
{   
	if (row < 0 || count < 1 || (row + count) > rowCount(parent) )
		return false;

	beginRemoveRows( QModelIndex(), row, row + count - 1 );

	for (int i = 0; i < count; ++i)
		mFindResult.removeAt(row);

	endRemoveRows();

    // emit modelCountChanged could let FmFindWidget switch style between EmptyTipWidget and  ResultListview
    // FmFindWidget will show an empty tip widget such as "No found files or folderss" if set 0 as parameter
    emit modelCountChanged( rowCount() );

	return true;
}

/*!
    Get QFileInfo by \a index
*/
QFileInfo FmFindResultModel::fileInfo( const QModelIndex &index ) const
{
    if (index.row() >= 0 && index.row() < mFindResult.size())
        return QFileInfo( mFindResult[index.row()] );
    else
        return QFileInfo();
}

/*
    Receive \a dataList as some found result
    Then insert dataList to model
*/
void FmFindResultModel::on_findThread_found( const QStringList &dataList )
{
    if( dataList.isEmpty() ) {
		return;
    }
    insertRows( rowCount(), dataList );
}

/*
    Get if \a index is valid
*/
bool FmFindResultModel::indexValid( const QModelIndex &index ) const
{
    if( ( index.row() < 0 ) || ( index.row() >= rowCount() ) ) {
        return false;
    } else if( ( index.column() < 0 ) || ( index.column() >= columnCount() ) ) {
        return false;
    } else {
        return true;
    }
}

/*
    Init model
*/
void FmFindResultModel::init()
{
    mFindThread = new FmFindThread( this );
    mFindThread->setObjectName( "findThread" );
    mIconProvider = new FmFileIconProvider();
}

/*
    Sort by name
*/
bool FmFindResultModel::caseNameLessThan(const QPair<QString,int> &s1,
                                         const QPair<QString,int> &s2)
{
    QFileInfo info1( s1.first );
    QFileInfo info2( s2.first );
    
    return info1.fileName() < info2.fileName();
}

/*
    Sort by time
*/
bool FmFindResultModel::caseTimeLessThan(const QPair<QString,int> &s1,
                                         const QPair<QString,int> &s2)
{
    QFileInfo info1( s1.first );
    QFileInfo info2( s2.first );
    
    return info1.lastModified() < info2.lastModified();
}

/*
    Sort by size
*/
bool FmFindResultModel::caseSizeLessThan(const QPair<QString,int> &s1,
                                         const QPair<QString,int> &s2)
{
    QFileInfo info1( s1.first );
    QFileInfo info2( s2.first );
    
    return info1.size() < info2.size();
}

/*
    Sort by type
*/
bool FmFindResultModel::caseTypeLessThan(const QPair<QString,int> &s1,
                                         const QPair<QString,int> &s2)
{
    QFileInfo info1( s1.first );
    QFileInfo info2( s2.first );
    
    if( info1.isDir() != info2.isDir() ){
        return info1.isDir();
    }
    else{
        return info1.suffix().toLower() < info2.suffix().toLower();   
    }
}

/*!
  \reimp
  Sort by \a column, which is aligned to \a SortFlag
  \sa SortFlag
*/
void FmFindResultModel::sort ( int column, Qt::SortOrder order )
{  
    // Sort algorithm comes from
    // void QListModel::sort(int column, Qt::SortOrder order)

    Q_UNUSED( order );
    emit layoutAboutToBeChanged();

    // store value and row pair.
    QVector < QPair<QString,int> > sorting(mFindResult.count());
    for (int i = 0; i < mFindResult.count(); ++i) {
        QString item = mFindResult.at(i);
        sorting[i].first = item;
        sorting[i].second = i;
    }

    // sort in "sorting"
   switch( ( SortFlag )column )
    {
    case Name:
        qSort( sorting.begin(), sorting.end(), caseNameLessThan );
        break;
    case Time:
        qSort( sorting.begin(), sorting.end(), caseTimeLessThan );
        break;
    case Size:
        qSort( sorting.begin(), sorting.end(), caseSizeLessThan );
        break;
    case Type:
        qSort( sorting.begin(), sorting.end(), caseTypeLessThan );
        break;
    }

    // create from Indexes and toIndexes, then set sorted data back to mFindResult
    QModelIndexList fromIndexes;
    QModelIndexList toIndexes;
    for (int r = 0; r < sorting.count(); ++r) {
        QString item = sorting.at(r).first;
        toIndexes.append(createIndex(r, 0));
        fromIndexes.append(createIndex(sorting.at(r).second, 0));
        mFindResult[r] = sorting.at(r).first;
    }
    changePersistentIndexList(fromIndexes, toIndexes);

    emit layoutChanged();
}
