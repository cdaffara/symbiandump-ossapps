/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  m3u playlist definitions
*
*/



#ifndef MPXM3UPLAYLISTDEFS_H
#define MPXM3UPLAYLISTDEFS_H

// CONSTANTS
_LIT(KMPXM3UTagExtm3u, "#EXTM3U");
_LIT(KMPXM3UTagExtinf, "#EXTINF:");
_LIT(KMPXM3UTagExt, "#");
_LIT(KMPXM3UPoint, ",");
_LIT(KMPXM3ULineChange, "\n");
_LIT(KMPXM3UAbsPath, ":\\");
_LIT(KMPXM3UExtension, ".m3u");

const TInt KMPXM3UCarriageReturn = 13;
const TInt KMPXM3ULengthOfLineChange = 1;
const TInt KMPXM3UMaxLengthOfExtinfStaticPart = 16;
const TInt KMPXM3UMaxTimeEntry = 999999; // ~278 hours
const TInt KMPXM3UNoOffset = 0;
// A 16-bit Unicode character may take 3 bytes at maximum when encoded to UTF-8
const TInt KMPXM3UUtf8ConvMultiplier = 3;

const TInt KMPXM3UIgnoreTimeEntry = -1;
const TInt KMPXM3UPlaylistMaxItemCount = KMaxTInt;


const TInt KPlaylistSampleLength = 10000;     // bytes
const TInt KPlaylistMaxSampleLength = 130000; // bytes
const TInt KMPXM3UNumOfLinesToProcess = 200;
const TInt KMPXM3UNumOfMediasToProcess = 200;

// MODULE DATA STRUCTURES
enum TMPXM3UPlaylistLineType
    {
    EMPXM3UPlaylistLineTypeExtinf = 1,
    EMPXM3UPlaylistLineTypePath = 2,
    EMPXM3UPlaylistLineTypeNotSupported = 3,
    EMPXM3UPlaylistLineTypeCorrupted = 4
    };

#endif      // MPXM3UPLAYLISTDEFS_H   
            
// End of File
