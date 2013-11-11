/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SConMetadata implementation
*
*/


// INCLUDE FILES

#include <utf.h>  // for CnvUtfConverter
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>
#include <MetaDataField.hrh>
#include <caf/content.h>
#include <caf/data.h>
#include <ExifRead.h>
#include <ExifTag.h>

#include "sconmetadata.h"
#include "sconconmltask.h"
#include "sconpcdconsts.h"
#include "sconvideoparser.h"
#include "debug.h"
#include "sconmetadatafielddefs.h"



const TInt KBufferSize(1024);
// Size of buffer to load from file start to get EXIF thumbnail
const TInt KJpegLoadBufferSize = 65536; // 64k


const TUint32 KFieldMaxLength = 0xFFFFFFFF; // four bytes reserved for "length" information

// GPS related Exif IDs
const TUint16 KGPSLatitudeRef	= 0x01;
const TUint16 KGPSLatitude 	 	= 0x02;
const TUint16 KGPSLongitudeRef	= 0x03;
const TUint16 KGPSLongitude		= 0x04;
const TUint16 KGPSAltitudeRef	= 0x05;
const TUint16 KGPSAltitude		= 0x06;


// extensions for exif parser
_LIT(KJpgExt, ".jpg");
_LIT(KJpegExt, ".jpeg");
// extensions for audio metadata parser
_LIT(KAacExt, ".aac");
_LIT(KMp3Ext, ".mp3");
_LIT(KMp4Ext, ".mp4");
_LIT(KWmaExt, ".wma");
// extensions for video metadata parser
_LIT(KM4aExt, ".m4a");
_LIT(K3gpExt, ".3gp");


// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SConMetadata::ProcessTask()
// ProcessTask
// -----------------------------------------------------------------------------
//
void SConMetadata::ProcessTask( CSConTask& aTask, RFs& aFs )
    {
    TRACE_FUNC_ENTRY;
    TInt err(KErrNone);
    if ( aTask.GetServiceId() == EGetMetadata )
        {
        LOGGER_WRITE_1( "SConMetadata::ProcessTask() : file %S",
            &aTask.iGetMetadataParams->iFilename );
        
#ifdef _DEBUG
        LOGGER_WRITE( "start.. " );
        TTimeIntervalMicroSeconds sec;
        TTime start;
        TTime end;
        start.HomeTime();
#endif
        
        TParsePtrC filename( aTask.iGetMetadataParams->iFilename );
        
        if ( filename.Ext().CompareF( KJpgExt ) == 0
            || filename.Ext().CompareF( KJpegExt ) == 0 )
            {
            // jpg file, try to read exif
            LOGGER_WRITE( "jpg file, Try to read exif" );
            TRAP( err, ReadExifDataL( aTask, aFs ) );
            }
        else if ( filename.Ext().CompareF( KMp4Ext ) == 0
            || filename.Ext().CompareF( K3gpExt ) == 0 )
        	{
        	LOGGER_WRITE( "Try to read video" );
        	TRAP( err, GetVideoMetadataL( aTask, aFs ) );
        	}
        else if ( filename.Ext().CompareF( KAacExt ) == 0
            || filename.Ext().CompareF( KM4aExt ) == 0
            || filename.Ext().CompareF( KMp3Ext ) == 0
            || filename.Ext().CompareF( KWmaExt ) == 0 )
            {

            // audio file, try to read audio
            LOGGER_WRITE( "Try to read audio" );
            TRAP( err, GetAudioMetadataL( aTask ) );
            }
        else
            {
            LOGGER_WRITE( "File extension not supported" );
            err = KErrNotSupported;
            }
        
#ifdef _DEBUG
        end.HomeTime();
        sec = end.MicroSecondsFrom( start );
        
        LOGGER_WRITE_1( "trapErr %d", err );
        LOGGER_WRITE_1( "duration: %Ld", sec.Int64() );
#endif
        }
    else
        {
        LOGGER_WRITE( "SConMetadata::ProcessTask() : not supported" );
        err = KErrNotSupported;
        }
    
    LOGGER_WRITE( "SConMetadata::ProcessTask() : CompleteTask" );
    
    TInt progress( KSConCodeTaskCompleted );        
    TBool complete ( ETrue );
    switch( err )
        {
        case KErrNone :
            progress =  KSConCodeTaskCompleted;
            break;
        case KErrNotFound :
        case KErrBadName :
            progress =  KSConCodeNotFound;
            break;
        default :
            progress = KSConCodeConflict;
            break;
        }
        
    aTask.SetCompleteValue( complete );
    aTask.SetProgressValue( progress );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// SConMetadata::GetAudioMetadataL()
// Read audio metadata
// -----------------------------------------------------------------------------
//
void SConMetadata::GetAudioMetadataL( CSConTask& aTask )
    {
    TRACE_FUNC_ENTRY;
    CMetaDataUtility* metadataUtil = CMetaDataUtility::NewL();
    CleanupStack::PushL( metadataUtil );
    
    LOGGER_WRITE( "SConMetadata::GetAudioMetadataL() : OpenFileL" );
    metadataUtil->OpenFileL( aTask.iGetMetadataParams->iFilename );
    
    TInt count = metadataUtil->MetaDataCount();
    LOGGER_WRITE_1( "SConMetadata::GetAudioMetadataL() : MetaDataCount %d", count );
    if ( count > 0 )
        {
        const CMetaDataFieldContainer& fields = metadataUtil->MetaDataFieldsL();
        count = fields.Count();
        
        CBufFlat* buffer = CBufFlat::NewL( KBufferSize );
        CleanupStack::PushL( buffer );
        buffer->Reset();
        
        
        TInt offset(0);
        // write object header
        // Item type (1 byte) + Version (1 byte) = 2 bytes
        buffer->ExpandL( offset, 2 );
    
        // header id
        TUint8 value( KSconMetadataHeaderAudio );
        buffer->Write( offset, TPtrC8(&value, 1) );
        offset++;
        
        // header version
        value = KSconMetadataHeaderVersion;
        buffer->Write( offset, TPtrC8(&value, 1) );
        offset++;
        
        // Add ID3 field
        value = metadataUtil->ID3Version();
        AppendTUintDataFieldL( buffer, value, KSconAudioID3Version );
        
        TMetaDataFieldId fieldId;
        TPtrC fieldData;
        
        for ( TInt i = 0; i < count; i++ )
            {
            // get field id
            fields.FieldIdAt( i , fieldId );
            LOGGER_WRITE_2( "AudioFieldId( %d ): 0x%02x", i, fieldId );
            
            if ( fieldId == EMetaDataJpeg )
                {
                TPtrC8 field8( fields.Field8( fieldId ) );
                AppendByteDataFieldL(
                    buffer,
                    field8,
                    AudioFieldId( fieldId ) );
                }
            else
                {
                // get field data and add UTF-8 formatted text to buffer
                fieldData.Set( fields.At( i , fieldId ) );
                AppendUtf8DataFieldL( buffer, fieldData, AudioFieldId( fieldId ) );
                }
            
            }
        
        if ( buffer->Size() > 0 )
            {
            LOGGER_WRITE_1( "SConMetadata::GetAudioMetadataL() : buffer->Size() %d", buffer->Size() );
            if ( aTask.iGetMetadataParams->iData )
                {
                delete aTask.iGetMetadataParams->iData;
                aTask.iGetMetadataParams->iData = NULL;
                }
            //Initialize the task data buffer
            aTask.iGetMetadataParams->iData = HBufC8::NewL( buffer->Size() );
            TPtr8 dataPtr = aTask.iGetMetadataParams->iData->Des();
            
            buffer->Read( 0, dataPtr, buffer->Size() );
            LOGGER_WRITE_1( "SConMetadata::GetAudioMetadataL() dataPtr len: %d", dataPtr.Length());
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    
    CleanupStack::PopAndDestroy( metadataUtil );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// SConMetadata::GetVideoMetadataL()
// Read video metadata
// -----------------------------------------------------------------------------
//
void SConMetadata::GetVideoMetadataL( CSConTask& aTask, RFs& aFs )
	{
	TRACE_FUNC_ENTRY;
	CBufFlat* buffer = CBufFlat::NewL( KBufferSize );
    CleanupStack::PushL( buffer );
    buffer->Reset();
    
    TInt offset(0);
    // write object header
    // Item type (1 byte) + Version (1 byte) = 2 bytes
    buffer->ExpandL( offset, 2 );

    // header id
    TUint8 value( KSconMetadataHeaderVideo );
    buffer->Write( offset, TPtrC8(&value, 1) );
    offset++;
    
    // header version
    value = KSconMetadataHeaderVersion;
    buffer->Write( offset, TPtrC8(&value, 1) );
    offset++;
    
	
	CSConVideoParser* videoParser = CSConVideoParser::NewLC();
	
	videoParser->OpenFileL( aFs, aTask.iGetMetadataParams->iFilename );
	LOGGER_WRITE( "Open completed" );
	
	const TUint KFormatMaxLength = 100;
	TBuf<KFormatMaxLength> formatMimeType;
    formatMimeType.Copy( videoParser->VideoFormatMimeTypeL() );
    LOGGER_WRITE_1( "formatMimeType: %S", &formatMimeType);
    AppendUtf8DataFieldL( buffer, formatMimeType, KSconVideoFormat );
	
    // we can't use TReal format, so convert frame rate to frames/ms (or 1000 frames/s)
	TReal32 frameRate = videoParser->VideoFrameRateL();
	const TUint KFrameRateMultiplier = 1000;
    TInt frm = frameRate * KFrameRateMultiplier; 
    LOGGER_WRITE_1( "frameRate: %f", frameRate );
    LOGGER_WRITE_1( "frm: %d", frm );
    AppendTUintDataFieldL( buffer, frm, KSconVideoFrameRate );
    
    TSize size;
    videoParser->VideoFrameSizeL( size );
    LOGGER_WRITE_2( "VideoFrameSizeL: %d, %d", size.iWidth, size.iHeight);
    AppendTUintDataFieldL( buffer, size.iWidth, KSconVideoFrameSizeWidth );
    AppendTUintDataFieldL( buffer, size.iHeight, KSconVideoFrameSizeHeight );
    


    const TUint8 KSconVideoAudioStreamMimetype	( 0x09 );
    const TUint8 KSconVideoVideoStreamMimetype	( 0x0a );
    
    TInt videoBitRate =  videoParser->VideoBitRateL();
    LOGGER_WRITE_1( "videoBitRate: %d", videoBitRate);
    AppendTUintDataFieldL( buffer, videoBitRate, KSconVideoVideoBitRate );
    
	TInt audioBitRate = videoParser->AudioBitRateL();
	LOGGER_WRITE_1( "audioBitRate: %d", audioBitRate);
	AppendTUintDataFieldL( buffer, audioBitRate, KSconVideoAudioBitRate );

	TInt durationMs = videoParser->DurationL();
	LOGGER_WRITE_1( "duration ms: %d", durationMs);
	AppendTUintDataFieldL( buffer, durationMs, KSconVideoDuration );
	
	TPtrC8 thumbnail = videoParser->Thumbnail();
	if ( thumbnail.Length() > 0 )
	    {
	    AppendByteDataFieldL( buffer, thumbnail, KSconVideoThumbnail );
	    }
	
	TPtrC audioMimeType = videoParser->AudioMimeTypeL();
	if ( audioMimeType.Length() > 0 )
		{
		AppendUtf8DataFieldL( buffer, audioMimeType, KSconVideoAudioStreamMimetype );
		}
	
	TPtrC videoMimeType = videoParser->VideoMimeTypeL();
	if ( videoMimeType.Length() > 0 )
		{
		AppendUtf8DataFieldL( buffer, formatMimeType, KSconVideoVideoStreamMimetype );
		}
	
	CleanupStack::PopAndDestroy( videoParser );
	LOGGER_WRITE( "videoParser deleted" );
	
	
	if ( buffer->Size() > 0 )
        {
        if ( aTask.iGetMetadataParams->iData )
            {
            delete aTask.iGetMetadataParams->iData;
            aTask.iGetMetadataParams->iData = NULL;
            }
        //Initialize the task data buffer
        aTask.iGetMetadataParams->iData = HBufC8::NewL( buffer->Size() );
        TPtr8 dataPtr = aTask.iGetMetadataParams->iData->Des();
        
        buffer->Read( 0, dataPtr, buffer->Size() );
        }
    CleanupStack::PopAndDestroy( buffer );
	
	
    TRACE_FUNC_EXIT;
	}

// -----------------------------------------------------------------------------
// SConMetadata::AudioFieldId()
// Map TMetaDataFieldId to KSconAudio field id
// -----------------------------------------------------------------------------
//
TUint8 SConMetadata::AudioFieldId( const TMetaDataFieldId fieldId )
    {
    TUint8 ret(NULL);
    switch( fieldId )
        {
        case EMetaDataSongTitle :
            ret = KSconAudioTitle;
            break;
        case EMetaDataArtist :
            ret = KSconAudioArtist;
            break;
        case EMetaDataAlbum :
            ret = KSconAudioAlbum;
            break;
        case EMetaDataYear :
            ret = KSconAudioYear;
            break;
        case EMetaDataComment :
            ret = KSconAudioComment;
            break;
        case EMetaDataAlbumTrack :
            ret = KSconAudioAlbumTrack;
            break;
        case EMetaDataGenre :
            ret = KSconAudioGenre;
            break;
        case EMetaDataComposer :
            ret = KSconAudioComposer;
            break;
        case EMetaDataCopyright :
            ret = KSconAudioCopyright;
            break;
        case EMetaDataOriginalArtist :
            ret = KSconAudioOriginalArtist;
            break;
        case EMetaDataUrl :
            ret = KSconAudioUrl;
            break;
        case EMetaDataUserUrl :
            ret = KSconAudioUserUrl;
            break;
        case EMetaDataJpeg :
            ret = KSconAudioJpeg;
            break;
        case EMetaDataVendor :
            ret = KSconAudioVendor;
            break;
        case EMetaDataRating :
            ret = KSconAudioRating;
            break;
        case EMetaDataUniqueFileIdentifier :
            ret = KSconAudioUniqueFileIdentifier;
            break;
        case EMetaDataDuration :
            ret = KSconAudioDuration;
            break;
        case EMetaDataDate :
            ret = KSconAudioDate;
            break;
        default :
            LOGGER_WRITE( "SConMetadata::AudioFieldId : ERR field not defined!" );
            break;
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// SConMetadata::GetExifTagL( CExifRead* aReader, const TUint8 aTagID )
// This function is used to read exif data that might leave.
// -----------------------------------------------------------------------------
//
HBufC8* SConMetadata::GetExifTagL( CExifRead* aReader, const TUint8 aTagID )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( " aTagID: 0x%02x", aTagID );
    HBufC8* tempBuf( NULL );
    
    switch( aTagID )
        {
        case KSconExifThumbnail:
            tempBuf = aReader->GetThumbnailL();
            break;
        case KSconExifDescription:
            tempBuf = aReader->GetImageDescriptionL();
            break;
        case KSconExifMake:
            tempBuf = aReader->GetMakeL();
            break;
        case KSconExifModel:
            tempBuf = aReader->GetModelL();
            break;
        case KSconExifDateTime:
            tempBuf = aReader->GetDateTimeL();
            break;
        case KSconExifSoftware:
            tempBuf = aReader->GetSoftwareL();
            break;
        case KSconExifCopyright:
            tempBuf = aReader->GetCopyrightL();
            break;
        case KSconExifIsoSpeedRatings:
            tempBuf = aReader->GetIsoSpeedRatingsL();
            if ( tempBuf )
                {
                TInt isoSpeed = ReadTUint32( tempBuf->Des() );
                LOGGER_WRITE_1(" isoSpeed: %d", isoSpeed);
                delete tempBuf;
                
                const TInt maxLength(5);
                // no need to push on cleanupstack as leave cannot
                // happen before returning tempBuf.
                tempBuf = HBufC8::NewL(maxLength);
                TPtr8 temp = tempBuf->Des();
                temp.Num( isoSpeed );
                }
            break;
        case KSconExifDateTimeOriginal:
            tempBuf = aReader->GetDateTimeOriginalL();
            break;
        case KSconExifDateTimeDigitized:
            tempBuf = aReader->GetDateTimeDigitizedL();
            break;
        case KSconExifMakerNote:
            // makernote contents are up to the manufacturer
            // not needed.
            User::Leave( KErrNotSupported );
            break;
        case KSconExifUserComment:
            tempBuf = aReader->GetUserCommentL();
            break;
        case KSconExifRelatedSoundFile:
            tempBuf = aReader->GetRelatedSoundFileL();
            break;
        default:
            User::Leave( KErrNotSupported );
        }
    TRACE_FUNC_EXIT;
    return tempBuf;
    }

// -----------------------------------------------------------------------------
// SConMetadata::ReadTUint32()
// Convert 8-bit binary data to unsigned integer 
// -----------------------------------------------------------------------------
//
TUint32 SConMetadata::ReadTUint32( const TDesC8& aData )
    {
    TRACE_FUNC_ENTRY;
    TUint32 result = 0;
    TUint8 c;
    
    for (TInt i=aData.Length()-1; i>=0; i--)
        {
        c = aData[i];
        result = (result << 8) | c;
        }
    
    LOGGER_WRITE_1( "SConMetadata::ReadTUint32() : returned %d ", result );
    return result;
    }

// -----------------------------------------------------------------------------
// SConMetadata::GetFileDataLC()
// Read data from file
// -----------------------------------------------------------------------------
//
HBufC8* SConMetadata::GetFileDataLC( const TDesC& aFilename, RFs& aFs )
	{
	TRACE_FUNC_ENTRY;
    RFile file;
    
    User::LeaveIfError( file.Open( aFs, aFilename,
        EFileRead | EFileShareReadersOnly ) );
    CleanupClosePushL( file );
    
    LOGGER_WRITE( "SConMetadata::GetFileDataLC() : file opened" );
    
    TInt dataSize(0);
    ContentAccess::CContent* content;
    content = ContentAccess::CContent::NewLC( file );
    ContentAccess::CData* data = content->OpenContentLC( ContentAccess::EPeek );
    data->DataSizeL(dataSize);
    
    LOGGER_WRITE( "SConMetadata::GetFileDataLC() : content opened" );
    // Read 64k from the beginning of the file
    if ( dataSize > KJpegLoadBufferSize )
        {
        dataSize = KJpegLoadBufferSize;
        }
    HBufC8* readBuffer = HBufC8::NewLC( dataSize );
    TPtr8 readPtr = readBuffer->Des();
    User::LeaveIfError( data->Read( readPtr, dataSize ) );
    
    LOGGER_WRITE( "SConMetadata::GetFileDataLC() : data readed" );
    CleanupStack::Pop( readBuffer );
    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( content );
    CleanupStack::PopAndDestroy( &file );
    
    CleanupStack::PushL( readBuffer );
    TRACE_FUNC_EXIT;
    return readBuffer;
	}

// -----------------------------------------------------------------------------
// SConMetadata::ReadExifDataL()
// Read Exif data
// -----------------------------------------------------------------------------
//
void SConMetadata::ReadExifDataL( CSConTask& aTask, RFs& aFs )
    {
    TRACE_FUNC_ENTRY;
    
    // read data from file
    HBufC8* jpegReadBuffer = GetFileDataLC( aTask.iGetMetadataParams->iFilename, aFs );
    
    CExifRead* reader = CExifRead::NewL(
        *jpegReadBuffer, CExifRead::ENoJpeg | CExifRead::ENoTagChecking );
    
    LOGGER_WRITE( "SConMetadata::ReadExifDataL() : reader created" );
    CleanupStack::PopAndDestroy( jpegReadBuffer );
    CleanupStack::PushL( reader );
    

    CBufFlat* buffer = CBufFlat::NewL( KBufferSize );
    CleanupStack::PushL( buffer );
    buffer->Reset();
    
    
    TInt offset(0);
    // header id (1 byte) + head.version (1 byte) = 2 bytes
    
    buffer->ExpandL( offset, 2 );
    
    // header id
    TUint8 value( KSconMetadataHeaderExif );
    buffer->Write( offset, TPtrC8(&value, 1) );
    offset++;
    
    // header version
    value = KSconMetadataHeaderVersion;
    buffer->Write( offset, TPtrC8(&value, 1) );
    offset++;
    
    
    HBufC8* tempBuf(NULL);
    
    TInt err;
    
    for ( TInt tagID = KSconExifThumbnail; tagID < KSconExifGpsVersion; tagID++ )
        {
        // GetExifTagL function is used only to tags that leaves if data is not found
        TRAP( err, tempBuf = GetExifTagL( reader, tagID ) );
        if ( !err )
            {
            LOGGER_WRITE("write to buffer");
            CleanupStack::PushL( tempBuf );
            AppendByteDataFieldL( buffer, tempBuf->Des(), tagID );
            CleanupStack::PopAndDestroy( tempBuf );
            tempBuf = NULL;
            }
        else
            {
            LOGGER_WRITE_1( "GetExifTagL Leave code %d", err );
            }
        }
    
    
    TUint16 value16a;
    TUint32 value32a;
    TUint32 value32b;
    TInt32 val32a;
    TInt32 val32b;
    
    // Orientation
    err = reader->GetOrientation( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifOrientation );
        }
    
    // XResolution
    err = reader->GetXResolution( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifXResolution1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifXResolution2 );
        }
    
    // YResolution
    err = reader->GetYResolution( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifYResolution1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifYResolution2 );
        }
    
    // ResolutionUnit
    err = reader->GetResolutionUnit( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifResolutionUnit );
        }
    
    // YCbCrPositioning
    err = reader->GetYCbCrPositioning( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifYCbCrPositioning );
        }
    
    // YCbCrPositioning
    err = reader->GetYCbCrPositioning( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifYCbCrPositioning );
        }
    
    
    // ExposureTime
    err = reader->GetExposureTime( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifExposureTime1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifExposureTime2 );
        }
    
    // ComponentsConfiguration
    TUint8 value8a;
    TUint8 value8b;
    TUint8 value8c;
    TUint8 value8d;
    err = reader->GetComponentsConfiguration( value8a, value8b, value8c, value8d );
    if ( !err )
        {
        TInt offset;
        offset = buffer->Size();
        
        // field ID (1 byte) + field lenght (4 bytes) + field data (4bytes) = 9 bytes
        buffer->ExpandL( offset, 9 );
        
        // field id (one byte)
        TUint8 fieldId( KSconExifComponentsConfiguration );
        buffer->Write( offset, TPtrC8( &fieldId, 1 ) );
        offset++;
        
        // field lenght
        WriteTUint32( buffer, offset, 4);
        offset += 4;
        
        buffer->Write( offset, TPtrC8(&value8a, 1) );
        offset++;
        buffer->Write( offset, TPtrC8(&value8b, 1) );
        offset++;
        buffer->Write( offset, TPtrC8(&value8c, 1) );
        offset++;
        buffer->Write( offset, TPtrC8(&value8d, 1) );
        offset++;
        }
    
    // Flash
    err = reader->GetFlash( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifFlash );
        }
    
    // ColorSpace
    err = reader->GetColorSpace( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifColorSpace );
        }
    
    // PixelXDimension
    err = reader->GetPixelXDimension( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifPixelXDimension );
        }
    
    // PixelYDimension
    err = reader->GetPixelYDimension( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifPixelYDimension );
        }
    
    // ExposureMode
    err = reader->GetExposureMode( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifExposureMode );
        }
    
    // WhiteBalance
    err = reader->GetWhiteBalance( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifWhiteBalance );
        }
    
    // SceneCaptureType
    err = reader->GetSceneCaptureType( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifSceneCaptureType );
        }
    
    // ExposureProgram
    err = reader->GetExposureProgram( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifExposureProgram );
        }
    
    // GetApertureValue
    err = reader->GetApertureValue( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifApertureValue1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifApertureValue2 );
        }
    
    // GetExposureBiasValue
    err = reader->GetExposureBiasValue( val32a, val32b );
    if ( !err )
        {
        value32a = val32a;
        value32b = val32b;
        AppendTUintDataFieldL( buffer, value32a, KSconExifExposureBiasValue1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifExposureBiasValue2 );
        }
    
    // GetMeteringMode
    err = reader->GetMeteringMode( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifMeteringMode );
        }
    
    // GetLightSource
    err = reader->GetLightSource( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifLightSource );
        }
    
    // GetFileSource
    TInt8 val8;
    err = reader->GetFileSource( val8 );
    if ( !err )
        {
        value8a = val8;
        AppendTUintDataFieldL( buffer, value8a, KSconExifFileSource );
        }
    
    // GetDigitalZoomRatio
    err = reader->GetDigitalZoomRatio( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifDigitalZoomRatio1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifDigitalZoomRatio2 );
        }
    
    // GetContrast
    err = reader->GetContrast( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifContrast );
        }
    
    // GetSaturation
    err = reader->GetSaturation( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifSaturation );
        }
    
    // GetSharpness
    err = reader->GetSharpness( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifSharpness );
        }
    
    // GetExifVersion
    err = reader->GetExifVersion( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifExifVersion );
        }
    
    // GetFlashPixVersion
    err = reader->GetFlashPixVersion( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifFlashPixVersion );
        }
    
    // GetThumbnailXResolution
    err = reader->GetThumbnailXResolution( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifThumbXResolution1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifThumbXResolution2 );
        }
    
    // GetThumbnailYResolution
    err = reader->GetThumbnailYResolution( value32a, value32b );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifThumbYResolution1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifThumbYResolution2 );
        }
    
    // GetThumbnailResolutionUnit
    err = reader->GetThumbnailResolutionUnit( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifThumbResolutionUnit );
        }
    
    // GetThumbnailCompression
    err = reader->GetThumbnailCompression( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifThumbCompression );
        }
    
    // GetJpegInterchangeFormat
    err = reader->GetJpegInterchangeFormat( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifThumbJpegInterchangeFormat );
        }
    
    // GetJpegInterchangeFormatLength
    err = reader->GetJpegInterchangeFormatLength( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifThumbJpegInterchangeFormatLength );
        }
    
    // GetShutterSpeedValue
    err = reader->GetShutterSpeedValue( val32a, val32b );
    if ( !err )
        {
        value32a = val32a;
        value32b = val32b;
        AppendTUintDataFieldL( buffer, value32a, KSconExifShutterSpeedValue1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifShutterSpeedValue2 );
        }
    
    // GetBrightnessValue
    err = reader->GetBrightnessValue( val32a, val32b );
    if ( !err )
        {
        value32a = val32a;
        value32b = val32b;
        AppendTUintDataFieldL( buffer, value32a, KSconExifBrightnessValue1 );
        AppendTUintDataFieldL( buffer, value32b, KSconExifBrightnessValue2 );
        }
    
    // GetCustomRendered
    err = reader->GetCustomRendered( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifCustomRendered );
        }
    
    // GetGainControl
    err = reader->GetGainControl( value16a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value16a, KSconExifGainControl );
        }
    
    // GetGpsVersion
    err = reader->GetGpsVersion( value32a );
    if ( !err )
        {
        AppendTUintDataFieldL( buffer, value32a, KSconExifGpsVersion );
        }
    
    // Get GPS coordinates
    const TUint KCoordinatesMaxLength = 50;
    TBuf<KCoordinatesMaxLength> gpsInfo;
    // Latidute
    err = GetExifGPSLatitudeL( *reader, gpsInfo );
    if ( !err )
    	{
    	AppendUtf8DataFieldL( buffer, gpsInfo ,KSconExifGPSLatitude );
    	}
    
    // Longitude
    err = GetExifGPSLongitudeL( *reader, gpsInfo );
    if ( !err )
    	{
    	AppendUtf8DataFieldL( buffer, gpsInfo ,KSconExifGPSLongitude );
    	}
    
    // Altidute
    err = GetExifGPSAltiduteL( *reader, gpsInfo );
    if ( !err )
    	{
    	AppendUtf8DataFieldL( buffer, gpsInfo ,KSconExifGPSAltitude );
    	}
    
    LOGGER_WRITE( "SConMetadata::ReadExifDataL() : All data collected" );
    
    if ( buffer->Size() > 0 )
        {
        LOGGER_WRITE_1( "SConMetadata::ReadExifDataL() : buffer->Size() %d", buffer->Size() );
        if ( aTask.iGetMetadataParams->iData )
            {
            delete aTask.iGetMetadataParams->iData;
            aTask.iGetMetadataParams->iData = NULL;
            }
        //Initialize the task data buffer
        aTask.iGetMetadataParams->iData = HBufC8::NewL( buffer->Size() );
        TPtr8 dataPtr = aTask.iGetMetadataParams->iData->Des();
        
        buffer->Read( 0, dataPtr, buffer->Size() );
        LOGGER_WRITE_1( "SConMetadata::ReadExifDataL() dataPtr len: %d", dataPtr.Length());
        }
    CleanupStack::PopAndDestroy( buffer );
    
    LOGGER_WRITE( "SConMetadata::ReadExifDataL() : task updated" );
    
    CleanupStack::PopAndDestroy( reader );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// void SConMetadata::ConvertRationalTag( const CExifTag& aTag, TDes& aPosDegrees )
