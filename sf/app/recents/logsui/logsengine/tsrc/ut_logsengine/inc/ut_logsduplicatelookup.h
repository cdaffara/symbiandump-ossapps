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
#ifndef UT_LOGSDUPLICATELOOKUP_H  
#define UT_LOGSDUPLICATELOOKUP_H  

#include <QObject>

class LogsDuplicateLookup;


class UT_LogsDuplicateLookup  : public QObject
{
Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
    void init();
    void cleanup();
	  
private slots: //test methods    

	void testConstructor();
	void testInvalidate();
	void testCleanup();
	void testAddLookupEntry();
	void testFindDuplicate();
	
private:
	
	void createTestData();
	
private:
	
    LogsDuplicateLookup* mLookup;
  
};

#endif //UT_LOGSDUPLICATELOOKUP_H  
