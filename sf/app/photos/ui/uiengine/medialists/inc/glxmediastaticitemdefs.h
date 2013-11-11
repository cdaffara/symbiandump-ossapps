/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Static item media items
*
*/




#ifndef _GLXMEDIASTATICITEMDEFS_H
#define _GLXMEDIASTATICITEMDEFS_H

#include <mpxattribute.h>

/**
 * Content ID identifying static item category 
 */
const TInt KGlxMediaIdStaticItem					= 0x200071C9;
const TInt KGlxMediaStaticItemPosition				= 0x00;
const TInt KGlxMediaStaticItemCommand				= 0x01;

const TInt KGlxStaticSpaceId						= 0x200071CA; /// @deprecated, use KGlxStaticItemIdSpaceId
const TInt KGlxStaticItemIdSpaceId                  = KGlxStaticSpaceId;

const TMPXAttributeData KGlxAttrMediaStaticItemPosition={KGlxMediaIdStaticItem, KGlxMediaStaticItemPosition};
/**
 *  TBool, ETrue is static, EFalse is not
 */
const TMPXAttributeData KGlxMediaStaticItem = { KGlxMediaIdStaticItem, 0x00 }; 

#endif  // _GLXMEDIASTATICITEMDEFS_H