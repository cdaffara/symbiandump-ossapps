// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CCALDATAFORMAT_H__ 
#define __CCALDATAFORMAT_H__ 

#include <e32cmn.h>

/** 
@file
@publishedAll
@released */ 

/** The UID for a VCal 1.0 data parser
Used in vCalendar Import. */
const TUid KUidVCalendar = {0x1020DB7B};


/** These flags control the behaviour of the data exchange parser.*/
/** This flag overrides the default character set when parsing 
vCalendar 1.0 files to Shift-JIS */
const TUint32 KCalDataExchangeDefaultShiftJIS = 0x00000001; 

/** This flag ensures that the STATUS property of an imported vCalendar
will map to the CCalEntry::TStatus value with exactly the same name. */
const TUint32 KCalDataExchangeImportStatusAsVCalendar = 0x00000002; 

#endif // __CCALDATAFORMAT_H__ 
