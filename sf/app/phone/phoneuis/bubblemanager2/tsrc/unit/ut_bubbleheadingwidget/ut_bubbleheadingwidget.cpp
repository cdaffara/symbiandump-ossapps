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
#include <hbtextitem.h>
#include <hbiconitem.h>

#include "bubbletest.h"
#include "bubbleheadingwidget.h"
#include "bubbleheader.h"
#include "bubblemanagerif.h"

class ut_BubbleHeadingWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanupTest();

    void testThreeLinedHeading();
    void testTwoLinedHeading();
    void testOneLinedHeading();

private:
    BubbleHeadingWidget* mHeading;
    HbMainWindow* mMainWindow;

    HbTextItem* mText1;
    HbTextItem* mText2;
    HbTextItem* mText3;

    HbIconItem* mIndi1;
    HbIconItem* mIndi2;

};

void ut_BubbleHeadingWidget::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mHeading = new BubbleHeadingWidget();
    mMainWindow->addView(mHeading);
    mMainWindow->show();

    mText1 = qgraphicsitem_cast<HbTextItem*>(
                 static_cast<HbWidget*>(mHeading)->primitive("text_line_1"));
    QVERIFY(mText1);
    mText2 = qgraphicsitem_cast<HbTextItem*>(
                 static_cast<HbWidget*>(mHeading)->primitive("text_line_2"));
    QVERIFY(mText2);
    mText3 = qgraphicsitem_cast<HbTextItem*>(
                 static_cast<HbWidget*>(mHeading)->primitive("text_line_3"));
    QVERIFY(mText3);

    mIndi1 = qgraphicsitem_cast<HbIconItem*>(
                static_cast<HbWidget*>(mHeading)->primitive("indicator_icon_1"));
    QVERIFY(mIndi1);
    mIndi2 = qgraphicsitem_cast<HbIconItem*>(
                static_cast<HbWidget*>(mHeading)->primitive("indicator_icon_2"));
    QVERIFY(mIndi2);
}

void ut_BubbleHeadingWidget::cleanupTestCase()
{
    delete mMainWindow;
}

void ut_BubbleHeadingWidget::cleanupTest()
{
    mHeading->hide();
    mHeading->reset();
}

void ut_BubbleHeadingWidget::testThreeLinedHeading()
{
    mHeading->setLineCount(3);

    BubbleHeader header;
    header.setCli("John Doe",Qt::ElideRight);
    header.setSecondaryCli("12345",Qt::ElideLeft);
    header.setText("",Qt::ElideRight);
    header.setTimerCost("0:00");
    header.setCallState(BubbleManagerIF::Active);
    header.setCallFlag(BubbleManagerIF::NoCiphering);
    QVERIFY(mHeading->lineCount()==3);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="three_lines_1");

    QVERIFY(mHeading->textLine1()=="John Doe");
    QVERIFY(mHeading->textLine2()=="12345");
    QVERIFY(mText2->alignment()&Qt::AlignLeft);
    QVERIFY(mHeading->textLine3()=="0:00");
    QVERIFY(mHeading->indicator1()==QLatin1String("qtg_mono_ciphering_off"));
    QVERIFY(mHeading->indicator2()==QLatin1String(""));

    header.setTimerCost("0:01");
    mHeading->updateTimerDisplayNow();
    QVERIFY(mHeading->textLine3()=="0:01");

    header.setCallState(BubbleManagerIF::Incoming);
    header.setCallFlag(BubbleManagerIF::NoCiphering);
    header.setCallFlag(BubbleManagerIF::Diverted);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="three_lines_2");
    QVERIFY(mHeading->indicator1()==QLatin1String("qtg_mono_ciphering_off"));
    QVERIFY(mHeading->indicator2()==QLatin1String("qtg_mono_call_diverted"));

    header.setCallFlags(BubbleManagerIF::Normal);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="three_lines");
}

void ut_BubbleHeadingWidget::testTwoLinedHeading()
{
    mHeading->setLineCount(2);

    BubbleHeader header;
    header.setCli("John Doe",Qt::ElideRight);
    header.setSecondaryCli("12345",Qt::ElideLeft);
    header.setText("",Qt::ElideRight);
    header.setTimerCost("0:00");
    header.setCallState(BubbleManagerIF::Active);
    header.setCallFlag(BubbleManagerIF::NoCiphering);
    QVERIFY(mHeading->lineCount()==2);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="two_lines_1");
    QVERIFY(mHeading->indicator1()==QLatin1String("qtg_mono_ciphering_off"));
    QVERIFY(mHeading->indicator2()==QLatin1String(""));

    QVERIFY(mHeading->textLine1()=="John Doe");
    QVERIFY(mHeading->textLine2()=="0:00");
    QVERIFY(mText2->alignment()&Qt::AlignLeft);
    QVERIFY(!mText3->isVisible());

    header.setTimerCost("0:01");
    mHeading->updateTimerDisplayNow();
    QVERIFY(mHeading->textLine2()=="0:01");

    header.setCallState(BubbleManagerIF::Incoming);
    header.removeCallFlag(BubbleManagerIF::NoCiphering);
    header.setCallFlag(BubbleManagerIF::Diverted);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="two_lines_1");
    QVERIFY(mHeading->indicator1()==QLatin1String("qtg_mono_call_diverted"));

    header.setCallFlags(BubbleManagerIF::Normal);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="two_lines");

}

void ut_BubbleHeadingWidget::testOneLinedHeading()
{
    mHeading->setLineCount(1);

    BubbleHeader header;
    header.setCli("John Doe",Qt::ElideRight);
    header.setSecondaryCli("12345",Qt::ElideLeft);
    header.setText("",Qt::ElideRight);
    header.setTimerCost("0:00");
    header.setCallState(BubbleManagerIF::Active);
    header.setCallFlag(BubbleManagerIF::NoCiphering);
    QVERIFY(mHeading->lineCount()==1);
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="one_line_1");
    QVERIFY(mHeading->textLine1()=="John Doe");
    QVERIFY(mHeading->textLine2()=="0:00");
    QVERIFY(mText2->alignment()&Qt::AlignRight);
    QVERIFY(!mText3->isVisible());

    header.setTimerCost("0:01");
    mHeading->updateTimerDisplayNow();
    QVERIFY(mHeading->textLine2()=="0:01");

    header.setCallState(BubbleManagerIF::Incoming);
    header.setCallFlag(BubbleManagerIF::NoCiphering);
    header.setCallFlag(BubbleManagerIF::Diverted);
    mHeading->hide();
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="one_line_2");
    QVERIFY(mText2->alignment()&Qt::AlignRight);

    header.setCallFlags(BubbleManagerIF::Normal);
    mHeading->hide();
    mHeading->readBubbleHeader(header);
    mHeading->show();
    QTest::qWait(200);
    QVERIFY(mHeading->layoutOption()=="one_line");
    QVERIFY(mText2->alignment()&Qt::AlignRight);
}

BUBBLE_TEST_MAIN(ut_BubbleHeadingWidget)
#include "ut_bubbleheadingwidget.moc"
