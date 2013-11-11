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
* Description: Declaration for CMMCScBkupIndexPublicDataFiles
*     
*
*/

#ifndef __CMMCSCBKUPINDEXPUBLICDATAFILES_H__
#define __CMMCSCBKUPINDEXPUBLICDATAFILES_H__

// System includes
#include <f32file.h>

// User includes
#include "CMMCScBkupIndexBase.h"

// Classes referenced
class CMMCScBkupFileInfo;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupIndexPublicDataFiles) : public CMMCScBkupIndexBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupIndexPublicDataFiles* NewLC();

        /**
        * C++ destructor
        */
        ~CMMCScBkupIndexPublicDataFiles();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupIndexPublicDataFiles();

    public: // API

        /**
        *
        */
        void AddIndexRecordL( const TMMCScBkupArchiveVector& aInfo, const CMMCScBkupFileInfo& aFile );

        /**
        *
        */
        inline TInt Count() const { return iEntries.Count(); }

        /**
        *
        */
        const CMMCScBkupFileInfo& At( TInt aIndex, TMMCScBkupArchiveVector& aInfo ) const;

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
        * @since 3.0
        */
        class TMMCScBkupPublicFileEntry
            {
            public:
                inline TMMCScBkupPublicFileEntry() { }
                inline TMMCScBkupPublicFileEntry(const TMMCScBkupArchiveVector& aInfo, const CMMCScBkupFileInfo& aFile)
                    : iInfo(aInfo), iFile(&aFile)
                    {
                    }
 
            public: // Data members
                TMMCScBkupArchiveVector iInfo;
                const CMMCScBkupFileInfo* iFile;
            };

    private: // Member data

        //
        RArray<TMMCScBkupPublicFileEntry> iEntries;

    };


#endif // __CMMCSCBKUPINDEXPUBLICDATAFILES_H__

//End of File
