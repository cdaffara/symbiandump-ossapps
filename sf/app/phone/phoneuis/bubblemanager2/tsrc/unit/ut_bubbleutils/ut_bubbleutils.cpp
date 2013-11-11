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
#include <bubblebutton.h>
#include <hbaction.h>

#include "bubbletest.h"
#include "bubbleutils.h"
#include "bubbleheader.h"
#include "bubbleconferenceheader.h"

class ut_BubbleUtils : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_setCallHeaderTexts3Lines_incoming();
    void test_setCallHeaderTexts3Lines_waiting();
    void test_setCallHeaderTexts3Lines_active();
    void test_setCallHeaderTexts3Lines_disconnected();
    void test_setCallHeaderTexts3Lines_outgoing();
    void test_setCallHeaderTexts3Lines_alerting();
    void test_setCallHeaderTexts3Lines_held();
    void test_setCallHeaderTexts3Lines_alerttodisconnected();

    void test_setCallHeaderTexts2Lines_waiting();
    void test_setCallHeaderTexts2Lines_alerttodisconnected();
    void test_setCallHeaderTexts2Lines_active();
    void test_setCallHeaderTexts2Lines_held();
    void test_setCallHeaderTexts2Lines_disconnected();
    void test_setCallHeaderTexts2Lines_outgoing();

    void test_compareHeaders();

    void test_setButtonStyleForAction();

    void test_setIndicators();

private:
};

void ut_BubbleUtils::initTestCase()
{
}

void ut_BubbleUtils::cleanupTestCase()
{
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_incoming()
{
    // incoming call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;
    
    header.setCallState( BubbleManager::Incoming );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "calling", Qt::ElideRight );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "calling" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );

    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "calling" );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_waiting()
{
    // incoming call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Waiting );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "waiting", Qt::ElideRight );

    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "waiting" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );

    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "waiting" );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_active()
{
    // active call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Active );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "", Qt::ElideRight );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "" );
    QVERIFY( text3.text() == "" );
    
    // add timer
    header.setTimerCost( "00:00" );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    QVERIFY( text1.text()== "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "00:00" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );
    header.setTimerCost( "" );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "" );
    
    // add timer
    header.setTimerCost( "00:00" );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    QVERIFY( text1.text()== "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "00:00" );
    QVERIFY( text3.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_disconnected()
{
    // disconnected call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Disconnected );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "discon", Qt::ElideRight );
    header.setTimerCost( "00:00" );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "discon" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "00:00" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "discon" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "00:00" );
    QVERIFY( text3.elideMode() == Qt::ElideRight );

    // without call timer
    text3.setText(QString());
    header.setTimerCost(QString());
    header.setSecondaryCli(QString(), Qt::ElideNone);

    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "discon" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );

    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "discon" );
    QVERIFY( text3.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_outgoing()
{
    // outgoing call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Outgoing );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "calling", Qt::ElideRight );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    
    QVERIFY( text1.text() == "calling" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "john" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "calling" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "john" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "12345" );
    QVERIFY( text3.elideMode() == Qt::ElideLeft );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_alerting()
{
    // alerting call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Alerting );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "calling", Qt::ElideRight );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "" );
    QVERIFY( text3.text() == "" );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "" );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_held()
{
    // held
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::OnHold );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "on hold", Qt::ElideRight );
    header.setTimerCost( "00:00" );
    
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
    
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "on hold" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );
    
    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);
        
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "on hold" );
    QVERIFY( text3.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts3Lines_alerttodisconnected()
{
    // incoming call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    HbTextItem text3;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Incoming );
    header.setCallState( BubbleManager::AlertToDisconnected );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "disconnected", Qt::ElideRight );

    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "disconnected" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
    QVERIFY( text3.text() == "" );

    // add secondary cli
    header.setSecondaryCli( "12345", Qt::ElideLeft );
    BubbleUtils::setCallHeaderTexts3Lines(header, text1, text2, text3, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "12345" );
    QVERIFY( text2.elideMode() == Qt::ElideLeft );
    QVERIFY( text3.text() == "disconnected" );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_waiting()
{
    // waiting call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Waiting );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "waiting", Qt::ElideRight );

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "waiting" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );

    // add secondary cli
    header.setSecondaryCli("12345",Qt::ElideLeft);
    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "waiting" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_alerttodisconnected()
{
    // waiting call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::AlertToDisconnected );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "disconnected", Qt::ElideRight );

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "disconnected" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );

    // add secondary cli
    header.setSecondaryCli("12345",Qt::ElideLeft);
    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "disconnected" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_active()
{
    // active call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Active );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "", Qt::ElideRight );

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "" );

    // add timer
    header.setTimerCost( "00:00" );
    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "00:00" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_held()
{
    // held
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::OnHold );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "on hold", Qt::ElideRight );
    header.setTimerCost( "00:00" );

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);

    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "on hold" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_disconnected()
{
    // disconnected call
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState( BubbleManager::Disconnected );
    header.setCli( "john", Qt::ElideRight );
    header.setText( "discon", Qt::ElideRight );
    header.setTimerCost( "00:00" );

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);
    QVERIFY( text1.text() == "john" );
    QVERIFY( text1.elideMode() == Qt::ElideRight );
    QVERIFY( text2.text() == "discon" );
    QVERIFY( text2.elideMode() == Qt::ElideRight );
}

