/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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
#include "cptaskexecutor.h"
#include <QMutexLocker>
#include <QDebug>
#include <QtAlgorithms>
#include <QPointer>

static QPointer<CpTaskExecutor> g_instance;

CpTaskExecutor *CpTaskExecutor::globalInstance()
{
    if (!g_instance) {
        g_instance = new CpTaskExecutor();
    }
    return g_instance.data();
}
    
void CpTaskExecutor::destroyGlobalInstance()
{
    delete g_instance.data();
}

CpTaskExecutor::CpTaskExecutor(QObject *parent /*=0*/) 
: QThread(parent), mStopped(false)
{
}

CpTaskExecutor::~CpTaskExecutor()
{
    stop();
}

bool CpTaskExecutor::runTask(CpTask *task,bool append /*= false*/)
{
    if (mStopped) {
        if (isRunning()) {
            qDebug() << "thread in stopping process...can not run task.\r\n";
            return false;
        }
        mStopped = false;
    }

    if (task) {
        {
            QMutexLocker locker(&mMutex);
            if (append) {
                mTasks.append(task);
            }
            else {
                mTasks.insert(0,task);
            }
        }

        if (!isRunning()) {
            qDebug() << "isRunning() == false, call start()\r\n";
            start();
        }
    }

    return true;
}

void CpTaskExecutor::stop()
{
    if (!mStopped && isRunning() ) {
       mStopped = true;
       removeTasks();
       wait(); //wait for finished
    }
}

void CpTaskExecutor::removeTasks()
{
    QMutexLocker locker(&mMutex);
    
    QList<CpTask*>::const_iterator begin(mTasks.begin());
    QList<CpTask*>::const_iterator end(mTasks.end());
    for (; begin != end; ++begin) {
        if ((*begin)->autoDelete()) {
            delete (*begin);
        }
    }

    mTasks.clear();
}

void CpTaskExecutor::run()
{
    while (!mStopped) {
        CpTask *task = 0;
        {
            QMutexLocker locker(&mMutex);
            if (mTasks.isEmpty()) {
                break;
            }
            task = mTasks.takeFirst();
        }
        try {
            task->execute(&mStopped);
            task->complete(&mStopped);
        }catch(...) {
            qDebug() << "excetion occur when running task\r\n";           
        }

        if (task->autoDelete()) {
            delete task;
        }
    }
}

bool CpTaskExecutor::toFront(CpTask *task)
{
    QMutexLocker locker(&mMutex);
    QList<CpTask*>::iterator foundIterator = ::qFind(mTasks.begin(),mTasks.end(),task);
    if (foundIterator == mTasks.end()) {
        return false;
    }
    
    mTasks.erase(foundIterator);
    mTasks.insert(0,task);

    return true;
}

//End of File
