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

#ifndef LOGSENGINEAPITESTER_H
#define LOGSENGINEAPITESTER_H

#include <HbView.h>

class HbListView;
class HbLabel;
class LogsModel;
class LogsCustomFilter;
class QAbstractItemModel;
class TestModel;
class LogsEvent;
class CRepository;
class LogsFilter;

class TestView : public HbView
{
    Q_OBJECT
public:
    TestView();
    ~TestView();

public slots:
    void changeFilter();
    void addMissedCall();
    void clearMissedCalls();
    void updateMissedCallsLabel();
    void deleteEvents();
    void markingCompleted(int err);
    void clearingCompleted(int err);
    
private:
    HbListView* mList;
    HbLabel* mLabel;
    TestModel* mModel;
    LogsModel* mLogsModel;
    LogsCustomFilter* mFilter;
    bool mShowAll;
    CRepository* mRepository;
    LogsFilter* mMissedCallsFilter;

};

class TestModel : public QAbstractListModel
    {
    Q_OBJECT
    
public:
    TestModel(QAbstractItemModel& logsModel);
    ~TestModel();
    
public: // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

public slots:
    void handleModelUpdated();

private:
    void readEvents();
    QString directionText(const LogsEvent& event);
    
private:
    QStringList mEvents;
    QAbstractItemModel& mLogsModel;
    
    };

#endif
