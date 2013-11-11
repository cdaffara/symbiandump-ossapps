/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fake implementation for the unittest_cxequalitypresetssymbian
*
*/


#include "imagingconfigmanager.h"


const int KNumberOfQualityLevels = 5;

// ---------------------------------------------------------------------------
// Constructor of CImagingConfigManager
// ---------------------------------------------------------------------------
//
CImagingConfigManager::CImagingConfigManager()
{
    iVideoQualitySets = new CArrayFixFlat< TVideoQualitySet >( 1 );
    iImageQualitySets = new CArrayFixFlat< TImageQualitySet >( 1 );

    // init
    for( TInt i = 0 ; i < KNumberOfQualityLevels ; i++ )
        {
        TVideoQualitySet videoset;
        initVideoQualitySet(videoset);
        iVideoQualitySets->AppendL(videoset);

        TImageQualitySet imageset;
        initImageQualitySet(imageset);
        iImageQualitySets->AppendL(imageset);
        }

}

// ---------------------------------------------------------------------------
// Destructor of CImagingConfigManager
// ---------------------------------------------------------------------------
//
CImagingConfigManager::~CImagingConfigManager()
{
    delete iVideoQualitySets;
    delete iImageQualitySets;
}




// CImagingConfigManager::NewL
CImagingConfigManager* CImagingConfigManager::NewL()
{
    return new CImagingConfigManager();
}

// returns no of video quality levels
TInt CImagingConfigManager::NumberOfVideoQualityLevels() const
{
    return iVideoQualitySets->Count();
}

// ---------------------------------------------------------------------------
// Get an array of video quality levels that are in use with the current
// product with given Camera/Display ID. If the ID is zero, then all levels
// dispite of the ID value are returned.
// ---------------------------------------------------------------------------
//
TInt CImagingConfigManager::GetVideoQualityLevelsL(
                                        CArrayFixFlat<TUint>& aLevels,
                                        TUint /*aCameraDisplayID*/ )
    {
    aLevels.Reset();
    aLevels.SetReserveL( iVideoQualitySets->Count() );
    for( TInt i = 0 ; i < NumberOfVideoQualityLevels() ; i++ )
        {
         aLevels.AppendL(
                iVideoQualitySets->At( i ).iVideoQualitySetLevel);
        }
    return KErrNone;
    }

// CImagingConfigManager::GetVideoQualitySet
TInt CImagingConfigManager::GetVideoQualitySet( TVideoQualitySet& aSet,
                                                TInt /*aLevel*/,
                                                TUint /*aCameraDisplayID*/ )
    {
    initVideoQualitySet(aSet);
    return KErrNone;
    }

// Get all settings for Camcorder
TInt CImagingConfigManager::GetCamcorderMMFPluginSettings(TCamcorderMMFPluginSettings& /*aSet*/) const
    {
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// Get number of defined image quality levels. This is always at least
// KNumberOfNominalLevels but can be higher
// ---------------------------------------------------------------------------
//
TInt CImagingConfigManager::NumberOfImageQualityLevels() const
    {
    return iImageQualitySets->Count();
    }

// ---------------------------------------------------------------------------
// Get an array of image quality levels that are in use with the current
// product with given Camera/Display ID. If the ID is zero, then all levels
// dispite of the ID value are returned.
// ---------------------------------------------------------------------------
//
TInt CImagingConfigManager::GetImageQualityLevelsL(
                                        CArrayFixFlat<TUint>& aLevels,
                                        TUint /*aCameraDisplayID*/ )
    {
    aLevels.Reset();
    for( TInt i = 0 ; i < NumberOfImageQualityLevels() ; i++ )
        {
        aLevels.AppendL(
                iImageQualitySets->At( i ).iImageQualitySetLevel );
        }
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Get image quality set associated with the given level and Camera/Display ID
// if it is nonzero. If the ID is zero, smallest matching ID set is returned.
// If there is no set associated with given intermediate
// level, then set from a nearest level is returned (dividable by
// KBasicQualityGranularity).
// ---------------------------------------------------------------------------
//
TInt CImagingConfigManager::GetImageQualitySet( TImageQualitySet& aSet,
                                                TInt /*aLevel*/,
                                                TUint /*aCameraDisplayID*/ )
{
    initImageQualitySet(aSet);
    return KErrNone;
}


void CImagingConfigManager::initImageQualitySet(TImageQualitySet& aSet)
{
    for( TUint i = 0 ; i < KMaxStringLength ; i++ )
        {
        aSet.iImageFileMimeType[ i ] = NULL;
        aSet.iImageFileExtension[ i ] = NULL;
        }
    aSet.iImageQualitySetLevel = 100;
    aSet.iImageWidth = 0;
    aSet.iImageHeight = 0;
    aSet.iImageEncoderUID.iUid = 0;
    aSet.iCompressionQuality = 0;
    aSet.iEstimatedSize = 0;
    aSet.iCameraDisplayId = 0;
    aSet.iCamcorderVisible = 100;

}

void CImagingConfigManager::initVideoQualitySet(TVideoQualitySet& aSet)
{
    for( TUint i = 0 ; i < KMaxStringLength ; i++ )
        {
        aSet.iVideoFileMimeType[ i ] = NULL;
        aSet.iVideoCodecMimeType[ i ] = NULL;
        aSet.iPreferredSupplier[ i ] = NULL;
        }
    aSet.iVideoQualitySetLevel = 100;
    aSet.iVideoWidth = 640;
    aSet.iVideoHeight = 360;
    aSet.iVideoFrameRate = 0.0;
    aSet.iVideoBitRate = 100;
    aSet.iVideoEncoderUID.iUid = 0;
    aSet.iRandomAccessRate = 0;
    aSet.iVideoPixelAspectRatioNum = 0;
    aSet.iVideoPixelAspectRatioDenom = 0;
    aSet.iAudioFourCCType = 0;
    aSet.iAudioBitRate = 100;
    aSet.iAudioSamplingRate = 0;
    aSet.iAudioChannels = 0;
    aSet.iAudioEncoderUID.iUid = 0;
    aSet.iAVSyncStartDelay = 0;
    aSet.iAVSyncResumeDelay = 0;
    aSet.iCameraDisplayId = 0;
    aSet.iCamcorderVisible = 100;

}
