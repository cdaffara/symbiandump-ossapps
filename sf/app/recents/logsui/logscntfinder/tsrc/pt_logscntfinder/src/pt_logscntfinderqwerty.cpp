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
#include "pt_logscntfinderqwerty.h"
#include "pt_logscntfinder.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>


void PtTest::execute( QContactManager& manager )
{

    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    QString pattern = mPattern + QChar(30) + QString("vqwerty");
    df.setValue( pattern );
    
    QTime t;
    t.start();
    
    QList<QContactLocalId> cnt_ids = manager.contactIds( df );
    mSamples.append( t.elapsed());
    mResults += cnt_ids.count();
    
}

void PT_LogsCntFinderQwerty::initTestCase()
{
}

void PT_LogsCntFinderQwerty::cleanupTestCase()
{
      
}


void PT_LogsCntFinderQwerty::init()
{   
    qDebug() << "PT_LogsCntFinderQwerty::init start";
    //open symbian database
    m_manager = new QContactManager("symbian");
    
    QList<QContactLocalId> cnt_ids = m_manager->contactIds();
    qDebug() << "contacts now in db" << cnt_ids.count();
    qDebug() << "PT_LogsCntFinderQwerty::init end";
}

void PT_LogsCntFinderQwerty::cleanup()
{
    delete m_manager;
    m_manager = 0;
}



void PT_LogsCntFinderQwerty::statistics()
{
    qDebug() << "=> PT_LogsCntFinderQwerty::statistics";
    int results = 0;
    int queries = 0;
    int ind = mSamples; //samples
    QTime t;

    QList<PtTest> tests;
    tests.append( PtTest("a") );
    tests.append( PtTest("5") );
    tests.append( PtTest("Paul") );
    tests.append( PtTest("Paul sw") );
    tests.append( PtTest("P S") );
   
    t.start();
    
    while( ind > 0 ) {
        for( int j = 0; j < tests.length(); j++ ) {
            tests[j].execute( *m_manager );
            queries++;
        }
        ind--;
    }
    
    int totalTime = t.elapsed();
    
    
    for( ind = 0; ind < tests.length(); ind++ ) {
        tests[ind].calculateMean();
        tests[ind].calculateVariance();
        tests[ind].calculateDeviation();
        results += tests[ind].mResults;
    }
    
    qDebug("-- Statistics --");
    qDebug("%d queries executed with %d results in %d ms.",
            queries, results, totalTime );
    qDebug() << PtTest::statHeader();
    for( ind = 0; ind < tests.length(); ind++ ) {
        tests[ind].print();
    }
    
    qDebug() << "<= PT_LogsCntFinderQwerty::statistics";
}

