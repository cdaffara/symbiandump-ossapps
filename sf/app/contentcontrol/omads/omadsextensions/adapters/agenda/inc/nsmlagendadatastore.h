/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS agenda data store
*
*/


#ifndef __NSMLAGENDADATASTORE_H__
#define __NSMLAGENDADATASTORE_H__


//  INCLUDES
#include "nsmlconstants.h"
#include "nsmlchangefinder.h"
#include "NSmlDataModBase.h"
#include "nsmlagendadefines.hrh"
#include <calentry.h>
#include <calsession.h> 
#include <caldataexchange.h> 
#include <calentryview.h>
#include <f32file.h>
#include <SmlDataProvider.h>
#include <SmlDataFormat.h>
#include <calprogresscallback.h>
#include <msvapi.h>
#include <CalenImporter.h>
#include <CalenExporter.h>
#include <calinstanceview.h>
#include <CalenInterimUtils2.h>
#include <versittls.h>

// CONSTANTS
const TInt KNSmlAgendaCompressLimit        = 20;
const TInt KNSmlAgendaLittleArraySize      = 4;
const TInt KNSmlAgendaSmallArraySize       = 8;
const TInt KNSmlAgendaLargeArraySize       = 128;
const TInt KNSmlAgendaOwnMaxObjectSize     = 1048576;
const TInt KNSmlCompactAfterChanges        = 30;
const TInt KNSmlDefaultStoreNameMaxSize    = 256;
const TInt KNSmlItemDataExpandSize         = 1024;
_LIT( KNSmlDriveC, "C" );
_LIT( KNSmlAgendaStoreNameForDefaultDB, "C:Calendar" );
_LIT8( KNSmlMRUtilitiesEComPlugInUID,   "AgnEntryUi" );
_LIT8( KNSmlVersitTokenRecurrenceID, 	"RECURRENCE-ID");
_LIT8( KNSmlVersitTokenXRecurrenceID, 	"X-RECURRENCE-ID");
_LIT8(KNSmlVersitTokenGeoID,"GEO");


// DATA TYPES
enum TNSmlDataStoreStatus
	{
	ENSmlClosed = 0,
	ENSmlOpenAndWaiting,
	ENSmlItemOpen,
	ENSmlItemCreating,
	ENSmlItemUpdating
	};
	
enum TNSmlDataEntryType
	{
	ENSmlVCal = 1,
	ENSmlICal,
	ENSmlNotSet
	};

// CLASS DECLARATIONS
// ----------------------------------------------------------------------------
// CNSmlAgendaProgressview provides progress observations for some calendar
// classes.
// @lib nsmlagendadataprovider.dll
// ----------------------------------------------------------------------------
class CNSmlAgendaProgressview : public CBase , public MCalProgressCallBack
    {
    public:
        /**
        * Two-phased constructor.
        */
    	static CNSmlAgendaProgressview* NewL();

        /**
        * Destructor.
        */
    	~CNSmlAgendaProgressview();

    	/**
        * This calls the observing class with the percentage complete
        * of the current operation.
        * @param  aPercentageCompleted    The percentage complete
        */
    	void Progress( TInt aPercentageCompleted );
    	
    	/**
        * This calls the observing class when the current operation is
        * finished.
        * @param    aError  The error if the operation failed,
        *                   or KErrNone if successful.
        */
    	void Completed( TInt aError );

    	/**
        * This method returns status of Completed method (see above).
        * @return   Value based on status of Completed method.
        *           KErrNotReady is returned if Completed method is not
        *           called or same value is Completed status.
        */
    	TInt GetCompletedStatus();
    	
    	/**
        * Asks the observing class whether progress callbacks are required.
        * @return   If the observing class returns EFalse, then the Progress()
        *           function will not be called.
        */
    	TBool NotifyProgress();

    private:
        /**
        * C++ default constructor.
        */
    	CNSmlAgendaProgressview();

        /**
        * Second phase constructor.
        */
    	void ConstructL();
        
    private:
    	TInt    iCompletedStatus;
	};

