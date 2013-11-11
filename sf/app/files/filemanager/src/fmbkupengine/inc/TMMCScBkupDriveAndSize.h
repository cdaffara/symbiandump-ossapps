/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for TMMCScBkupDriveAndSize
*     
*
*/

#ifndef __TMMCSCBKUPDRIVEANDSIZE_H__
#define __TMMCSCBKUPDRIVEANDSIZE_H__

// System includes
#include <s32strm.h>
#include <f32file.h>



/**
*
* @since 3.0
*/
NONSHARABLE_CLASS(TMMCScBkupDriveAndSize)
    {
    public:

        /**
        * C++ default constructor
        */
        inline TMMCScBkupDriveAndSize() : iDrive( EDriveC ), iSize( 0 ) { }

        /**
        * C++ default constructor
        */
        inline TMMCScBkupDriveAndSize( TDriveNumber aDrive, TInt aSize ) : iDrive( aDrive), iSize( aSize ) { }

    public:

        /**
        * Internalize object from stream
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize object to stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    public: // API

        /**
        *
        */
        inline TInt64 Size() const { return iSize; }

        /**
        *
        */
        inline TDriveNumber Drive() const { return iDrive; }

        /**
        *
        */
        inline void SetSize( TInt64 aSize ) { iSize = aSize; }

        /**
        *
        */
        inline void AddToSize( TInt64 aAmount ) { iSize += aAmount; }

        /**
        *
        */
        inline void SetDrive( TDriveNumber aDrive ) { iDrive = aDrive; }

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            //EStreamFormatVersion2 = 2 // If 64-bit handling will be supported in the future.
                                        // That will mean in practice a break in archive file format.
                                        // Change are also needed in TMMCScBkupArchiveVector class
                                        // to support larger than 2 GB files and data in it.
            };

    private: // Data members
        TDriveNumber iDrive;
        TInt64 iSize;
    };





#endif // __TMMCSCBKUPDRIVEANDSIZE_H__

//End of File
