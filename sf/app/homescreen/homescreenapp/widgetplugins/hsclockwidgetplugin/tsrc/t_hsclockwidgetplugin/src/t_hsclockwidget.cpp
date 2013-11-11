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
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QComboBox>
#include <QMap>

#include "t_hsclockwidget.h"
#include "hsclockwidget.h"
#include "hsanalogclockwidget.h"
#include "hsdigitalclockwidget.h"
#include "hsclockwidgettimer.h"

#ifdef Q_OS_SYMBIAN
#include <bacntf.h> // TLocale
#include <apgtask.h>
#include <eikenv.h>
#endif //Q_OS_SYMBIAN

namespace
{
    const char ANALOG[] = "analog";
    const char DIGITAL[] = "digital";
    const char TIME12[] = "TIME12";
    const char TIME24[] = "TIME24";
}


#ifdef Q_OS_SYMBIAN
#define KClockApUid TUid::Uid(0x10005903)
#endif //Q_OS_SYMBIAN

void TestClockWidget::initTestCase()
    {
#ifdef Q_OS_SYMBIAN
    TLocale locale;
    mClockFormat = locale.ClockFormat();
    mTimeFormat = locale.TimeFormat();
#endif
    }

void TestClockWidget::cleanupTestCase()
    {
#ifdef Q_OS_SYMBIAN
    TLocale locale;
    locale.SetClockFormat(mClockFormat);
    locale.SetTimeFormat(mTimeFormat);
    locale.Set();
#endif
    }

void TestClockWidget::init()
    {
    QGraphicsWidget *hostWidget = new QGraphicsWidget();
    mClockWidget = new HsClockWidget(hostWidget);
    mClockWidget->resize(100,100);
    QVERIFY( mClockWidget );
    }

void TestClockWidget::cleanup()
    {
    //delete mClockWidget->parentItem();
    delete mClockWidget;
    mClockWidget = 0;

    }

void TestClockWidget::createClockWidget()
    {
    }


void TestClockWidget::toggleAnalogDigital()
    {
    mClockWidget->onInitialize();
    QString clockType = mClockWidget->mClockType;
    
    mClockWidget->onClockTapped();

#ifndef Q_OS_SYMBIAN
    // toggles analog/digital on windows
    if (clockType == DIGITAL) {
        QCOMPARE( mClockWidget->mClockType, QString(ANALOG));
    } else {
        QCOMPARE( mClockWidget->mClockType, QString(DIGITAL));
    }

    clockType = mClockWidget->mClockType;

    mClockWidget->onClockTapped();

    if (clockType == DIGITAL) {
        QCOMPARE( mClockWidget->mClockType, QString(ANALOG));
    } else {
        QCOMPARE( mClockWidget->mClockType, QString(DIGITAL));
    }
#else    
    QCOMPARE( mClockWidget->mClockType, clockType);   
    QTest::qWait(6000);
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());    
    TApaTask task = taskList.FindApp(KClockApUid);    
    if (task.Exists()){
        task.SendToBackground();        
    }    
    else {
        QWARN("Clock application start failed");
    }
#endif
    }


void TestClockWidget::testClockOnInitalize()
{
    mClockWidget->onInitialize();
    QVERIFY(mClockWidget->mWidget);
}

void TestClockWidget::testClockOnShow()
{
    mClockWidget->onInitialize();
    QVERIFY(mClockWidget->mWidget);
    QVERIFY(!HsClockWidgetTimer::instance()->mTimer->isActive());
    mClockWidget->onShow();
    QVERIFY(HsClockWidgetTimer::instance()->mTimer->isActive());

    QSignalSpy updateSpy(HsClockWidgetTimer::instance()->mTimer,SIGNAL(timeout()));
    QEventLoop waitUntilTimeoutSignaled;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(1500);
    eventLoopTimer.setSingleShot(true);
    connect(HsClockWidgetTimer::instance()->mTimer, SIGNAL(timeout()),
            &waitUntilTimeoutSignaled, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()), &waitUntilTimeoutSignaled, SLOT(quit()));
    eventLoopTimer.start();
    waitUntilTimeoutSignaled.exec();

    //Verify timeout was received instead of eventLoopTimer has elapsed.
    QCOMPARE(updateSpy.count(), 1);
    QVERIFY(eventLoopTimer.isActive());
    eventLoopTimer.stop();

}

void TestClockWidget::testClockOnHide()
{
    mClockWidget->onInitialize();
    mClockWidget->onShow();
    mClockWidget->onHide();
}

void TestClockWidget::testOnUninitialize()
{
    mClockWidget->onInitialize(); 
    mClockWidget->onUninitialize();
}


void TestClockWidget::testOnSettingsChanged()
{
    mClockWidget->mClockType = QString(ANALOG);
    mClockWidget->mTimeType = QString(TIME12);

    mClockWidget->onInitialize();
    mClockWidget->onShow();
    mClockWidget->onSettingsChanged(QString(DIGITAL), QString(TIME24));
    mClockWidget->onSettingsChanged(QString(DIGITAL), QString(TIME12));
    mClockWidget->onSettingsChanged(QString(ANALOG), QString(TIME24));
    mClockWidget->onSettingsChanged(QString(ANALOG), QString(TIME12));
}

void TestClockWidget::testClockShape()
{
    mClockWidget->onInitialize();
    mClockWidget->shape();	
}

QTEST_MAIN(TestClockWidget)

// end of file
