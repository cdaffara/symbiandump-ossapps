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
* Description: Declaration for CMMCScBkupIndexDataOwners
*     
*
*/

#ifndef __CMMCSCBKUPINDEXDATAOWNERS_H__
#define __CMMCSCBKUPINDEXDATAOWNERS_H__

// System includes
#include <f32file.h>

// User includes
#include "CMMCScBkupIndexBase.h"

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexDataOwners) : public CMMCScBkupIndexBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupIndexDataOwners* NewLC();

        /**
        * C++ destructor
        */
        ~CMMCScBkupIndexDataOwners();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexDataOwners();

    public: // API

        /**
        *
        */
        void AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo );

        /**
        *
        */
        inline TInt Count() const
            { return iEntries.Count(); }

        /**
        *
        */
        inline const TMMCScBkupArchiveVector& At( TInt aIndex ) const
            { return iEntries[ aIndex ]; }

    public: // From CMMCScBkupIndexBase

        /**
        *
        */
        void StoreL(MMMCScBkupDriver& aDriver);

        /**
        *
        */
        void RestoreL(MMMCScBkupDriver& aDriver);

    private: // Internal enumerations
        enum
            {
            EStreamFormatVersion1 = 1
            };

    private: // Member data

        //
        RArray<TMMCScBkupArchiveVector> iEntries;

    };




#endif // __CMMCSCBKUPINDEXDATAOWNERS_H__

//End of File
