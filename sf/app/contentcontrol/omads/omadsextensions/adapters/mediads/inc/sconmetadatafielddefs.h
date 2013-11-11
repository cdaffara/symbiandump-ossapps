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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#ifndef _SCONMETADATAFIELDDEFS_H_
#define _SCONMETADATAFIELDDEFS_H_

// Content type header
const TUint8 KSconMetadataHeaderAudio( 0x01 );
const TUint8 KSconMetadataHeaderExif ( 0x02 );
const TUint8 KSconMetadataHeaderVideo( 0x03 );

const TUint8 KSconMetadataHeaderVersion ( 0x03 ); // format version = 3

// fields for audio
const TUint8 KSconAudioID3Version   ( 0x01 );
const TUint8 KSconAudioTitle        ( 0x02 );
const TUint8 KSconAudioArtist       ( 0x03 );
const TUint8 KSconAudioAlbum        ( 0x04 );
const TUint8 KSconAudioYear         ( 0x05 );
const TUint8 KSconAudioComment      ( 0x06 );
const TUint8 KSconAudioAlbumTrack   ( 0x07 );
const TUint8 KSconAudioGenre        ( 0x08 );
const TUint8 KSconAudioComposer     ( 0x09 );
const TUint8 KSconAudioCopyright    ( 0x0a );
const TUint8 KSconAudioOriginalArtist( 0x0b );
const TUint8 KSconAudioUrl          ( 0x0c );
const TUint8 KSconAudioUserUrl      ( 0x0d );
const TUint8 KSconAudioJpeg         ( 0x0e );
const TUint8 KSconAudioVendor       ( 0x0f );
const TUint8 KSconAudioRating       ( 0x10 );
const TUint8 KSconAudioUniqueFileIdentifier( 0x11 );
const TUint8 KSconAudioDuration     ( 0x12 );
const TUint8 KSconAudioDate         ( 0x13 );
const TUint8 KSconAudioMimeType     ( 0x14 );
const TUint8 KSconAudioFilename     ( 0x15 );
const TUint8 KSconAudioFileSize     ( 0x16 );

// fields for exif
const TUint8 KSconExifThumbnail         ( 0x01 );
const TUint8 KSconExifDescription       ( 0x02 );
const TUint8 KSconExifMake              ( 0x03 );
const TUint8 KSconExifModel             ( 0x04 );
const TUint8 KSconExifDateTime          ( 0x05 );
const TUint8 KSconExifSoftware          ( 0x06 );
const TUint8 KSconExifCopyright         ( 0x07 );
const TUint8 KSconExifOrientation       ( 0x08 );
const TUint8 KSconExifXResolution1      ( 0x09 );
const TUint8 KSconExifXResolution2      ( 0x0a );
const TUint8 KSconExifYResolution1      ( 0x0b );
const TUint8 KSconExifYResolution2      ( 0x0c );
const TUint8 KSconExifResolutionUnit    ( 0x0d );
const TUint8 KSconExifYCbCrPositioning  ( 0x0e );
const TUint8 KSconExifIsoSpeedRatings   ( 0x0f );
const TUint8 KSconExifDateTimeOriginal  ( 0x10 );
const TUint8 KSconExifDateTimeDigitized ( 0x11 );
const TUint8 KSconExifMakerNote         ( 0x12 );
const TUint8 KSconExifUserComment       ( 0x13 );
const TUint8 KSconExifRelatedSoundFile  ( 0x14 );
const TUint8 KSconExifExposureTime1     ( 0x15 );
const TUint8 KSconExifExposureTime2     ( 0x16 );
const TUint8 KSconExifComponentsConfiguration ( 0x17 );
const TUint8 KSconExifFlash             ( 0x18 );
const TUint8 KSconExifColorSpace        ( 0x19 );
const TUint8 KSconExifPixelXDimension   ( 0x1a );
const TUint8 KSconExifPixelYDimension   ( 0x1b );
const TUint8 KSconExifExposureMode      ( 0x1c );
const TUint8 KSconExifWhiteBalance      ( 0x1d );
const TUint8 KSconExifSceneCaptureType  ( 0x1e );
const TUint8 KSconExifExposureProgram   ( 0x1f );
const TUint8 KSconExifApertureValue1    ( 0x20 );
const TUint8 KSconExifApertureValue2    ( 0x21 );
const TUint8 KSconExifExposureBiasValue1 ( 0x22 );
const TUint8 KSconExifExposureBiasValue2 ( 0x23 );
const TUint8 KSconExifMeteringMode      ( 0x24 );
const TUint8 KSconExifLightSource       ( 0x25 );
const TUint8 KSconExifFileSource        ( 0x26 );
const TUint8 KSconExifDigitalZoomRatio1 ( 0x27 );
const TUint8 KSconExifDigitalZoomRatio2 ( 0x28 );
const TUint8 KSconExifContrast          ( 0x29 );
const TUint8 KSconExifSaturation        ( 0x2a );
const TUint8 KSconExifSharpness         ( 0x2b );
const TUint8 KSconExifExifVersion       ( 0x2c );
const TUint8 KSconExifFlashPixVersion   ( 0x2d );
const TUint8 KSconExifThumbXResolution1 ( 0x2e );
const TUint8 KSconExifThumbXResolution2 ( 0x2f );
const TUint8 KSconExifThumbYResolution1 ( 0x30 );
const TUint8 KSconExifThumbYResolution2 ( 0x31 );
const TUint8 KSconExifThumbResolutionUnit   ( 0x32 );
const TUint8 KSconExifThumbCompression      ( 0x33 );
const TUint8 KSconExifThumbJpegInterchangeFormat        ( 0x34 );
const TUint8 KSconExifThumbJpegInterchangeFormatLength  ( 0x35 );
const TUint8 KSconExifShutterSpeedValue1 ( 0x36 );
const TUint8 KSconExifShutterSpeedValue2 ( 0x37 );
const TUint8 KSconExifBrightnessValue1  ( 0x38 );
const TUint8 KSconExifBrightnessValue2  ( 0x39 );
const TUint8 KSconExifCustomRendered    ( 0x3a );
const TUint8 KSconExifGainControl       ( 0x3b );
const TUint8 KSconExifGpsVersion        ( 0x3c );
const TUint8 KSconExifGPSLatitude    	( 0x3d );
const TUint8 KSconExifGPSLongitude      ( 0x3e );
const TUint8 KSconExifGPSAltitude       ( 0x3f );
const TUint8 KSconExifBitDepth	        ( 0x40 );

// fields for video
const TUint8 KSconVideoFormat			( 0x01 );
const TUint8 KSconVideoFrameRate		( 0x02 );
const TUint8 KSconVideoFrameSizeWidth	( 0x03 );
const TUint8 KSconVideoFrameSizeHeight	( 0x04 );
const TUint8 KSconVideoVideoBitRate		( 0x05 );
const TUint8 KSconVideoAudioBitRate		( 0x06 );
const TUint8 KSconVideoDuration			( 0x07 );
const TUint8 KSconVideoThumbnail		( 0x08 );
const TUint8 KSconVideoAudioStreamMimetype	( 0x09 );
const TUint8 KSconVideoVideoStreamMimetype	( 0x0a );

#endif // _SCONMETADATAFIELDDEFS_H_
