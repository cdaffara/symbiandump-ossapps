// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "caltestoom.h"
#include "caltestlib.h"

#include <calsession.h>
#include <e32debug.h>
#include <ecom/ecom.h>

EXPORT_C void CalTestOom::OutOfMemoryTestL(MCalTestOomCallBack& aCallBack, MCalTestOomCallBack::TType aType, CCalSession& aCalSession)
	{
	TInt clientFailAt(0);
	TInt serverFailAt(0);
	
	TAny* dummy = NULL;
	
	TBool cleanCache = EFalse;
	
	for (TInt i(0) ; i < 10000 ; ++i)
		{
		CleanupStack::PushL(dummy);
		}
		
	CleanupStack::Pop(10000);
	
	if(aType == MCalTestOomCallBack::EClient)
		{
		FOREVER
			{
			aCalSession.__dbgClearTzClientCacheL(cleanCache);
			__UHEAP_SETFAIL(RHeap::EDeterministic, ++clientFailAt);
			__UHEAP_MARK;
			
			TRAPD(err, aCallBack.OomTestL(MCalTestOomCallBack::EClient, clientFailAt));
			
			__UHEAP_RESET;
			
			REComSession::FinalClose();
			
			aCalSession.__dbgClearTzClientCacheL(cleanCache);
						
			if (err == KErrNoMemory)
				{
				__UHEAP_MARKEND;
				}
			else if (err == KErrNone)
			    {
			    break;
			    }
			else
				{
				User::Leave(err);
				}
			}
			cleanCache = ETrue;
			aCalSession.__dbgClearTzClientCacheL(cleanCache);
		}
	else if(aType == MCalTestOomCallBack::EServer)
		{
		
		// server side oom test
		FOREVER
			{
			aCalSession._DebugSetHeapFailL(RAllocator::EDeterministic, ++serverFailAt);
			
			TRAPD(err, aCallBack.OomTestL(MCalTestOomCallBack::EServer, serverFailAt));
			
			REComSession::FinalClose();
			
			if (err == KErrNoMemory)
				{
				aCalSession._DebugSetHeapFailL(RAllocator::ENone, 0);
				}
			else if (err == KErrNone)
				{
				aCalSession._DebugSetHeapFailL(RAllocator::ENone, 0);
				break;
				}
			else
				{
				aCalSession._DebugSetHeapFailL(RAllocator::ENone, 0);
				User::Leave(err);
				}
			}
		}
	}
