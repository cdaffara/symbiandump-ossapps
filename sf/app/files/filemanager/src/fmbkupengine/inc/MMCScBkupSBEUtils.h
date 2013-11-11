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
* Description: Declaration for MMCScBkupSBEUtils
*     
*
*/

#ifndef __MMCSCBKUPSBEUTILS_H__
#define __MMCSCBKUPSBEUTILS_H__

// System includes
#include <f32file.h>
#include <connect/sbtypes.h>
#include <connect/sbeclient.h>

// Namespaces
using namespace conn;


/**
*
*
* @since 3.0
*/
class MMCScBkupSBEUtils
    {
    public:

        /**
        *
        */
        static TSecureId SecureIdFromGenericL(const CSBGenericDataType& aGeneric);

        /**
        *
        */
        static TUid PackageIdFromGenericL(const CSBGenericDataType& aGeneric);

        /**
        *
        */
        static HBufC* JavaHashFromGenericLC(const CSBGenericDataType& aGeneric);

        /**
        *
        */
        static CSBGenericDataType* CopyLC(const CSBGenericDataType& aToBeCopied);

        /**
        *
        */
        static CSBGenericTransferType* TransferTypeLC(const CSBGenericDataType& aDT, TDriveNumber aDrive, 
            TTransferDataType aTransferType, TInt aVersion);

        /**
        *
        */
        static CSBGenericTransferType* TransferTypeLC(const CSBGenericDataType& aDT, TDriveNumber aDrive, 
            TPackageDataType aDataType, TInt aVersion);

        /**
        *
        */
        static CSBGenericTransferType* TransferTypeLC(const CSBGenericDataType& aDT, TDriveNumber aDrive, 
            TJavaTransferType aDataType, TInt aVersion);

        /**
        *
        */
        static TBool PhoneIsInBackupOrRestoreModeL();

        /**
        *
        */
        static void EndBackupOrRestoreL( CSBEClient& aSBEClient );
        
        /**
        *
        */
        static TBool HasSystemDataL( const CDataOwnerInfo& aDataOwner );

        /**
        *
        */
        static TBool HasJavaDataL( const CDataOwnerInfo& aDataOwner );

        /**
        *
        */
        static TBool HasPassiveDataL( const CDataOwnerInfo& aDataOwner );

        /**
        *
        */
        static TBool HasActiveDataL( const CDataOwnerInfo& aDataOwner );

        /**
        *
        */
        static TBool HasPublicDataL( const CDataOwnerInfo& aDataOwner );
    };




#endif // __MMCSCBKUPSBEUTILS_H__

//End of File
