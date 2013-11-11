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

#ifndef CNTCOLLECTIONLISTMODELWORKER_H
#define CNTCOLLECTIONLISTMODELWORKER_H

#include <QThread>
#include <QMutex>
#include <QPair>

#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntCollectionListModelWorker : public QThread
{
    Q_OBJECT
    friend class TestCntCollectionListModelWorker;
    
public:
    CntCollectionListModelWorker(QString unnamed, QString noFavs, QString noMembers, int sortOrder);
    ~CntCollectionListModelWorker();
    
    void run();
    bool event(QEvent *event);
    
    void scheduleJob(int id);

private:
    void processJobs();
    void fetchInformation(int id);
    
signals:
    void fetchDone(int id, const QString& secondRowText, const QList<QContactLocalId>& members);
    
private:
    bool                        mStarted;
    bool                        mProcessingJobs;
    bool                        mStopped;
    QMutex                      mMutex;
    
    QList<int>                  mJobs;

    QContactManager            *mManager;
    
    QString                     mUnnamed;
    QString                     mNoFavorites;
    QString                     mNoMembers;
    int                         mSortOrder;
};

#endif // CNTCOLLECTIONLISTMODELWORKER_H
