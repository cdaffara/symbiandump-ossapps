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
* Description: Declaration of CMMCScBkupDriveAndOperationTypeManager
*     
*
*/

#ifndef __CMMCSCBKUPDRIVEANDOPERATIONTYPEMANAGER_H__
#define __CMMCSCBKUPDRIVEANDOPERATIONTYPEMANAGER_H__

// System includes
#include <connect/sbdefs.h>
#include <connect/sbtypes.h>
#include <barsread.h>

// User includes
#include "TMMCScBkupOwnerDataType.h"
#include "CMMCScBkupOperationParameters.h"
// Namespaces
using namespace conn;



/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(TMMCScBkupDriveAndOperationType)
    {
    public:
        inline TMMCScBkupDriveAndOperationType()
            : iDrive( EDriveC ), iType( EMMCScBkupOwnerDataTypeAny ) { }

        inline TMMCScBkupDriveAndOperationType( TDriveNumber aDrive, TMMCScBkupOwnerDataType aType )
            : iDrive( aDrive ), iType( aType ) { }
 
    public: // Common attributes

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
        inline TMMCScBkupOwnerDataType DataType() const { return iType; }

        /**
        *
        */
        inline void SetDataType( TMMCScBkupOwnerDataType aType ) { iType = aType; }

    private: // Data members

        //
        TDriveNumber iDrive;
        //
        TMMCScBkupOwnerDataType iType;
    };







/**
*
*
* @since 3.0
*/
NONSHARABLE_CLASS(CMMCScBkupDriveAndOperationTypeManager) : public CBase
    {
    public:

        /**
        *
        */
        IMPORT_C static CMMCScBkupDriveAndOperationTypeManager* NewL( 
        		BkupDrivesAndOperationList *aDriveList );

        /**
        *
        */
        IMPORT_C ~CMMCScBkupDriveAndOperationTypeManager();

    private:

        /**
        *
        */
        CMMCScBkupDriveAndOperationTypeManager();

        /**
        *
        */
        void ConstructL( BkupDrivesAndOperationList *aDriveList );


    public: // API

        /**
        *
        */
        TInt Count() const;

        /**
        *
        */
        const TMMCScBkupDriveAndOperationType& At( TInt aIndex ) const;
        
        /**
        *
        */
        inline const TDriveList& DriveList() const { return iCalculatedDriveList; }

        /**
        *
        */
        TBool IsDataTypeAllowedToAccessDrive( TDriveNumber aDrive, TMMCScBkupOwnerDataType aDataType ) const;

    private: // Internal methods


    private: // Data members
        
        //
        RArray< TMMCScBkupDriveAndOperationType > iEntries;
        //
        TDriveList iCalculatedDriveList;

    };

#endif // __CMMCSCBKUPDRIVEANDOPERATIONTYPEMANAGER_H__

//End of File
