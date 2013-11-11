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
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbcolorscheme.h>
#include <hbiconitem.h>

#include "bubbletest.h"
#include "bubblebutton.h"

class TestBubbleButton : public BubbleButton
{
public:
    TestBubbleButton() {};
    ~TestBubbleButton() {};

    bool testSceneEvent(QEvent *event) { return BubbleButton::sceneEvent(event); };
};

class ut_BubbleButton : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testButtonType();
    void testUpdatePrimitives();
    void testSceneEvent();

private:
    TestBubbleButton *mButton;
};

void ut_BubbleButton::initTestCase()
{
    mButton = new TestBubbleButton();
    mButton->setIcon(HbIcon(":/resources/qtg_mono_call.svg"));
}

void ut_BubbleButton::cleanupTestCase()
{
    delete mButton;
}

void ut_BubbleButton::testButtonType()
{
    QVERIFY(mButton->buttonType()==BubbleButton::DefaultButton);
    mButton->setButtonType(BubbleButton::GreenButton);
    QVERIFY(mButton->buttonType()==BubbleButton::GreenButton);
    mButton->setButtonType(BubbleButton::RedButton);
    QVERIFY(mButton->buttonType()==BubbleButton::RedButton);
}

void ut_BubbleButton::testUpdatePrimitives()
{
    // function button
    HbFrameItem* frame =
        qgraphicsitem_cast<HbFrameItem*>(
            static_cast<HbWidget*>(mButton)->primitive("background"));
    QVERIFY(frame);

    HbIconItem* icon =
        qgraphicsitem_cast<HbIconItem*>(
            static_cast<HbWidget*>(mButton)->primitive("icon"));
    QVERIFY(icon);

    QColor iconColor;

    mButton->setVisible(false);
    mButton->setButtonType(BubbleButton::DefaultButton);
    mButton->setVisible(true);

    QVERIFY(frame->frameDrawer().frameGraphicsName()!="qtg_fr_btn_green_normal");
    QVERIFY(frame->frameDrawer().frameGraphicsName()!="qtg_fr_btn_green_normal");
    iconColor = HbColorScheme::color("qtc_button_normal");
    QVERIFY(icon->color()==iconColor);
    mButton->setDown(true);
    QVERIFY(frame->frameDrawer().frameGraphicsName()!="qtg_fr_btn_green_pressed");
    QVERIFY(frame->frameDrawer().frameGraphicsName()!="qtg_fr_btn_green_pressed");
    iconColor = HbColorScheme::color("qtc_button_pressed");
    QVERIFY(icon->color()==iconColor);


    mButton->setDown(false);
    mButton->setVisible(false);
    mButton->setButtonType(BubbleButton::RedButton);
    mButton->setVisible(true);

    QVERIFY(frame->frameDrawer().frameGraphicsName()=="qtg_fr_btn_red_normal");
    iconColor = HbColorScheme::color("qtc_callhandling_reject_normal");
    QVERIFY(icon->color()==iconColor);
    mButton->setDown(true);
    QVERIFY(frame->frameDrawer().frameGraphicsName()=="qtg_fr_btn_red_pressed");
    iconColor = HbColorScheme::color("qtc_callhandling_reject_pressed");
    QVERIFY(icon->color()==iconColor);

    mButton->setDown(false);
    mButton->setVisible(false);
    mButton->setButtonType(BubbleButton::GreenButton);
    mButton->setVisible(true);

    QVERIFY(frame->frameDrawer().frameGraphicsName()=="qtg_fr_btn_green_normal");
    iconColor = HbColorScheme::color("qtc_callhandling_answer_normal");
    QVERIFY(icon->color()==iconColor);
    mButton->setDown(true);
    QVERIFY(frame->frameDrawer().frameGraphicsName()=="qtg_fr_btn_green_pressed");
    iconColor = HbColorScheme::color("qtc_callhandling_answer_pressed");
    QVERIFY(icon->color()==iconColor);
}

void ut_BubbleButton::testSceneEvent()
{
    QEvent event(QEvent::UngrabMouse);
    QSignalSpy spy( mButton, SIGNAL( clicked()) );
    QSignalSpy spy2( mButton, SIGNAL( released()) );
    mButton->setVisible(false);
    mButton->setDown(false);
    mButton->testSceneEvent(&event);
    QVERIFY(spy.count()==0);
    QVERIFY(spy2.count()==0);

    QEvent eventNone(QEvent::None);
    mButton->testSceneEvent(&eventNone);
    QVERIFY(spy.count()==0);
    QVERIFY(spy2.count()==0);

    mButton->setVisible(true);
    mButton->setDown(true);
    mButton->testSceneEvent(&event);
    QVERIFY(spy.count()==1);
    QVERIFY(spy2.count()==1);
    QVERIFY(mButton->isDown()==false);
}

BUBBLE_TEST_MAIN(ut_BubbleButton)
#include "ut_bubblebutton.moc"
