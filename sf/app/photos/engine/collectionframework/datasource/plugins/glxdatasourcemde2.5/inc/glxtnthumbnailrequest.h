/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef T_GLXTHUMBNAILREQUEST_H
#define T_GLXTHUMBNAILREQUEST_H

#include "glxmediaid.h"
#include "glxthumbnailinfo.h"

class TGlxThumbnailRequest 
	{
public:
	enum TPriorityMode
		{
		EPrioritizeSpeed,	// Speed of thumbnail provisioning is mode important that quality
		EPrioritizeQuality,	// Quality of thumbnail provisioning is mode important that speed
		};

public:
    inline TGlxThumbnailRequest();

    inline TGlxThumbnailRequest(TGlxMediaId aId,
        const TSize& aSizeClass, TPriorityMode aPriorityMode,
        TInt aBitmapHandle, TBool aDrmAllowed);

    inline TGlxThumbnailRequest(TGlxMediaId aId,
        const TSize& aSizeClass, TPriorityMode aPriorityMode,
        TInt aBitmapHandle, TBool aDrmAllowed,
        const TRect& aCroppingRect, TGlxThumbnailFilter aFilter);

public:
	TGlxMediaId iId;
	TSize iSizeClass;
	TPriorityMode iPriorityMode; 
	TInt iBitmapHandle; // Handle of target bitmap for the thumbnail
	TBool iDrmAllowed;  // True if client has DRM capability
    TRect iCroppingRect;
    TGlxThumbnailFilter iFilter;
	};

#include "glxtnthumbnailrequest.inl"

#endif // T_GLXTHUMBNAILREQUEST_H
