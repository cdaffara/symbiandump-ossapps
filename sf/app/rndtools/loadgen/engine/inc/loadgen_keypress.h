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


#ifndef LOADGEN_KEYPRESS_H
#define LOADGEN_KEYPRESS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS



// CLASS DECLARATIONS

class CKeyPress : public CLoadBase
    {
public:
    static CKeyPress* NewL(TKeyPressAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CKeyPress();
        
private: // Constructors
    CKeyPress(TKeyPressAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TKeyPressAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TKeyPressAttributes& aAttributes) { iAttributes = aAttributes; }

public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);

private:  // New static methods
    static void GenerateLoad(TKeyPressAttributes& aAttributes);

private: // Data
    TKeyPressAttributes         iAttributes;    
    RThread                     iThread;
    };


class CKeyPressManager : public CActive
    {
public:
    static CKeyPressManager* NewL(TKeyPressAttributes& aAttributes);
    ~CKeyPressManager();

private:
    CKeyPressManager(TKeyPressAttributes& aAttributes);
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack(TAny* aAny);
    void SimulateKeyEvent();

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }

private:
    TKeyPressAttributes&    iAttributes;    
    CPeriodic*              iPeriodicTimer;
    RWsSession              iWsSession;
    }; 

#endif
