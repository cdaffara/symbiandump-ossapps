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
* Description: Declaration of CMMCScBkupArchive 
*     
*
*/

#ifndef __CMMCSCBKUPARCHIVE_H__
#define __CMMCSCBKUPARCHIVE_H__

// System includes
#include <f32file.h>
#include <babitflags.h>

// Classes referenced
class MMMCScBkupDriver;
class CMMCScBkupArchiveHeader;
class CMMCScBkupArchiveFooter;
class MMMCScBkupProgressObserver;
class CMMCScBkupArchiveDataManager;
class MMMCScBkupArchiveDataInterface;


/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupArchive) : public CBase
    {
    public:

        /**
        * Static constructor
        */
        static CMMCScBkupArchive* NewL( RFs& aFsSession, MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupDriver& aDriver, TBitFlags aCategory );

        /**
        * C++ destructor
        */
        ~CMMCScBkupArchive();

    private:

        /**
        * C++ default constructor
        */
        CMMCScBkupArchive( RFs& aFsSession, MMMCScBkupProgressObserver& aProgressManager, MMMCScBkupDriver& aDriver, TBitFlags aCategory );

        /**
        *
        */
        void ConstructL();

    public: // API

        /**
        *
        */
        void OpenForReadingL(const TDesC& aName);

        /**
        *
        */
        void OpenForWritingL(const TDesC& aName);
        
        /**
        *
        */
        void RestoreOldArchive();
        
        /**
        *
        */
        void DeleteOldArchive();

        /**
        *
        */
        void Close( TInt aError );

        /**
        *
        */
        CMMCScBkupArchiveHeader& Header() const;

        /**
        *
        */
        CMMCScBkupArchiveFooter& Footer() const;

        /**
        *
        */
        MMMCScBkupArchiveDataInterface& ADI() const;

        /**
        *
        */
        static TBool ValidArchiveForRestoreL( RFs& aFsSession, const TDesC& aFileName );
        
        /**
        *
        */
        TBitFlags Category() const { return iCategory; }

    private: // Internal enumerations

        /**
        *
        */
        enum TMode
            {
            EModeUninitialised = 0,
            //
            EModeReading,
            EModeWriting
            };

    private: // Internal methods
       
        /**
        *
        */
        TInt PrepareToOverwrite( const TDesC& aFile );

        /**
        *
        */
        void SetMode(TMode aMode);

        /**
        *
        */
        void PrepareObjectsL();
        
        /**
        *
        */
        inline TMode Mode() const { return iMode; }

    private: // Member data

        //
        RFs& iFsSession;
        //
        RFile64 iArchiveFile;
        //
        MMMCScBkupProgressObserver& iProgressManager;
        //
        MMMCScBkupDriver& iDriver;
        //
        TMode iMode;
        //
        CMMCScBkupArchiveDataManager* iDataManager;
        //
        CMMCScBkupArchiveHeader* iHeader;
        //
        CMMCScBkupArchiveFooter* iFooter;
        //
        HBufC* iArchiveFileName;
        //
        HBufC* iOldArchiveFileName;
        //
        TBitFlags iCategory;
    };




#endif // __CMMCSCBKUPARCHIVE_H__

//End of File
