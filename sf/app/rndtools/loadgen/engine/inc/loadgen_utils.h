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


#ifndef LOADGEN_UTILS_H
#define LOADGEN_UTILS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

const TUint KHalFunction_EKernelHalNumLogicalCpus = 16; //EKernelHalNumLogicalCpus
const TUint KHalFunction_EKernelHalLockThreadToCpu = 19; //EKernelHalLockThreadToCpu

class CLoadGenUtils {

public:
		static TThreadPriority SettingItemToThreadPriority(TInt aIndex);
		static void SettingItemToThreadDescription(TInt aIndex, TDes& aBuf);
		static void SettingItemToSourceDescription(TInt aIndex, TDes& aBuf);
		static TInt MilliSecondsToMicroSeconds(TInt aMilliSeconds, TInt aRandomVariance=0);
		static TInt RandomNumber(TInt aMin, TInt aMax);
		static TSize ScreenSize();

private:
          /* Desctructor pure Virtual -> static class cannot be instantiated or inherited */
		virtual ~CLoadGenUtils()=0;	
	    static TInt64 iRandomNumberSeed;

};

#endif		  