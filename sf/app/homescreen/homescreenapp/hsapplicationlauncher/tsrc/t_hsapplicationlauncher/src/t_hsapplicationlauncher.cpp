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
* Description:  Main test class for hsapplicationlauncher library.
*
*/

#include <e32base.h>
#include <e32property.h>
#include "t_hsapplicationlauncher.h"
#include "hsapplicationlauncher.h"

const TInt KPSCategoryUid(0x20022F36);
const TInt KPSCrashCountKey(1);


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::initTestCase()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::cleanupTestCase()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testActivate()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();        
    delete hsApplicationLauncher;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testRunL()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();     
    hsApplicationLauncher->RunL();
    delete hsApplicationLauncher;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testInitProcessMonitorL()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->InitProcessMonitorL(0);
    QVERIFY(hsApplicationLauncher->iProcessMonitor);
    hsApplicationLauncher->InitProcessMonitorL(0);
    QVERIFY(hsApplicationLauncher->iProcessMonitor);
    delete hsApplicationLauncher;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testDoCancel()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->InitProcessMonitorL(0);    
    hsApplicationLauncher->DoCancel();
    delete hsApplicationLauncher;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testShutdownApp()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->ShutdownApp(KErrGeneral);
    QVERIFY(hsApplicationLauncher->iApplicationReturnValue == KErrGeneral);
    QCOMPARE(hsApplicationLauncher->ApplicationReturnValue(), KErrGeneral);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testRunError()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->RunError(KErrGeneral);
    QVERIFY(hsApplicationLauncher->iApplicationReturnValue == KErrGeneral);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testListeningLanguageSelection()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->StartListeningLanguageSelectionL();
    QVERIFY(hsApplicationLauncher->iCenRepNotifyHandler);
    hsApplicationLauncher->StopListeningLanguageSelection();
    QVERIFY(!hsApplicationLauncher->iCenRepNotifyHandler);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testHandleNotifyInt()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->StartListeningLanguageSelectionL();
    hsApplicationLauncher->HandleNotifyInt(0x00000007, 1);
    QVERIFY(!hsApplicationLauncher->iCenRepNotifyHandler);
    hsApplicationLauncher->HandleNotifyInt(0x00000008, 1);
    QVERIFY(!hsApplicationLauncher->iCenRepNotifyHandler);    
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testStartHomeScreen()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->StartHomeScreen();
    QVERIFY(hsApplicationLauncher->iProcessMonitor);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testProcessEnded()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);

    TExitCategoryName panicString;
    hsApplicationLauncher->ProcessEnded(EExitPanic, KErrNone, panicString);
    TInt crashCount = 0;
    RProperty::Get(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, crashCount);
    QVERIFY(crashCount == 1);

    if(hsApplicationLauncher->IsActive())
    {
        hsApplicationLauncher->Cancel();
    }

    hsApplicationLauncher->ProcessEnded(EExitKill, KErrGeneral, panicString);
    RProperty::Get(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, crashCount);
    QVERIFY(crashCount == 2);    

    if(hsApplicationLauncher->IsActive())
    {
        hsApplicationLauncher->Cancel();
    }

    hsApplicationLauncher->ProcessEnded(EExitTerminate, KErrGeneral, panicString);
    RProperty::Get(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, crashCount);
    QVERIFY(crashCount == 2);
    
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testProcessMonitoringError()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->ProcessMonitoringError(KErrGeneral);
    QVERIFY(hsApplicationLauncher->iApplicationReturnValue == KErrGeneral);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testSystemShutdownEvent()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->SystemShutdownEvent();    
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testIsLanguageSelectedL()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->StartListeningLanguageSelectionL();
    TBool languageSelected = ETrue;
    TRAP(errNo, languageSelected = hsApplicationLauncher->IsLanguageSelectedL());
    QCOMPARE(errNo, KErrNone);
#if defined( __WINS__ )    
    QVERIFY(languageSelected == EFalse);
#else
    QVERIFY(languageSelected);
#endif
    hsApplicationLauncher->StopListeningLanguageSelection();
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testProcessMonitorRunL()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->InitProcessMonitorL(0);
    hsApplicationLauncher->iProcessMonitor->RunL();
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testProcessMonitorDoCancel()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->InitProcessMonitorL(0);    
    hsApplicationLauncher->iProcessMonitor->DoCancel();
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testProcessMonitorRunError()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    hsApplicationLauncher->InitProcessMonitorL(0);  
    hsApplicationLauncher->iProcessMonitor->RunError(KErrGeneral);
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testShutDownMonitorRunL()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    QVERIFY(hsApplicationLauncher->iShutdownMonitor);
    if(hsApplicationLauncher->iShutdownMonitor->IsActive())
    {
        hsApplicationLauncher->iShutdownMonitor->Cancel();
    }
    hsApplicationLauncher->iShutdownMonitor->RunL();
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testShutDownMonitorDoCancel()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    QVERIFY(hsApplicationLauncher->iShutdownMonitor);
    hsApplicationLauncher->iShutdownMonitor->DoCancel();
    delete hsApplicationLauncher;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsapplicationlauncher::testShutDownMonitorRunError()
{
    CHsLaunch *hsApplicationLauncher = 0;
    TRAPD(errNo, hsApplicationLauncher = CHsLaunch::NewL());    
    QCOMPARE(errNo, KErrNone);
    hsApplicationLauncher->Activate();
    QVERIFY(hsApplicationLauncher->iShutdownMonitor);
    errNo = hsApplicationLauncher->iShutdownMonitor->RunError(KErrGeneral);
    QCOMPARE(errNo, KErrGeneral);
    delete hsApplicationLauncher;
}

QTEST_MAIN(t_hsapplicationlauncher)
