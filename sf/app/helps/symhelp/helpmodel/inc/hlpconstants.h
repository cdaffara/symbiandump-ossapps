// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @released 
*/

#ifndef __HLPCONSTANTS_H__
#define __HLPCONSTANTS_H__

// System includes
#include <e32std.h>

// Constants
const TUid KUidHlpApp		=	{ 0x0100052b3 };	// UID 3
const TUid KUidHlpFile		=	{ 0x0100052b4 };	// UID 2
const TUid KUidHlpDbStream	=	{ 0x0100052b5 };	// Database stream in the help file
const TUid KUidHlpUidStream	=	{ 0x0100052b6 };	// UID stream in the help file
//
const TInt KTitleColumn	= 1;
const TInt KIdColumn = 2;
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
const TInt KMaxTitleColumn = 120;
#endif
const TInt KHlpSQLMaxLength = 512;
const TInt KHlpMaxTextColLength = 200;
const TInt KHlpMaxContextColLength = 32;




#endif