void ut_BubbleUtils::test_setCallHeaderTexts2Lines_outgoing()
{
    // outgoing
    BubbleHeader header;
    HbTextItem text1;
    HbTextItem text2;
    int cliLine;
    int timerLine;

    header.setCallState(BubbleManager::Outgoing);
    header.setCli("john", Qt::ElideRight);
    header.setText("calling", Qt::ElideRight);
    header.setTimerCost("00:00");

    BubbleUtils::setCallHeaderTexts2Lines(header, text1, text2, cliLine, timerLine);
    QCOMPARE(text1.text(), QString("calling"));
    QCOMPARE(text1.elideMode(), Qt::ElideRight);
    QCOMPARE(text2.text(), QString("john"));
    QCOMPARE(text2.elideMode(), Qt::ElideRight);
}

void ut_BubbleUtils::test_compareHeaders()
{
    BubbleHeader header1;
    BubbleHeader header2;
    BubbleConferenceHeader headerC;

    header1.setCallState(BubbleManagerIF::Active);
    header2.setCallState(BubbleManagerIF::Waiting);
    QVERIFY( BubbleUtils::compareHeaders(&header1,&header2) == false );

    header1.setCallState(BubbleManagerIF::Active);
    header2.setCallState(BubbleManagerIF::OnHold);
    QVERIFY( BubbleUtils::compareHeaders(&header1,&header2) == true );

    headerC.setCallState(BubbleManagerIF::Active);
    header1.setCallState(BubbleManagerIF::OnHold);

    QVERIFY( BubbleUtils::compareHeaders(&headerC,&header1) == false );
}

void ut_BubbleUtils::test_setButtonStyleForAction()
{
    BubbleButton button;
    HbAction action;

    action.setSoftKeyRole(QAction::NoSoftKey);
    BubbleUtils::setButtonStyleForAction(button,action);
    QVERIFY(button.buttonType()==BubbleButton::DefaultButton);

    action.setSoftKeyRole(QAction::PositiveSoftKey);
    BubbleUtils::setButtonStyleForAction(button,action);
    QVERIFY(button.buttonType()==BubbleButton::GreenButton);

    // The next test is flagged out from winscw build because seems that
    // in Qt 4.7 the behaviour of QAction::setSoftKeyRole and
    // QAction::softKeyRole is broken.
#if not defined(__WINSCW__)
    action.setSoftKeyRole(QAction::NegativeSoftKey);
    BubbleUtils::setButtonStyleForAction(button,action);
    QVERIFY(button.buttonType()==BubbleButton::RedButton);
#endif
}

void ut_BubbleUtils::test_setIndicators()
{
    HbIconItem icon1;
    icon1.hide();
    HbIconItem icon2;
    icon2.hide();

    BubbleUtils::setIndicators(
        BubbleManagerIF::Incoming, BubbleManagerIF::NoCiphering, icon1, icon2);
    QVERIFY(icon1.iconName()=="qtg_mono_ciphering_off");
    QVERIFY(icon1.isVisible()==true);
    QVERIFY(icon2.iconName()=="");
    QVERIFY(icon2.isVisible()==false);

    BubbleUtils::setIndicators(
        BubbleManagerIF::Incoming, BubbleManagerIF::Diverted, icon1, icon2);
    QVERIFY(icon1.iconName()=="qtg_mono_call_diverted");
    QVERIFY(icon1.isVisible()==true);
    QVERIFY(icon2.iconName()=="");
    QVERIFY(icon2.isVisible()==false);

    BubbleUtils::setIndicators(
        BubbleManagerIF::Active, BubbleManagerIF::Diverted, icon1, icon2);
    QVERIFY(icon1.iconName()=="");
    QVERIFY(icon1.isVisible()==false);
    QVERIFY(icon2.iconName()=="");
    QVERIFY(icon2.isVisible()==false);

    int flags = 0;
    flags |= BubbleManagerIF::NoCiphering;
    flags |= BubbleManagerIF::Diverted;
    BubbleUtils::setIndicators(
        BubbleManagerIF::Incoming, flags, icon1, icon2);
    QVERIFY(icon1.iconName()=="qtg_mono_ciphering_off");
    QVERIFY(icon1.isVisible()==true);
    QVERIFY(icon2.iconName()=="qtg_mono_call_diverted");
    QVERIFY(icon2.isVisible()==true);


    BubbleUtils::setIndicators(
        BubbleManagerIF::Incoming, BubbleManagerIF::Normal, icon1, icon2);
    QVERIFY(icon1.iconName()=="");
    QVERIFY(icon1.isVisible()==false);
    QVERIFY(icon2.iconName()=="");
    QVERIFY(icon2.isVisible()==false);
}

BUBBLE_TEST_MAIN(ut_BubbleUtils)
#include "ut_bubbleutils.moc"
