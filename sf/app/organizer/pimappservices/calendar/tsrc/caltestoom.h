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

#ifndef __CALTESTOOM_H__
#define __CALTESTOOM_H__

#include <e32base.h>

class CCalSession;

class MCalTestOomCallBack
	{
public:
	enum TType
		{
		EClient,
		EServer
		};
public:
	virtual void OomTestL(TType aType, TInt aFailAt) = 0;
	};

class CalTestOom : public CBase
	{
public:
	IMPORT_C static void OutOfMemoryTestL(MCalTestOomCallBack& aCallBack, MCalTestOomCallBack::TType aType, CCalSession& aCalSession);
	};
	
#endif // __CALTESTOOM_H__
