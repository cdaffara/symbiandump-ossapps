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
* Description:    
*
*/


// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include <swi/sisregistrysession.h>
#include "AppRegExec_test.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAppRegExec_test::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CAppRegExec_test::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CAppRegExec_test::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CAppRegExec_test::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Positive_TestCase", CAppRegExec_test::Positive ),
        ENTRY( "Negative_TestCase", CAppRegExec_test::Negative ),
        ENTRY( "StartBackRoundApp",CAppRegExec_test::StartBackRoundApp),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


/*
 * In this Test Case we assume that the console App is running in backround and will try to copy GUI app resource 
 * registration file . So the GUI app corresponding to this resource should launch successfully.
 */
TInt CAppRegExec_test::Positive(CStifItemParser& /*aItem*/)
    {
    TInt err = KErrNone;
    
    // delete Test data from C:\logs\stopmodetestdata.txt
    CheckAndDeleteL(KTestDataSessionPath,KTestDataFile);
    
    // Start unRegistration Process & wait till it get exit/terminated.
    StartProcess(KAppUnRegisterEXE,ETrue);
        
    // delete Resource Registration files
    CheckAndDeleteL(KResourceSessionPath,KResRegFile);
    
    //Transfer the Resource Registration files
    TInt ret = BaflUtils::CopyFile(iFsSession,KTestDataRSC_SRC,KTestDataRSC_DST);
    
    // wait some time till this resource get registered by background process
    User::After(5000000);
    
    // Start GUI app under test which will write some testdata in filesystem and exit.
    StartProcess(KTestDataEXE,ETrue);
  
    // check whether the test data written by GUI app exist .If exist then assuming that the GUI app launched 
    // Successfully else not launched.
    TBool isPresent = IsExist(KTestDataSessionPath,KTestDataFile);
    if(!isPresent)
        {
        _LIT( KLogStr02, "Has not launched Hello.exe" );
        iLog->Log( KLogStr02 );
        RDebug::Print(KLogStr02);
        return KErrGeneral;
        }
    return err;
    }

/*
 * In this Test Case,the console App is not running in backround.Will try to copy resource 
 * registration file . So the GUI app corresponding to this resource should not launch successfully.
 */
TInt CAppRegExec_test::Negative(CStifItemParser& /*aItem*/)
    {
    TInt err = KErrNone;
    
    // delete Test data from C:\logs\stopmodetestdata.txt
    CheckAndDeleteL(KTestDataSessionPath,KTestDataFile);
    
    // Start unRegistration Process & wait till it get exit/terminated.
    StartProcess(KAppUnRegisterEXE,ETrue);
    
    // delete Resource Registration files
    CheckAndDeleteL(KResourceSessionPath,KResRegFile);
    
        
    // Find the Backround Process & shut down if it is already running 
    TBool found = FindProcess(KMatchName);
    // If not found then there is no need to proceed with negative testing
    // Simply returning KErrNotFound
    if(!found)
        {
        return KErrNotFound;
        }
    
    //Transfer the Resource Registration files
    TInt ret = BaflUtils::CopyFile(iFsSession,KTestDataRSC_SRC,KTestDataRSC_DST);
    
    // Start GUI app under test which will write some testdata in filesystem and exit.
    StartProcess(KTestDataEXE,ETrue);
    
    // check whether the test data written by GUI app exist .If exist then assuming that the GUI app launched 
    // Successfully else not launched.
    TBool isPresent = IsExist(KTestDataSessionPath,KTestDataFile);
    
    if(isPresent)
        {
        _LIT( KLogStr03, "Has launched Hello.exe" );
        iLog->Log( KLogStr03 );
        RDebug::Print(KLogStr03);
        return KErrGeneral;
        }
    
    return err;
    }

/*
 * StartBackRoundApp: This Test cases should be used to launch the Backround process
 * which actually registers the GUI app.& which was shut down by Negative Test Case.
 * 
 */
TInt CAppRegExec_test::StartBackRoundApp(CStifItemParser& /*aItem*/)
    {
    return StartProcess(KAppRegisterEXE,EFalse);
    }

/*
 * CheckAndDeleteL- will takes dirPath & Filename.
 * will check whether the provided file is Valid or not. If valid then delete it.
 */
void CAppRegExec_test::CheckAndDeleteL(const TDesC16& aPath,const TDesC16& aFile)
    {
    TBuf<256> abuf;
    abuf.Copy(aPath);
    abuf.Append(aFile);
    
    TBool ret = BaflUtils::FileExists(iFsSession,abuf);
    if(ret)
        {
        _LIT( KLogStr01, "CAppRegExec_test:: File deleted" );
        iLog->Log( KLogStr01 );
        RDebug::Print(KLogStr01);
        iFsSession.Delete(abuf);
        }
    
    }

/*
 * IsExist - will takes dirPath & Filename.
 * return True if valid file else False.
 */
TBool CAppRegExec_test::IsExist(const TDesC16& aPath,const TDesC16& aFile)
    {
    TBuf<256> abuf;
    abuf.Copy(aPath);
    abuf.Append(aFile);
    
    TBool ret = BaflUtils::FileExists(iFsSession,abuf);
    _LIT( KLogStr04, "CAppRegExec_test:: IsExist -FileExists bool = %d" );
    RDebug::Print(KLogStr04,ret);
    iLog->Log(KLogStr04,ret); 
    return ret;
    }


/*
 * StartProcess - wil be used to launch any application.
 */
TInt CAppRegExec_test::StartProcess(const TDesC16& aFileName,TBool wait)  // Filename with drive and path
    {
    RProcess proc;
    TInt err = KErrNone;
    err = proc.Create(aFileName, KNullDesC);
    
    if(wait)
        {
        TRequestStatus stat;
        proc.Logon(stat);
        proc.Resume(); 
        // Thread is executing. Can add code here to run in parallel... 
        // blocks here while process is running
        User::WaitForRequest(stat);
        // Process is ended, you can use proc.ExitType()
        proc.Close();
        }
    else{
        proc.Resume();
        proc.Close();
        }
    return err;
    }

/*
 * FindProcess - This function searches the given process . 
 * it will shutdown the process if running in backround.
 */
TBool CAppRegExec_test::FindProcess(const TDesC16& aPath)
    {
    RProcess procHandle;
    TFindProcess procFinder;
    TFullName result;
    TBool found = EFalse;
    while(procFinder.Next(result)== KErrNone)
        {
        //Checking for AppRegExec.exe process with its length(14). 
        //Since the TFindProcess conatins the process Name + some orbitary values
        //associated with it.something like <ProcessName>[0X234567]

        if(result.Left(14) == aPath)
            {
            
            RDebug::Print(_L("**** Found the Process ****"));
            RDebug::Print(result);
            TInt err = procHandle.Open(procFinder,EOwnerThread);
            if(err == KErrNone)
                {
                procHandle.Kill(0);
                found = ETrue;
                break;
                }
            }
        }
    procHandle.Close();
    return found;
    }




//  [End of File] - Do not remove

// End of File
