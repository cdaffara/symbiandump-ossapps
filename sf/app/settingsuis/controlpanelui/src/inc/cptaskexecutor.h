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
#ifndef CPTASKEXECUTOR_H
#define CPTASKEXECUTOR_H

#include <cpcategoryglobal.h>
#include <QThread>
#include <QList>
#include <QMutex>

class CpTask
{
public:
    explicit CpTask(bool autoDelete = true) 
    : mAutoDelete(autoDelete)
    {
    }

    virtual ~CpTask()
    {
    }

    bool autoDelete() const 
    {
        return mAutoDelete;
    }

    virtual void execute(volatile bool *stopped)
    {
        Q_UNUSED(stopped);
    }

    virtual void complete(volatile bool *stopped)
    {
        Q_UNUSED(stopped);
    }

private:
    bool mAutoDelete;
};

class CP_CATEGORY_EXPORT CpTaskExecutor : public QThread
{
    Q_OBJECT
public:
    explicit CpTaskExecutor(QObject *parent = 0);

    virtual ~CpTaskExecutor();
    
    /*
    return the gloabl instance
    */
    static CpTaskExecutor *globalInstance();
    
    /*
    destroy the global instance 
    */
    static void destroyGlobalInstance();

    /*
    add the task to running queue
    @task the task
    @append ture -- append to tail false -- preappend to head
    */

    bool runTask(CpTask *task,bool append = false);

    /*
    stop the thread and remove all not running tasks
    */
    void stop();

    /*
    move a task to front of the queue
    */
    bool toFront(CpTask *task);

    /*
    move a task to front of the queue
    */
    template <typename Predicate> bool toFront(Predicate pred);

protected:
    /*
    From QThread, run tasks
    */
    virtual void run();
private:
    /*
    remove not running tasks
    */
    void removeTasks();
private:
    QList<CpTask*> mTasks;
    QMutex mMutex;
    volatile bool mStopped;
};


template<typename Predicate>
bool CpTaskExecutor::toFront(Predicate pred)
{
    QMutexLocker locker(&mMutex);
    
    QList<CpTask*>::iterator workIterator = mTasks.begin();
    QList<CpTask*>::iterator endIterator(mTasks.end());

    for(;workIterator != endIterator;++workIterator)
    {
        if (pred(*workIterator)) {
            break;
        }
    }

    if (workIterator == endIterator) {
        return false;
    }

    CpTask *task = *workIterator;
    mTasks.erase(workIterator);
    mTasks.insert(0,task);

    return true;
}

#endif  //CPTASKECECUTOR_H

//End of File
