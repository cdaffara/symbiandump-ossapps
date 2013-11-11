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
* Description: Declaration of CMMCScBkupDriveSizer
*     
*
*/

#ifndef __CMMCSCBKUPDRIVEDATASIZEMANAGER_H__
#define __CMMCSCBKUPDRIVEDATASIZEMANAGER_H__

// System includes
#include <s32strm.h>
#include <f32file.h>

// User includes
#include "TMMCScBkupDriveAndSize.h"
#include "RMMCScBkupPointerArray.h"
#include "TMMCScBkupOwnerDataType.h"


/**
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDriveSizer) : public CBase
    {
    public:

        /**
        * 
        */
        static CMMCScBkupDriveSizer* NewLC( TMMCScBkupOwnerDataType aType );

        /**
        * 
        */
        static CMMCScBkupDriveSizer* NewLC( RReadStream& aStream );

        /**
        * C++ destructor
        */
        ~CMMCScBkupDriveSizer();

    private:

        /**
        * C++ constructor
        */
        CMMCScBkupDriveSizer( TMMCScBkupOwnerDataType aType );

        /**
        * 
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        inline TMMCScBkupOwnerDataType DataType() const { return iDataType; }

        /**
        *
        */
        void AddToSizeL( TInt64 aAmount, TDriveNumber aDrive );

        /**
        *
        */
        void Reset();

        /**
        *
        */
        void Reset( TDriveNumber aDrive );

        /**
        *
        */
        TInt64 Size() const;

        /**
        *
        */
        TInt64 Size( TDriveNumber aDrive ) const;

    public:

        /**
        * Internalize object from stream
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize object to stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    private: // Internal methods

        /**
        *
        */
        TMMCScBkupDriveAndSize* EntryByDrive( TDriveNumber aDrive );

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Data members

        //
        RArray<TMMCScBkupDriveAndSize> iEntries;
        //
        TMMCScBkupOwnerDataType iDataType;
    };






/**
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDataTypeSizer) : public CBase
    {
    public:

        /**
        * 
        */
        static CMMCScBkupDataTypeSizer* NewL();
        
        /**
        * 
        */
        static CMMCScBkupDataTypeSizer* NewLC( RReadStream& aStream );

        /**
        * C++ destructor
        */
        ~CMMCScBkupDataTypeSizer();

    private:

        /**
        * C++ constructor
        */
        CMMCScBkupDataTypeSizer();

        /**
        * 
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        void AddToSizeL( TMMCScBkupOwnerDataType aDataType, TInt64 aAmount, TDriveNumber aDrive );

        /**
        *
        */
        void Reset( TMMCScBkupOwnerDataType aDataType );

        /**
        *
        */
        void Reset( TMMCScBkupOwnerDataType aDataType, TDriveNumber aDrive );

        /**
        *
        */
        TInt64 Size( TMMCScBkupOwnerDataType aDataType ) const;

        /**
        *
        */
        TInt64 Size( TMMCScBkupOwnerDataType aDataType, TDriveNumber aDrive ) const;

        /**
        *
        */
        void GetSizesL( RArray<TMMCScBkupDriveAndSize>& aSizes ) const;

        /**
        *
        */
        TInt64 CombinedSize( TDriveNumber aDrive ) const;

    public:

        /**
        * Internalize object from stream
        */
        void InternalizeL( RReadStream& aStream );

        /**
        * Externalize object to stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    private: // Internal methods

        /**
        *
        */
        CMMCScBkupDriveSizer* SizerByDataType( TMMCScBkupOwnerDataType aDataType );

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Data members

        //
        RMMCScBkupPointerArray<CMMCScBkupDriveSizer> iSizers;
    };






#endif // __CMMCSCBKUPDRIVEDATASIZEMANAGER_H__

//End of File
