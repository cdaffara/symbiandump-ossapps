/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for TMMCScBkupArchiveVector
*     
*
*/

#ifndef __TMMCSCBKUPARCHIVEVECTOR_H__
#define __TMMCSCBKUPARCHIVEVECTOR_H__

// System includes
#include <s32strm.h>
#include <f32file.h>


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(TMMCScBkupArchiveVector)
    {
    public:

        /**
        * C++ default constructor
        */
        inline TMMCScBkupArchiveVector()
            { Reset(); }

        /**
        * C++ default constructor
        */
        inline TMMCScBkupArchiveVector( TInt aOffset, TInt aLength )
            : iOffset( aOffset ), iLength( aLength )
            {}

        /**
        * C++ copy constructor
        */
        inline TMMCScBkupArchiveVector( const TMMCScBkupArchiveVector& aVector )
            : iOffset( aVector.Offset() ), iLength( aVector.Length() )
            {}

    public:

        /**
        *
        */
        inline TInt Offset() const { return iOffset; }

        /**
        *
        */
        inline void SetOffset( TInt aOffset ) { iOffset = aOffset; }

        /**
        *
        */
        inline TInt Length() const { return iLength; }

        /**
        *
        */
        inline void SetLength( TInt aLength ) { iLength = aLength; }

        /**
        *
        */
        inline TInt EndOffset() const { return (iOffset + iLength); }

        /**
        *
        */
        inline void AdjustByDelta( TInt aOffsetDelta ) { iOffset += aOffsetDelta; }

        /**
        *
        */
        static TInt ExternalizedSize();

    public:

        /**
        *
        */
        inline void Reset() { iLength = iOffset = 0; }

        /**
        *
        */
        void ExternalizeL( RWriteStream& aStream ) const;

        /**
        *
        */
        void InternalizeL( RReadStream& aStream );

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Member data
        
        /**
        *
        */
        TInt iOffset;

        /**
        *
        */
        TInt iLength;
    };



/**
*
* @since 3.0
*/
NONSHARABLE_CLASS(TMMCScBkupArchiveDriveAndVector)
    {
    public:

        /**
        * C++ default constructor
        */
        inline TMMCScBkupArchiveDriveAndVector() : iDrive( EDriveC ) { }

        /**
        * C++ default constructor
        */
        inline TMMCScBkupArchiveDriveAndVector( TDriveNumber aDrive, const TMMCScBkupArchiveVector& aVector ) : iDrive( aDrive), iVector( aVector ) { }

    public:

        /**
        * Internalize object from stream
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize object to stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    public: // Data members
        TDriveNumber iDrive;
        TMMCScBkupArchiveVector iVector;
    };




#endif // __TMMCSCBKUPARCHIVEVECTOR_H__

//End of File
