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
* Description:   Definitions for media attributes
*
*/



#ifndef __T_GLXMEDIACOLLECTIONINTERNALDEFS_H__
#define __T_GLXMEDIACOLLECTIONINTERNALDEFS_H__

#include <e32base.h>
#include <mpxattribute.h>
/**
* Content ID identifying progress messages
*/
const TInt KGlxMediaIdCollectionInternal   = 0x2000A7BA;

/**
*  Attributes supported for KGlxMediaIdCollectionInternal
*/
const TMPXAttributeData KGlxMediaCollectionInternalStartDate = { KGlxMediaIdCollectionInternal, 0x01 }; // TTime
const TMPXAttributeData KGlxMediaCollectionInternalEndDate = { KGlxMediaIdCollectionInternal, 0x02 }; // TTime
const TMPXAttributeData KGlxMediaCollectionInternalUsageCount = { KGlxMediaIdCollectionInternal, 0x04 }; // TInt
const TMPXAttributeData KGlxMediaCollectionInternalSystemItemType = { KGlxMediaIdCollectionInternal, 0x08 }; // TInt

const TMPXAttributeData KGlxMediaItemTypeImage = { KGlxMediaIdCollectionInternal, 0x16 }; // TInt
const TMPXAttributeData KGlxMediaItemTypeVideo = { KGlxMediaIdCollectionInternal, 0x32 }; // TInt
#endif // __T_GLXMEDIACOLLECTIONINTERNALDEFS_H__
