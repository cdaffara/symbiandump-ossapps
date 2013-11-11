/*
* Copyright (c) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test code
*
*/

#include <QtTest/QtTest>
#include <QStateMachine>
#include <QGraphicsSceneMouseEvent>

#include <HbTouchArea>
#include <HbFrameItem>

#include "t_hsshortcutwidget.h"
#include "hsshortcutwidget.h"
#include "hsshortcutservice.h"
#include "hsconfiguration.h"

#include "caentry.h"
#include "caservice.h"

void TestShortcutWidget::initTestCase()
{
    mMachine = new QStateMachine;
    mShortcutService = HsShortcutService::instance(mMachine);
    HsConfiguration::setInstance(new HsConfiguration);
}

void TestShortcutWidget::cleanupTestCase()
{
    delete mMachine;
}

void TestShortcutWidget::testConstruction()
{
    HSCONFIGURATION_SET(setShortcutLabelVisible, true);
    HsShortcutWidget *w = new HsShortcutWidget;
    QVERIFY(w->mBackground);
    QVERIFY(w->mIcon);
    QVERIFY(w->mText);    
    QCOMPARE(w->mCaEntryId, -1);
    QCOMPARE(w->mCaEntryRole, ItemEntryRole);
    w->createPrimitives();
    QVERIFY(w->mBackground);
    QVERIFY(w->mIcon);
    QVERIFY(w->mText);
    QCOMPARE(w->mCaEntryId, -1);
    QCOMPARE(w->mCaEntryRole, ItemEntryRole);
    
    w->setText("testing");
    QCOMPARE(w->text(), QString("testing"));
    
    delete w;
    
    HSCONFIGURATION_SET(setShortcutLabelVisible, false);
    w = new HsShortcutWidget;
    QVERIFY(!w->mText);
    w->setText("testing");
    QCOMPARE(w->text(), QString(""));
    delete w;
}

void TestShortcutWidget::testProperties()
{
    HsShortcutWidget *w = new HsShortcutWidget;

    QCOMPARE(w->property("caEntryId").toInt(), -1);
    w->setProperty("caEntryId", 1);
    QCOMPARE(w->property("caEntryId").toInt(), 1);

    QCOMPARE(w->property("uid").toString(), QString());
    w->setProperty("uid", "0xABBAACDC");
    QCOMPARE(w->property("uid").toString(), QString("0xABBAACDC"));

    delete w;
}

void TestShortcutWidget::testSignals()
{
    HsShortcutWidget *w = new HsShortcutWidget;

    QSignalSpy spy(w, SIGNAL(finished()));
    w->onInitialize();
    QCOMPARE(spy.count(), 1);
    spy.clear();

    CaEntry e;
    ChangeType c = RemoveChangeType;

    w->onEntryChanged(e, c);
    QCOMPARE(spy.count(), 1);
    spy.clear();

    delete w;
}

void TestShortcutWidget::testShowHide()
{
    HsShortcutWidget *w = new HsShortcutWidget;

    w->onShow();
    QVERIFY(w->isVisible());
    w->onHide();

    w->mCaEntryFlags = MissingEntryFlag;
    w->onShow();
    QVERIFY(!w->isVisible());
    w->onHide();

    delete w;
}

void TestShortcutWidget::testCaNotifier()
{
    HSCONFIGURATION_SET(setShortcutLabelVisible, true);
    HsShortcutWidget *w = new HsShortcutWidget;

    w->createCaNotifier();
    
    CaEntry e;
    ChangeType c = UpdateChangeType;
    w->onEntryChanged(e, c);
    QVERIFY(w->isVisible());

    //Media detached
    QSignalSpy spy(w, SIGNAL(finished()));
    c = RemoveChangeType;
    e.setFlags(e.flags() | MissingEntryFlag);
    w->onEntryChanged(e, c);
    QVERIFY(!w->isVisible());
    QCOMPARE(spy.count(), 0);
    spy.clear();

    //Genuinely removed
    c = RemoveChangeType;
    //Remove MissingEntryFlag
    e.setFlags(e.flags() & ~MissingEntryFlag);
    w->onEntryChanged(e, c);
    QVERIFY(w->isVisible());
    QCOMPARE(spy.count(), 1);
    spy.clear();
    delete w;
    

    HSCONFIGURATION_SET(setShortcutLabelVisible, false);
    w = new HsShortcutWidget;
    w->createCaNotifier();
    c = UpdateChangeType;
    w->onEntryChanged(e, c);
    QVERIFY(w->isVisible());
    delete w;
    
}

void TestShortcutWidget::testUserInteraction()
{
    HsShortcutWidget *w = new HsShortcutWidget;
    w->resize(100, 100);

    w->setBackgroundToNormal();
    QCOMPARE(w->mBackground->frameDrawer().frameGraphicsName(), QLatin1String("qtg_fr_hsshortcut_normal"));

    w->setBackgroundToPressed();
    QCOMPARE(w->mBackground->frameDrawer().frameGraphicsName(), QLatin1String("qtg_fr_hsitems_pressed"));
        
    w->launch();

    delete w;
}

QTEST_MAIN(TestShortcutWidget)