// parses degrees, minutes and seconds from CExifTag and converts it to string format
// -----------------------------------------------------------------------------
//
TInt SConMetadata::ConvertRationalTag( const CExifTag& aTag, TDes& aPosDegrees )
	{
	TRACE_FUNC_ENTRY;
	TInt numer(0);
	TInt denom(0);
	TReal64 degrees(0);
	TReal64 minutes(0);
	TReal64 seconds(0);
	
	const TUint8* ratData = aTag.Data().Ptr();
	for ( TUint y=0; y < aTag.TagInfo().iDataCount; y++ )
		{
		numer = 0;
		denom = 0;
		Mem::Copy(&numer, ratData + ((y * 2) * sizeof(numer)), sizeof(numer));
		Mem::Copy(&denom, ratData + (((y * 2) + 1) * sizeof(numer)), sizeof(denom));	
		if (denom != 0)
		    {
            if ( y == 0 )// degrees
                {
                degrees = numer/denom;
                }
            else if ( y == 1 )// minutes
                {
                minutes = numer/denom;
                }
            else if ( y == 2 )// seconds
                {
                seconds = numer/denom;
                }
		    }
		}
	_LIT(KFormat, "%.0f°%.0f'%.2f\"" );
	aPosDegrees.Format( KFormat, degrees, minutes, seconds );
	TRACE_FUNC_EXIT;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// void SConMetadata::GetExifGPSLatitudeL( CExifRead& aExifRead, TDes& aLatitude )
// Read latidute from ExifReader 
// -----------------------------------------------------------------------------
//
TInt SConMetadata::GetExifGPSLatitudeL( CExifRead& aExifRead, TDes& aLatitude )
	{
	TRACE_FUNC_ENTRY;
	TInt ret(KErrNone);
	if ( aExifRead.TagExists( KGPSLatitudeRef, EIfdGps )
		&& aExifRead.TagExists( KGPSLatitude, EIfdGps) )
		{
		// get latidute
		const CExifTag* lat = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSLatitude );
		User::LeaveIfError( ConvertRationalTag( *lat, aLatitude ) );
		if ( aLatitude.Length()+2 > aLatitude.MaxLength() )
			{
			User::Leave( KErrTooBig );
			}
		
		// south or north
		const CExifTag* latRef = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSLatitudeRef );
		_LIT8( KNorthTagDef8, "N" );
		_LIT( KNorth, "N " );
		_LIT( KSouth, "S " );
		if ( latRef->Data().Find( KNorthTagDef8 ) != KErrNotFound )
			{
			aLatitude.Insert( 0, KNorth );
			}
		else
			{
			aLatitude.Insert( 0, KSouth );
			}
		}
	else
		{
		ret=KErrNotFound;
		}
	TRACE_FUNC_EXIT;
	return ret;
	}

