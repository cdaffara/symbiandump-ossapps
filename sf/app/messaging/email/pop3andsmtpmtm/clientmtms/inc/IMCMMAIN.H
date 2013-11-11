
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
 @publishedAll
 @released
*/

#if !defined (__IMCMMAIN_H__)
#define __IMCMMAIN_H__

#if !defined (__E32STD_H__)
#include <e32std.h>
#endif

#if !defined (__BARSC_H__)
#include <barsc.h>		// RResourceFile
#endif

GLREF_C void OpenResourceFileL(RResourceFile& aFile, RFs& anFs);
GLDEF_C void CloseResourceFile( TAny* aResourceFile );


#endif
