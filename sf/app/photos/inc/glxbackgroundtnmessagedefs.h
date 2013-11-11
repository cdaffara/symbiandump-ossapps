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
* Description:   Background Thumbnail messages
*
*/



#ifndef _GLXBACKGROUNDTNMESSAGEDEFS_H
#define _GLXBACKGROUNDTNMESSAGEDEFS_H

#include <e32base.h>

class CFbsBitmap;

/**
 * Background Thumbnail message
 */
const TInt KGlxMessageIdBackgroundThumbnail         = 0x2000A76E;

/**
 *  Attributes supported for KGlxMessageIdBackgroundThumbnail
 */    
/**
 *  TMPXItemId (as def'd in CollectionPath)
 *  ID of media changed
 */
const TMPXAttributeData KGlxBackgroundThumbnailMediaId={KGlxMessageIdBackgroundThumbnail, 0x01};
/**
 *  TSize
 *  (Optional) size of thumbnail created
 */
const TMPXAttributeData KGlxBackgroundThumbnailSize={KGlxMessageIdBackgroundThumbnail, 0x02};
/**
 *  TInt
 *  (Optional) size of thumbnail created
 */
const TMPXAttributeData KGlxBackgroundThumbnailError={KGlxMessageIdBackgroundThumbnail, 0x04};


#endif // _GLXBACKGROUNDTNMESSAGEDEFS_H
