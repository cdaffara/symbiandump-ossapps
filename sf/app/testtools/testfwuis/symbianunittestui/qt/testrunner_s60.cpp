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
#include "testrunner_s60.h"
#include "symbianunittestrunner.h"
#include "widgettestrunner.h"
_LIT( KOutputFileName, "SymbianUnitTestResults" );
_LIT( KDefaultOutputFormat, "html" );

TestRunnerPrivate* TestRunnerPrivate::NewLC(WidgetTestRunner* aPublicApi)
    {
    TestRunnerPrivate* self = new (ELeave) TestRunnerPrivate(aPublicApi);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

TestRunnerPrivate* TestRunnerPrivate::NewL(WidgetTestRunner* aPublicApi)
    {
    TestRunnerPrivate* self = TestRunnerPrivate::NewLC(aPublicApi);
    CleanupStack::Pop(self);
    return self;
    }

TestRunnerPrivate::TestRunnerPrivate(WidgetTestRunner* aPublicApi) :
    CActive(EPriorityStandard), d_ptr(aPublicApi), iTimeout(
            KSymbianUnitTestDefaultTimeout)
    {
    }

void TestRunnerPrivate::ConstructL()
    {
    User::SetJustInTime(EFalse); // Do not stop on test case panics

    iTestDllNames = new (ELeave) CDesCArrayFlat(1);
    iTestCaseNames = new (ELeave) CDesCArrayFlat(1);
    iOutputFormat = KDefaultOutputFormat().AllocL();
    CActiveScheduler::Add(this);
    }

TestRunnerPrivate::~TestRunnerPrivate()
    {
    Cancel();
    delete iTestDllNames;
    delete iTestCaseNames;
    delete iOutputFormat;
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void TestRunnerPrivate::InfoMsg(const TDesC& aMessage)
    {
    QString msg = QString::fromUtf16(aMessage.Ptr(), aMessage.Length());
    emit d_ptr->infoMsgReceived(msg);
    }

int TestRunnerPrivate::SetTestDllNames(const QStringList& aDllNames)
    {
    TRAPD(err, SetTestDllNamesL(aDllNames));
    return err;
    }

int TestRunnerPrivate::SetTestCaseNames(const QStringList& aCaseNames)
    {
    TRAPD(err, SetTestCaseNamesL(aCaseNames));
    return err;
    }

int TestRunnerPrivate::SetOutputFormat(const QString& aFmt)
    {
    TRAPD(err, SetOutputFormatL(aFmt));
    return err;
    }

int TestRunnerPrivate::ListTestCases(const QString& aDllName, QStringList& aCaseNames)
    {
    TRAPD(err, ListTestCasesL(aDllName, aCaseNames));
    return err;
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void TestRunnerPrivate::InfoMsg(const TDesC& aFormat, const TDesC& aMessage)
    {
    //    iConsole->Printf( aFormat, &aMessage );
    }

void TestRunnerPrivate::TestsCount()
    {

    }

void TestRunnerPrivate::TestsExecuted()
    {
    emit d_ptr->testExecuted(iPassedTestsCount + iFailedTestsCount);
    }

void TestRunnerPrivate::SetTestDllNamesL(const QStringList& aDllNames)
    {
    if (!aDllNames.isEmpty())
        {
        CDesCArray* testDllNames = new (ELeave) CDesCArrayFlat(1);
        CleanupStack::PushL(testDllNames);
        foreach (QString tmp, aDllNames)
                {
                TPtrC myDescriptor(static_cast<const TUint16*> (tmp.utf16()),
                        tmp.length());
                HBufC* buffer = myDescriptor.AllocL();
                testDllNames->AppendL(buffer->Des());
                }
        delete iTestDllNames;
        iTestDllNames = testDllNames;
        CleanupStack::Pop(testDllNames);
        }
    }

void TestRunnerPrivate::SetTestCaseNamesL(const QStringList& aNames)
    {
    if (!aNames.isEmpty())
        {
        CDesCArray* testCaseNames = new (ELeave) CDesCArrayFlat(1);
        CleanupStack::PushL(testCaseNames);
        foreach (QString tmp, aNames)
                {
                TPtrC myDescriptor(static_cast<const TUint16*> (tmp.utf16()),
                        tmp.length());
                HBufC* buffer = myDescriptor.AllocL();
                testCaseNames->AppendL(buffer->Des());
                }
        delete iTestCaseNames;
        iTestCaseNames = testCaseNames;
        CleanupStack::Pop(testCaseNames);
        }
    }

void TestRunnerPrivate::SetAllocFailureSimulation(const TBool aSimulation)
    {
    iAllocFailureSimulation = aSimulation;
    }

void TestRunnerPrivate::SetTimeout(const TInt aTimeout)
    {
    iTimeout = aTimeout;
    }

void TestRunnerPrivate::SetOutputFormatL(const QString& aFormat)
    {
    TPtrC outputFormatPtr(aFormat.utf16());
    HBufC* outputFormat = outputFormatPtr.AllocL();
    delete iOutputFormat;
    iOutputFormat = outputFormat;
    }

void TestRunnerPrivate::RunL()
    {
    iTestRunner = CSymbianUnitTestRunner::NewL(*this);
    iPassedTestsCount = 0;
    iFailedTestsCount = 0;
    TRAPD(err, iTestRunner->ExecuteTestsL(*iTestDllNames,
                    iAllocFailureSimulation,
                    KOutputFileName,
                    *iOutputFormat,
                    *iTestCaseNames,
                    iTimeout));
    if (err != KErrNone)
        {
        emit d_ptr->errorOccurred(err);
        }
    delete iTestRunner;
    iTestRunner = NULL;
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//
void TestRunnerPrivate::DoCancel()
    {
    }

void TestRunnerPrivate::RunTests()
    {
    // Set ready to run immediately
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
    if (!IsActive())
        {
        SetActive();
        }
    }

void TestRunnerPrivate::ListTestCasesL(const QString& aDllName,
        QStringList& aTestCases)
    {
    iTestRunner = CSymbianUnitTestRunner::NewL(*this);
    TPtrC dllName(static_cast<const TUint16*> (aDllName.utf16()),
            aDllName.length());
    CDesCArray* testCaseNames = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(testCaseNames);
    iTestRunner->TestCaseNamesL(dllName, *testCaseNames);
    for (int i = 0; i < testCaseNames->Count(); ++i)
        {
        aTestCases << QString::fromUtf16((*testCaseNames)[i].Ptr(),
                (*testCaseNames)[i].Length());
        }
    CleanupStack::PopAndDestroy(testCaseNames);
    delete iTestRunner;
    iTestRunner = NULL;
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void TestRunnerPrivate::TestPass(const TDesC& aTestCaseName)
    {
    QString caseName = QString::fromUtf16(aTestCaseName.Ptr(),
            aTestCaseName.Length());
    emit
    d_ptr->infoMsgReceived(caseName);
    emit
    d_ptr->testPassed(++iPassedTestsCount);
    TestsExecuted();
    }

// -----------------------------------------------------------------------------
// From MSymbianUnitTestUiCallBack
// -----------------------------------------------------------------------------
//
void TestRunnerPrivate::TestFailed(const TDesC& aTestCaseName,
        const TDesC8& aErrMsg)
    {
    QString caseName = QString::fromUtf16(aTestCaseName.Ptr(),
            aTestCaseName.Length());
    QString reason = QString::fromUtf8(
            reinterpret_cast<const char*> (aErrMsg.Ptr()), aErrMsg.Length());
    emit
    d_ptr->testFailed(caseName, reason, ++iFailedTestsCount);
    TestsExecuted();
    }
