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
* Description: Declaration for RMMCScBkupArchiveReadStream
*     
*
*/

#ifndef __RMMCSCBKUPARCHIVESTREAMS_H__
#define __RMMCSCBKUPARCHIVESTREAMS_H__

// System includes
#include <s32strm.h>
#include <s32file.h>

// User includes
#include "TMMCScBkupArchiveVector.h"

// Classes referenced
class CMMCScBkupArchiveDataManager;



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(RMMCScBkupArchiveBuf) : public RFileBuf
    {
    public:

        /**
        *
        */
        RMMCScBkupArchiveBuf();

    public: // API

        /**
        *
        */
        void AttachL(CMMCScBkupArchiveDataManager& aADM, RFile64& aFile, TInt aPos, TBool aForWrite );

        /**
        *
        */
        IMPORT_C void Close();

        /**
        *
        */
        TBool IsOpen() const;

    private: // From MStreamBuf

        /**
        *
        */
        void DoRelease();

        /**
        *
        */
        void DoSynchL();
      
#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        void DoWriteL(const TAny* aPtr,TInt aLength);
#endif
      
    private:

        /**
        *
        */
        void UpdateInfoL();

    private: // Member data

        //
        TBool iWriteMode;
        //
        CMMCScBkupArchiveDataManager* iADM;
        //
        TMMCScBkupArchiveVector iInfo;

    };

/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(RMMCScBkupArchiveReadStream) : public RReadStream
    {
    public: // Constructors

        /**
        *
        */
        inline RMMCScBkupArchiveReadStream() { }

    public: // API

        /**
        *
        */
        void OpenLC(CMMCScBkupArchiveDataManager& aADM, RFile64& aArchiveFile, TInt aPos=0);

        /**
        *
        */
        TBool IsOpen() const;

    private: // Member data
        
        //
        RMMCScBkupArchiveBuf iSource;
    };



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(RMMCScBkupArchiveWriteStream) : public RWriteStream
    {
    public: // Constructors

        /**
        *
        */
        inline RMMCScBkupArchiveWriteStream()
            {}

    public: // API

        /**
        *
        */
        void OpenLC(CMMCScBkupArchiveDataManager& aADM, RFile64& aArchiveFile, TInt aPos=0);

        /**
        *
        */
        TBool IsOpen() const;

    private: // Member data
        
        //
        RMMCScBkupArchiveBuf iSink;
    };



#endif // __RMMCSCBKUPARCHIVESTREAMS_H__

//End of File
