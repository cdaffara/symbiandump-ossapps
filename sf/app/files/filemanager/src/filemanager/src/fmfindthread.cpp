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
 *     The find thread header file of file manager
 */

#include "fmfindthread.h"
#include "fmutils.h"
#include "fmcommon.h"

#include <QDir>

// current path, it may come from findDirs.first().entryInfoList()
#define CurrentDir QString( "." )

// parent path, it may come from findDirs.first().entryInfoList()
#define ParentDir QString( ".." )

// if got 5 result and have not send notify event, then send notify event
const int notifyPerCount = 5;

// if got notifyPerElapsedTime milliseconds and have not send notify event, then send notify event.
const int notifyPerElapsedTime = 500;

/*!
    \fn void found( const QStringList &dataList )
    This signal is emitted when some data has been found and \a dataList is provided as data list.
    Please connect this signal by Qt::BlockingQueuedConnection as dataList will be cleared immediately
*/

/*!
    Constructor, set thread LowPriority
*/
FmFindThread::FmFindThread( QObject *parent )
    : QThread( parent )
{
    setPriority( LowPriority );
}

/*!
    Destructor
*/
FmFindThread::~FmFindThread()
{
    FM_LOG("FmFindThread::~FmFindThread()");
}

/*!
    Set find path list \a pathList
*/
void FmFindThread::setFindPathList( const QStringList &pathList )
{
    mFindPathList.clear();
    mFindPathList = pathList;
}

/*!
    Set find pattern
*/
void FmFindThread::setPattern( const QRegExp &regExp )
{
    findPattern = regExp;
}

/*!
    Stop find
*/
void FmFindThread::stop()
{
    FM_LOG("FmFindThread::stop()");
    mStop = true;
}

/*
    Thread function
*/
void FmFindThread::run()
{
    FM_LOG( "FmFindThread::run() started ");
    mStop = false;
    if (findPattern.isEmpty() || !findPattern.isValid()) {
        FM_LOG( "FmFindThread::run() canceled because error param ");
        return;
    }
 
    QList<QDir> findDirs;
    foreach( const QString &path, mFindPathList ) {
        QDir dir( path );
        findDirs.append( dir );
    }

    count = 0;
    time.restart();
    mStop = false;
    while (!findDirs.isEmpty()) {
        if (mStop) {
            FM_LOG("FmFindThread::run() stopped");
            return;
        }
        QFileInfoList infoList = findDirs.first().entryInfoList();
        for (QFileInfoList::Iterator it = infoList.begin(); it != infoList.end(); ++it) {
            if (mStop) {
                FM_LOG("FmFindThread::run() stopped");
                return;
            }
			QString name = it->fileName();
			QString absolutPath = it->absoluteFilePath();
            if (findPattern.exactMatch( it->fileName() )) {
                if( !FmUtils::isSystemFolder( it->absoluteFilePath()) ) {
                    tempResultList.append( it->absoluteFilePath() );
                    if (tempResultList.count() > notifyPerCount) {
                        emitFound();
                    } else if (time.elapsed() > notifyPerElapsedTime && tempResultList.count() > 0) {
                        emitFound();
                    }
                }
            }

            // exclude directory named ".." and "."
            if (it->isDir() && it->fileName() != ParentDir && it->fileName() != CurrentDir ) {
                if( !FmUtils::isSystemFolder( it->absoluteFilePath()) ) {
                    findDirs.append( QDir( it->absoluteFilePath() ) );
                }
            }
        }

        findDirs.removeFirst();
    }
    
    emitFound();
}

/*
    Emit signal "found" to send out found data
*/
void FmFindThread::emitFound()
{
    if (mStop) {
        FM_LOG("FmFindThread::emitFound() return because stopped");
        return;
    }
    if( tempResultList.count() > 0 ) {
        emit found( tempResultList );
        tempResultList.clear();
        time.restart();
    }
}
