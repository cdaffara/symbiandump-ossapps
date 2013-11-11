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
#include <hbwidget.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <hbfontspec.h>

#include "bubbletest.h"
#include "bubbleeffecthandler.h"

class Container : public HbWidget
{
public:
    Container();
    ~Container();

    HbLabel *mLabel;
};

Container::Container()
{
    mLabel = new HbLabel(this);
    mLabel->setAlignment(Qt::AlignCenter);
    HbFontSpec spec;
    spec.setRole(HbFontSpec::Primary);
    spec.setTextHeight(30);
    mLabel->setFontSpec(spec);
    QGraphicsLinearLayout* layout =
        new QGraphicsLinearLayout(Qt::Vertical);
    layout->addItem(mLabel);

    setLayout(layout);
}

Container::~Container()
{
}

class ut_BubbleEffectHandler : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testMuteAppearEffect();
    void testMuteDisappearEffect();
    void testCancelAllEffects();

private:
    HbMainWindow* mWindow;
    BubbleEffectHandler *mEffectHandler;
    Container *mContainer;
};

void ut_BubbleEffectHandler::initTestCase()
{
    mWindow = new HbMainWindow();
    mEffectHandler = new BubbleEffectHandler();
    mContainer = new Container();
    mContainer->mLabel->setVisible(false);
    mWindow->addView(mContainer);
    mWindow->show();
}

void ut_BubbleEffectHandler::cleanupTestCase()
{
    delete mEffectHandler;
    delete mWindow;
}

void ut_BubbleEffectHandler::testMuteAppearEffect()
{
    QVERIFY(mContainer->mLabel->isVisible()==false);
    QTest::qWait(500);

    mContainer->mLabel->setPlainText("Appearing");

    mEffectHandler->addEffect(mContainer->mLabel,BubbleMutedAppear);
    mEffectHandler->startEffect(BubbleMutedAppear);
    QTest::qWait(1000);

    QVERIFY(mContainer->mLabel->isVisible()==true);

    mEffectHandler->removeEffect(mContainer->mLabel,BubbleMutedAppear);
}

void ut_BubbleEffectHandler::testMuteDisappearEffect()
{
    QVERIFY(mContainer->mLabel->isVisible()==true);

    mContainer->mLabel->setPlainText("Disappearing");

    mEffectHandler->addEffect(mContainer->mLabel,BubbleMutedDisappear);
    mEffectHandler->startEffect(BubbleMutedDisappear);
    QTest::qWait(1000);

    QVERIFY(mContainer->mLabel->isVisible()==false);

    mEffectHandler->removeEffect(mContainer->mLabel,BubbleMutedDisappear);
}

void ut_BubbleEffectHandler::testCancelAllEffects()
{
    QVERIFY(mContainer->mLabel->isVisible()==false);
    QTest::qWait(500);

    mContainer->mLabel->setPlainText("Appearing");

    mEffectHandler->addEffect(mContainer->mLabel,BubbleMutedAppear);
    mEffectHandler->startEffect(BubbleMutedAppear);
    QTest::qWait(100);
    QVERIFY(HbEffect::effectRunning(mContainer->mLabel));
    mEffectHandler->cancelAllEffects(mContainer->mLabel);
    QVERIFY(!HbEffect::effectRunning(mContainer->mLabel));

    mEffectHandler->removeEffect(mContainer->mLabel,BubbleMutedAppear);
}

BUBBLE_TEST_MAIN(ut_BubbleEffectHandler)
#include "ut_bubbleeffecthandler.moc"
