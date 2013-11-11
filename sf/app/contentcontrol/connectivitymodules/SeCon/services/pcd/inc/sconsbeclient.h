/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CSConSBEClient header file
*
*/


#ifndef _SCONSBECLIENT_H_
#define _SCONSBECLIENT_H_

// INCLUDES
#include <connect/sbeclient.h>
#include "sconconmltask.h"

using namespace conn;

//============================================================
// Class CSConSBEClient declaration
//============================================================
NONSHARABLE_CLASS ( CSConSBEClient ) : public CActive
    {
    public:
        /**
         * Two-phase constructor
         * @param aFs aReference to RFs connection.
         * @return CSConSBEClient instance
         */
        static CSConSBEClient* NewL( RFs& aFs );
        /**
         * Destructor
         * @return none
         */
        ~CSConSBEClient();
        /**
         * Set backup/restore mode
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void SetBURMode( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Lists public files from data owners
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void ListPublicFiles( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Lists participant data owners
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void ListDataOwners( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Lists data sizes from data owners
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void GetDataSize( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Request a data from a data owner
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void RequestData( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Lists the status of data owners
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void GetDataOwnerStatus( TRequestStatus& aStatus, CSConTask*& aTask );
        /**
         * Supplies a data to a data owner
         * @param aStatus TRequestStatus of the request
         * @param aTask Task parameters
         * @return none
         */
        void SupplyData( TRequestStatus& aStatus, CSConTask*& aTask );
    
    private:
        /**
         * Constructor
         * @param aFs aReference to RFs connection.
         * @return none
         */
        CSConSBEClient( RFs& aFs );
        
        /**
         * Implementation of CActive::DoCancel()
         * @return none
         */
        void DoCancel();
        /**
         * Implementation of CActive::RunL()
         * @return none
         */
        void RunL();
        
        /**
         * Executes SetBURMode task
         * @return none
         */
        void ProcessSetBURModeL();
        /**
         * Executes ListPublicFiles task
         * @return none
         */
        void ProcessListPublicFilesL();
        /**
         * Executes ListDataOwners task
         * @return none
         */
        void ProcessListDataOwnersL();
        /**
         * Executes GetDataSize task
         * @return none
         */
        void ProcessGetDataSizeL();
        /**
         * Executes RequestData task
         * @return KErrNone if no erros. Else system wide error codes.
         */
        TInt ProcessRequestDataL();
		/**
		 * Request data to iDataBuffer
		 * @return none
		 */
		void RequestDataL( CSBGenericTransferType& aGenericTransferType );
		/**
         * Executes GetDataOwnerStatus task
         * @return none
         */
        void ProcessGetDataOwnerStatusL();
        /**
         * Executes SupplyData task
         * @return KErrNone if no errors. Else system wide error codes.
         */
        TInt ProcessSupplyDataL();
        /**
         * Maps TInt drive number to TDriveNumber
         * @param aDrive Drive number as TInt
         * @return drive number as TDriveNumber
         */
        TDriveNumber GetDriveNumber( TInt aDrive ) const;
        /**
         * Filters the drive list
         * @param aDriveList Drive list to be filtered
         * @return none
         */     
        void FilterDriveList( TDriveList& aDriveList ) const;
        /**
         * Matches the uids
         * @param aFirst The first uid
         * @param aSecon The second uid
         * @return ETrue if uids match, else EFalse
         */
        static TBool Match( const TUid& aFirst, 
                            const TUid& aSecond );
        /**
         * Handle error received from Secure Backup Engine
         * @param aErr Error code from Secure Backup Engine
         * @return none
         */
        void HandleSBEErrorL( TInt& aErr );
        
        /**
         * Appends public files to CSConFile array.
         * @param aFiles source files to append
         * @param aSconFiles dataowner list where to add
         * @return none
         */
        void AppendFilesToFilelistL( const RFileArray& aFiles, RPointerArray<CSConFile>& aSconFiles );
        
        /**
         * Gets Package dataowner size
         * @param aPackageId dataowner
         * @param TDriveList drives to include
         * @param TPackageDataType package data type 
         * @return dataowner data size in specified drives
         */
        TUint PackageDataSizeL( TUid aPackageId, const TDriveList& aDriveList,
                TPackageDataType aPackageDataType ) const;
        
        /**
         * Gets Sid dataowner size
         * @param aSid dataowner
         * @param TDriveList drives to include
         * @param TTransferDataType transfer data type 
         * @return dataowner data size in specified drives
         */
        TUint SidDataSizeL( TUid aSid, const TDriveList& aDriveList,
                TTransferDataType aTransferDataType ) const;
        
        /**
         * Gets Java dataowner size
         * @param aJavaHash Java hash value
         * @param TDriveList drives to include
         * @return dataowner data size in specified drives
         */
         TUint JavaDataSizeL( const TDesC& aJavaHash, const TDriveList& aDriveList ) const;
         
	private:
		CSBEClient*						iSBEClient;
		TRequestStatus* 				iCallerStatus;
		CSConTask*						iCurrentTask;
		TBool							iBURModeNormal;
		TBool                           iRestoreMode;
		RFs&                            iFs;
		TBool                           iAllSnapshotsSuppliedCalled;
		TBool                           iLastChunk;
		CBufFlat*                       iDataBuffer;
		TInt                            iDataBufferSize;
	};

#endif

// End of file
