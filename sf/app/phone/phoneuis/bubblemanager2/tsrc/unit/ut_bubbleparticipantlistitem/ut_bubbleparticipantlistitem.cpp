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
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbstyle.h>

#include "bubbletest.h"
#include "bubbleparticipantlistitem.h"

class ut_BubbleParticipantListItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCreateItem();
    void testUpdateChildItems();

private:
    BubbleParticipantListItem* mItem;
    HbMainWindow* mMainWindow;
    int mStyleBaseId;
};

void ut_BubbleParticipantListItem::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mItem = new BubbleParticipantListItem();
    mMainWindow->addView(mItem);
    BubbleParticipantListItem* item =
        static_cast<BubbleParticipantListItem*>(mItem);
    item->updateChildItems();
    mMainWindow->show();
    QTest::qWait(300); // Give HbMainWindow time to complete show() function call.
}

void ut_BubbleParticipantListItem::cleanupTestCase()
{
    mItem->clearActions();
    delete mMainWindow;
}

void ut_BubbleParticipantListItem::testCreateItem()
{
    HbAbstractViewItem* item = mItem->createItem();
    Q_ASSERT(item);
}

void ut_BubbleParticipantListItem::testUpdateChildItems()
{
    HbAction act1("Action-1");
    HbAction act2("Action-2");
    mItem->addAction(&act1);
    mItem->addAction(&act2);
    mItem->updateChildItems();
    mItem->updateChildItems();
}

BUBBLE_TEST_MAIN(ut_BubbleParticipantListItem)
#include "ut_bubbleparticipantlistitem.moc"
