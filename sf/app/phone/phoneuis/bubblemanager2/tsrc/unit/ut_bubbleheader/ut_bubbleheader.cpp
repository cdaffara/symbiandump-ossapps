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

#include "bubbletest.h"
#include "bubbleheader.h"

class ut_BubbleHeader : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testIsUsed();
    void testBubbleId();
    void testCallState();
    void testCallFlags();
    void testNumberType();
    void testText();
    void testCli();
    void testSecondaryCli();
    void testTimerCost();
    void testCallImage();
    void testShowDefaultAvatar();
    void testAddActions();
    void testIsConference();
    void testIsInConference();
    void testSetExpandAction();
    void testReset(); // this should run last

private:
    BubbleHeader* bubbleHeader; // class under test
};

void ut_BubbleHeader::initTestCase()
{
    bubbleHeader = new BubbleHeader();
}

void ut_BubbleHeader::cleanupTestCase()
{
    delete bubbleHeader;
}

void ut_BubbleHeader::testIsUsed()
{
    bubbleHeader->setIsUsed( true );
    QVERIFY( bubbleHeader->isUsed() == true );
}

void ut_BubbleHeader::testBubbleId()
{
    bubbleHeader->setBubbleId( 1 );
    QVERIFY( bubbleHeader->bubbleId() == 1 );
}

void ut_BubbleHeader::testCallState()
{
    bubbleHeader->setCallState( BubbleManager::Active );
    QVERIFY( bubbleHeader->callState() == BubbleManager::Active );

    bubbleHeader->setCallState( BubbleManager::Disconnected );
    QVERIFY( bubbleHeader->callState() == BubbleManager::Disconnected );

    bubbleHeader->setCallState( BubbleManager::Incoming );
    bubbleHeader->setCallState( BubbleManager::Disconnected );
    QVERIFY( bubbleHeader->callState() == BubbleManager::AlertToDisconnected );
}

void ut_BubbleHeader::testCallFlags()
{
    // set all bits
    bubbleHeader->setCallFlags( BubbleManager::Diverted );
    bubbleHeader->setCallFlags( BubbleManager::Video );
    QVERIFY( bubbleHeader->callFlags() == BubbleManager::Video );

    // set normal (reset all bits)
    bubbleHeader->setCallFlag( BubbleManager::Normal );
    QVERIFY( bubbleHeader->callFlags() == 0 );

    // set invidual bit
    bubbleHeader->setCallFlag( BubbleManager::Diverted );
    bubbleHeader->setCallFlag( BubbleManager::Video );
    QVERIFY( bubbleHeader->callFlags() & BubbleManager::Diverted );
    QVERIFY( bubbleHeader->callFlags() & BubbleManager::Video );
    QVERIFY( !(bubbleHeader->callFlags() & BubbleManager::NoCiphering) );

    // reset invidual bit
    bubbleHeader->removeCallFlag( BubbleManager::Video );
    bubbleHeader->removeCallFlag( BubbleManager::Normal ); // does nothing
    QVERIFY( bubbleHeader->callFlags() & BubbleManager::Diverted );
    QVERIFY( !(bubbleHeader->callFlags() & BubbleManager::Video) );
    QVERIFY( !(bubbleHeader->callFlags() & BubbleManager::NoCiphering) );
}

void ut_BubbleHeader::testNumberType()
{
    bubbleHeader->setNumberType( BubbleManager::Mobile );
    QVERIFY( bubbleHeader->numberType() == BubbleManager::Mobile );
}

void ut_BubbleHeader::testText()
{
    bubbleHeader->setText( "Calling", Qt::ElideRight );
    QVERIFY( bubbleHeader->text() == "Calling" );
    QVERIFY( bubbleHeader->textClipDirection() == Qt::ElideRight );

    bubbleHeader->setText( "", Qt::ElideLeft );
    QVERIFY( bubbleHeader->text() == "" );
    QVERIFY( bubbleHeader->textClipDirection() == Qt::ElideLeft );
}

void ut_BubbleHeader::testCli()
{
    bubbleHeader->setCli( "John Doe", Qt::ElideRight );
    QVERIFY( bubbleHeader->cli() == "John Doe" );
    QVERIFY( bubbleHeader->cliClipDirection() == Qt::ElideRight );

    bubbleHeader->setCli( "+358501234567", Qt::ElideLeft );
    QVERIFY( bubbleHeader->cli() == "+358501234567" );
    QVERIFY( bubbleHeader->cliClipDirection() == Qt::ElideLeft );
}

void ut_BubbleHeader::testSecondaryCli()
{
    bubbleHeader->setSecondaryCli( "John Doe", Qt::ElideRight );
    QVERIFY( bubbleHeader->secondaryCli() == "John Doe" );
    QVERIFY( bubbleHeader->secondaryCliClipDirection() == Qt::ElideRight );

    bubbleHeader->setSecondaryCli( "+358501234567", Qt::ElideLeft );
    QVERIFY( bubbleHeader->secondaryCli() == "+358501234567" );
    QVERIFY( bubbleHeader->secondaryCliClipDirection() == Qt::ElideLeft );
}

void ut_BubbleHeader::testTimerCost()
{
    bubbleHeader->setTimerCost( "10:01" );
    QVERIFY( bubbleHeader->timerCost() == "10:01" );
}

void ut_BubbleHeader::testCallImage()
{
    bubbleHeader->setCallImage( "image.png" );
    QVERIFY( bubbleHeader->callImage() == "image.png" );
}

void ut_BubbleHeader::testShowDefaultAvatar()
{
    bubbleHeader->setShowDefaultAvatar(true);
    QVERIFY( bubbleHeader->showDefaultAvatar() == true);
    bubbleHeader->setShowDefaultAvatar(false);
    QVERIFY( bubbleHeader->showDefaultAvatar() == false);
}

void ut_BubbleHeader::testAddActions()
{
    for ( int i=0; i < 4; i++ )
    {
        HbAction* action = new HbAction( "Test", this);
        bubbleHeader->addAction( action );
    }

    QVERIFY( bubbleHeader->actions().count() == 4 );

    bubbleHeader->clearActions();
}

void ut_BubbleHeader::testIsConference()
{
    QVERIFY( bubbleHeader->isConference() == false );
}

void ut_BubbleHeader::testIsInConference()        
{
    bubbleHeader->setInConference(true);
    QVERIFY( bubbleHeader->isInConference() == true );    
}

void ut_BubbleHeader::testSetExpandAction()
{
    HbAction* action = new HbAction( "Test", this);

    bubbleHeader->setExpandAction( action );
    QVERIFY( bubbleHeader->expandAction() == action );

    bubbleHeader->setExpandAction( 0 );
    QVERIFY( bubbleHeader->expandAction() == 0 );
}

void ut_BubbleHeader::testReset()
{
    bubbleHeader->reset();
    QVERIFY( bubbleHeader->isUsed() == false );
    QVERIFY( bubbleHeader->bubbleId() == 1 );
    QVERIFY( bubbleHeader->callState() == BubbleManager::None );
    QVERIFY( bubbleHeader->callFlags() == 0 );
    QVERIFY( bubbleHeader->numberType() == BubbleManager::NotSet );
    QVERIFY( bubbleHeader->cli() == "" );
    QVERIFY( bubbleHeader->secondaryCli() == "" );
    QVERIFY( bubbleHeader->timerCost() == "" );
    QVERIFY( bubbleHeader->callImage() == "" );
    QVERIFY( bubbleHeader->isInConference() == false );
}

BUBBLE_TEST_MAIN(ut_BubbleHeader)
#include "ut_bubbleheader.moc"
