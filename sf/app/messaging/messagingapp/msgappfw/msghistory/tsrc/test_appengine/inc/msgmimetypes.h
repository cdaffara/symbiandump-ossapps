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
 * Description: Message mime type constants
 */

#ifndef __MSGMIMETYPES_H
#define __MSGMIMETYPES_H

// CONSTANTS

// MIME types (always 8-bit):
_LIT8( KMsgMimeTextPlain,   "text/plain" );
_LIT8( KMsgMimeVCard,       "text/X-vCard" );
_LIT8( KMsgMimeVCal,        "text/x-vCalendar" );
_LIT8( KMsgMimeICal,        "text/calendar" );
_LIT8( KMsgMimeImageJpeg,   "image/jpeg" );
_LIT8( KMsgMimeImageGif,    "image/gif" );
_LIT8( KMsgMimeImageWbmp,   "image/vnd.wap.wbmp" );
_LIT8( KMsgMimeImagePng,    "image/png" );
_LIT8( KMsgMimeImageBmp,    "image/bmp" );
_LIT8( KMsgMimeImageOtaBmp, "image/x-ota-bitmap" );
_LIT8( KMsgMimeImageSvg,    "image/svg+xml" );
_LIT8( KMsgMimeAudioAmr,    "audio/amr" );
_LIT8( KMsgMimeAudioSpMidi, "audio/sp-midi" );
_LIT8( KMsgMimeAudioQcelp,  "audio/qcelp" );
_LIT8( KMsgMimeVideo3gpp,   "video/3gpp" );
_LIT8( KMsgMimeVideo3gpp2,  "video/3gpp2" );
_LIT8( KMsgMimeVideoMp4,    "video/mp4" );
_LIT8( KMsgMimeSmil,        "application/smil" );
_LIT8( KMsgMimeTextXhtml,   "application/xhtml+xml" );
_LIT8( KMsgMimeUnknown,     "application/octet-stream" );
_LIT8( KEpocApp,            "x-epoc/x-app*" );
// CCommonContentPolicy uses 16-bit MIME types!
_LIT16( KMsgMimeSisPackage, "application/vnd.symbian.install" );

// File extensions (16-bit):
_LIT16( KMsgExtSmil1,       ".smil" );
_LIT16( KMsgExtSmil2,       ".smi" );
_LIT16( KMsgExtSmil3,       ".sml" );

#endif

// End of file
