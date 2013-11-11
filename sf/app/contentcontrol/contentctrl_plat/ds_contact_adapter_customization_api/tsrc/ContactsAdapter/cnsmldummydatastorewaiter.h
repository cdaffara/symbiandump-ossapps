/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for datastorewaiter helper class.
*
*/

#ifndef __CNSMLDUMMYDATASTOREWAITER_H__
#define __CNSMLDUMMYDATASTOREWAITER_H__

// INCLUDES
#include <e32base.h>
#include "nsmldummydatastore.h"

// FORWARD DECLARATIONS
class MSmlSyncRelationship;

// CLASS DEFINITION


NONSHARABLE_CLASS( CNSmlDummyDataStoreWaiter ):
    public CActive
    {
    public:  // Constructors and destructor
        static CNSmlDummyDataStoreWaiter* NewL( CNSmlDummyDataStore& aStore );
        static CNSmlDummyDataStoreWaiter* NewLC( CNSmlDummyDataStore& aStore );
        virtual ~CNSmlDummyDataStoreWaiter();
        
    private:  // Constructors
        CNSmlDummyDataStoreWaiter( CNSmlDummyDataStore& aStore );
        void ConstructL();
        
    private:  // From CActive
        void DoCancel();
        void RunL();
    
    public:
        void OpenStoreL( MSmlSyncRelationship& aRelationship, 
            const TDesC& aStoreName = KNSmlContactStoreNameForDefaultDB );
        TInt DoCommitChangeInfoL( const MSmlDataItemUidSet& aItems );
        TInt DoResetChangeInfoL();
        TInt DoCommitChangeInfoL();
        
        TInt DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize, 
            const TDesC8& aMimeType, const TDesC8& aMimeVer );
        TInt DoCommitItemL();
                
        TInt DoCommitBatchL( RArray<TInt>& aResultArray );
        void DoCancelBatch();
        
        TInt DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, 
            TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, 
            TDes8& aMimeVer );
        TInt DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize, 
            TSmlDbItemUid aParent, TBool aFieldChange );
        TInt DoDeleteItemL( TSmlDbItemUid aUid );
        TInt DoDeleteAllItemsL();

    private:  // Data
        CActiveSchedulerWait* iWait;
        CNSmlDummyDataStore& iStore;
        TBool iOpen;
    };

#endif // __CNSMLDUMMYDATASTOREWAITER_H__

// End of file
