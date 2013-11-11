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

#include <coemain.h>
#include <eikenv.h>
#include <pathinfo.h> 
#include <apgtask.h>
#include <bautils.h>
 
#include "launchertraces.h"
#include "e32image.h"
#include "launcherxmlparser.h"
#include "launcherdllparser.h"
#include "launcherengine.h"

_LIT(KLogFileName, "LauncherLog.txt");
_LIT(KBCLogFileName, "LauncherBCLog.txt");
_LIT(KSystemDllsFileName, "SystemDlls.txt");
_LIT(KRequiredDllsFileName, "RequiredDlls.xml");
_LIT(KDotXML,".xml");
_LIT(KDotLauncherXML,".launcherxml");

_LIT(KFileSeparator, "\t");
_LIT(KFileNewLine, "\r\n");

_LIT(KNewLine, "\n");

const TInt KMaxAppsArraySize=250;
const TInt KMaxDllArraySize=5000;
const TInt KLauncherLogBufferSize = 4096;

// After this many issues, issues are buffered and printed 
// in the end of analysis
const TInt KBigBufferUsageThreshold=10;

// Buffer allocation unit
const TInt KBigBufferAllocBytes=1024;

// ---------------------------------------------------------------------------

CLauncherEngine* CLauncherEngine::NewL(MLauncherUI *aLauncherUI)
    {
    CLauncherEngine* self = new(ELeave) CLauncherEngine;
    CleanupStack::PushL(self);
    self->ConstructL(aLauncherUI);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CLauncherEngine::CLauncherEngine() : CActive(EActivePriorityIpcEventsHigh)
    {
    }

// ---------------------------------------------------------------------------

template <typename T>
void AppendLogBufferL(const TDesC& aText, T*& aBuf, TInt aIncreaseStep = KLauncherLogBufferSize)
    {
    if( aBuf == 0 )
        {
        aBuf = T::NewL(aIncreaseStep);
        }
    
    TInt currentMaxLength = aBuf->Des().MaxLength();
    if( aBuf->Des().Length() + aText.Length() > currentMaxLength )
        {
        TInt increaseSize = Max(aText.Length(), aIncreaseStep);                
        aBuf = aBuf->ReAllocL(aBuf->Des().MaxLength() + increaseSize );
        }
    aBuf->Des().Append(aText);        
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::ConstructL(MLauncherUI *aLauncherUI)
    {
    LOGSTRING("Launcher: CLauncherEngine::ConstructL");
    
    User::LeaveIfNull( aLauncherUI );
    
    iLauncherUI = aLauncherUI;
    

    iEnv = CEikonEnv::Static();
    iLaunchingIsActive = EFalse;
    iDLLAnalysisIsActive = EFalse;
    iSkipHiddenAndEmbedOnly = ETrue;

    User::LeaveIfError(iTimer.CreateLocal());

    User::LeaveIfError(iLs.Connect());
    User::LeaveIfError(iWs.Connect());

    iAppThreadChecker = CAppThreadChecker::NewL(this);
    iAppRunningChecker = CAppRunningChecker::NewL(this);

    iAllAppsArray = new(ELeave) CDesCArrayFlat(KMaxAppsArraySize);
    iAppsArray = new(ELeave) CDesCArrayFlat(KMaxAppsArraySize);
    iSystemDllArray = new(ELeave) CDesCArrayFlat(KMaxDllArraySize);

    iLogWriteBuf = HBufC8::NewL(KLauncherLogBufferSize);

    iLogFilePath = PathInfo::PhoneMemoryRootPath();
    iLogFilePath.Append( KLogFileName );
    
    iBCLogFilePath = PathInfo::PhoneMemoryRootPath();
    iBCLogFilePath.Append( KBCLogFileName );

    iSystemDllsFilePath = PathInfo::PhoneMemoryRootPath();
    iSystemDllsFilePath.Append( KSystemDllsFileName );

    iRequiredDllsFilePath = PathInfo::PhoneMemoryRootPath();
    iRequiredDllsFilePath.Append( KRequiredDllsFileName );
            
    iDLLParser = CLauncherDLLParser::NewL();

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------

CLauncherEngine::~CLauncherEngine()
    {
    LOGSTRING("Launcher: CLauncherEngine::~CLauncherEngine");

    Cancel();

    // close the log
    iLogFile.Close();
    iBCLogFile.Close();
    if (iLogWriteBuf)
        delete iLogWriteBuf;

    if (iAppRunningChecker)
        delete iAppRunningChecker;
    
    if (iAppThreadChecker)
        delete iAppThreadChecker;

    if (iSystemDllArray)
        {
        iSystemDllArray->Reset();
        delete iSystemDllArray;
        }

    if (iAppsArray)
        {
        iAppsArray->Reset();
        delete iAppsArray;
        }

    
    if (iAllAppsArray)
        {
        iAllAppsArray->Reset();
        delete iAllAppsArray;
        }
    
    delete iXMLParser;
   // delete iDLLParser;
    //delete iDLLElement;
    delete iBCIssuesBigBuffer;            
    
    iWs.Close();
    iLs.Close();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::DoCancel()
    {
    LOGSTRING("Launcher: CLauncherEngine::DoCancel");
    
    if( iXMLParser )
        {
        iXMLParser->Cancel();
        }
    iTimer.Cancel();
    
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::RunL()
    {
    
    LOGSTRING("Launcher: CLauncherEngine::RunL");  
    TInt err = iStatus.Int();
    User::LeaveIfError(err);
    
    // write full app path to the log file
    WriteInitialStuffToTheLogL(iAppsArray->MdcaPoint(iAppLaunchCounter), iLogFile);

    // get the uid of the current app
    iCurrentAppUid = KNullUid;
    TApaAppInfo appInfo;
    iLs.GetAllApps();
    while (iLs.GetNextApp(appInfo) == KErrNone)
        {
        if (appInfo.iFullName.CompareF( iAppsArray->MdcaPoint(iAppLaunchCounter) ) == 0)
            {
            iCurrentAppUid = appInfo.iUid;
            break;
            }
        }

    if (iCurrentAppUid == KNullUid)
        {        
        iLogWriteBuf->Des().Append(_L("[WARN: App has no UID] "));
        }

    // parse the filename
    TParse nameParser;
    nameParser.SetNoWild(iAppsArray->MdcaPoint(iAppLaunchCounter), NULL, NULL);
    iCurrentAppNameAndExt.Copy(nameParser.Drive());
    iCurrentAppNameAndExt.Append(nameParser.NameAndExt());

    // do not try to launch these apps
    if (iAppsArray->MdcaPoint(iAppLaunchCounter).FindF(_L("\\Launcher.")) != KErrNotFound
        || iAppsArray->MdcaPoint(iAppLaunchCounter).FindF(_L("\\Phone.")) != KErrNotFound
        || iAppsArray->MdcaPoint(iAppLaunchCounter).FindF(_L("\\Startup.")) != KErrNotFound  
        || iAppsArray->MdcaPoint(iAppLaunchCounter).FindF(_L("\\SplashScreen.")) != KErrNotFound  
        || iAppsArray->MdcaPoint(iAppLaunchCounter).FindF(_L("\\eshell.")) != KErrNotFound)  
        {

        iLauncherUI->PrintText(iCurrentAppNameAndExt);
        iLauncherUI->PrintText(_L(": SKIPPED automatically\n"));
        
        iLogWriteBuf->Des().Append(_L("[SKIPPED automatically] "));
        iLogWriteBuf->Des().Append(KFileNewLine);
        iLogFile.Write(iLogWriteBuf->Des());

        iSkippedCases++;
        CheckForMoreAppsL();
        }

    else
        {

        // dependency check not needed if the app is in the ROM/ROFS, because they'll be checked automatically
        // when the rom image is built
        
        if (iCurrentAppNameAndExt[0] != 'Z' && iCurrentAppNameAndExt[0] != 'z')
            {

            if (iCurrentAppNameAndExt[2] == '[')  // this is quite likely a Java application, no dependency test needed 
                {
                iLogWriteBuf->Des().Append(_L("[Dependency check not done] "));
                }
            else
                {  // otherwise check depencies
            
                #ifdef __WINS__
                
                    // emulator not supported

                #else

                    // ELF binaries

                    CDesCArray* missingDllArray = NULL;

                    TRAPD(err, missingDllArray = DependencyCheckForE32ImageL());

                    // some error happened while processing the E32 image
                    if (err != KErrNone)
                        {

                        iLauncherUI->PrintText(iCurrentAppNameAndExt);
                        iLauncherUI->PrintText(_L(": unable to read import table!\n"));
                        
                        iLogWriteBuf->Des().Append(_L("[Unable to read import table!] "));
                        }

                    // print missing dependencies
                    else if (err==KErrNone && missingDllArray)
                        {
                        if (missingDllArray->MdcaCount() > 0)
                            {
                            iLauncherUI->PrintText(iCurrentAppNameAndExt);
                            iLauncherUI->PrintText(_L(": missing dependencies: "));
                            
                            iLogWriteBuf->Des().Append(_L("[Missing dependencies: "));

                            for (TInt k=0; k<missingDllArray->MdcaCount(); k++)
                                {
                                iLauncherUI->PrintText(missingDllArray->MdcaPoint(k));
                                iLauncherUI->PrintText(_L(" "));
                                
                                iLogWriteBuf->Des().Append(missingDllArray->MdcaPoint(k));
                                iLogWriteBuf->Des().Append(_L(" "));
                                }

                            iLauncherUI->PrintText(_L("\n"));

                            iLogWriteBuf->Des().Append(_L("] "));
                            }
                        }

                    if (missingDllArray)
                        delete missingDllArray;

                #endif

                }  // if '['
            }  // if 'Z'



        // check if the app is already running
        TApaTaskList taskList(iWs);
        TApaTask thisTask = taskList.FindApp(iCurrentAppUid);
        if (thisTask.Exists())
            {
            iLogWriteBuf->Des().Append(_L(" [OK: App already running]"));
            iLogWriteBuf->Des().Append(KFileNewLine);
            iLogFile.Write(iLogWriteBuf->Des());

            iOkCases++;
            CheckForMoreAppsL();
            }
        
        else
            {
            // check the program's capabilities
            TApaAppCapabilityBuf buf;
            iLs.GetAppCapability(buf, iCurrentAppUid);
            TApaAppCapability cap = buf();

            // if it's embeddable only, don't launch if setting is enabled
            if (cap.iEmbeddability == TApaAppCapability::EEmbeddableOnly && SkipHiddenAndEmbedOnly())  
                {
              
                iLauncherUI->PrintText(iCurrentAppNameAndExt);
                iLauncherUI->PrintText(_L(": SKIPPED: embeddable only\n"));

                iLogWriteBuf->Des().Append(_L("[SKIPPED: embeddable only] "));
                iLogWriteBuf->Des().Append(KFileNewLine);
                iLogFile.Write(iLogWriteBuf->Des());

                iSkippedCases++;
                CheckForMoreAppsL();
                }
        
            // if it's hidden, don't launch if setting is enabled
            else if (cap.iAppIsHidden && SkipHiddenAndEmbedOnly())  
                {
                
                iLauncherUI->PrintText(iCurrentAppNameAndExt);
                iLauncherUI->PrintText(_L(": SKIPPED: hidden\n"));
                
                iLogWriteBuf->Des().Append(_L("[SKIPPED: hidden] "));
                iLogWriteBuf->Des().Append(KFileNewLine);
                iLogFile.Write(iLogWriteBuf->Des());

                iSkippedCases++;
                CheckForMoreAppsL();
                }

            // otherwise do the normal launch test
            else
                {
                LaunchApplicationL();
                }
            
            } //if (thisTask.Exists())
        
        } //if (iAppsArray->MdcaPoint(iAppLaunchCounter).FindF
        
    }

// ---------------------------------------------------------------------------

CDesCArray* CLauncherEngine::ListOfAllAppsL()
    {
    LOGSTRING("Launcher: CLauncherEngine::ListOfAllAppsL");

    // find all DLLs from the system
    iSystemDllArray->Reset();
    TRAP_IGNORE( FindFiles(_L("*.dll"), _L("\\sys\\bin\\")) );

    // write the list of DLLs to a file
    RFile dllFile;
    if (dllFile.Replace(iEnv->FsSession(), iSystemDllsFilePath, EFileWrite) == KErrNone)
        {
        TBuf8<KMaxFileName> dllName;

        for (TInt i=0; i<iSystemDllArray->MdcaCount(); i++)
            {
            dllName.Copy( iSystemDllArray->MdcaPoint(i) );
            dllName.Append( KFileNewLine );

            dllFile.Write( dllName );
            }

        dllFile.Close();
        }

    // reset the apps list
    iAllAppsArray->Reset();

    // search all apps
    TApaAppInfo appInfo;
    iLs.GetAllApps();

    while (iLs.GetNextApp(appInfo) == KErrNone)
        {
        iAllAppsArray->AppendL(appInfo.iFullName);
        }


    // remove launcher.app / launcher.exe from the list
    for (TInt i=0; i<iAllAppsArray->MdcaCount(); i++)
        {
        if (iAllAppsArray->MdcaPoint(i).FindF(_L("\\Launcher.")) != KErrNotFound)
            {
            iAllAppsArray->Delete(i);
            iAllAppsArray->Compress();
            break;
            }
        }

    // sort the elements
    iAllAppsArray->Sort();

    return iAllAppsArray;
    }

// ---------------------------------------------------------------------------

TInt CLauncherEngine::FindFiles(const TDesC& aFileName, const TDesC& aPath)
    {
    TFindFile fileFinder(iEnv->FsSession());
    CDir* fileList; 
    TInt err = fileFinder.FindWildByDir(aFileName, aPath, fileList);

    while (err == KErrNone)
        {
        for (TInt i=0; i<fileList->Count(); i++)
            {
            TParse fullentry;
            fullentry.Set((*fileList)[i].iName, &fileFinder.File(), NULL);

            TRAP(err, iSystemDllArray->AppendL(fullentry.NameAndExt())); 
            }

        delete fileList;
        err = fileFinder.FindWild(fileList);
        }
    return err;
    }

// ---------------------------------------------------------------------------


void CLauncherEngine::StartAppLaunchingL(const CArrayFix<TInt>* aSelectedApps, TBool aAutoClose)
    {
    LOGSTRING("Launcher: CLauncherEngine::StartAppLaunchingL");

    // check that we have something to launch
    if (aSelectedApps->Count() <= 0)
        {
        _LIT(message, "Nothing selected");
        iLauncherUI->ShowErrorMessage(message);
        }
    else
        {
        // update the list of applications to be tested
        iAppsArray->Reset();

        TInt ref(0);
        TKeyArrayFix key(0, ECmpTUint16);
        TInt index(0);

        for (TInt i=0; i<iAllAppsArray->MdcaCount(); i++)
            {
            ref = i;

            // if the application is selected, append it to the apps array
            if (aSelectedApps->Find(ref, key, index) == 0)  
                {
                iAppsArray->AppendL(iAllAppsArray->MdcaPoint(i));
                }
            }


        // to make sure that our algorithm works correctly
        if (iAppsArray->MdcaCount() != aSelectedApps->Count())
            {
            _LIT(message, "Something went wrong...");
            iLauncherUI->ShowErrorMessage(message);
            }

        
        // init
        Cancel();
        iLaunchingIsActive = ETrue;
        iAutoClose = aAutoClose;
        iAppLaunchCounter = 0;
        iFailedCases = 0;
        iOkCases = 0;
        iSkippedCases = 0;
        iTotalNumberOfCases = iAppsArray->MdcaCount();
        iCurrentAppUid = KNullUid;

        // open the log file for writing
        if (iLogFile.Open(iEnv->FsSession(), iLogFilePath, EFileWrite) != KErrNone)
            {
            iEnv->FsSession().MkDirAll(iLogFilePath);
            iLogFile.Replace(iEnv->FsSession(), iLogFilePath, EFileWrite);
            }
        else
            {
            // file opens correctly, seek to the end
            TInt fileSize=0;
            iLogFile.Size(fileSize);
            iLogFile.Seek(ESeekCurrent, fileSize);
            }

        // change focus to output view!!
        iLauncherUI->ChangeFocusToOutputView();

        //text to log
        iLauncherUI->PrintText(_L("New test started.\n"));

        // start the first launch!
        IssueLaunch();

        }
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::IssueLaunch()
    {
    LOGSTRING("Launcher: CLauncherEngine::IssueLaunch");

    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("Timing error?"), 100));

    // this should keep the backlight on
    User::ResetInactivityTime();

    iDLLAnalysisIsActive = EFalse;
    // some delay
    iTimer.After(iStatus, 1000000);    
    SetActive();
    }

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------

void CLauncherEngine::WriteInitialStuffToTheLogL(const TDesC& aOwnData, RFile& aFile)
    {
    LOGSTRING("Launcher: CLauncherEngine::WriteInitialStuffToTheLog");

    TTime time;
    time.HomeTime();
    TBuf<32> currentTime;
    TBuf<32> currentDate;

    // current date            
    _LIT(KCurrentDate,"%D%M%Y%/0%1%/1%2%/2%3%/3");
    time.FormatL(currentDate, KCurrentDate);    
    iLogWriteBuf->Des().Copy(currentDate);    
    AppendLogBufferL(KFileSeparator(), iLogWriteBuf);

    // current time
    _LIT(KCurrentTime,"%-B%:0%J%:1%T%:2%S%:3%+B");
    time.FormatL(currentTime, KCurrentTime);    
    AppendLogBufferL(currentTime, iLogWriteBuf);
    AppendLogBufferL(KFileSeparator(), iLogWriteBuf);

    // available RAM memory
    TMemoryInfoV1Buf memory;
    UserHal::MemoryInfo(memory);
    iLogWriteBuf->Des().AppendNum(memory().iFreeRamInBytes);  
    AppendLogBufferL(KFileSeparator(), iLogWriteBuf);

    // own data, eg. application name
    AppendLogBufferL(aOwnData, iLogWriteBuf);
    AppendLogBufferL(KFileSeparator(), iLogWriteBuf);
    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);    

    // write the buffer to the file
    aFile.Write(iLogWriteBuf->Des()); 

    // clear the buffer
    iLogWriteBuf->Des().Copy(_L(""));
    }

// ---------------------------------------------------------------------------


void CLauncherEngine::CheckForMoreAppsL()
    {
    LOGSTRING("Launcher: CLauncherEngine::CheckForMoreAppsL");

    // make sure the launcher app is in the foreground
    TApaTaskList taskList(iWs);
    TUid launcherAppUid;
    launcherAppUid.iUid = 0x101FB74F;
    TApaTask launcherTask = taskList.FindApp(launcherAppUid);
    launcherTask.BringToForeground();


    // check if we have more test to be executed
    if ( iAppLaunchCounter >= iTotalNumberOfCases-1 )
        {
        
        // all done, show stats
        TBuf<200> message;
        message.Append( _L("Done: ") );
        message.AppendNum( iOkCases );
        message.Append( _L(" ok, ") );
        message.AppendNum( iFailedCases );
        message.Append( _L(" failed, ") );
        message.AppendNum( iSkippedCases );
        message.Append( _L(" skipped.") );

        // print the message to the output screen
        iLauncherUI->PrintText(message);
        iLauncherUI->PrintText(_L("\n\n"));
        
        //write same stuff to the log
        WriteInitialStuffToTheLogL(message, iLogFile);
        iLogWriteBuf->Des().Copy(KFileNewLine);
        iLogWriteBuf->Des().Append(KFileNewLine);        
        iLogFile.Write(iLogWriteBuf->Des());

        // close the log
        iLogFile.Close();

        iLaunchingIsActive = EFalse;

        // all apps launched
        _LIT(KAllDoneMessage, "All apps launched");
        iLauncherUI->ShowInfoMessage(KAllDoneMessage);
        }
    else
        {
        // more apps to be launched, maintain requests
        iAppLaunchCounter++;
        IssueLaunch();
        }
  
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::LaunchApplicationL()
    {
    LOGSTRING("Launcher: CLauncherEngine::LaunchApplication");
    LOGSTRING3("Launcher: Trying to launch %S, UID: %d", &iCurrentAppNameAndExt, iCurrentAppUid.iUid);

    TRAPD(err, DoLaunchApplicationL());
    
    if (err!=KErrNone)
        {
        iLogWriteBuf->Des().Append(_L("[FAIL: Cannot launch the app] "));        
       
        iLauncherUI->PrintText(iCurrentAppNameAndExt);
        iLauncherUI->PrintText(_L(": cannot launch\n"));
        
        // write the buffer to the log
        iLogWriteBuf->Des().Append(KFileNewLine);        
        iLogFile.Write(iLogWriteBuf->Des());

        // this application isn't even launchable, go to next one
        iFailedCases++;
        CheckForMoreAppsL();        
        }
    
    }
// ---------------------------------------------------------------------------


void CLauncherEngine::DoLaunchApplicationL()
    {
    LOGSTRING("Launcher: CLauncherEngine::DoLaunchApplicationL");

    // create a new handle
    RThread newThreadHandle;
    iCurrentAppThread = newThreadHandle;


    // Find the task with uid3
    TApaTaskList tasklist(iWs);
    TApaTask task=tasklist.FindApp(iCurrentAppUid);

    if (task.Exists())
        // Task exists, bring it to foreground
        {
        task.BringToForeground();
        }
    else
        // Task doesn't exist, launch a new instance of an application
        {
        TApaAppInfo appInfo;
        User::LeaveIfError(iLs.GetAppInfo(appInfo, iCurrentAppUid));
        TApaAppCapabilityBuf capBuf;
        User::LeaveIfError(iLs.GetAppCapability(capBuf, iCurrentAppUid));
        TApaAppCapability& caps = capBuf();

        CApaCommandLine* cmdLine=CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo.iFullName);

        if ( caps.iLaunchInBackground )
            // Apps capability defines that the app is launched in background
            {
            cmdLine->SetCommandL(EApaCommandBackground);
            }
        else
            {
            cmdLine->SetCommandL(EApaCommandRun);
            }

        // start the app
        User::LeaveIfError(iLs.StartApp(*cmdLine, iCurrentAppThreadId));

        // activate thread checker active object
        iAppThreadChecker->ActivateChecking();

        // now open a handle to the thread and register death notifier
        TInt err = iCurrentAppThread.Open(iCurrentAppThreadId);
        if (err == KErrNone)
            iCurrentAppThread.Logon(iAppThreadChecker->iStatus);
        else
            {
            iCurrentAppThread.Close();
            TRequestStatus* status = &iAppThreadChecker->iStatus;
            User::RequestComplete(status, KErrNone);
            iAppThreadChecker->Cancel();
            User::Leave(err);                
            }

        CleanupStack::PopAndDestroy(); // cmdLine
        }

    // the application is now running, start a check to see if it's still alive
    iAppRunningChecker->StartTesting(); 

    }

// ---------------------------------------------------------------------------

void CLauncherEngine::CheckIfAppIsRunningL()
    {
    LOGSTRING("Launcher: CLauncherEngine::CheckIfAppIsRunningL");

    // cancel the death notifier since it isn't needed anymore
    if( iCurrentAppThread.Handle() != 0 )
        {
        iCurrentAppThread.LogonCancel(iAppThreadChecker->iStatus);
        }

    // cancel checkers
    iAppThreadChecker->Cancel();
    iAppRunningChecker->Cancel(); 



    // check from the window server if the app is running
    TApaTaskList taskList(iWs);
    TApaTask thisTask = taskList.FindApp(iCurrentAppUid);

    if( !thisTask.Exists() ) // application not running -> FAIL
        {
        // check from the thread why it quit
        CheckWhyThreadDiedL();

        // --> CheckForMoreApps() and all the other stuff called from CheckWhyThreadDied() !

        }
    else
        {
        // app is running!
        iOkCases++;

        // close handle to the thread        
        iCurrentAppThread.Close();


        iLogWriteBuf->Des().Append(_L(" [OK]"));
        iLogWriteBuf->Des().Append(KFileNewLine);        
        iLogFile.Write(iLogWriteBuf->Des());


        // close the running application if needed
        if (iAutoClose)
            {
            // since the application is still open, let's close it
            thisTask.EndTask();
            //User::After(1000);
            //thisTask.SendSystemEvent(EApaSystemEventShutdown);
            //thisTask.KillTask();
            //User::After(1000);
            }

        // this app is done now, move to the next one!
        CheckForMoreAppsL();
                
        }    
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::CheckWhyThreadDiedL()
    {
    LOGSTRING("Launcher: CLauncherEngine::CheckWhyThreadDiedL");

    // cancel the death notifier since it isn't needed anymore
    if( iCurrentAppThread.Handle() != 0 )
        {
        iCurrentAppThread.LogonCancel(iAppThreadChecker->iStatus);
        }

    // make sure all checkers are cancelled
    iAppRunningChecker->Cancel(); 
    iAppThreadChecker->Cancel(); 

    TBuf<256> outputText;
    outputText.Append(_L("App.Closed. "));


    if (iCurrentAppThread.ExitType() == EExitKill)
        {
        outputText.Append(_L("\"EExitKill\""));
        }
    else if (iCurrentAppThread.ExitType() == EExitTerminate)
        {
        outputText.Append(_L("\"EExitTerminate\""));
        }
    else if (iCurrentAppThread.ExitType() == EExitPanic)
        {
        outputText.Append(_L("\"EExitPanic\""));
        }
    else if (iCurrentAppThread.ExitType() == EExitPending)
        {
        outputText.Append(_L("\"EExitPending\""));
        }
    else // unknown reason
        {
        outputText.Append(_L("\"Exit_Unknown_Reason\""));
        }        
    
    outputText.Append(_L(" code:"));
    TInt exitReason = iCurrentAppThread.ExitReason();
    outputText.AppendNum(exitReason);
    outputText.Append(_L(" \""));

    TPtrC exitCategory = iCurrentAppThread.ExitCategory();
    outputText.Append(exitCategory);

    outputText.Append(_L("\""));


    // print to screen
    iLauncherUI->PrintText(iCurrentAppNameAndExt);
    iLauncherUI->PrintText(_L(": "));
    iLauncherUI->PrintText(outputText);
    iLauncherUI->PrintText(_L("\n"));
    
    // write to the log also
    iLogWriteBuf->Des().Append(_L(" [FAIL: "));
    iLogWriteBuf->Des().Append(outputText);
    iLogWriteBuf->Des().Append(_L("]"));
    iLogWriteBuf->Des().Append(KFileNewLine);
    
    iLogFile.Write(iLogWriteBuf->Des());

    // close handle to the thread
    //iCurrentAppThread.Close();    <-- not safe the close the handle because of the "App.Closed" dialog
    //                                  somehow takes ownership of the thread or something

    // nothing to do anymore, move to the next app
    iFailedCases++;
    CheckForMoreAppsL();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

CDesCArray* CLauncherEngine::DependencyCheckForE32ImageL()
    {
    LOGSTRING("Launcher: CLauncherEngine::DependencyCheckForE32ImageL");

    // create an empty array for the missing dll names
    CDesCArray* missingDllArray = new(ELeave) CDesCArrayFlat(100);
    CleanupStack::PushL(missingDllArray);

    // get a list of DLLs from the E32 image file
    E32ImageReader* reader = E32ImageReader::NewLC();
    CDesCArray* dllArray = reader->ListOfDLLsL( iAppsArray->MdcaPoint(iAppLaunchCounter) );
    CleanupStack::PopAndDestroy(); // reader
    CleanupStack::PushL(dllArray);

    // compare system DLL and image DLL arrays
    TInt pos(0);
    for (TInt j=0; j<dllArray->MdcaCount(); j++)
        {
        if (iSystemDllArray->Find(dllArray->MdcaPoint(j), pos, ECmpFolded) != 0)  
            {
            // DLL not found, append the name to the list of missing DLLs
            missingDllArray->AppendL(dllArray->MdcaPoint(j));  
            }
        }

    CleanupStack::PopAndDestroy(); // dllArray
    CleanupStack::Pop();  // missingDllArray

    LOGSTRING("Launcher: CLauncherEngine::DependencyCheckForE32ImageL returns");

    return missingDllArray;
    }

// ---------------------------------------------------------------------------

TInt CLauncherEngine::DeleteLogFile()
    {
    LOGSTRING("Launcher: CLauncherEngine::DeleteLogFile");
    return BaflUtils::DeleteFile(iEnv->FsSession(), iLogFilePath);
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::StopLaunchingL()
    {
    LOGSTRING("Launcher: CLauncherEngine::StopLaunchingL");
    if(iLaunchingIsActive)
        {
        //write to the log
        WriteInitialStuffToTheLogL(_L("Cancelled by the user !!! "), iLogFile);
        iLogWriteBuf->Des().Copy(KFileNewLine);
        iLogWriteBuf->Des().Append(KFileNewLine);
        iLogFile.Write(iLogWriteBuf->Des());
    
        // close the log
        iLogFile.Close();
    
        // print to the screen
        iLauncherUI->PrintText(_L("Launching cancelled.\n\n"));
    
        // cancel all active objects
        if( iCurrentAppThread.Handle() != 0 )
            {
            iCurrentAppThread.LogonCancel(iAppThreadChecker->iStatus);
            }
        Cancel();
        iAppRunningChecker->Cancel(); 
        iAppThreadChecker->Cancel(); 
        
        iLaunchingIsActive = EFalse;
    
        _LIT(KMessage, "Launching cancelled");
        iLauncherUI->ShowInfoMessage(KMessage);

        }
    }

// ---------------------------------------------------------------------------

TBool CLauncherEngine::LogFileExists()
    {
    LOGSTRING("Launcher: CLauncherEngine::LogFileExists");
    return BaflUtils::FileExists(iEnv->FsSession(), iLogFilePath);
    }

// ---------------------------------------------------------------------------

TInt CLauncherEngine::DeleteBCLogFile()
    {
    LOGSTRING("Launcher: CLauncherEngine::DeleteBCLogFile");
    return BaflUtils::DeleteFile(iEnv->FsSession(), iBCLogFilePath);
    }


// ---------------------------------------------------------------------------

TBool CLauncherEngine::BCLogFileExists()
    {
    LOGSTRING("Launcher: CLauncherEngine::BCLogFileExists");
    return BaflUtils::FileExists(iEnv->FsSession(), iBCLogFilePath);
    }

// ---------------------------------------------------------------------------
void CLauncherEngine::AnalyseDLLsL( const TDesC& aFileName )
    {
    LOGSTRING("Launcher: CLauncherEngine::AnalyseDLLsL");
    _LIT(KStartSeparator, "******************************");
    _LIT(KStartingAnalysis, "Starting BC Analysis for DLLs.");    
    _LIT(KInputFileSelected, "Input file selected: ");
    
    // Reset found issues counter and buffer
    iFoundBCIssues = 0;    
    delete iBCIssuesBigBuffer;
    iBCIssuesBigBuffer = 0;
    
    // Reset log writing buffer:
    iLogWriteBuf->Des().Zero();
                
    if( IsActive() )
        {
        Cancel();
        }   
    
    iRequiredDllsFilePath = aFileName;
    
    if( BaflUtils::FileExists(iEnv->FsSession(), iRequiredDllsFilePath) == EFalse )
        {
        LOGSTRING2("Launcher: CLauncherEngine::AnalyseDLLsL - can't find input file: %S", &iRequiredDllsFilePath);
        User::Leave( KErrNotFound );
        }
    
    // open the log file for writing
    if (iBCLogFile.Open(iEnv->FsSession(), iBCLogFilePath, EFileWrite) != KErrNone)
        {
        iEnv->FsSession().MkDirAll(iLogFilePath);
        User::LeaveIfError( iBCLogFile.Replace(iEnv->FsSession(), iBCLogFilePath, EFileWrite) );
        }
    else
        {
        // file opens correctly, seek to the end
        TInt fileSize=0;
        iBCLogFile.Size(fileSize);
        iBCLogFile.Seek(ESeekCurrent, fileSize);
        }
    
    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
    AppendLogBufferL(KStartSeparator(), iLogWriteBuf);
    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
    iBCLogFile.Write(iLogWriteBuf->Des());
    iLogWriteBuf->Des().Zero();
    
    // Resolve file type. Should we use XML parsing or just compare DLL list
    TBool xmlParsing =
        KDotXML().Compare(iRequiredDllsFilePath.Right(KDotXML().Length())) == 0 ||
        KDotLauncherXML().Compare(iRequiredDllsFilePath.Right(KDotLauncherXML().Length())) == 0;
    
    // Log analysis starting time and selected input file:
    WriteInitialStuffToTheLogL(KStartingAnalysis, iBCLogFile);    
    AppendLogBufferL(KInputFileSelected(), iLogWriteBuf);
    AppendLogBufferL(iRequiredDllsFilePath, iLogWriteBuf);
    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
    iBCLogFile.Write(iLogWriteBuf->Des());
    iLogWriteBuf->Des().Zero();
    
    if( xmlParsing )
        {
        iLauncherUI->ChangeFocusToOutputView();
        DoBCAnalysisL();
        }
    else
        {
        DoCompareDLLListsL();
        }
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::DoBCAnalysisL()
    {
    LOGSTRING("Launcher: CLauncherEngine::DoBCAnalysisL");
    if( iXMLParser == 0 )
        {
        iXMLParser = CLauncherXMLParser::NewL(iEnv->FsSession());
        }
    
    TEntry entry;
    User::LeaveIfError(iEnv->FsSession().Entry(iRequiredDllsFilePath, entry));
    TInt fileSize = entry.iSize;
    
    _LIT(KAnalysing,"Analysing DLLs");
    iLauncherUI->ShowProgressBar(KAnalysing, 0, fileSize);
    iDLLAnalysisIsActive = ETrue;
    
    iXMLParser->ParseL(iRequiredDllsFilePath, this);
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::CancelBCAnalysis()
    {
    LOGSTRING("Launcher: CLauncherEngine::CancelBCAnalysis");
    if( iXMLParser )
        {
        iXMLParser->Cancel();
        }
    iLauncherUI->PrintText(_L("\nAnalysis cancelled.\n\n"));
    WriteInitialStuffToTheLogL(_L("Analysis cancelled by user"), iBCLogFile);
    iLogWriteBuf->Des().Zero();
    iBCLogFile.Close();
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::DoCompareDLLListsL()
    {
    LOGSTRING("Launcher: CLauncherEngine::DoCompareDLLListsL");
    
    RFile file;
    CleanupClosePushL(file);
        
    // Show progress bar 
    _LIT(KAnalysing,"Analysing DLLs");
    iLauncherUI->ShowWaitDialog(KAnalysing);

    if(file.Open(iEnv->FsSession(), iRequiredDllsFilePath, EFileRead) != KErrNone)
        {
        // Hide wait dialog  
        iLauncherUI->HideWaitDialog();
        
        iLauncherUI->ChangeFocusToOutputView();

        TBuf<200> msg;

        
        msg.Format(_L("Unable to open %S for reading!\n\n"), &iRequiredDllsFilePath);
        iLauncherUI->PrintText( msg );
        }
    else
        {
        
        CDesCArray* requiredDllArray = new(ELeave) CDesCArrayFlat(KMaxDllArraySize);
        CleanupStack::PushL(requiredDllArray);

        // read all lines the text file
        TFileName dllName;
        TInt i(0);
        while( ReadLineFromFileL(file, dllName) == KErrNone && i<KMaxDllArraySize )
            {
            dllName.TrimAll();

            if (dllName.Length() > 1)
                requiredDllArray->AppendL(dllName);
            
            i++;
            }

        if (requiredDllArray->MdcaCount() == 0)
            {
            // Hide wait dialog  
            iLauncherUI->HideWaitDialog();
            
            iLauncherUI->ChangeFocusToOutputView();

            TBuf<200> msg;
            msg.Format(_L("File %S is empty!\n\n"), &iRequiredDllsFilePath);
            iLauncherUI->PrintText( msg );
            }
        else
            {
            // compare the arrays and print any missing items
            CDesCArray* missingDllArray = new(ELeave) CDesCArrayFlat(KMaxDllArraySize);
            CleanupStack::PushL(missingDllArray);

            TInt pos(0);
            for (TInt j=0; j<requiredDllArray->MdcaCount(); j++)
                {
                if (iSystemDllArray->Find(requiredDllArray->MdcaPoint(j), pos, ECmpFolded) != 0)  
                    {
                    // DLL not found, append the name to the list of missing DLLs
                    missingDllArray->AppendL(requiredDllArray->MdcaPoint(j));  
                    }
                }

            
            if (missingDllArray->MdcaCount() == 0)
                {
                _LIT(KMessage, "No missing files found");
                AppendLogBufferL(KMessage(), iLogWriteBuf);
                iLauncherUI->ShowInfoMessage(KMessage);
                }
            else
                {
                iLauncherUI->ChangeFocusToOutputView();
                _LIT(KMissingFiles, "Missing files:\n");

                //iLauncherUI->PrintText( KMissingFiles );
                AppendLogBufferL(KMissingFiles(), iBCIssuesBigBuffer, KBigBufferAllocBytes);
                
                AppendLogBufferL(KMissingFiles(), iLogWriteBuf);
                
                for (TInt i=0; i<missingDllArray->MdcaCount(); i++)
                    {
                    //iLauncherUI->PrintText( missingDllArray->MdcaPoint(i) );
                    AppendLogBufferL(missingDllArray->MdcaPoint(i), iBCIssuesBigBuffer, KBigBufferAllocBytes);
                    //iLauncherUI->PrintText( _L("\n") );         
                    AppendLogBufferL(KNewLine(), iBCIssuesBigBuffer, KBigBufferAllocBytes);
                    
                    AppendLogBufferL(missingDllArray->MdcaPoint(i), iLogWriteBuf);
                    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
                    }
                AppendLogBufferL(KNewLine(), iBCIssuesBigBuffer, KBigBufferAllocBytes);
                iLauncherUI->PrintText( iBCIssuesBigBuffer->Des() );
                }
            
            // Hide wait dialog  
            iLauncherUI->HideWaitDialog();
            
            CleanupStack::PopAndDestroy(); // missingDllArray            
            }

        CleanupStack::PopAndDestroy(); // requiredDllArray
        }

    CleanupStack::PopAndDestroy(); //file
    iBCLogFile.Write(iLogWriteBuf->Des());
    iLogWriteBuf->Des().Zero();
    iBCLogFile.Close();
    }

// ---------------------------------------------------------------------------

void CLauncherEngine::ParsingProgressedL(TInt aBytes)
    {
    LOGSTRING2("Launcher: CLauncherEngine::ParsingProgressedL - Bytes: %d", aBytes);
    iLauncherUI->SetProgressBarValue(aBytes);
    }

// ---------------------------------------------------------------------------


void CLauncherEngine::ElementParsedL(const CLauncherDLLElement& aDllElement)
    {
    LOGSTRING("Launcher: CLauncherEngine::ElementParsedL");
    User::ResetInactivityTime();

    _LIT(KIndent, "    ");
    _LIT(KIssuesFound, "Binary compatibility issues found:"); 
    _LIT(KDLLMissing,"DLL is missing");
    _LIT(KUID1Changed,"UID1 changed");
    _LIT(KUID2Changed,"UID2 changed");
    _LIT(KUID3Changed,"UID3 changed");
    _LIT(KSIDChanged,"SID changed");
    _LIT(KCapabilityChanged,"Capability changed");
    
    if( iDLLElement == 0 )
        {
        iDLLElement = CLauncherDLLElement::NewL();
        }
    
    TFindFile fileFinder(iEnv->FsSession());
    _LIT(KDLLPath, "\\sys\\bin\\");
    TInt err = fileFinder.FindByPath(aDllElement.Name(), &KDLLPath); 
    
    TBuf<256> issueStr;
    
    if( err == KErrNotFound )
        {
        if( iFoundBCIssues++ == 0 )
            {                        
            iLauncherUI->PrintText( KIssuesFound );
            iLauncherUI->PrintText( KNewLine );            
            AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
            AppendLogBufferL(KIssuesFound(), iLogWriteBuf);
            AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
            }
        issueStr.Copy(aDllElement.Name());
        issueStr.Append(KNewLine);
        issueStr.Append(KIndent);
        issueStr.Append(KDLLMissing);
        issueStr.Append(KNewLine);
                
        AppendLogBufferL(issueStr, iLogWriteBuf);
        
        TFileName dllName = aDllElement.Name();
        LOGSTRING2("Launcher: DLL not found: %S", &dllName);
        if( iFoundBCIssues > KBigBufferUsageThreshold )
            {
            if( iBCIssuesBigBuffer == 0)
                {
                iBCIssuesBigBuffer = HBufC::NewL(KBigBufferAllocBytes);
                }
            TInt maxSize = iBCIssuesBigBuffer->Des().Length() + issueStr.Length();
            if( maxSize >= iBCIssuesBigBuffer->Des().MaxLength())
                {
                iBCIssuesBigBuffer = iBCIssuesBigBuffer->ReAllocL(maxSize + KBigBufferAllocBytes );                
                }
            TPtr ptr(iBCIssuesBigBuffer->Des());
            ptr += issueStr;            
            }
        else
            {
            iLauncherUI->PrintText( issueStr);
            }
        }
    else if( err == KErrNone)
        {
        // File is found, so let's try to open it:
        RFile dllFile;
        CleanupClosePushL(dllFile);        
        if( dllFile.Open(iEnv->FsSession(), fileFinder.File(), EFileRead) == KErrNone )
            {
            // Parse DLL:
            iDLLParser->ParseL(iEnv->FsSession(), dllFile, *iDLLElement);
            CleanupStack::PopAndDestroy(); // dllFile            
            RArray<CLauncherDLLElement::TDifference> diffs;
            CleanupClosePushL(diffs);
                        
            // Compare DLLs:              
            if( iDLLElement->CompareL(aDllElement, diffs))
                {                
                if( iFoundBCIssues++ == 0 )
                    {
                    iLauncherUI->PrintText( KIssuesFound );
                    iLauncherUI->PrintText( KNewLine );                   
                    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
                    AppendLogBufferL(KIssuesFound(), iLogWriteBuf);
                    AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
                    }
                // Differencies found:
                for( TInt i = 0; i < diffs.Count(); ++i )
                    {                
                    // Print DLL name:
                    if( i == 0 )
                        {
                        issueStr.Copy(aDllElement.Name());
                        issueStr.Append(KNewLine);
                        }
                    
                    // Print differencies:
                    issueStr.Append(KIndent);
                    switch(diffs[i])
                        {
                        case CLauncherDLLElement::EDifference_UID1:
                            issueStr.Append(KUID1Changed);
                            break;
                        case CLauncherDLLElement::EDifference_UID2:
                            issueStr.Append( KUID2Changed );
                            break;
                        case CLauncherDLLElement::EDifference_UID3:
                            issueStr.Append( KUID3Changed );
                            break;
                        case CLauncherDLLElement::EDifference_SID:
                            issueStr.Append( KSIDChanged );
                            break;
                        case CLauncherDLLElement::EDifference_Capability:
                            issueStr.Append( KCapabilityChanged );
                            break;
                        }
                    issueStr.Append( KNewLine );                    
                    }
                AppendLogBufferL(issueStr, iLogWriteBuf);
                if( iFoundBCIssues > KBigBufferUsageThreshold )
                    {
                    // To improve performance, don't print issues to output anymore.
                    // Instead, store the issues in buffer and print them when analysis is done.
                    if( iBCIssuesBigBuffer == 0)
                        {
                        iBCIssuesBigBuffer = HBufC::NewL(KBigBufferAllocBytes);
                        }
                    TInt maxSize = iBCIssuesBigBuffer->Des().Length() + issueStr.Length();
                    if( maxSize >= iBCIssuesBigBuffer->Des().MaxLength())
                        {
                        TInt increaseSize = Max(issueStr.Length(), KBigBufferAllocBytes);
                        iBCIssuesBigBuffer = iBCIssuesBigBuffer->ReAllocL(maxSize + increaseSize );                            
                        }
                    TPtr ptr(iBCIssuesBigBuffer->Des());
                    ptr += issueStr;
                    }
                else
                    {
                    iLauncherUI->PrintText( issueStr);
                    }
                }
            CleanupStack::Pop(); // diffs
            diffs.Close();            
            }
        else
            {
            CleanupStack::PopAndDestroy(); // dllFile
            }
        }
    }

// ---------------------------------------------------------------------------
void CLauncherEngine::DocumentParsedL(TInt aErrorCode)
    {            
    LOGSTRING2("Launcher: CLauncherEngine::DocumentParsedL (Error code: %d)", aErrorCode);
        
    iDLLAnalysisIsActive = EFalse;
    _LIT(KParseError, "Parse error: ");
    _LIT(KNoIssues, "No binary compatibility issues found");
    
    if( IsActive() )
        {
        Cancel();
        }
     
    //hide progress bar 
    iLauncherUI->HideProgressBar();

    if( aErrorCode != KErrNone )
        {
        TBuf<16> errorCodeString;
        errorCodeString.AppendNum(aErrorCode);
        iLauncherUI->PrintText( KNewLine );
        iLauncherUI->PrintText( KParseError );
        iLauncherUI->PrintText( errorCodeString );
        iLauncherUI->PrintText( KNewLine );        
        AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
        AppendLogBufferL(KParseError(), iLogWriteBuf);
        AppendLogBufferL(errorCodeString, iLogWriteBuf);
        AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
        }
    else if(iFoundBCIssues == 0)
        {
        iLauncherUI->PrintText( KNoIssues );
        iLauncherUI->PrintText( KNewLine );        
        AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
        AppendLogBufferL(KNoIssues(), iLogWriteBuf);
        AppendLogBufferL(KFileNewLine(), iLogWriteBuf);
        }
    else if( iBCIssuesBigBuffer && iBCIssuesBigBuffer->Des().Length() > 0 )
        {
        iLauncherUI->PrintText(iBCIssuesBigBuffer->Des());
        delete iBCIssuesBigBuffer;
        iBCIssuesBigBuffer = 0;
        }
    if( iLogWriteBuf->Length() > 0 && iBCLogFile.SubSessionHandle() != 0)
        {
        iBCLogFile.Write(iLogWriteBuf->Des());
        }
    WriteInitialStuffToTheLogL(_L("Analysis ready"), iBCLogFile);
    iLogWriteBuf->Des().Zero();
    iBCLogFile.Close();
    }

// ---------------------------------------------------------------------------

TInt CLauncherEngine::ReadLineFromFileL(RFile& aFile, TDes& aReadBuf)
    {
    LOGSTRING("Launcher: CLauncherEngine::ReadLineFromFile");

    _LIT8(KImcvCRLF, "\r\n");
    TInt err(KErrNone);

    HBufC8* tempLine = HBufC8::NewLC(1000);
    TPtr8 buffer = tempLine->Des();

    // clear the target buffer
    aReadBuf.Zero();

    // get the current file position
    TInt filePos(0);
    aFile.Seek(ESeekCurrent, filePos);

    // read the buffer
    err = aFile.Read(buffer);

    // check if it's the end of file
    TInt s = buffer.Length();
    if (s == 0)
        err = KErrEof;

    if (err == KErrNone)
        {
        // copy to the lfcr and then set the file pointer to the point after that
        TInt pos = buffer.Find(KImcvCRLF);
        if (pos != -1)
            {
            TFileName tempBuf;
            buffer.SetLength(pos);
            tempBuf.Copy(buffer);
            aReadBuf.Justify(tempBuf, pos, ELeft, ' ');
            filePos += (pos+2);

            // set the file pointer back to after the lfcr
            aFile.Seek(ESeekStart, filePos);
            }
        
        // else fill the whole buffer
        else
            {
            aReadBuf.Copy(buffer);
            }
        }

    CleanupStack::PopAndDestroy(); // tempLine
    return err;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------

CAppThreadChecker* CAppThreadChecker::NewL(CLauncherEngine* aLauncherEngine)
    {
    LOGSTRING("Launcher: CAppThreadChecker::NewL");

    CAppThreadChecker* self = new(ELeave) CAppThreadChecker;
    CleanupStack::PushL(self);
    self->ConstructL(aLauncherEngine);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CAppThreadChecker::CAppThreadChecker() : CActive(EActivePriorityIpcEventsHigh)
    {
    }

// ---------------------------------------------------------------------------

void CAppThreadChecker::ConstructL(CLauncherEngine* aLauncherEngine)
    {
    LOGSTRING("Launcher: CAppThreadChecker::ConstructL");

    iEnv = CEikonEnv::Static();

    iLauncherEngine = aLauncherEngine;

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------

CAppThreadChecker::~CAppThreadChecker()
    {
    LOGSTRING("Launcher: CAppThreadChecker::~CAppThreadChecker");

    Cancel();
    }

// ---------------------------------------------------------------------------

void CAppThreadChecker::ActivateChecking()
    {
    LOGSTRING("Launcher: CAppThreadChecker::ActivateChecking");

    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("Thread Checker"), 300));

    SetActive();
    }

// ---------------------------------------------------------------------------

void CAppThreadChecker::RunL()
    {
    LOGSTRING("Launcher: CAppThreadChecker::RunL");

    // check the state of the thread
    iLauncherEngine->CheckWhyThreadDiedL();
    }

// ---------------------------------------------------------------------------

void CAppThreadChecker::DoCancel()
    {
    LOGSTRING("Launcher: CAppThreadChecker::DoCancel");
    }

// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------

CAppRunningChecker* CAppRunningChecker::NewL(CLauncherEngine* aLauncherEngine)
    {
    LOGSTRING("Launcher: CAppRunningChecker::NewL");

    CAppRunningChecker* self = new(ELeave) CAppRunningChecker;
    CleanupStack::PushL(self);
    self->ConstructL(aLauncherEngine);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------

CAppRunningChecker::CAppRunningChecker() : CActive(EActivePriorityIpcEventsHigh)
    {
    }

// ---------------------------------------------------------------------------

void CAppRunningChecker::ConstructL(CLauncherEngine* aLauncherEngine)
    {
    LOGSTRING("Launcher: CAppRunningChecker::ConstructL");

    iEnv = CEikonEnv::Static();
    User::LeaveIfError(iTimer.CreateLocal());

    iLauncherEngine = aLauncherEngine;

    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------

CAppRunningChecker::~CAppRunningChecker()
    {
    LOGSTRING("Launcher: CAppRunningChecker::~CAppRunningChecker");

    Cancel();

    iTimer.Close();
    }

// ---------------------------------------------------------------------------

void CAppRunningChecker::StartTesting()
    {
    LOGSTRING("Launcher: CAppRunningChecker::StartTesting");

    __ASSERT_ALWAYS(!IsActive(), User::Panic(_L("Running Checker"), 200));

    // async delay of seven seconds
    iTimer.After(iStatus, 7000000);    
    SetActive();
    }

// ---------------------------------------------------------------------------

void CAppRunningChecker::RunL()
    {
    LOGSTRING("Launcher: CAppRunningChecker::RunL");

    // check if the application is running
    iLauncherEngine->CheckIfAppIsRunningL();
    }

// ---------------------------------------------------------------------------

void CAppRunningChecker::DoCancel()
    {
    LOGSTRING("Launcher: CAppRunningChecker::DoCancel");
    iTimer.Cancel();
    }

// ---------------------------------------------------------------------------



//////////////////////////////////////////////////////////////////////////////////////