// -----------------------------------------------------------------------------
// void SConMetadata::GetExifGPSLongitudeL( CExifRead& aExifRead, TDes& aLongitude )
// Read longitude from ExifReader 
// -----------------------------------------------------------------------------
//
TInt SConMetadata::GetExifGPSLongitudeL( CExifRead& aExifRead, TDes& aLongitude )
	{
	TRACE_FUNC_ENTRY;
	TInt ret(KErrNone);
	if ( aExifRead.TagExists( KGPSLongitudeRef, EIfdGps )
		&& aExifRead.TagExists( KGPSLongitude, EIfdGps) )
		{
		// get longitude
		const CExifTag* lon = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSLongitude );
		User::LeaveIfError( ConvertRationalTag( *lon, aLongitude ) );
		if ( aLongitude.Length()+2 > aLongitude.MaxLength() )
			{
			User::Leave( KErrTooBig );
			}
		
		// east or west
		const CExifTag* lonref = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSLongitudeRef );
		_LIT8( KEastTagDef8, "E" );
		_LIT( KEast, "E " );
		_LIT( KWest, "W " );
		if ( lonref->Data().Find( KEastTagDef8 ) != KErrNotFound )
			{
			aLongitude.Insert( 0, KEast );
			}
		else
			{
			aLongitude.Insert( 0, KWest );
			}
		}
	else
		{
		ret = KErrNotFound;
		}
	TRACE_FUNC_EXIT;
	return ret;
	}

