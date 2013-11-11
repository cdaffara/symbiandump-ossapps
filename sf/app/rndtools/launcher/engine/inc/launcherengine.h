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

#ifndef LAUNCHERENGINE_H
#define LAUNCHERENGINE_H

#include <e32base.h>
#include <apgcli.h>
#include <w32std.h>
#include <e32std.h>
#include <e32hal.h>

#include "launcherparserobserver.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CEikonEnv;
class CAppThreadChecker;
class CAppRunningChecker;
class CLauncherXMLParser;
class CLauncherDLLParser;


// INFERFACE FOR COMMUNICATION WITH UI

class MLauncherUI
{
public:
    /**
     * Method for writing text to output window.
     * @param aText A text to be written.
     * @return None.
     */
    virtual void PrintText(const TDesC& aText) = 0;

    /**
     * Shows info message with text
     * @param aText An text to be shown in message.
     * @return None.
     */
    virtual void ShowInfoMessage(const TDesC& aText) = 0;
    
    /**
     * Shows error message with error text
     * @param aText An error text to be shown in message.
     * @return None.
     */
    virtual void ShowErrorMessage(const TDesC& aText) = 0;
    
    /**
     * Shows progress bar with text
     * @param aText A text to be shown at top of the progress bar.
     * @param aMinimum A minimum progress bar value.
     * @param aMaximum A maximum progress bar value.
     * @return None.
     */
    virtual void ShowProgressBar(const TDesC& aText, TInt aMinimum, TInt aMaximum ) = 0;

    /**
     * Sets progress bar value
     * @param aValue A value between min and max value of the progress bar range
     * @return None.
     */
    virtual void SetProgressBarValue(TInt aValue) = 0;

    /**
     * Hides progress bar
     * @return None.
     */
    virtual void HideProgressBar() = 0;

    /**
     * Shows wait dialog with text
     * @param aText A text to be shown at top of the wait bar.
     * @return None.
     */
    virtual void ShowWaitDialog(const TDesC& aText) = 0;

    /**
     * Hides wait dialog
     * @return None.
     */
    virtual void HideWaitDialog() = 0;
    
    /**
     * Changes focus to output view
     * @return None.
     */
    virtual void ChangeFocusToOutputView() = 0;
    
};

// CLASS DECLARATION

class CLauncherEngine: public CActive, public MLauncherParserObserver
{

public:
    static CLauncherEngine* NewL(MLauncherUI *aLauncherUI);
    ~CLauncherEngine();
    CDesCArray* ListOfAllAppsL();
    void StartAppLaunchingL(const CArrayFix<TInt>* aSelectedApps, TBool aAutoClose);
    void CheckIfAppIsRunningL();
    void CheckWhyThreadDiedL();
    void StopLaunchingL();
    TBool LogFileExists();
    TBool BCLogFileExists();
    
    TInt DeleteLogFile();
    TInt DeleteBCLogFile();
    
    void AnalyseDLLsL( const TDesC& aFileName );
    
    void ElementParsedL(const CLauncherDLLElement& aDllElement);
    void DocumentParsedL(TInt aErrorCode);
    void ParsingProgressedL(TInt aBytes);
    TInt ReadLineFromFileL(RFile& aFile, TDes& aReadBuf);
    
    const TDesC& RequiredDllsFilePath() const { return iRequiredDllsFilePath; };
    
    void CancelBCAnalysis();
    
private:
    CLauncherEngine();
    void ConstructL(MLauncherUI *aLauncherUI);
    
    void RunL();
    void DoCancel();
        
    TInt FindFiles(const TDesC& aFileName, const TDesC& aPath);
    void IssueLaunch();
    void WriteInitialStuffToTheLogL(const TDesC& aOwnData, RFile& aFile);
    void CheckForMoreAppsL();
    void LaunchApplicationL();
    void DoLaunchApplicationL();
    CDesCArray* DependencyCheckForE32ImageL();

    /**
     * Performs binary compatibility analysis based on reference data given
     * in XML-file.
     */
    void DoBCAnalysisL();
    
    /**
     * Compares given DLL list to system DLLs
     */
    void DoCompareDLLListsL();
    
private:

    MLauncherUI *iLauncherUI;
    
    RTimer          iTimer;
    CEikonEnv*      iEnv;
    RApaLsSession   iLs;
    RWsSession      iWs;
    HBufC8*         iLogWriteBuf;    
    RFile           iLogFile;
    RFile           iBCLogFile;  
    TFileName       iLogFilePath;
    TFileName       iBCLogFilePath;
    TFileName       iSystemDllsFilePath;
    TFileName       iRequiredDllsFilePath;        
        
    CDesCArray*     iAppsArray;
    CDesCArray*     iAllAppsArray;
    CDesCArray*     iSystemDllArray;
//  CDesCArray*     iRequiredDllArray;

    TBool           iLaunchingIsActive;
    TBool           iDLLAnalysisIsActive;
    TBool           iSkipHiddenAndEmbedOnly;
    TBool           iAutoClose;
    TInt            iAppLaunchCounter;
    TInt            iFailedCases;
    TInt            iOkCases;
    TInt            iSkippedCases;
    TInt            iTotalNumberOfCases;

    TUid            iCurrentAppUid;
    TThreadId       iCurrentAppThreadId;
    TBuf<128>       iCurrentAppNameAndExt;
    RThread         iCurrentAppThread;
//
    CAppThreadChecker*          iAppThreadChecker;
    CAppRunningChecker*         iAppRunningChecker;

    CLauncherXMLParser*     iXMLParser;
    CLauncherDLLParser*     iDLLParser;
    CLauncherDLLElement*    iDLLElement;
    
    TInt                iFoundBCIssues;
    HBufC*              iBCIssuesBigBuffer;

        
public:
    inline TBool SkipHiddenAndEmbedOnly()              { return iSkipHiddenAndEmbedOnly; };
    inline void SetSkipHiddenAndEmbedOnly(TBool aSkip) { iSkipHiddenAndEmbedOnly = aSkip; };
    inline const TDesC& LogFilePath() { return iLogFilePath; }
    inline const TDesC& BCLogFilePath() { return iBCLogFilePath; }
    inline const TDesC& SystemDllsFilePath() { return iSystemDllsFilePath; }
};



class CAppRunningChecker : public CActive
    {
public:
    static CAppRunningChecker* NewL(CLauncherEngine* aLauncherEngine);
    ~CAppRunningChecker();

    void StartTesting();

private:
    CAppRunningChecker();
    void ConstructL(CLauncherEngine* aLauncherEngine);

    void RunL();
    void DoCancel();

private:
    RTimer iTimer;
    CEikonEnv* iEnv;
    CLauncherEngine* iLauncherEngine;
    };

class CAppThreadChecker : public CActive
    {
public:
    static CAppThreadChecker* NewL(CLauncherEngine* aLauncherEngine);
    ~CAppThreadChecker();

    void ActivateChecking();
    void CheckIfAppIsRunningL();

private:
    CAppThreadChecker();
    void ConstructL(CLauncherEngine* aLauncherEngine);

    void RunL();
    void DoCancel();

private:
    CEikonEnv* iEnv;
    CLauncherEngine* iLauncherEngine;
    };


#endif //LAUNCHERENGINE_H
