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


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
inline CGlxtnFileInfo::CGlxtnFileInfo()
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
inline CGlxtnFileInfo::~CGlxtnFileInfo()
    {
    delete iFilePath;
    }

// -----------------------------------------------------------------------------
// CopyInfoL
// -----------------------------------------------------------------------------
//
inline void CGlxtnFileInfo::CopyInfoL(CGlxtnFileInfo& aInfo)
    {
    SetFilePathL(aInfo.FilePath());
    iFileSize = aInfo.iFileSize;
    iFileTime = aInfo.iFileTime;
    iTemporary = aInfo.iTemporary;
    iIsVideo = aInfo.iIsVideo;
    iIsProtected = aInfo.iIsProtected;
    }

// -----------------------------------------------------------------------------
// FilePath
// Get the full path to the media file.
// -----------------------------------------------------------------------------
//
inline const TDesC& CGlxtnFileInfo::FilePath() const
    {
    return *iFilePath;
    }

// -----------------------------------------------------------------------------
// SetFilePathL
// Set the full path to the media file.
// -----------------------------------------------------------------------------
//
inline void CGlxtnFileInfo::SetFilePath(HBufC* aPath)
    {
    delete iFilePath;
    iFilePath = aPath;
    }

// -----------------------------------------------------------------------------
// SetFilePathL
// Set the full path to the media file.
// -----------------------------------------------------------------------------
//
inline void CGlxtnFileInfo::SetFilePathL(const TDesC& aPath)
    {
    delete iFilePath;
    iFilePath = NULL;
    iFilePath = aPath.AllocL();
    }