// -----------------------------------------------------------------------------
// void SConMetadata::GetExifGPSAltiduteL( CExifRead& aExifRead, TDes& aAltidute )
// Read altidute from ExifReader 
// -----------------------------------------------------------------------------
//
TInt SConMetadata::GetExifGPSAltiduteL( CExifRead& aExifRead, TDes& aAltidute )
	{
	TRACE_FUNC_ENTRY;
	
	TInt ret(KErrNone);
	if ( aExifRead.TagExists( KGPSAltitudeRef, EIfdGps )
		&& aExifRead.TagExists( KGPSAltitude, EIfdGps) )
		{
		if ( aAltidute.MaxLength() < 5 )
			{
			User::Leave( KErrTooBig );
			}
		
		// get altidute
		const CExifTag* alt = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSAltitude );
		const TUint8* ratData = alt->Data().Ptr();
		
		TInt numer;
		TInt denom;
		Mem::Copy(&numer, ratData , sizeof(numer));
		Mem::Copy(&denom, ratData + (sizeof(numer)), sizeof(denom));
		
		TReal32 tmp = numer / denom;
		
		// sea level
		const CExifTag* altref = aExifRead.GetTagL( 
				EIfdGps, 
				KGPSAltitudeRef );
		_LIT8( KAltSealevelDef8, "1" );
		if ( altref->Data().Find( KAltSealevelDef8 ) != KErrNotFound )
			{
			// seaLevelReference -> negative value
			tmp *= -1;
			}
		TRealFormat format;
		format.iType = KRealFormatFixed;
		format.iPlaces=1;
		User::LeaveIfError( aAltidute.Num( tmp, format ) );
		}
	else
		{
		ret = KErrNotFound;
		}
	TRACE_FUNC_EXIT;
	return ret;
	}

