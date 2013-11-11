/*!
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

#include <QtGui>
#include <QtTest/QtTest>
#include <hbapplication.h>
#include <hbtextitem.h>
#include <hbiconitem.h>

#include "bubbletest.h"
#include "bubbleparticipantlistmodel.h"

class ut_BubbleParticipantListModel : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanupTest();
    
    void test_addParticipant();
    void test_removeParticipant();
    void test_bubbleId();
    void test_dataModel();
    void test_reset();

private:
    BubbleParticipantListModel *mModel;
};

void ut_BubbleParticipantListModel::initTestCase()
{
    mModel = new BubbleParticipantListModel();
}

void ut_BubbleParticipantListModel::cleanupTestCase()
{
    delete mModel;
}

void ut_BubbleParticipantListModel::cleanupTest()
{
    mModel->reset();
}

void ut_BubbleParticipantListModel::test_addParticipant()
{
    QSignalSpy spyDataChanged(mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    mModel->addParticipant(1,"Bubble-1");
    mModel->addParticipant(2,"Bubble-2");
    mModel->addParticipant(3,"Bubble-3");
    QVERIFY(mModel->rowCount()==3);
    QVERIFY(spyDataChanged.count()==0);

    // name changed
    mModel->addParticipant(2,"Bubble-Two");
    QVERIFY(mModel->rowCount()==3);
    QVERIFY(spyDataChanged.count()==1);
}

void ut_BubbleParticipantListModel::test_removeParticipant()
{
    mModel->addParticipant(1,"Bubble-1");
    mModel->addParticipant(2,"Bubble-2");
    mModel->addParticipant(3,"Bubble-3");

    mModel->removeParticipant(2);
    QVERIFY(mModel->rowCount()==2);

    mModel->removeParticipant(3);
    QVERIFY(mModel->rowCount()==1);

    mModel->addParticipant(2,"Bubble-2");
    QVERIFY(mModel->rowCount()==2);
}

void ut_BubbleParticipantListModel::test_bubbleId()
{
    mModel->addParticipant(1,"Bubble-1");
    mModel->addParticipant(2,"Bubble-2");
    mModel->addParticipant(3,"Bubble-3");

    QVERIFY(mModel->bubbleId(1)==2);
    QVERIFY(mModel->bubbleId(4)==-1);
}

void ut_BubbleParticipantListModel::test_dataModel()
{
    mModel->addParticipant(1,"Bubble-1");
    mModel->addParticipant(2,"Bubble-2",Qt::ElideLeft);
    mModel->addParticipant(3,"Bubble-3");

    QModelIndex index = mModel->index(1,0);

    QVERIFY(index.data(Qt::DisplayRole).toString()=="Bubble-2");
    QVERIFY(index.data(Qt::TextAlignmentRole).toInt()==Qt::ElideLeft);

    QVERIFY(index.data(Qt::EditRole).isNull());

    index = mModel->index(4,0);
    QVERIFY(index.data(Qt::DisplayRole).isNull());
}

void ut_BubbleParticipantListModel::test_reset()
{
    mModel->reset();
    QVERIFY(mModel->rowCount()==0);
}

BUBBLE_TEST_MAIN(ut_BubbleParticipantListModel)
#include "ut_bubblepartlistmodel.moc"
