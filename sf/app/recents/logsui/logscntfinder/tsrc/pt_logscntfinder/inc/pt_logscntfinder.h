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
#ifndef PT_LOGSCNTFINDER_H
#define PT_LOGSCNTFINDER_H

#include <QObject>
#include <qcontactmanager.h>

QTM_USE_NAMESPACE
class LogsCntFinder;

class PtTest
{
public:
    
    PtTest( const QString& name );
    void execute( LogsCntFinder& engine );
    void execute( QContactManager& manager );
    void calculateMean();
    void calculateVariance();
    void calculateDeviation();
    void print();
    static QString statHeader(); 
    
public:
    
    QString mPattern;
    QList<int> mSamples;
    float mMean;
    float mVariance;
    float mStdDeviation;
    int mMin;
    int mMax;
    int mMinSample;
    int mMaxSample;
    int mResults;

};



class PT_LogsCntFinder : public QObject                 
{
     Q_OBJECT
public:
     
     inline PT_LogsCntFinder( int samples ) : mSamples(samples) {}
     
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
     
    void testPredictiveQuery();
    void testExtendedQuery();
    void testDiffHitQuery();
    void statistics();

private:
    void createContact_one_Contact(QString& firstname, QString& Lastname, QString phnumber);
    
private:
    
    QContactManager             *m_manager;
    LogsCntFinder               *m_finder;
    int                         mSamples;
};


#endif //PT_LOGSCNTFINDER_H
