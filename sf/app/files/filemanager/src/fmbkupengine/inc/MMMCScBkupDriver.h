/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Declaration for MMMCScBkupDriver
*     
*
*/

#ifndef __MMMCSCBKUPDRIVER_H__
#define __MMMCSCBKUPDRIVER_H__

// System includes
#include <connect/sbeclient.h>

// User includes
#include "MMCScBkupStateIds.h"
#include "MMCScBkupOperations.h"
#include "CMMCScBkupOperationParameters.h"

// Classes referenced
class CMMCScBkupState;
class CMMCScBkupArchive;
class MMMCScBkupProgressObserver;
class CMMCScBkupDataOwnerCollection;
class CMMCScBkupFileListCollection;
class MMMCScBkupArchiveDataInterface;

// Namespaces
using namespace conn;


/**
*
*
* @since 3.0
*/
class MMMCScBkupDriver
    {
    public: // From MMMCScBkupDriver

        /**
        *
        */
        virtual MMMCScBkupArchiveDataInterface& DrvADI() const = 0;

        /**
        *
        */
        virtual CMMCScBkupArchive& DrvArchive() const = 0;

        /**
        *
        */
        virtual CSBEClient& DrvSecureBackupClient() const = 0;

        /**
        *
        */
        virtual TMMCScBkupOperationType DrvOperation() const = 0;

        /**
        *
        */
        virtual CMMCScBkupOpParamsBase& DrvParamsBase() const = 0;

        /**
        *
        */
        virtual CMMCScBkupDataOwnerCollection& DrvDataOwners() const = 0;

        /**
        *
        */
        virtual RPointerArray<CMMCScBkupDataOwnerCollection>& DrvDataOwnersAll() = 0;
        
        /**
        *
        */
        virtual CMMCScBkupFileListCollection& DrvFileList() const = 0;
        
        /**
        *
        */
        virtual TBool DrvLastCategory() const = 0;

        /**
        *
        */
        virtual void DrvStoreTotalProgress(TInt64 aProgress) = 0;
        
        /**
        *
        */
        virtual TInt64 DrvTotalProgress() const = 0;
        
        /**
        *
        */
        virtual MMMCScBkupProgressObserver& DrvProgressHandler() const = 0;
    };




#endif // __MMMCSCBKUPDRIVER_H__

//End of File
