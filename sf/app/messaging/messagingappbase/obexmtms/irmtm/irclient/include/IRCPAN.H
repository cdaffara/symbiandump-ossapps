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
// $Workfile: IRCPAN.H $
// $Author: Stevep $
// $Revision: 2 $
// $Date: 9/11/01 15:29 $
// 
//

#if !defined(__IrCPAN_H__)
#define __IrCPAN_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

enum TIrPanic
	{
	EIrMTMNoCMsvEntrySet,
	EIrMTMNotAMessageEntry,
	EIrMTMNoText
	};

GLREF_C void gPanic(TIrPanic aPanic);

#endif
