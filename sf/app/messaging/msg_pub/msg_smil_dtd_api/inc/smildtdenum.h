// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// smildtdenum.h
// 
//
/**
 * @file 
 * @internalComponent
 * @released
 */
#ifndef __SMILDTDENUM_H__
#define __SMILDTDENUM_H__

#include <e32def.h>

// Structures and definitions for child element checking.
// The child element checks are managed by a state machine and this type defines
// allowed transitions.
struct TSMILDTDChildStateType
/**
@internalComponent
@released
*/
	{
	TInt         FromState;	// Starting state - 0 = initial state
	TInt         ToState;	// Next state - -1 = terminating state
	const TText *TagName;	// Name of tag causing the transition, empty string = finished
	TInt         TagLength;	// Length of tag string (helps to make the descriptor)
	};

#endif // __SMILDTDENUM_H__