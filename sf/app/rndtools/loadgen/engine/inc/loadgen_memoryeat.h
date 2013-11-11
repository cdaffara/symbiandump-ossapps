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


#ifndef LOADGEN_MEMORYEAT_H
#define LOADGEN_MEMORYEAT_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <e32hal.h>

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS



// CLASS DECLARATIONS

class CMemoryEat : public CLoadBase
    {
public:
    static CMemoryEat* NewL(TMemoryEatAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CMemoryEat();
        
private: // Constructors
    CMemoryEat(TMemoryEatAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TMemoryEatAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TMemoryEatAttributes& aAttributes) { iAttributes = aAttributes; }

public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);
private:
    static void GenerateLoad(TMemoryEatAttributes& aAttributes);

private: // Data
    TMemoryEatAttributes    iAttributes;    
    RThread                 iThread;
    };


class CMemoryEatManager : public CActive
    {
public:
    static CMemoryEatManager* NewL(TMemoryEatAttributes& aAttributes);
    ~CMemoryEatManager();

private:
    CMemoryEatManager(TMemoryEatAttributes& aAttributes);
    void ConstructL();

private:
    void RunL();
    void DoCancel();

private:
    static TInt PeriodicTimerCallBack(TAny* aAny);
    void InitMemoryEatL();
    void DestroyMemoryEat();
    void EatMemoryL();
    void DoEatMemoryL( TInt64 aNewSize );
    void HandleEatMemoryL( TInt64 aNewSize );
    void AllocMemoryL( TInt64 aNewSize );
    void FreeMemoryL( TInt64 aNewSize );
    TInt64 ReadFreeMemory();
    

public:
    inline CPeriodic* PeriodicTimer() { return iPeriodicTimer; }

private:
    TMemoryEatAttributes&   iAttributes;    
    CPeriodic*              iPeriodicTimer;
    RChunk                  iEatChunk;
    RFile                   iEatFile;
    RFs                     iFs;
    TBool                   iWavyEatMoreMemory;
    TInt     iFilesCounter;
    };
     

#endif
