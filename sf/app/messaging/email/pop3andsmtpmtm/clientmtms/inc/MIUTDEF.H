// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This header file defines the bit flags that may be used to instruct
// the CImMessageOperation class how to construct a new Email message
// 
//

#if !defined (__MIUTDEF_H__)
#define __MIUTDEF_H__

/** Type for email creation flags used in the @c aMsvEmailTypeList parameter of CImEmailOperation 
functions.

@see KMsvEmailTypeListMHTMLMessage
@see KMsvEmailTypeListInvisibleMessage
@see KMsvEmailTypeListMessageInPreparation 
@publishedAll
@released
*/
typedef TUint TMsvEmailTypeList;

/** Flags that a new message should be of MHTML type. 

If this is not set, a plain-text message is created. 

@publishedAll
@released
@see TMsvEmailTypeList
*/
const TMsvEmailTypeList KMsvEmailTypeListMHTMLMessage				= 0x00000001;	// if not set Email is a "normal" (i.e. non-MHTML) Email message
/** Flags that a new message should be invisible.
@publishedAll
@released
@see TMsvEmailTypeList
*/
const TMsvEmailTypeList KMsvEmailTypeListInvisibleMessage			= 0x00000002;	// if set, message stays invisible on completion

/** Flags that a new message should have the "in preparation" flag set. 
@publishedAll
@released
@see TMsvEmailTypeList
*/
const TMsvEmailTypeList KMsvEmailTypeListMessageInPreparation		= 0x00000004;	// if set, message stays marked "In preparation" on completion

#endif //MIUTDEF.H

