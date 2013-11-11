/*
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
#ifndef ST_LOGSCNTFINDER_H
#define ST_LOGSCNTFINDER_H

#include <QObject>
#include <qcontactmanager.h>
#include <hbinputlanguage.h>

QTM_USE_NAMESPACE

class LogsCntFinder;


class ST_LogsCntFinder : public QObject                 
{
     Q_OBJECT
public:
     ST_LogsCntFinder( bool createContacts = true ) : mCreateContacts( createContacts ) {}
     
private slots:

/*
 * In addition, there are four private slots that are not treated as testfunctions. 
 * They will be executed by the testing framework and can be used to initialize and clean up 
 * either the entire test or the current test function.
 * 
 * initTestCase() will be called before the first testfunction is executed.
 * cleanupTestCase() will be called after the last testfunction was executed.
 * init() will be called before each testfunction is executed.
 * cleanup() will be called after every testfunction.
*/
     void initTestCase();
     void cleanupTestCase();
     void init();
     void cleanup();

private slots: //test methods

    void testKeymap_2();
    void testKeymap_3();
    void testKeymap_4();
    void testKeymap_5();
    void testKeymap_6();
    void testKeymap_7();
    void testKeymap_8();
    void testKeymap_9();
    
    void testPredictiveSearchQueryPartialCached();
    void testPredictiveSearchQueryFullyCached();
    void testPredictiveSearchQueryPartialCachedNoResults();
    void testPredictiveSearchQueryFullyCachedNoResults();
    void testPredictiveSearchQueryPartialCachedZeroCase();
    void testPredictiveSearchQueryFullyCachedZerosStartCase();
    void testPredictiveSearchQueryFullyCachedZerosStartCase_2();
    void testPredictiveSearchQueryFullyCachedZerosEndCase();
    void testPredictiveSearchQueryLogs();
    void testPredictiveSearchQueryLogsZeroCase();
    void testPredictiveSearchQueryLogsContactsPartialCached();
    void testPredictiveSearchQueryLogsContactsFullyCached();
    void testPredictiveSearchQueryLogsContactsZeroCase();
    void testPredictiveSearchQueryLogsContactsPhoneNumberMatch();
    void testPredictiveSearchQueryLimit();
    void testQueryOrder();
    void testContactWithSpecialChars();
    
    void testPredictiveSearchQueryZeroStart();
    void testPredictiveSearchQueryZeroStartZeroEnd();
    void testPredictiveSearchQueryZeroMiddle();
    void testPredictiveSearchQueryMultiZerosMiddle();
    void testPredictiveSearchQueryZeroMiddleLong();
    void testPredictiveSearchQueryMultiZerosAndZeroMiddle();

    
private:

    
    void createOneContact(
        QString firstname, 
        QString Lastname, 
        QString phnumber);
    QContactFilter::MatchFlags flag(int f);
    void createContacts();
    void createContactsForQueryOrder();
    void createContactsForQueryZero();
    void createContactsWithSpecialChars();
    void createHistoryEvents();
    void createLogEvent(
        QString firstname, 
        QString Lastname, 
        QString phnumber);

    bool checkHighlights( int index, int expected );
    
private:
    QContactManager *m_manager;
    LogsCntFinder   *m_finder;
    bool mCreateContacts;
};


#endif // ST_LOGSCNTFINDER_H
