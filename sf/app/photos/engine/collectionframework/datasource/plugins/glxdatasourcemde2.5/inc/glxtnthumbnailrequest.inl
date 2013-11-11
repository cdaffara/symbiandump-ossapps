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


inline TGlxThumbnailRequest::TGlxThumbnailRequest() :
    iPriorityMode(EPrioritizeQuality), iBitmapHandle(0),
    iDrmAllowed(EFalse), iFilter(EGlxThumbnailFilterNone)
    {
    }

inline TGlxThumbnailRequest::TGlxThumbnailRequest(TGlxMediaId aId,
        const TSize& aSizeClass, TPriorityMode aPriorityMode,
        TInt aBitmapHandle, TBool aDrmAllowed) :
    iId(aId), iSizeClass(aSizeClass), iPriorityMode(aPriorityMode), 
    iBitmapHandle(aBitmapHandle), iDrmAllowed(aDrmAllowed),
    iFilter(EGlxThumbnailFilterNone)
    {
    }

inline TGlxThumbnailRequest::TGlxThumbnailRequest(TGlxMediaId aId,
        const TSize& aSizeClass, TPriorityMode aPriorityMode,
        TInt aBitmapHandle, TBool aDrmAllowed,
        const TRect& aCroppingRect, TGlxThumbnailFilter aFilter) :
    iId(aId), iSizeClass(aSizeClass), iPriorityMode(aPriorityMode), 
    iBitmapHandle(aBitmapHandle), iDrmAllowed(aDrmAllowed),
	iCroppingRect(aCroppingRect), iFilter(aFilter)
    {
    }
