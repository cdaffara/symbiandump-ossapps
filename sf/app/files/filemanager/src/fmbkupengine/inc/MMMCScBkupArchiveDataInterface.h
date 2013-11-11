/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for MMMCScBkupArchiveDataInterface
*     
*
*/

#ifndef __MMMCSCBKUPARCHIVEDATAINTERFACE_H__
#define __MMMCSCBKUPARCHIVEDATAINTERFACE_H__

// System includes
#include <e32std.h>
#include <f32file.h>

// User includes
#include "TMMCScBkupArchiveVector.h"
#include "MMCScBkupOperations.h"

// Constants
const TInt KMMCScBkupArchiveWriteStreamCurrentPos = -1;
const TInt KMMCScBkupArchiveReadStreamCurrentPos = -1;

/**
*
*
* @since 3.0
*/
class MMMCScBkupArchiveDataInterface
    {
    public: // From MMMCScBkupArchiveDataInterface

        /**
        *
        */
        virtual RFs& ADIFsSession() const = 0;

        /**
        *
        */
        virtual RFile64& ADIRawArchiveFile() const = 0;

        /**
        *
        */
        virtual const TMMCScBkupArchiveVector& ADICurrentArchiveVectorInfo() const = 0;

        /**
        *
        */
        virtual const TMMCScBkupArchiveVector& ADIOverallArchiveVectorInfo() const = 0;

        /**
        *
        */
        virtual RWriteStream& ADIWriteStreamUncompressedLC( TInt aPos = KMMCScBkupArchiveWriteStreamCurrentPos ) = 0;

        /**
        *
        */
        virtual RReadStream& ADIReadStreamUncompressedLC( TInt aPos = KMMCScBkupArchiveReadStreamCurrentPos ) = 0;

        /**
        *
        */
        virtual const TMMCScBkupArchiveVector& ADIWriteL( const TDesC8& aData ) = 0;

        /**
        *
        */
        virtual const TMMCScBkupArchiveVector& ADIReadL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo ) = 0;

        /**
        *
        */
        virtual void ADIWriteFileL( const TDesC& aSourceFileName, TRequestStatus& aStatus ) = 0;

        /**
        *
        */
        virtual void ADIReadFileL( const TDesC& aDestinationFileName, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus ) = 0;

        /**
        *
        */
        virtual void ADIWriteCompressedL( const TDesC8& aData, TRequestStatus& aStatus ) = 0;

        /**
        *
        */
        virtual void ADIReadDecompressedL( TDes8& aSink, const TMMCScBkupArchiveVector& aInfo, TRequestStatus& aStatus ) = 0;
        
        /**
        *
        */
        virtual void ADIAsynchronousCancel() = 0;

        /**
        *
        */
        virtual void ADIResetResources(TMMCScBkupOperationType aType) = 0;

#ifdef RD_FILE_MANAGER_BACKUP
        /**
        *
        */
        virtual void ADIActivateCrcCalculation(TBool aType) = 0;

        /**
        *
        */
        virtual TUint32 ADIArchiveCrc() = 0;
#endif
    };




#endif // __MMMCSCBKUPARCHIVEDATAINTERFACE_H__

//End of File
