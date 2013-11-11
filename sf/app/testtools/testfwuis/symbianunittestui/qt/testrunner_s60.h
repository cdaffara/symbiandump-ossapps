/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Symbian C++ test runner Class.
*
*/

#ifndef TESTRUNNER_S60_H_
#define TESTRUNNER_S60_H_

#include <e32base.h>
#include <badesca.h> 
#include <QStringList>
#include <QString>
#include <symbianunittestuicallback.h>

class CSymbianUnitTestRunner;
class WidgetTestRunner;

class TestRunnerPrivate: public CActive, public MSymbianUnitTestUiCallBack
    {
public: // Constructors and destructor

    static TestRunnerPrivate* NewLC(WidgetTestRunner* aPublicApi);
    static TestRunnerPrivate* NewL(WidgetTestRunner* aPublicApi);
    ~TestRunnerPrivate();
    
protected: // From CActive
    
    void RunL();
    void DoCancel();
    
public: // From MSymbianUnitTestUiCallBack
    
    void InfoMsg( const TDesC& aMessage );
    void InfoMsg( const TDesC& aFormat, const TDesC& aMessage );
    void TestPass(const TDesC& aTestCaseName);
    void TestFailed(const TDesC& aTestCaseName, const TDesC8& aErrMsg);
    
public:
    void RunTests();
    int SetTestDllNames(const QStringList&);
    int SetTestCaseNames(const QStringList&);
    void SetTimeout(const TInt);
    int SetOutputFormat(const QString&);
    int ListTestCases(const QString&, QStringList&);
    void SetAllocFailureSimulation(const TBool);
    
private:
    void SetTestDllNamesL(const QStringList&);
    void SetTestCaseNamesL(const QStringList&);
    void SetOutputFormatL(const QString&);
    void ListTestCasesL(const QString&, QStringList&);
    
private:
    void TestsCount();
    void TestsExecuted();

private: // Constructors

    TestRunnerPrivate(WidgetTestRunner* aPublicApi);
    void ConstructL();
    
private: // Data
    WidgetTestRunner* d_ptr;
    CSymbianUnitTestRunner* iTestRunner;
    TInt iExecutedTestCount;
    TInt iPassedTestsCount;
    TInt iFailedTestsCount;
    
    CDesCArray* iTestDllNames;
    CDesCArray* iTestCaseNames;
    
    // Settings
    TBool iAllocFailureSimulation;
    HBufC* iOutputFormat;
    TInt iTimeout;
    };

#endif /* TESTRUNNER_S60_H_ */
