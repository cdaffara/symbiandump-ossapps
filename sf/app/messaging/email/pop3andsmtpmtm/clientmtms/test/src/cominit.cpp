// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//#include "testenv.h"
#include <e32hal.h>
#include <c32comm.h>

#if defined (__WINS__)
#define PDD_NAME		_L("ECDRV")
#define LDD_NAME		_L("ECOMM")
#define LDD_FNAME		_L("ECOMM")
#else
#define PDD_NAME		_L("EUART1")
#define PDD2_NAME		_L("EUART2")
#define LDD_NAME		_L("ECOMM")
#define LDD_FNAME		_L("FCOMM")
#endif

GLDEF_C void CommInitL()
	{
	TInt err;
	err=StartC32();
	if (err!=KErrNone && err!=KErrAlreadyExists)
		{
		User::Leave(err);
		}
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	}

