/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SConMetadata header file
*
*/


#ifndef _SCONMETADATA_H_
#define _SCONMETADATA_H_

#include <MetaDataField.hrh>

class CSConTask;
class CExifRead;
class CExifTag;
//============================================================
// Class CSConMetadata declaration
//============================================================  
NONSHARABLE_CLASS( SConMetadata )
    {
    public:
    
    /**
     * Start processing metadata task
     * @param aTask used task
     * @param aFs aReference to RFs connection.
     * @return none
     */
    static void ProcessTask( CSConTask& aTask, RFs& aFs );
    
    private:
    
    /**
    * Get audio metadata to task
    * @return none
    */
    static void GetAudioMetadataL( CSConTask& aTask );
    
    /**
    * Get video metadata to task
    * @param aFs aReference to RFs connection.
    * @return none
    */
    static void GetVideoMetadataL( CSConTask& aTask, RFs& aFs );
    
    /**
    * parse exif field
    * @return HBufC8* parsed data. Leaves if not found
    */
    static HBufC8* GetExifTagL( CExifRead* aReader, const TUint8 aTagID ) ;
    
    /**
    * Parse TUint32 value from aData
    * @return TUint32 value
    */
    static TUint32 ReadTUint32( const TDesC8& aData );
    
    /**
     * Read data from file
     * @param aFs aReference to RFs connection.
     * @return file data
     */
    static HBufC8* GetFileDataLC( const TDesC& aFilename, RFs& aFs );
    
    /**
    * Get exif metadata to task
    * @param aFs aReference to RFs connection.
    * @return none
    */
    static void ReadExifDataL( CSConTask& aTask, RFs& aFs );
    
    /**
     * Convert latidute/longitude tag to TReal64 value
     * @return KErrNone if successfully converted
     */
    static TInt ConvertRationalTag( const CExifTag& aTag, TDes& aPosDegrees );
    
    /**
     * Get latidute value from Exif
     * @param aExifRead ExifReader used to read
     * @param aLatitude readed latidute
     * @return KErrNone if found
     */
    static TInt GetExifGPSLatitudeL( CExifRead& aExifRead, TDes& aLatitude );
    
    /**
     * Get longitude value from Exif
     * @param aExifRead ExifReader used to read
     * @param aLongitude readed longitude
     * @return KErrNone if found
     */
    static TInt GetExifGPSLongitudeL( CExifRead& aExifRead, TDes& aLongitude );
    
    /**
     * Get altidute value from Exif
     * @param aExifRead ExifReader used to read
     * @param aAltidute readed altidute
     * @return KErrNone if found
     */
    static TInt GetExifGPSAltiduteL( CExifRead& aExifRead, TDes& aAltidute );
    
    /**
    * Appends data field to buffer
    * @return none
    */
    static void AppendUtf8DataFieldL( CBufFlat* aBuffer, const TPtrC aAppendData,
        const TUint8 aFieldId );
    
    /**
    * Appends data field to buffer
    * @return none
    */
    static void AppendByteDataFieldL( CBufFlat* aBuffer, const TPtrC8 aAppendData,
        const TUint8 aFieldId );
    
    /**
    * Writes TUint32 value to buffer
    * @return none
    */
    static void WriteTUint32( CBufFlat* aBuffer, TInt offset, TUint32 aValue );
    
    /**
    * Appends data field to buffer
    * @return none
    */
    static void AppendTUintDataFieldL( CBufFlat* aBuffer, TUint32 aValue,
        const TUint8 aFieldId );
    
    /**
    * Get AudioFieldId
    * @return TUint8
    */
    static TUint8 AudioFieldId( const TMetaDataFieldId fieldId );
    
    };

#endif // _SCONMETADATA_H_
