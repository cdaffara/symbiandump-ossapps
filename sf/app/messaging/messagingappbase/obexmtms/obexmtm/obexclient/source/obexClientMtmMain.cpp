// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: obexClientMtmMain.cpp $
// $Author: Stevep $
// $Revision: 4 $
// $Date: 25/03/02 9:46 $
// 
//


#include <e32std.h>
#include "obexClientMtmPanic.h"



GLDEF_C void gPanic(TObexMtmPanic aPanic)
	{
	_LIT (KObexClientMtmPanic, "ObexClientMtm");
	User::Panic(KObexClientMtmPanic,aPanic);
	}
