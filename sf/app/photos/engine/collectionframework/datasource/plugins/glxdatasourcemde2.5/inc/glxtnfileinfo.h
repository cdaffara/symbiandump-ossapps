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


#ifndef C_GLXTNFILEINFO_H
#define C_GLXTNFILEINFO_H

#include <e32base.h>

/**
 * Object holding information about a file to be thumbnailed.
 *
 */
class CGlxtnFileInfo : public CBase
    {
public:
	inline CGlxtnFileInfo();
    inline	~CGlxtnFileInfo();

    /**
    * Copy info from one object to another.
    */
    inline void CopyInfoL(CGlxtnFileInfo& aInfo);

    /**
    * Get the full path to the media file.
    * @return Media file path.
    */
    inline const TDesC& FilePath() const;

    /**
    * Set the full path to the media file.
    * @param aPath Media file path (takes ownership).
    */
    inline void SetFilePath(HBufC* aPath);

    /**
    * Set the full path to the media file.
    * @param aPath Media file path.
    */
    inline void SetFilePathL(const TDesC& aPath);

    /**
    * Set the full path to the media file.
    * @param aIsVideo Set to true if the file is a video.
    * @param aIsProtected Set to true if the file is DRM protected.
    */
    void IdentifyFileL(TBool& aIsVideo, TBool& aIsProtected);

public:
    /** Size of the media file */
    TInt iFileSize;
    /** Last modified time of the media file */
    TTime iFileTime;
    /** True if the thumbnail is temporary and should not be stored */
    TBool iTemporary;
    TBool iIsVideo;
    TBool iIsProtected;

private:
    /** Full path of the media file */
    HBufC* iFilePath;
    };

#include "glxtnfileinfo.inl"
   
#endif // C_GLXTNFILEINFO_H