// ----------------------------------------------------------------------------
// CNSmlAgendaDataStore provides services to cope with calendar database access
// for calendar events manipulation.
// @lib nsmlagendadataprovider.dll
// ----------------------------------------------------------------------------
class CNSmlAgendaDataStore : public CSmlDataStore
	{
    public:
        
        /**
        * Two-phased constructor.
        */
	    static CNSmlAgendaDataStore* NewL();
        
        /**
        * Destructor.
        */
	    virtual ~CNSmlAgendaDataStore();

    public: // New functions
        
        /**
        * Returns a pointer to array of all possible Calendar database names
        * which are provited by Symbian OS Calendar API.
        * @param none
        * @return Pointer to array of all possible Calendar database names 
        */
	    CDesCArray *DoListAgendaFilesLC() const;
 
        /**
        * Returns a reference to default calendar file name which is provited
        * by Symbian OS Calendar API.
        * @param none
        * @return Reference to default calendar file name
        */
	    const TDesC& DoGetDefaultFileNameL() const;
	    
    private:

        /**
        * C++ default constructor.
        */
 	    CNSmlAgendaDataStore();

        /**
        * Returns the used store format.
        */
	    CSmlDataStoreFormat* DoOwnStoreFormatL();

	    /**
	    * Opens the data store specified by aStoreName asynchronously.
	    * @param	aStoreName		The name of the data store to open.
	    * @param	aContext		Identifies the specific synchronisation
	    *                           relationship to use for the
	    *                           synchronisation.
	    * @param	aStatus			On completion of the open,
	    *                           contains the result code.
	    */
	    void DoOpenL( const TDesC& aStoreName, MSmlSyncRelationship& aContext,
	                    TRequestStatus& aStatus );

	    /**
	    * Cancel the current asynchronous request, including open. 
	    * Only one asynchronous request may be outstanding at any one time.
	    */
	    void DoCancelRequest();

	    /**
	    * Returns the name of the open data store.
	    */
	    const TDesC& DoStoreName() const;

	    /**
	    * BeginTransactionL() starts the transaction mode. During this mode
	    * calls to CreateItemL, ReplaceItemL, WriteItemL, CommitItemL,
	    * MoveItemL, DeleteItemL and SoftDeleteItemL will be part of this
	    * transaction.
	    * Their RequestStatus must be completed, even if the change is not
	    * yet really executed in the Data Store.
	    * If a RequestStatus is completed with an error code, the transaction
	    * has failed and a rollback must be done. In this case
	    * RevertTransaction will be called.
	    */
	    void DoBeginTransactionL();

	    /**
	    * CommitTransactionL() will be called at the end of a successful
	    * transaction. At this point in time the operations within the
	    * transaction are applied to the Data Store in an atomic way.
	    * If all operations succeed, the RequestStatus must be completed
	    * with KErrNone. If an operation fails, a rollback must be done and
	    * the RequestStatus must be completed with an appropriate error code.
	    */
	    void DoCommitTransactionL( TRequestStatus& aStatus );

	    /**
	    * RevertTransaction() will be called to abort an ongoing transaction.
	    * None of the operations already submitted may be applied to the Data
	    * Store. The RequestStatus must be completed with KErrNone as a revert
	    * cannot fail.
	    */
	    void DoRevertTransaction( TRequestStatus& aStatus );

	    /**
	    * BeginBatchL() starts the batch mode. During this mode calls to
	    * CreateItemL, ReplaceItemL, WriteItemL, CommitItemL, MoveItemL,
	    * DeleteItemL and SoftDeleteItemL will be part of this batch.
	    * Their RequestStatus must be completed with KErrNone, which only
	    * signals acceptance of the operation for batch processing.
	    */
	    void DoBeginBatchL();

	    /**
	    * CommitBatchL() will be called at the end of the batch mode. This
	    * tells the Data Store to process the batched operations (in the
	    * order they were submitted), and to append the error code for each
	    * operation to aResultArray.
	    * The error codes in aResultArray are only valid if the RequestStatus
	    * is completed with KErrNone.
	    * If the RequestStatus is completed with an error code none of the
	    * operations in the batch mode were applied to the Data Store.
	    */
	    void DoCommitBatchL( RArray<TInt>& aResultArray,
                        TRequestStatus& aStatus );

	    /**
	    * CancelBatch() will be called to abort an ongoing batch mode.
	    * None of the operations already submitted may be applied to
	    * the Data Store.
	    */
	    void DoCancelBatch();

	    /**
	    * Sets the SyncML server Data Format - this may optionally be used
	    * by the Data Provider to filter out properties that the server does
	    * not support, and should be used to avoid deleting these properties
	    * in case the server sends a changed item to the Data Provider
	    */
	    void DoSetRemoteStoreFormatL(
	                    const CSmlDataStoreFormat& aServerDataStoreFormat );

	    /**
	    * Sets the SyncML server maximum object size - this may optionally
	    * be used by the Data Provider to not send items to the server
	    * exceeding its maximum size. 0 means there is no limit.
	    */
	    void DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize );

	    /**
	    * Gets the Data Store maximum object size which is reported to
	    * the SyncML server. 0 means there is no limit.
	    */
	    TInt DoMaxObjectSize() const;

		/**
		* Opens the data item specified by aUid asynchronously for reading.
		* @param		aUid				Item UID which going to be read.
		* @param		aFieldChange		Accept field changes.
		* @param		aParent				Parent of the item.
		* @param		aSize				Size of the item data.
		* @param		aMimeType			MIME type of the item.
		* @param		aMimeVer			MIME version used on item.
		* @param		aStatus				On completion of the opening of
		*                                   item, contains the result code.
		*/
	    void DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize,
	                        TSmlDbItemUid& aParent, TDes8& aMimeType,
	                        TDes8& aMimeVer, TRequestStatus& aStatus );

		/**
		* Sets the item properties and reference to aUid which will be created.
		* @param		aUid				Reference to item UID which going
		*                                   to be created.
		* @param		aSize				Size of the item to be created.
		* @param		aParent				Parent of the item.
		* @param		aMimeType			MIME type of the item.
		* @param		aMimeVer			MIME version used on item.
		* @param		aStatus				On completion of the creating an
		*                                   item, contains the result code.
		*/
	    void DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize,
	                    TSmlDbItemUid aParent, const TDesC8& aMimeType,
                        const TDesC8& aMimeVer, TRequestStatus& aStatus );

		/**
		* Opens the data item specified by aUid asynchronously to be updated.
		* @param		aUid				Item UID which going to be updated.
		* @param		aSize				Size of the item data.
		* @param		aParent				Parent of the item.
		* @param		aFieldChange		Accept field changes.
		* @param		aStatus				On completion of the updating of
		*                                   item, contains the result code.
		*/
	    void DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize,
                        TSmlDbItemUid aParent, TBool aFieldChange,
                        TRequestStatus& aStatus );

		/**
		* Reads data(or size of aBuffer) of an item opened in DoOpenItemL()
		* to given aBuffer.
		* @param		aBuffer				Buffer to item data.
		*/
	    void DoReadItemL( TDes8& aBuffer );

		/**
		* Writes aData of an item opened in DoCreateItemL() or DoReplaceItemL()
		*  to be saved on database.
		* @param		aData				Item data (or part of data).
		*/
	    void DoWriteItemL( const TDesC8& aData );

		/**
		* Completes an item operation started in DoCreateItemL() or
		* DoReplaceItemL().
		* @param		aStatus				On completion of the operation,
		*                                   contains the result code.
		*/
	    void DoCommitItemL( TRequestStatus& aStatus );

		/**
		* Completes an item operation started in DoOpenItemL().
		*/
	    void DoCloseItem();

		/**
		* Moves item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be moved.
		* @param		aNewParent			A new parent of the item.
		* @param		aStatus				On completion of the moving an
		*                                   item, contains the result code.
		*/
	    void DoMoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, TRequestStatus& aStatus );

		/**
		* Deletes item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be deleted.
		* @param		aStatus				On completion of the deleting an
		*                                   item, contains the result code.
		*/
	    void DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );

		/**
		* Softdeletes item specified by aUid asynchronously.
		* @param		aUid				Item UID which going to be
		*                                   softdeleted.
		* @param		aStatus				On completion of the softdeleting
		*                                   an item, contains the result code.
		*/
	    void DoSoftDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus );

		/**
		* Deletes all items from opened database asynchronously.
		* @param		aStatus				On completion of delete, contains
		*                                   the result code.
		*/
	    void DoDeleteAllItemsL( TRequestStatus& aStatus );

		/**
		* Check if previous sync with opened server and context.
		*/
	    TBool DoHasSyncHistory() const;

		/**
		* Get all added items on client since previous synchronization.
		*/
	    const MSmlDataItemUidSet& DoAddedItems() const;

		/**
		* Get all deleted items on client since previous synchronization.
		*/
	    const MSmlDataItemUidSet& DoDeletedItems() const;

		/**
		* Get all softdeleted items on client since previous synchronization.
		*/
	    const MSmlDataItemUidSet& DoSoftDeletedItems() const;

		/**
		* Get all modified items on client since previous synchronization.
		*/
	    const MSmlDataItemUidSet& DoModifiedItems() const;

		/**
		* Get all moved items on client since previous synchronization.
		*/
	    const MSmlDataItemUidSet& DoMovedItems() const;

		/**
		* Resets client synchronization data => next time will be slow sync.
		* @param		aStatus				On completion of reset, contains
		*                                   the result code.
		*/
	    void DoResetChangeInfoL( TRequestStatus& aStatus );

		/**
		* Commits client synchronization changes for given aItems list.
		* @param		aStatus				On completion of given items,
		*                                   contains the result code.
		* @param		aItems				Item ids to be commited.
		*/
	    void DoCommitChangeInfoL( TRequestStatus& aStatus,
	                    const MSmlDataItemUidSet& aItems );

		/**
		* Commits all client synchronization changes.
		* @param		aStatus				On completion of all items,
		*                                   contains the result code.
		*/
	    void DoCommitChangeInfoL( TRequestStatus& aStatus );

        /**
        * Registers current snapshot.
        */
	    void RegisterSnapshotL();
	    
	    /**
        * Creates new snapshot. Method gets data from database.
        * @param	aUid    UID of item.
        * @return   New snapshot based on item with given UID
        */
	    TNSmlSnapshotItem CreateSnapshotItemL( const TCalLocalUid& aUid );

        /**
        * Second phase constructor.
        */
	    void ConstructL();

        /**
        * Returns ETrue if indicated entry can be syncronized.
        */
		TBool CanBeSynchronized(
		       const CCalEntry::TReplicationStatus& aReplicationStatus ) const;

	    /**
		* Completes an item operation started in DoCreateItemL().
		*/
	    void DoCommitCreateItemL();
	    
	    /**
		* Completes an item operation started in DoReplaceItemL().
		*/
	    void DoCommitReplaceItemL();
	    
	    /**
		* Gets property from old item.
		* @param	aOldItem        Data of old item.
		* @param	aValue          Value of searched property.
		* @param	aProperty       Property that is searched.
		*/
	    void GetPropertiesFromDataL( CBufFlat* aOldItem,
	                                 HBufC8*& aValue,
	                                 const TDesC8& aProperty );
	                                         
        /**
		* Adds property and it's value to new item.
		* This method also removes original property and it's value.
		* @param	aValue          Value of added property.
		* @param	aProperty       Property that is added.
		*/
	    void SetPropertiesToDataL( HBufC8*& aValue,
	                               const TDesC8& aProperty );
        

    private: // Data
        // Key
	    TKeyArrayFix iKey;
        // Position of buffer
	    TInt iPos;
        // Flag if Synchronization history is available or not
	    TBool iHasHistory;
        // Modification count
	    TInt iModificationCount;
        // Internal state of this class
	    TNSmlDataStoreStatus iState;
        // Flag if SnapshotRegistered is registered or not
	    TBool iSnapshotRegistered;
        // Drive letter
	    TInt iDrive;
        // Item's ID that is to be replaced
	    TInt iReplaceItemId;
        // Internal entry type for received item
	    TNSmlDataEntryType iRXEntryType;
	    // Internal entry type for transmitted item
	    TNSmlDataEntryType iTXEntryType;
        // ChangeFinder
	    CNSmlChangeFinder* iChangeFinder;
        // DataModel
	    CNSmlDataModBase* iDataMod;
        // Status code from functions
	    TRequestStatus* iCallerStatus;
        // A handle to a file server session
	    RFs iRfs;
        // ServerMaxObjectSize
	    TInt iServerMaxObjectSize;
        // Name of the opened database
	    HBufC* iOpenedStoreName;
        // Default name of the database
	    HBufC* iDefaultStoreName;
        // Opened database ID
	    TInt64 iOpenedStoreId;
        // Item's ID that is to be added
	    TInt* iAddItemId;
        // Item's data that is to be written
	    CBufBase* iItemData;
        // Own store format
	    CSmlDataStoreFormat* iOwnFormat;
        // StringPool
	    RStringPool iStringPool;
        // Uids of new items
	    CNSmlDataItemUidSet* iNewUids;
        // Uids of deleted items
	    CNSmlDataItemUidSet* iDeletedUids;
        // Uids of softdeleted items
	    CNSmlDataItemUidSet* iSoftDeletedUids;
        // Uids of replaced items
	    CNSmlDataItemUidSet* iReplacedUids;
        // Uids of moved items
	    CNSmlDataItemUidSet* iMovedUids;
        // Session to calendar server
	    CCalSession* iVCalSession;
        // Internal progress notifier
	    CNSmlAgendaProgressview* iAgendaProgressview;
	    // Interim Utility
	    CCalenInterimUtils2* iInterimUtils;
	    
        // Importer
	    CCalenImporter* iImporter;
	    // Exporter
	    CCalenExporter* iExporter;
	    
	    //Instance View
	    CCalInstanceView*   iInstanceView;
	    
	    // Entry view
        CCalEntryView* iEntryView;
        CVersitTlsData *iVersitTlsData; // for better performance
	};

#endif // __NSMLAGENDADATASTORE_H__
            
// End of File
