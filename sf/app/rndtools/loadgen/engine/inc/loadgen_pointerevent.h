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


#ifndef LOADGEN_POINTEREVENT_H
#define LOADGEN_POINTEREVENT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include <e32svr.h>
#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS



// CLASS DECLARATIONS

class CPointerEvent : public CLoadBase
    {
public:
    static CPointerEvent* NewL(TPointerEventAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CPointerEvent();
        
private: // Constructors
    CPointerEvent(TPointerEventAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TPointerEventAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TPointerEventAttributes& aAttributes) { iAttributes = aAttributes; }

public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);

private:  // New static methods
    static void GenerateLoad(TPointerEventAttributes& aAttributes);

private: // Data
    TPointerEventAttributes         iAttributes;    
    RThread                     iThread;
    };


class CPointerEventManager : public CActive
    {
public:
    static CPointerEventManager* NewL(TPointerEventAttributes& aAttributes);
    ~CPointerEventManager();

private:
    CPointerEventManager(TPointerEventAttributes& aAttributes);
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack(TAny* aAny);
    void SimulatePointerEvent();

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }

private:
    TPointerEventAttributes&    iAttributes;    
    CPeriodic*                  iPeriodicTimer;
    //RWsSession                  iWsSession;
    }; 

#endif