// -----------------------------------------------------------------------------
// void SConMetadata::AppendUtf8DataFieldL( CBufFlat* aBuffer,
//     const TPtrC aAppendData, const TUint8 aFieldId )
// Write 8bit field id value, data length (32bit) and UTF8 data to buffer 
// -----------------------------------------------------------------------------
//
void SConMetadata::AppendUtf8DataFieldL( CBufFlat* aBuffer,
    const TPtrC aAppendData, const TUint8 aFieldId )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFieldId: 0x%02x", aFieldId);
    HBufC8* tempBuf = HBufC8::NewLC( aAppendData.Size() );
    TPtr8 temp = tempBuf->Des();
    
    CnvUtfConverter::ConvertFromUnicodeToUtf8( temp, aAppendData );
    LOGGER_WRITE_1( "lenght: %d", temp.Length() );
    
    if ( temp.Length() > KFieldMaxLength )
        {
        LOGGER_WRITE("Data length is too big, field skipped");
        CleanupStack::PopAndDestroy( tempBuf );
        return;
        }
    
    TInt offset;
    offset = aBuffer->Size();
    // field ID (1 byte) + field lenght (4 bytes) + field data
    aBuffer->ExpandL( offset, temp.Length() + 5 );
    
    // field id (one byte)
    aBuffer->Write( offset, TPtrC8(&aFieldId, 1) );
    offset++;
    
    WriteTUint32( aBuffer, offset, temp.Length());
    offset += 4;
    
    // field data
    aBuffer->Write( offset, temp, temp.Length() );
    offset += temp.Length();
    
    CleanupStack::PopAndDestroy( tempBuf );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// void SConMetadata::AppendByteDataFieldL( CBufFlat* aBuffer,
