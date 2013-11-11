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

#ifndef TESTMSGINIT_H_
#define TESTMSGINIT_H_

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT 
#endif

// INCLUDES
#include <QtTest/QtTest>

// FORWARD DECLARATIONS
class MsgSimNumDetector;
class CMsvOperation;
class COutboxObserver;
class CMobileSignalStrengthHandler;
class CTestMsgSimOperation;

class TEST_EXPORT TestMsgInit: public QObject
{
    Q_OBJECT
    
private slots:

    /**
     * Intializes component for testing
     */
    void initTestCase();

    /**
     * Initilazes the test case data to be executed
     */
    void init();
    
    void validateMobileSignalStrength();
    
    void validateOutboxSenderOperation();
    
    void validateOutboxObserver();
    
    void validateSimNumDetector();    
    /**
     * Clean the test case data
     */
    void cleanup();

    /**
     * Delete the initialized component for testing
     */
    void cleanupTestCase();
    
private: 
    MsgSimNumDetector* iSimHandler;
    CMsvOperation* iMsvOperation;
    COutboxObserver* iOutBoxObserver;
    CTestMsgSimOperation* iTestMsgSimOperation;
    CMobileSignalStrengthHandler* iSignalStrengthHandler;

};
#endif /* TESTMSGINIT_H_ */
