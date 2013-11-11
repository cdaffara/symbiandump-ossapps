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
// $Workfile: irmsgtypeuid.h $
// $Author: Stevep $
// $Revision: 4 $
// $Date: 4/12/01 9:45 $
// 
//

/**
 @file
 @publishedAll
 @released
*/

#if !defined (__IRMSGTYPEUID_H__)
#define __IRMSGTYPEUID_H__

const TInt32 KUidMsgTypeIrTInt32 = 0x100053A4;
// This is defined in irobutil.h, which we don't control.
// const TUid KUidMsgTypeIr = {KUidMsgTypeIrTInt32};

const TInt32 KUidMsgTypeIr			 =	0x100053A4;

const TUid KUidMsgTypeIrUID = {KUidMsgTypeIr};	// we will revert this when we persuade PAN to make the other one a real TUid.

#endif // __IRMSGTYPEUID_H__
