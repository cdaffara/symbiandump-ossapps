/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for CMMCScBkupIndexJavaDataEntry and 
*              CMMCScBkupIndexJavaData
*     
*
*/

#ifndef __CMMCSCBKUPINDEXJAVADATA_H__
#define __CMMCSCBKUPINDEXJAVADATA_H__

// User includes
#include "CMMCScBkupIndexWithIdentifier.h"



/**
*
* @since 3.0
*/
NONSHARABLE_CLASS( CMMCScBkupIndexJavaDataEntry ) : public CBase
    {
    public:

        /**
        *
        */
        static CMMCScBkupIndexJavaDataEntry* NewLC();

        /**
        *
        */
        static CMMCScBkupIndexJavaDataEntry* NewLC( RReadStream& aStream );

        /**
        * C++ destructor
        */
        ~CMMCScBkupIndexJavaDataEntry();

    private: 

        /**
        *
        */
        CMMCScBkupIndexJavaDataEntry();

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        inline const TDesC& Hash() const { return *iHash; }

        /**
        *
        */
        void SetHashL( const TDesC& aHash );

        /**
        *
        */
        inline TDriveNumber Drive() const { return iDrive; }

        /**
        *
        */
        inline void SetDrive( TDriveNumber aDrive ) { iDrive = aDrive; }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& MidletInfo() const { return iMidletInfo; }

        /**
        *
        */
        inline void SetMidletInfo( const TMMCScBkupArchiveVector& aInfo ) { iMidletInfo = aInfo; }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& MidletDataInfo() const { return iMidletDataInfo; }

        /**
        *
        */
        inline void SetMidletDataInfo( const TMMCScBkupArchiveVector& aInfo ) { iMidletDataInfo = aInfo; }
        
        /**
        *
        */
        inline TBool HasMidletDataInfo() const { return iMidletDataInfo.Length() > 0 && iMidletDataInfo.Offset() > 0; }

    public: // Store & restore

        /**
        *
        */
        void InternalizeL( RReadStream& aStream );

        /**
        *
        */
        void ExternalizeL( RWriteStream& aStream ) const;

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Data members
        //
        HBufC* iHash;
        //
        TDriveNumber iDrive;
        //
        TMMCScBkupArchiveVector iMidletInfo;
        //
        TMMCScBkupArchiveVector iMidletDataInfo;
    };

    
    
/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexJavaData) : public CMMCScBkupIndexBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupIndexJavaData* NewLC();

        /**
        * C++ destructor
        */
        ~CMMCScBkupIndexJavaData();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexJavaData();

    public: // API

        /**
        *
        */
        void AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo, const TDesC& aHash, TDriveNumber aDrive, TJavaTransferType aType );

        /**
        *
        */
        inline TInt Count() const { return iEntries.Count(); }

        /**
        *
        */
        const CMMCScBkupIndexJavaDataEntry& At( TInt aIndex ) const;

    public: // From CMMCScBkupIndexBase

        /**
        *
        */
        void StoreL( MMMCScBkupDriver& aDriver );

        /**
        *
        */
        void RestoreL( MMMCScBkupDriver& aDriver );

    private: // Internal classes

        /**
        *
        */
        CMMCScBkupIndexJavaDataEntry* EntryByHash( const TDesC& aHash, TDriveNumber aDrive ) const;

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Member data

        //
        RMMCScBkupPointerArray< CMMCScBkupIndexJavaDataEntry > iEntries;

    };





#endif // __CMMCSCBKUPINDEXJAVADATA_H__

//End of File
