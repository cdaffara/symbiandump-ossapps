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


#ifndef LOADGEN_PHONECALL_H
#define LOADGEN_PHONECALL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <etel3rdparty.h>


#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS
class CPhoneCallManager;
class CDialer;


// CLASS DECLARATIONS

class CPhoneCall : public CLoadBase
    {
public:
    static CPhoneCall* NewL(TPhoneCallAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CPhoneCall();
        
private: // Constructors
    CPhoneCall(TPhoneCallAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TPhoneCallAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TPhoneCallAttributes& aAttributes) { iAttributes = aAttributes; }
	
public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);

private:  // New static methods
    static void GenerateLoad(TPhoneCallAttributes& aAttributes);

private: // Data
    TPhoneCallAttributes        iAttributes;    
    RThread                     iThread;
    };

    

class CPhoneCallManager : public CActive
    { 
private:
    enum TState
        {
        EStateIdle = 0,
        EStateCall
        };
public:
    static CPhoneCallManager* NewL(TPhoneCallAttributes& aAttributes);
    ~CPhoneCallManager();

private:
    CPhoneCallManager(TPhoneCallAttributes& aAttributes);
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack(TAny* aAny);
    void HandleCalls();
    void DoDial();
    void DoHangup();

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }
    void HandleStatus(TInt aErr);

private:
    TPhoneCallAttributes&               iAttributes;
    CDialer*                            iDialer;   
    CPeriodic*                          iPeriodicTimer;
    TInt                                iState;
    }; 


class CDialer : public CActive
    { 
public:
    static CDialer* NewL(CPhoneCallManager& aManager);
    ~CDialer();

private:
    CDialer(CPhoneCallManager& aManager);
    void ConstructL();

private:
    void RunL();
    void DoCancel();

public:
    void Dial(const TDesC& aDestination);
    void Hangup();

private:
    CPhoneCallManager&                  iManager;
    CTelephony*                         iTelephony;
    CTelephony::TCallId                 iCallId;
    CTelephony::TCallParamsV1           iCallParams;
    CTelephony::TCallParamsV1Pckg       iCallParamsPckg;
    };
        
#endif