//     const TPtrC8 aAppendData, const TUint8 aFieldId )
// Write 8bit field id value, data length (32bit) and n*8bit data to buffer 
// -----------------------------------------------------------------------------
//
void SConMetadata::AppendByteDataFieldL( CBufFlat* aBuffer,
    const TPtrC8 aAppendData, const TUint8 aFieldId )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFieldId: 0x%02x", aFieldId);
    LOGGER_WRITE_1( "lenght: %d", aAppendData.Length() );
    if ( aAppendData.Length() > KFieldMaxLength )
        {
        LOGGER_WRITE("Data length is too big, field skipped");
        return;
        }
    TInt offset;
    offset = aBuffer->Size();
    
    // field id (1) + field len (4) + datalen = 5 + datalen
    aBuffer->ExpandL( offset, aAppendData.Length() + 5 );
    
    // field id (one byte)
    aBuffer->Write( offset, TPtrC8(&aFieldId, 1) );
    offset++;
    
    // field data length (32bit unsigned integer, two byte)
    WriteTUint32( aBuffer, offset, aAppendData.Length() );
    offset += 4;
    
    // field data
    aBuffer->Write( offset, aAppendData );
    offset += aAppendData.Length();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// void SConMetadata::AppendTUintDataFieldL( CBufFlat* aBuffer,
