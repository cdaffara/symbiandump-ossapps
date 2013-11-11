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
* Description:  Engine video quality level and container.
*
*/

#ifndef VIDEORECORDINGQUALITYLEVELS_H_
#define VIDEORECORDINGQUALITYLEVELS_H_


const TInt KMaxVideoStringParLength = 32;
const TInt KMaxAudioTypeStringParLength = 4;

const TInt KMaxVideoQltyLevelCount = 6;

const TInt KImgWidthSubQCIF  = 128; 
const TInt KImgHeightSubQCIF = 96; 

const TInt KImgWidthQCIF     = 176; 
const TInt KImgHeightQCIF    = 144; 

const TInt KImgWidthCIF      = 352; 
const TInt KImgHeightCIF     = 288; 

_LIT8(KVideoMimeType, "video/3gpp");

_LIT(KPreferredSupplier, "Nokia");

_LIT8(KVideoType, "video/H263-2000");

_LIT8(KAudioType, " AMR");

class CVideoRecordingQualityLevel : public CBase
    {
public:
    CVideoRecordingQualityLevel(){}
    virtual ~CVideoRecordingQualityLevel(){}
    
public:

    // The video recording frame size.
    TSize iFrameSize;

    // The video recording frame rate.
    TReal32 iFrameRate;

    // The video recording bit rate.
    TInt iBitRate;

    // A Boolean indicating if video recording audio is enabled or not.
    TBool iAudioEnabled;

    // The video recording audio bit rate.
    TInt iAudioBitRate;

    // The video MIME type (e.g. "video/3gpp").
    TBuf8<KMaxVideoStringParLength> iMimeType;

    // The preferred supplier of video controller (e.g. "Company X").
    TBuf<KMaxVideoStringParLength> iPreferredSupplier;

    // The video type (e.g. "video/H263-2000").
    TBuf8<KMaxVideoStringParLength> iVideoType;

    // The audio type expressed with four characters (e.g. " AMR").
    TBuf8<KMaxAudioTypeStringParLength> iAudioType;

    // The estimated video recording bit rate to storage (bits per second).
    TInt iStorageRate;
    };


class CVideoRecordingQualityLevels : public CBase
    {
public:
    /**
    * Destructor.
    */
    virtual ~CVideoRecordingQualityLevels();
    
    /**
    * Two-phased constructor.
    */
    static CVideoRecordingQualityLevels* NewL();

    /**
    * Initializes quality levels to hardcoded default values.
    * @return The count of quality levels initialized
    */
    TInt InitDefaultsL();
    
    /**
    * Gets the quality level count.
    * @return Quality level count
    */
    TInt Count() const;

    /**
    * Gets the specified quality level.
    * @param aIndex Quality level index
    * @return Video quality level
    */
    CVideoRecordingQualityLevel& At( 
        TInt aIndex ) const;
    
private:

    /**
    * C++ constructor.
    */
    CVideoRecordingQualityLevels();

    /**
    * Symbian OS 2nd phase constructor that can leave.
    */
    void ConstructL();
    
private:

    // Supported video recording quality levels.
    CArrayFixFlat<CVideoRecordingQualityLevel>* iQualityLevels;
    };

#endif /* VIDEORECORDINGQUALITYLEVELS_H_ */
