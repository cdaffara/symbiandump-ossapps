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
#ifndef UT_LOGSCONTACT_H
#define UT_LOGSCONTACT_H

#include <QObject>
#include <QList>

class LogsContact;
class LogsEvent;
class LogsDbConnector;

class UT_LogsContact : public QObject                 
{
     Q_OBJECT
    
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
     
    void testConstructor();
	void testAllowedRequestType();
	void testOpen();
	void testAddNew();
	void testUpdateExisting();
	void testIsContactInPhonebook();
    void testIsContactRequestAllowed();
	void testHandleRequestCompeted();
	void testCancelServiceRequest();
    
private:
 
    LogsContact* mLogsContact; 
    LogsEvent* mLogsEvent;

    QList<LogsEvent*> mEvents;
    LogsDbConnector* mDbConnector;
};


#endif //UT_LOGSCALL_H
