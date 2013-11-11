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



#ifndef LOADGEN_CPULOAD_H
#define LOADGEN_CPULOAD_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "loadgen_loadbase.h"
#include "loadgen_loadattributes.h"


// FORWARD DECLARATIONS



// CLASS DECLARATIONS

class CCPULoad : public CLoadBase
    {
public:
    static CCPULoad* NewL(TCPULoadAttributes& aAttributes, TInt aReferenceNumber);
    virtual ~CCPULoad();
        
private: // Constructors
    CCPULoad(TCPULoadAttributes& aAttributes, TInt aReferenceNumber);
    void ConstructL();  

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TCPULoadAttributes& Attributes() { return iAttributes; }
	inline void SetAttributes(const TCPULoadAttributes& aAttributes) { iAttributes = aAttributes; }

public:  // New static methods
    static TInt ThreadFunction(TAny* aThreadArg);

private:  // New static methods
    static void GenerateLoad(TCPULoadAttributes& aAttributes);
    static void DoHeaveStuff(TInt aMode);

private: // Data
    TCPULoadAttributes      iAttributes;    
    RThread                 iThread;
    };
 

#endif