//     const TUint32 aValue, const TUint8 aFieldId )
// Write 8bit field id value, data length (16bit) and n*8bit data to buffer 
// -----------------------------------------------------------------------------
//
void SConMetadata::AppendTUintDataFieldL( CBufFlat* aBuffer,
    TUint32 aValue, const TUint8 aFieldId )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aFieldId: 0x%02x", aFieldId);
    LOGGER_WRITE_1( "aValue dec: %u", aValue);
    TInt offset;
    offset = aBuffer->Size();
    TInt dataLen(1);
    TUint8 temp[4];
    TInt i(0);
    
    temp[i] = aValue & 0xFF;
    
    aValue >>= 8;
    while( aValue > 0 )
        {
        i++;
        temp[i] = aValue & 0xFF;
        aValue >>= 8;
        dataLen++;
        }
    // field id (1) + field len (4) + datalen = 5 + datalen
    aBuffer->ExpandL( offset, 5 + dataLen );
    
    // field id (one byte)
    aBuffer->Write( offset, TPtrC8(&aFieldId, 1) );
    offset++;
    
    // field data length (32bit unsigned integer, two byte)
    WriteTUint32( aBuffer, offset, dataLen );
    offset += 4;
    
    // field data
    for( TInt i = dataLen-1; i>=0; i-- )
        {
        aBuffer->Write( offset, TPtrC8(&temp[i], 1) );
        offset++;
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// void SConMetadata::WriteTUint32( CBufFlat* aBuffer, TInt offset, TUint32 aValue )
// Write 32bit unsigned integer value to buffer.
// -----------------------------------------------------------------------------
//
void SConMetadata::WriteTUint32( CBufFlat* aBuffer, TInt offset, TUint32 aValue )
    {
    TUint8 temp[4];
    TInt i(0);
    
    temp[i] = aValue & 0xFF;
    
    aValue >>= 8;
    for ( TInt i=1; i<4; i++ )
        {
        temp[i] = aValue & 0xFF;
        aValue >>= 8;
        }
    
    // high to low
    for ( TInt i=3; i>=0; i-- )
        {
        aBuffer->Write( offset, TPtrC8(&temp[i], 1) );
        offset++;
        }
    }

// End of file
