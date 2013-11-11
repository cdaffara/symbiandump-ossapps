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
#include "bubbleconferenceheader.h"

static const int PARTICIPANT_COUNT=5;

class ut_BubbleConferenceHeader : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testIsConference();
    void testAddHeader();
    void testRemoveHeader();
    void testAddParticipantListActions();
    void testSetExpanded();
    void testSetSelectedHeader();
    void testReset(); // keep this last

private:
    BubbleConferenceHeader* confHeader; // class under test
    QList<BubbleHeader*> participants;
};

void ut_BubbleConferenceHeader::initTestCase()
{
    confHeader = new BubbleConferenceHeader();

    for (int i=0; i<PARTICIPANT_COUNT; i++) {
        BubbleHeader* header = new BubbleHeader();
        header->setBubbleId(i);
        participants.append(header);
    }
}

void ut_BubbleConferenceHeader::cleanupTestCase()
{
    qDeleteAll(participants);
    participants.clear();

    delete confHeader;
}

void ut_BubbleConferenceHeader::testIsConference()
{
    QVERIFY( confHeader->isConference() == true );
}

void ut_BubbleConferenceHeader::testAddHeader()
{
    foreach(BubbleHeader* participant,participants) {
        confHeader->addHeader(participant);
    }

    QVERIFY( confHeader->headers().count()==PARTICIPANT_COUNT );
    QVERIFY( confHeader->headers().at(0)->isInConference()==true );
}

void ut_BubbleConferenceHeader::testRemoveHeader()
{
    confHeader->removeHeader(0);
    confHeader->removeHeader(0);
    confHeader->removeHeader(2);

    QVERIFY( confHeader->headers().count()==PARTICIPANT_COUNT-2 );
    QVERIFY( participants.at(2)->isInConference()==false );
}

void ut_BubbleConferenceHeader::testAddParticipantListActions()
{
    for ( int i=0; i < 2; i++ ) {
        HbAction* action = new HbAction( "Test", this);
        confHeader->addParticipantListAction(action);
    }

    QVERIFY( confHeader->participantListActions().count() == 2 );
    confHeader->clearParticipantListActions();
    QVERIFY( confHeader->participantListActions().count() == 0 );    

    HbAction* action = new HbAction( "Test", this);
    confHeader->addParticipantListAction(action);
}

void ut_BubbleConferenceHeader::testSetExpanded()
{
    confHeader->setExpanded(true);
    QVERIFY(confHeader->isExpanded());
    confHeader->setExpanded(true);
}

void ut_BubbleConferenceHeader::testSetSelectedHeader()
{
    confHeader->setSelectedHeader(1);
    QVERIFY(confHeader->selectedHeader()==1);
    confHeader->setSelectedHeader(1);
}

void ut_BubbleConferenceHeader::testReset()
{
    confHeader->reset();
    QVERIFY( confHeader->headers().count()==0 );
    QVERIFY( participants.at(1)->isInConference()==false );
    QVERIFY( confHeader->participantListActions().count()==0 );
    QVERIFY( confHeader->isExpanded()==false );
    QVERIFY( confHeader->selectedHeader()==-1 );
}

BUBBLE_TEST_MAIN(ut_BubbleConferenceHeader)
#include "ut_bubbleconferenceheader.moc"
