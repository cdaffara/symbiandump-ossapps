/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

#ifndef VCXMYVIDEOSTESTCOMMON_H
#define VCXMYVIDEOSTESTCOMMON_H

const TInt KVCXMYVideosTestMessageNotAccepted                    = 0;

const TInt KVCXMYVideosTestMessageDlSucceeded                    = 1000;
const TInt KVCXMYVideosTestMessageDlFailed                       = 1001;
const TInt KVCXMYVideosTestMessageDlPaused                       = 1002;
const TInt KVCXMYVideosTestMessageDlGeneral                      = 1003;
const TInt KVCXMYVideosTestMessageDlRunning                      = 1020;
const TInt KVCXMYVideosTestMessageDlCanceled                     = 1021;
const TInt KVCXMYVideosTestMessageDlProgressed                   = 1022;

const TInt KVCXMYVideosTestMessageCollectionMessageError         = 2000;
const TInt KVCXMYVideosTestMessageCollectionOpened               = 2001;
const TInt KVCXMYVideosTestMessagePlaylistOpened                 = 2002;
const TInt KVCXMYVideosTestMessageCollectionMedia                = 2003;
const TInt KVCXMYVideosTestMessageCommandComplete                = 2004;
const TInt KVCXMYVideosTestMessageGotMediasByKMpxId              = 2005;
const TInt KVCXMYVideosTestMessageMoveResp                       = 2006;
const TInt KVCXMYVideosTestMessageCopyResp                       = 2007;
const TInt KVCXMYVideosTestMessageListComplete                   = 2008;
const TInt KVCXMYVideosTestMessageCopyOrMoveStarted              = 2009;
const TInt KVCXMYVideosTestMessageDeleteStarted                  = 2010;
const TInt KVCXMYVideosTestMessageDeleteResp                     = 2011;
const TInt KVCXMYVideosTestMessageCommandCanceled                = 2012;

// These are only handled when WaitForMessage is called.
const TInt KVCXMYVideosTestMessageCollectionGeneral              = 3000;

const TInt KVCXMYVideosTestMessageMpxVideoInserted               = 4000;
const TInt KVCXMYVideosTestMessageMpxCategoryInserted            = 4001;
const TInt KVCXMYVideosTestMessageMpxAlbumInserted               = 4002;
const TInt KVCXMYVideosTestMessageMpxCategoryDeleted             = 4003;
const TInt KVCXMYVideosTestMessageMpxVideoDeleted                = 4004;
const TInt KVCXMYVideosTestMessageMpxAlbumDeleted                = 4005;
const TInt KVCXMYVideosTestMessageMpxVideoModified               = 4006;
const TInt KVCXMYVideosTestMessageMpxCategoryModified            = 4007;
const TInt KVCXMYVideosTestMessageMpxAlbumModified               = 4008;

const TInt KVCXMyVideosTestMessageVideoListOrderChanged          = 5000;                 

const TInt KVCXMYVideosTestErrorDownloadNotFound                 = 10001;
const TInt KVCXMYVideosTestErrorFileNotFound                     = 10002;

#endif      // VCXMYVIDEOSTESTCOMMON_H

// End of File
