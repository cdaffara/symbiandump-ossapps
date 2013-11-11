/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store utilities.
*
*/



#ifndef __CONTAINER_STORE_UTILS_H__
#define __CONTAINER_STORE_UTILS_H__

// ========
// INCLUDES
// ========

#include <e32std.h>
#include <f32file.h>
#include <d32dbms.h>  // database
#include <s32file.h>  // stream store 
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

// ====================
// FORWARD DECLARATIONS
// ====================

class RDbDatabase;
class RDbRowSet;
class RDbTable;
class CCompactionHandler;
class CDeleteHandler;

// =======
// CLASSES
// =======

// : WOULD BE GOOD TO RENAME THIS SOMETHING LIKE CContainerStoreDatabase

// ===========================
// CLASS: CContainerStoreUtils
// ===========================
class CContainerStoreUtils : public CBase
    {
    public:

        // ==============
        // PUBLIC METHODS
        // ==============
    
        static CContainerStoreUtils* NewL( TDriveNumber    aDriveNumber, 
                                           TInt            aCompactionPriority,
                                           const TDesC&    aDbFilename,
                                           CDeleteHandler& aDeleteHandler );
        
        static void LeaveIfFalse( TBool aCondition, TInt aLeaveCode );
        
        virtual ~CContainerStoreUtils();
    
		RFs& FileSystem();												
		
		void LeaveIfLowDiskSpaceL( TUint aBytesToWrite );
		
		const TDesC& PrivatePath() const;
		
		void SuspendCompactionLC();
		
		void ResumeCompaction();

        // Database management functions

        void CreateDatabaseL();
        
        void CreateTableL( const TDesC& aTableName, CDbColSet& aColSet );
        
        void CreateIndexL( const TDesC& aName, const TDesC& aTable, const CDbKey& aKey );

        void OpenDatabaseL();
        
        void OpenTableL( RDbTable& aTable, const TDesC& aTableName );

        void CloseTable( RDbTable& aTable );        

        void CloseDatabaseL();        
		
        // Ignore compactation
        void CloseDatabase();
        
		// Long database column functions

	    void ReadLongColumnL( RDbRowSet& aRowSet, 
		                      TUint      aColNum, 
		                      RBuf8&     aBuffer,
		                      TUint      aBufferPadSpace = 0 );

		void WriteLongColumnL( RDbRowSet&    aRowSet,
                	           TUint         aColNum, 
                      	       const TDesC8& aBuffer );

        // SQL view functions
                              	       
        void PopulateViewL( RDbView& aView, const TDesC& aSqlQuery, RDbRowSet::TAccess aAccess = RDbRowSet::EReadOnly );

        void Execute( const TDesC &aSql, TDbTextComparison aComparison=EDbCompareNormal );
                      	       
        // Database transaction functions
        
		void BeginDatabaseTransactionLC();		

		void CommitDatabaseTransactionL();		

		void RollbackDatabaseTransactionL();
		
		inline TBool InTransaction() { return iDatabase.InTransaction(); }
        
        TBool FindFirstEncryptedOrUnencryptedL( RDbTable& aTable, const TDesC& aColName, TBool aFindEncrypted, TDbBookmark& aBookmark );
	
        #ifdef _DEBUG		
		void SimulateLowDiskSpace( TInt aLatency );
        #endif		
		
    private:
    
        // ===============
        // PRIVATE METHODS
        // ===============
    
        CContainerStoreUtils( TDriveNumber    aDriveNumber, 
                              TInt            aCompactionPriority,
                              const TDesC&    aDbFilename,
                              CDeleteHandler& aDeleteHandler );
        
        void ConstructL(); 

        static void RollbackDatabaseTransactionL( TAny* aObject );
    
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
    
        const TDriveNumber      iDriveNumber;
        const TInt              iCompactionPriority;
        const TDesC&            iDbFilename;
        CDeleteHandler&         iDeleteHandler;
        RDbStoreDatabase        iDatabase;
        RFs                     iFs;
		CFileStore*             iFileStore;		
        TFileName               iPrivatePath;
   		CCompactionHandler*     iCompactionHandler;
   		
   		RPointerArray<RDbTable> iTables;

        #ifdef _DEBUG		
        TInt               iLowDiskSpaceLatency;
        #endif

        __LOG_DECLARATION

    }; // end class ContainerStoreUtils
    
#endif
