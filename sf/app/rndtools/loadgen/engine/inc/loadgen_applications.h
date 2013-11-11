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



#ifndef LOADGEN_APPLICATIONS_H
#define LOADGEN_APPLICATIONS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS
class CLauncherEngine;

// CLASS DECLARATIONS

class CAppLauncher : public CLoadBase
    {
public:
    static CAppLauncher* NewL( TApplicationsAttributes& aAttributes,
                                TInt aReferenceNumber );
    virtual ~CAppLauncher();
        
private: // Constructors
    CAppLauncher( TApplicationsAttributes& aAttributes, 
                    TInt aReferenceNumber );
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TApplicationsAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TApplicationsAttributes& aAttributes) { iAttributes = aAttributes; }
	

public:  // New static methods
    static TInt ThreadFunction( TAny* aThreadArg );

private:  // New static methods
    static void GenerateLoad( TApplicationsAttributes& aAttributes );

private: // Data
    TApplicationsAttributes         iAttributes;    
    RThread                         iThread;
    };

// Class to start launching applications in given period 

// CLASS DECLARATIONS
class CAppLauncherManager : public CActive
    {
public:
    static CAppLauncherManager* NewL( TApplicationsAttributes& aAttributes );
    ~CAppLauncherManager();

private:
    CAppLauncherManager( TApplicationsAttributes& aAttributes );
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack( TAny* aAny );
    void SimulateEventL();

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }

private:
    TApplicationsAttributes&    iAttributes;    
    CPeriodic*                  iPeriodicTimer;
    RWsSession                  iWsSession;
    CLauncherEngine*            iLauncherEngine;
    TInt                        iAppEventType;
    }; 

// A class where applicationg launching and ending is implemented 
#include <aknappui.h>
#include <apgcli.h>

// FORWARD DECLARATIONS
class CAppKeyPressManager;

// CLASS DECLARATIONS
class CLauncherEngine : public CBase
    {
public:
    static CLauncherEngine* NewL( TApplicationsAttributes& aAttributes );
    ~CLauncherEngine();

public:
    void StartAppLaunchingL();
    void StopApplication( TBool aRandomApplication );
    
    
private:
    CLauncherEngine( TApplicationsAttributes& aAttributes );
    void ConstructL();
    TBool LaunchApplicationL();
    void DoLaunchApplicationL();

public:    
    inline TInt AppLaunchCounter() { return iAppLaunchCounter; }
    
private:
    RTimer          iTimer;
    RApaLsSession   iLs;
    RWsSession      iWs;
    TInt            iAppLaunchCounter;
    TUid            iCurrentAppUid;
    TThreadId       iCurrentAppThreadId;
    TBuf<128>       iCurrentAppNameAndExt;
    RThread         iCurrentAppThread;
    TInt            iParallelApplicationsCounter;
    TApplicationsAttributes&    iAttributes;

    CAppKeyPressManager* iAppKeyPressManager;
    };


// A class where key presses can be added to application
#include <e32cmn.h>
// FORWARD DECLARATIONS
class CApplicationKeyPresses;
// CLASS DECLARATIONS
class CAppKeyPressManager : public CBase
    {
public:
    static CAppKeyPressManager* NewL( TApplicationsAttributes& aAttributes );
    ~CAppKeyPressManager();
    void AddNewApplicationUidToKeyEventsL( TUid aUid );
    TUid KillApplication( TBool aRandomApplication );
    
private:
    CAppKeyPressManager( TApplicationsAttributes& aAttributes );
    void ConstructL();

private:
    RPointerArray<CApplicationKeyPresses> iKeyPresses;
    TApplicationsAttributes& iAttributes;    
    }; 

// A class which sends key press events to application
// CLASS DECLARATIONS
class CApplicationKeyPresses : public CBase
    {
public:
    static CApplicationKeyPresses* NewL( TUid aUid, 
                                            TApplicationsAttributes& aAttributes );
    ~CApplicationKeyPresses();
    
private:
    CApplicationKeyPresses( TUid aUid, TApplicationsAttributes& aAttributes );
    void ConstructL();

private:
    static TInt PeriodicTimerCallBack( TAny* aAny );
    void SimulateKeyEvent();

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }

private:
    TUid                        iUid;
    TApplicationsAttributes&    iAttributes;
    CPeriodic*                  iPeriodicTimer;
    RWsSession                  iWsSession;
public:
    inline TUid ApplicationUid() { return iUid; };    
    }; 

#endif // LOADGEN_APPLICATIONS
