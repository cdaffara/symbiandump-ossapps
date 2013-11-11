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
* Description: Declaration of CMMCScBkupArchiveHeader
*     
*
*/

#ifndef __CMMCSCBKUPARCHIVEHEADER_H__
#define __CMMCSCBKUPARCHIVEHEADER_H__

// System includes
#include <f32file.h>
#include <babitflags.h>

// User includes
#include "MMCScBkupArchiveFlags.h"

// Classes referenced
class MMMCScBkupDriver;
class MMMCScBkupArchiveDataInterface;

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupArchiveHeader) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupArchiveHeader* NewL( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver );

        /**
        * C++ destructor
        */
        ~CMMCScBkupArchiveHeader();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupArchiveHeader( MMMCScBkupArchiveDataInterface& aDataInterface, MMMCScBkupDriver& aDriver );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        void StoreL(TBitFlags aCategory);

        /**
        *
        */
        void RestoreL();

    public: // For Archive Utils 

        /**
        *
        */
        inline void SetCheckedUid( const TCheckedUid& aCheckedUid ) { iCheckedUid = aCheckedUid; }

        /**
        *
        */
        inline const TCheckedUid& CheckedUid() const { return iCheckedUid; }

        /**
        *
        */
        inline void SetVersion( const TVersion& aVersion ) { iVersion = aVersion; }

        /**
        *
        */
        inline const TVersion& Version() const { return iVersion; }

        /**
        *
        */
        inline void SetFooterLength( TInt aLength ) { iFooterLength = aLength; }

        /**
        *
        */
        inline TInt FooterLength() const { return iFooterLength; }

        /**
        *
        */
        void SetPhoneModelIdentifierL( const TDesC8& aModel );

        /**
        *
        */
        inline const TDesC8& PhoneModelIdentifier() const { return *iPhoneModelIdentifier; }

        /**
        *
        */
        inline void SetArchiveFlags( TUint32 aFlagsValue ) { iArchiveFlags.SetValue( aFlagsValue ); }

        /**
        *
        */
        inline const TBitFlags32& ArchiveFlags() const { return iArchiveFlags; }

    private: // Member data

        //
        MMMCScBkupArchiveDataInterface& iDataInterface;
        //
        MMMCScBkupDriver& iDriver;
        //
        TCheckedUid iCheckedUid;
        //
        TVersion iVersion;
        //
        TInt iFooterLength;
        //
        HBufC8* iPhoneModelIdentifier;
        //
        TBitFlags32 iArchiveFlags;

    };




#endif // __CMMCSCBKUPARCHIVEHEADER_H__

//End of File
