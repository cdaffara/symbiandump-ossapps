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
// $Workfile: obexClientMtmPanic.h $
// $Author: Stevep $
// $Revision: 3 $
// $Date: 9/11/01 15:22 $
// 
//


#if !defined(__OBEXCLIENTMTMPANIC_H__)
#define __OBEXCLIENTMTMPANIC_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

//Panic values for the Obex mtm
enum TObexMtmPanic
	{
	EObexMtmNoCMsvEntrySet, //< No CMsvEntry set for attepted operation
	EObexMtmNotAMessageEntry, //< Attempt to manipulate an entry which is not a message
	EObexMtmNoText, //< No text in filename, for filename externalise attempt
	EObexHeaderAlreadyLoaded //< Attempting to load an already loaded obex headers (Not used in code)
	};

GLREF_C void gPanic(TObexMtmPanic aPanic);

#endif // __OBEXCLIENTMTMPANIC_H__
