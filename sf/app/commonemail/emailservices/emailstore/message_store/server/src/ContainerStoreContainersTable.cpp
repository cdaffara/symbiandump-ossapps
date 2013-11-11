/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store containers table implementation.
*
*/



// ========
// INCLUDES
// ========

#include <s32mem.h>  // for descriptor read/write streams

#include "ContainerStoreContainersTable.h"
#include "ContainerStoreDefs.h"
#include "ContainerStoreEncryption.h"
#include "ContainerStoreUtils.h"
#include "PropertiesSerializer.h"
#include "MessageStoreClientServer.h"

// =========
// CONSTANTS
// =========

_LIT( KSelect,               "SELECT " );
_LIT( KComma,                "," );
_LIT( KFrom,                 " FROM " );
_LIT( KWhere,                " WHERE " );
_LIT( KAnd,                  " AND " );
_LIT( KEqualTo,              "=" );
_LIT( KGreaterThanOrEqualTo, ">=" );
_LIT( KLessThanOrEqualTo,    "<=" );
_LIT( KOrderBy,              " ORDER BY ");
_LIT( KAsc,                  " ASC"      );

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreContainersTable* CContainerStoreContainersTable::OpenL( CContainerStoreUtils&      aUtils,
																	   MCustomBehaviorProvider&   aCustomBehaviorProvider, 
													  				   TUint                      aCountsLength,
													  				   CContainerStoreEncryption& aEncryption )
	{
	CContainerStoreContainersTable* self = new(ELeave) CContainerStoreContainersTable( aUtils, aCustomBehaviorProvider, aCountsLength, aEncryption );
	CleanupStack::PushL( self );
	self->OpenTableL();
	CleanupStack::Pop( self );
	return self;	
	} // end OpenL

// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreContainersTable* CContainerStoreContainersTable::CreateL( CContainerStoreUtils&      aUtils,
																	     MCustomBehaviorProvider&   aCustomBehaviorProvider, 
													  				     TUint                      aCountsLength,
													  				     CContainerStoreEncryption& aEncryption  )
	{
	CContainerStoreContainersTable* self = new(ELeave) CContainerStoreContainersTable( aUtils, aCustomBehaviorProvider, aCountsLength, aEncryption );
	CleanupStack::PushL( self );
	self->CreateTableL();
	CleanupStack::Pop( self );
	return self;	
	} // end CreateL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreContainersTable::CContainerStoreContainersTable( CContainerStoreUtils&      aUtils,
																MCustomBehaviorProvider&   aCustomBehaviorProvider, 
															  	TUint                      aCountsLength,
									  							CContainerStoreEncryption& aEncryption ) :
    CContainerStoreTable( aUtils ),									  							
	iCustomBehaviorProvider( aCustomBehaviorProvider ),
	iCountsLength( aCountsLength ),
	iEncryption( aEncryption )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreContainersTable" )
	} // end constructor
		
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreContainersTable::~CContainerStoreContainersTable()
	{
	iUtils.CloseTable( iTable );
	
    __LOG_DESTRUCT
	} // end destructor
	
// ==========================================================================
// FUNCTION: HierarchyL
// ==========================================================================
void CContainerStoreContainersTable::HierarchyL( TContainerId          aId, 
                                                 RArray<TContainerId>& aHierarchy )
	{
	__LOG_ENTER_SUPPRESS( "HierarchyL" )

	aHierarchy.Reset();
	
	TBool done = EFalse;
	
	TContainerId currentId = aId;

   	if( !iTable.SeekL( currentId ) )
   	    {
		__LOG_WRITE_ERROR( "container not found" )
		User::Leave( KErrNotFound );    	   	    
   	    } // end if

    // Set a bookmark at the original container, for a quick return.
    TDbBookmark bookmark = iTable.Bookmark();

	aHierarchy.Append( currentId );		
	
	// Build the hierarchy of the container.
	while( !done )
	    {
    	iTable.GetL();
	
		TContainerId parentId = iTable.ColUint32( iParentIdColNum );
    
        switch( parentId )
            {
            case KContainerInvalidId:
            
        		// Stop when the container has no parent container.
                done = ETrue;
                break;
        
            case KUncommittedContainers:
            
                // Stop when the container is uncommitted.
        		aHierarchy.Append( KUncommittedContainers );
                done = ETrue;
                break;
        
            case KToBeDeletedContainers:
            
    		    // If the container is the To Be Deleted hierarchy then treat this as a "not found" case.
        		__LOG_WRITE_ERROR( "container is in to be deleted hierarchy" )
		        User::Leave( KErrNotFound );
                break;
                
            default:
            
                // Add the parent to the hierarchy.
        		aHierarchy.Append( parentId );
        		
        		// Traverse to the parent entry.
		        if( !iTable.SeekL( parentId ) )
    		        {
            		// This should not happen.  If it has, there are orphaned records in the database.            		
            		__LOG_WRITE8_FORMAT1_ERROR( "orphaned container found %i", parentId )
            		User::Leave( KErrNotFound );    	
            		} // end if
		        break;
		        
            } // end case
		
		} // end while	

	// Go back to the original container.
    iTable.GotoL( bookmark );    			
    
	} // end HierarchyL
		
// ==========================================================================
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreContainersTable::SeekL( TContainerId          aId, 
                                            RArray<TContainerId>& aHierarchy )
    {
    HierarchyL( aId, aHierarchy );
	} // end SeekL

// ==========================================================================
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreContainersTable::SeekL( TContainerId aId )
	{
	__LOG_ENTER_SUPPRESS( "SeekL" )
	
	if( !iTable.SeekL( aId ) )
		{
		__LOG_WRITE_ERROR( "container not found" )
		User::Leave( KErrNotFound );
		} // end if
		
	iTable.GetL();
	} // end SeekL

// ==========================================================================
// FUNCTION: SeekL
// ==========================================================================
void CContainerStoreContainersTable::SeekL( TContainerId  aId, 
                                            TContainerId& aParentId )
    {
    SeekL( aId );
    
    iTable.GetL();
    
    aParentId = iTable.ColUint32( iParentIdColNum );
    } // end SeekL

// ==========================================================================
// FUNCTION: FindL
// ==========================================================================
TContainerId CContainerStoreContainersTable::FindL( const TDesC& aSqlQuery )
	{
	__LOG_ENTER_SUPPRESS( "FindL" )
	__LOG_WRITE_FORMAT1_DEBUG3( "query=%S", &aSqlQuery )
	
	TContainerId returnValue;
	
	iTable.FirstL();
	
	if( iTable.FindL( RDbRowSet::EForwards, aSqlQuery ) == KErrNotFound )
	    {
	    __LOG_WRITE_INFO( "No match found" )
	    returnValue = KContainerInvalidId;
	    }
	else 
	    {
	    iTable.GetL();
	    returnValue = iTable.ColUint32( iIdColNum );
	    } // end if
	
	return returnValue;	
	
	} // end FindL

// ==========================================================================
// FUNCTION: GotoL
// ==========================================================================
TContainerId CContainerStoreContainersTable::GotoL( TDbBookmark aBookmark )
    {
    iTable.GotoL( aBookmark );
    
    iTable.GetL();
    
    return iTable.ColUint32( iIdColNum );
    } // end GotoL
    
// ==========================================================================
// FUNCTION: Bookmark
// ==========================================================================
TDbBookmark CContainerStoreContainersTable::Bookmark()
    {
    return iTable.Bookmark();
    } // end Bookmark

// ==========================================================================
// FUNCTION: ParentId
// ==========================================================================
TContainerId CContainerStoreContainersTable::ParentId()
    {
    return iTable.ColUint32( iParentIdColNum );
    } // end ParentId

// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreContainersTable::OpenTableL()
	{
	__LOG_ENTER( "OpenTableL" )
	
	iUtils.OpenTableL( iTable, KContainersTableName );
	
	// Get the column numbers for the containers table.
	CDbColSet* colSet = iTable.ColSetL();
	CleanupStack::PushL( colSet );
	
	iIdColNum              = colSet->ColNo( KContainersTableIdCol );
	iParentIdColNum        = colSet->ColNo( KContainersTableParentIdCol );
	iChildrenCountsColNum  = colSet->ColNo( KContainersTableChildrenCountsCol );
	iChildrenCountColNum   = colSet->ColNo( KContainersTableChildrenCountCol );
	iPropertiesColNum      = colSet->ColNo( KContainersTablePropertiesCol );
	iQuickPropertiesColNum = colSet->ColNo( KContainersTableQuickPropertiesCol );
	iCountsColNum          = colSet->ColNo( KContainersTableCountsCol );
    iIsRowEncryptedColNum  = colSet->ColNo( KContainersTableIsRowEncryptedCol );

	CleanupStack::PopAndDestroy( colSet );

    // make sure KContainersTableParentIdIndex exists
    TInt err = iTable.SetIndex( KContainersTableParentIdIndex );
    if ( err == KErrNotFound )
        {
        iUtils.CloseTable( iTable );
        CreateIndexL( KContainersTableName, KContainersTableParentIdIndex, KContainersTableParentIdCol );
        iUtils.OpenTableL( iTable, KContainersTableName );
        }
    else
        {
        User::LeaveIfError( err );
        }

    // Set the table's index to the ID index.
    User::LeaveIfError( iTable.SetIndex( KContainersTableIdIndex ) );
	
	__LOG_EXIT
	} // end OpenTableL

// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreContainersTable::CreateTableL()
	{
	__LOG_ENTER( "CreateContainersTableL" )
	
	// Create table columns
	
	CDbColSet* colSet = CDbColSet::NewLC();
	
	TDbCol idCol( KContainersTableIdCol, EDbColUint32 );
	colSet->AddL( idCol );	
	
	TDbCol parentIdCol( KContainersTableParentIdCol, EDbColUint32 );
	colSet->AddL( parentIdCol );	

	TDbCol childrenCountCol( KContainersTableChildrenCountCol, EDbColUint16 );
	colSet->AddL( childrenCountCol );	

	TDbCol childrenCountsCol( KContainersTableChildrenCountsCol, EDbColBinary, iCountsLength );
	colSet->AddL( childrenCountsCol );	

	TDbCol propertiesCol( KContainersTablePropertiesCol, EDbColLongBinary );
	colSet->AddL( propertiesCol );	
	
	TDbCol quickPropertiesCol( KContainersTableQuickPropertiesCol, EDbColLongBinary );
	colSet->AddL( quickPropertiesCol );	
	
	TDbCol countsCol( KContainersTableCountsCol, EDbColBinary, iCountsLength );
	colSet->AddL( countsCol );	
	
    TDbCol isEncryptedCol( KContainersTableIsRowEncryptedCol, EDbColUint8 );
    colSet->AddL( isEncryptedCol );  
    
	// Create table.
	
	iUtils.CreateTableL( KContainersTableName, *colSet );

	CleanupStack::PopAndDestroy( colSet );
	
	// Create table index.
    CreateIndexL( KContainersTableName, KContainersTableIdIndex, KContainersTableIdCol );
    CreateIndexL( KContainersTableName, KContainersTableParentIdIndex, KContainersTableParentIdCol );

    OpenTableL();
	
	__LOG_EXIT	
	} // end CreateTableL
	
// ==========================================================================
// FUNCTION: WritePropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::WritePropertiesColumnL( const TDesC8& aProperties )
    {	
    __LOG_ENTER_SUPPRESS( "WritePropertiesColumnL" )
    										        
    if( aProperties.Length() > 0 )
        {
        if ( IsEncrypted() )
            {
            EncryptAndWritePropertiesColumnL( aProperties );
            }
        else
            {
            WriteUnencryptedPropertiesColumnL( aProperties );
            }
        }
    else
        {
        iTable.SetColNullL( iPropertiesColNum );
        } // end if
	
    } // end WritePropertiesColumnL

// ==========================================================================
// FUNCTION: ReadPropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::ReadPropertiesColumnL( RBuf8& aProperties )
    {
    __LOG_ENTER_SUPPRESS( "ReadPropertiesColumnL" )
    
    if ( IsEncrypted() )
        {
        ReadAndDescrptPropertiesColumnL( aProperties );
        }
    else
        {
        ReadUnencryptedPropertiesColumnL( aProperties );
        }
    } // end ReadPropertiesColumnL

// ==========================================================================
// FUNCTION: CreateContainerL
// ==========================================================================
void CContainerStoreContainersTable::CreateContainerL( TContainerId  aId,
                                                       TContainerId  aParentId,
											           const TDesC8& aProperties,
											           RBuf8&        aQuickProperties )
	{
	__LOG_ENTER_SUPPRESS( "CreateContainerL" )

	RBuf8 countsBuf;
	CleanupClosePushL( countsBuf );
	countsBuf.CreateL( iCountsLength );
	iCustomBehaviorProvider.InitializeCounts( countsBuf );

	// Insert an entry in the folders table.
	
	iTable.LastL();
	
	InsertRowLC();

	iTable.SetColL( iIdColNum,             aId );
	iTable.SetColL( iParentIdColNum,       aParentId );
	iTable.SetColL( iChildrenCountColNum,  0 );
	iTable.SetColL( iChildrenCountsColNum, countsBuf );
    
    iTable.SetColL( iIsRowEncryptedColNum, static_cast<TUint8>( iEncryption.IsEncryptionOn() ) );
	
    WritePropertiesColumnL( aProperties );

    // Add the type bits to the assigned ID.
	TUint type = aId & KContainerTypeMask;
	
	UpdateQuickPropertiesAndCountsL( type, aProperties, aQuickProperties, countsBuf );
    	
	_LIT( KMyCounts, "myCounts" );
    iCustomBehaviorProvider.LogCounts( KMyCounts, countsBuf );
    
	PutRowUpdatesL();
	
	if( aParentId != KUncommittedContainers && aParentId != KContainerInvalidId )
	    {
	    SeekL( aParentId );
	    IncrementParentCountsL( countsBuf );				    	    
	    } // end if

	CleanupStack::PopAndDestroy( &countsBuf );
	} // end CreateContainerL

// ==========================================================================
// FUNCTION: IncrementParentCountsL
// ==========================================================================
void CContainerStoreContainersTable::IncrementParentCountsL( const TDesC8& aCountsDelta )
	{	
	__LOG_ENTER_SUPPRESS( "IncrementParentCountsL" )
	
	PrepareRowForUpdateLC();

	// Read old values.
	TUint16 childrenCount = iTable.ColUint16( iChildrenCountColNum );
	
	RBuf8 countsBuf;
	CleanupClosePushL( countsBuf );
	countsBuf.CreateL( iTable.ColDes8( iChildrenCountsColNum ) );
    
	_LIT( KOldChildrenCounts, "oldChildrenCounts" );
    iCustomBehaviorProvider.LogCounts( KOldChildrenCounts, countsBuf );
	
	// Update values.
	childrenCount++;
	
	iCustomBehaviorProvider.IncrementParentCounts( countsBuf, aCountsDelta );
    _LIT( KNewChildrenCounts, "newChildrenCounts" );
    iCustomBehaviorProvider.LogCounts( KNewChildrenCounts, countsBuf );

	// Write new values to database.	
	iTable.SetColL( iChildrenCountColNum, childrenCount );
	
	iTable.SetColL( iChildrenCountsColNum, countsBuf );
	CleanupStack::PopAndDestroy( &countsBuf );
			
	PutRowUpdatesL();
	
	} // end IncrementParentCountsL	

// ==========================================================================
// FUNCTION: DecrementParentCountsL
// ==========================================================================
void CContainerStoreContainersTable::DecrementParentCountsL( const TDesC8& aCountsDelta )
	{
	__LOG_ENTER_SUPPRESS( "DecrementParentCountsL" )
	
	PrepareRowForUpdateLC();

	// Read old values.
	TUint16 childrenCount = iTable.ColUint16( iChildrenCountColNum );
	
	RBuf8 countsBuf;
	CleanupClosePushL( countsBuf );
	countsBuf.CreateL( iTable.ColDes8( iChildrenCountsColNum ) );
    
	_LIT( KOldChildrenCounts, "oldChildrenCounts" );
    iCustomBehaviorProvider.LogCounts( KOldChildrenCounts, countsBuf );

	// Update values.
	childrenCount--;
	
	iCustomBehaviorProvider.DecrementParentCounts( countsBuf, aCountsDelta );
    
	_LIT( KNewChildrenCounts, "newChildrenCounts" );
    iCustomBehaviorProvider.LogCounts( KNewChildrenCounts, countsBuf );

	// Write new values to database.	
	iTable.SetColL( iChildrenCountColNum, childrenCount );
	
	iTable.SetColL( iChildrenCountsColNum, countsBuf );
	CleanupStack::PopAndDestroy( &countsBuf );
			
	PutRowUpdatesL();
	} // end DecrementParentCountsL	

// ==========================================================================
// FUNCTION: UpdateQuickPropertiesAndCountsL
// ==========================================================================
void CContainerStoreContainersTable::UpdateQuickPropertiesAndCountsL( TContainerId  aType,
															 		  const TDesC8& aProperties,
															 		  RBuf8&        aQuickProperties,
															 		  TDes8&        aCounts )
	{
	__LOG_ENTER_SUPPRESS( "UpdateQuickPropertiesAndCountsL" )
	
	aQuickProperties.SetLength( 0 );
	
	iCustomBehaviorProvider.QuickPropertiesAndCountsL( aType,
										 		 	   aProperties,
												 	   aQuickProperties,
  										 			   aCounts );
												 			 
	__LOG_WRITE8_FORMAT1_INFO( "aQuickProperties.Length()=%i", aQuickProperties.Length() )
	
	if( aQuickProperties.Length() > 0 )
		{
	    // The quick properties are not encrypted.	
		iUtils.WriteLongColumnL( iTable, 
                    		     iQuickPropertiesColNum, 
                    		     aQuickProperties );
		}
	else
		{
		// Clear the quick properties column.		
		iTable.SetColNullL( iQuickPropertiesColNum );
		
		} // end if

	_LIT( KMyCounts, "myCounts" );
    iCustomBehaviorProvider.LogCounts( KMyCounts, aCounts );
    
	iTable.SetColL( iCountsColNum, aCounts );	
	} // end UpdateQuickPropertiesAndCountsL
	
// ==========================================================================
// FUNCTION: DeleteL
// ==========================================================================
void CContainerStoreContainersTable::DeleteL()
	{
	iTable.DeleteL();	

    // This function is only called for objects that are already in the "to be deleted" folder hierarchy.
    //  There is no need to update the parent container since the parent container is also part of the
    // "to be deleted" hierarchy.	
	
	} // end DeleteL

// ==========================================================================
// FUNCTION: MarkAsDeleteRetryL
// ==========================================================================
void CContainerStoreContainersTable::MarkAsDeleteRetryL()
    {
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iParentIdColNum, KToRetryDeleteContainers );

    PutRowUpdatesL();
    } // end MarkAsDeleteRetryL

// ==========================================================================
// FUNCTION: MarkForDeleteL
// ==========================================================================
void CContainerStoreContainersTable::MarkForDeleteL()
    {
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iParentIdColNum, KToBeDeletedContainers );

    PutRowUpdatesL();
    } // end MarkForDeleteL

// ==========================================================================
// FUNCTION: MoveL
// ==========================================================================
void CContainerStoreContainersTable::MoveL( TContainerId aNewParentId, TDes8& aTotalCountsDelta )
	{
	__LOG_ENTER( "MoveL" )
		
	// Update the entry's parent ID.
	TContainerId oldParentId = iTable.ColUint32( iParentIdColNum );
	
	TUint length = iTable.ColLength( iQuickPropertiesColNum );

	PrepareRowForUpdateLC();
			
	iTable.SetColL( iParentIdColNum, aNewParentId );

	PutRowUpdatesL();
	
	RBuf8 myDescendentCounts;
	CleanupClosePushL( myDescendentCounts );
	myDescendentCounts.CreateL( iTable.ColLength( iChildrenCountsColNum ) );
	
	ChildrenCountsL( myDescendentCounts, ETrue  );
	
	RBuf8 myCounts;
	CleanupClosePushL( myCounts );
	myCounts.CreateL( iTable.ColDes8( iCountsColNum ) );
    
	_LIT( KMyCounts, "myCounts" );
    iCustomBehaviorProvider.LogCounts( KMyCounts, myCounts );

    iCustomBehaviorProvider.InitializeCounts( aTotalCountsDelta );

	// Increment the new parent container's child counts.

    if( aNewParentId != KToBeDeletedContainers )
        {        
	    SeekL( aNewParentId );
	    IncrementParentCountsL( myCounts );
        }
     else 
        {
        // The item is being deleted.  Decrement the total counts.
        iCustomBehaviorProvider.DecrementParentCounts( aTotalCountsDelta, myCounts );
        iCustomBehaviorProvider.DecrementParentCounts( aTotalCountsDelta, myDescendentCounts );
        } // end if
		
	// Decrement the old parent container's child counts.
	
	if( oldParentId != KUncommittedContainers )
	    {
    	SeekL( oldParentId );
    	DecrementParentCountsL( myCounts );
	    }
	else 
	    {
        // The item is being committed.  Increment the total counts.
        iCustomBehaviorProvider.IncrementParentCounts( aTotalCountsDelta, myCounts );	    
        iCustomBehaviorProvider.IncrementParentCounts( aTotalCountsDelta, myDescendentCounts );	    
	    } // end if
    
	_LIT( KaTotalCountsDelta, "aTotalCountsDelta" );
    iCustomBehaviorProvider.LogCounts( KaTotalCountsDelta, aTotalCountsDelta );

	CleanupStack::PopAndDestroy( &myCounts );
	CleanupStack::PopAndDestroy( &myDescendentCounts );
	   
	__LOG_EXIT
	} // end MoveL

// ==========================================================================
// FUNCTION: CopyL
// ==========================================================================
void CContainerStoreContainersTable::CopyL( TContainerId aNewId, TContainerId aDestinationId, RBuf8& aQuickProperties )
	{	
	__LOG_ENTER( "Copy" )
	
	// Copy properties.
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	
	ReadPropertiesColumnL( propertiesBuf );
	
	CreateContainerL( aNewId, aDestinationId, propertiesBuf, aQuickProperties );
	
	CleanupStack::PopAndDestroy( &propertiesBuf );
	
	__LOG_EXIT
	} // end CopyL

// ==========================================================================
// FUNCTION: ChildrenCountsL
// ==========================================================================
void CContainerStoreContainersTable::ChildrenCountsL( TDes8& aCounts, TBool aRecursive )
	{
	if ( !aRecursive )
		{
		aCounts.Copy( iTable.ColDes8( iChildrenCountsColNum ) );
		}
	else
		{
		RBuf8 totalCounts;
		CleanupClosePushL( totalCounts );
		RBuf8 childrenCounts;
		CleanupClosePushL( childrenCounts );
		
		TContainerId id = iTable.ColUint32( iIdColNum );
		
		totalCounts.CreateL( iTable.ColLength( iCountsColNum ) );
		iCustomBehaviorProvider.InitializeCounts( totalCounts );
		
		childrenCounts.CreateL( iTable.ColDes8(iChildrenCountsColNum) );
	    iCustomBehaviorProvider.IncrementParentCounts( totalCounts, childrenCounts );
	    
		TDbBookmark bookmark = iTable.Bookmark();

	    RArray<TContainerId> childrens;
		CleanupClosePushL( childrens );
		ListChildrenL( childrens, id, EMsgStoreFolderBits, ETrue );
		
		for ( int i = 0 ; i < childrens.Count() ; i++ )
			{
			User::LeaveIfError(iTable.SeekL( childrens[i] ));
			iTable.GetL();
			childrenCounts.Copy( iTable.ColDes8(iChildrenCountsColNum) );
		    iCustomBehaviorProvider.IncrementParentCounts( totalCounts, childrenCounts );
			}
		
		aCounts.Copy( totalCounts );
		
		iTable.GotoL( bookmark );
		
		CleanupStack::PopAndDestroy( &childrens );
		
		CleanupStack::PopAndDestroy( &childrenCounts );
		CleanupStack::PopAndDestroy( &totalCounts );
		
		}
	} // end ChildrenCountsL
	
// ==========================================================================
// FUNCTION: UpdatePropertiesL
// ==========================================================================
void CContainerStoreContainersTable::UpdatePropertiesL( const TDesC8& aProperties,
                                                        RBuf8&        aQuickProperties,
                                                        TDes8&        aTotalCountsDelta,
                                                        TContainerId& aRealParentId )
	{
	__LOG_ENTER_SUPPRESS( "UpdatePropertiesL" )
	
	RBuf8 newCountsBuf;
	CleanupClosePushL( newCountsBuf );

	RBuf8 oldCountsBuf;
	CleanupClosePushL( oldCountsBuf );
	
	PrepareRowForUpdateLC();
	
	// Read the parent ID of the row.
    aRealParentId = iTable.ColUint32( iParentIdColNum );
				
	// Get the old counts.
	oldCountsBuf.CreateL( iTable.ColDes8( iCountsColNum ) );
    
	_LIT( KoldmyCountBuf, "old(my)CountBuf" );
    iCustomBehaviorProvider.LogCounts( KoldmyCountBuf, oldCountsBuf );
    
	// Update the properties column.
    WritePropertiesColumnL( aProperties );
												 
	// Update the quick properties and counts columns.

	TContainerId id = iTable.ColUint32( iIdColNum );
	TContainerId type = (id & KContainerTypeMask);
	
	newCountsBuf.CreateL( iCountsLength );
	UpdateQuickPropertiesAndCountsL( type, aProperties, aQuickProperties, newCountsBuf );
    _LIT(KnewmyCountBuf, "new(my)CountBuf" );
    iCustomBehaviorProvider.LogCounts( KnewmyCountBuf, newCountsBuf );

	PutRowUpdatesL();

    // The total counts delta is the new counts minus the old counts.
    aTotalCountsDelta.Copy( newCountsBuf );
    iCustomBehaviorProvider.DecrementParentCounts( aTotalCountsDelta, oldCountsBuf );
    
    _LIT( KaTotalCountsDelta, "aTotalCountsDelta" );
    iCustomBehaviorProvider.LogCounts( KaTotalCountsDelta, aTotalCountsDelta );

    if( aRealParentId != KUncommittedContainers )
        {        
	    SeekL( aRealParentId );

    	// Decrement the parent row's counts by the old count values.
	    DecrementParentCountsL( oldCountsBuf );

    	// Increment the parent row's counts by the new count values.
    	IncrementParentCountsL( newCountsBuf );		
    	
        } // end if        	
	
	CleanupStack::PopAndDestroy( &oldCountsBuf );
	CleanupStack::PopAndDestroy( &newCountsBuf );	
	
	} // end UpdatePropertiesL

// ==========================================================================
// FUNCTION: UpdatePropertyL
// ==========================================================================
void CContainerStoreContainersTable::UpdatePropertyL( const TDesC8& aName,
                                                      TUint8        aType,
													  const TDesC8& aValue,
													  RBuf8&        aQuickProperties,
													  TDes8&        aTotalCountsDelta,
													  TContainerId& aRealParentId )
	{
	__LOG_ENTER_SUPPRESS( "UpdatePropertyL" )
	
	RBuf8 propertiesBuf;
	CleanupClosePushL( propertiesBuf );
	ReadPropertiesColumnL( propertiesBuf );
													
	// Look for an existing occurrence of the property.
	
	TPropertiesSerializer serializer( propertiesBuf );
	
	TBool found = serializer.Find( aName );
    
    if( found )
        {        
		// Property already exists.  Replace the value length and value.
		serializer.UpdateValueL( aValue );
		}
	else
		{
		// Add the property.
		serializer.AddPropertyL( aName, aType, aValue );
		} // end if
				
	UpdatePropertiesL( propertiesBuf, aQuickProperties, aTotalCountsDelta, aRealParentId );
				
	CleanupStack::PopAndDestroy( &propertiesBuf );

	} // end UpdatePropertyL

// ==========================================================================
// FUNCTION: PropertiesL
// ==========================================================================
void CContainerStoreContainersTable::PropertiesL( RBuf8& aProperties )
	{
    ReadPropertiesColumnL( aProperties );

 	} // end PropertiesL

// ==========================================================================
// FUNCTION: PropertiesSizeL
// ==========================================================================
TUint CContainerStoreContainersTable::PropertiesSizeL( TContainerId aId )
    {
    SeekL( aId );
    return iTable.ColLength( iPropertiesColNum );
    } // end PropertiesSizeL

// ==========================================================================
// FUNCTION: QuickPropertiesL
// ==========================================================================
void CContainerStoreContainersTable::QuickPropertiesL( RBuf8& aQuickProperties )
	{
    // Quick properties are not encrypted
    iUtils.ReadLongColumnL( iTable, iQuickPropertiesColNum, aQuickProperties );	

 	} // end PropertiesL
	
// ==========================================================================
// FUNCTION: ListChildrenL
// ==========================================================================
void CContainerStoreContainersTable::ListChildrenL( RArray<TContainerId>& aChildren, TContainerId aId, TContainerId aType, TBool aRecursive )
	{
	__LOG_ENTER_SUPPRESS( "ListChildrenL" )
	
	aChildren.Reset();
	
	const TUint bufSsize = 200;
	TBuf<bufSsize> queryString;
	
	queryString.Copy( KSelect );
	queryString.Append( KContainersTableIdCol );

    // Only need the children count if this is a recursive search.
	if( aRecursive )
	    {
	    queryString.Append( KComma );
    	queryString.Append( KContainersTableChildrenCountCol );
	    } // end if
	
	queryString.Append( KFrom );
	queryString.Append( KContainersTableName );	
	queryString.Append( KWhere );

	// If the requested type isn't "any", then narrow down the query.
	if( aType != KContainerTypeAny )
		{
		const TContainerId lowestId  = aType;
		const TContainerId highestId = (aType | ~KContainerTypeMask ); 
		
		queryString.Append( KContainersTableIdCol );
		queryString.Append( KGreaterThanOrEqualTo );
		queryString.AppendNum( lowestId );
		queryString.Append( KAnd );
		queryString.Append( KContainersTableIdCol );
		queryString.Append( KLessThanOrEqualTo );
		queryString.AppendNum( highestId );		
		queryString.Append( KAnd );
		} // end if

	queryString.Append( KContainersTableParentIdCol );
	queryString.Append( KEqualTo );
	
    TInt queryLengthWithoutId = queryString.Length();
	
	RDbView view;
	CleanupClosePushL( view );
	
	const TUint granularity = 25;
    RArray<TContainerId> toBeSearched( granularity );
    CleanupClosePushL( toBeSearched );
    
    toBeSearched.Append( aId );
    
   	const TInt idColNum            = 1;
   	const TInt childrenCountColNum = 2;
	
	while( toBeSearched.Count() > 0 )
	    {
        // Update the query to query the next ID to be searched.
        queryString.SetLength( queryLengthWithoutId );
	    queryString.AppendNum( toBeSearched[0] );    	    
        toBeSearched.Remove( 0 );
        
        if ( aType == EMsgStorePartBits || 
           ( ( aId & KContainerTypeMask ) == EMsgStoreMessageBits ) ||
           ( ( aId & KContainerTypeMask ) == EMsgStorePartBits ) )
        	{
        	//must sort the message parts by containerId in ascending order
        	// to preserve the same order as they are created
    	    queryString.Append( KOrderBy );    	    
    		queryString.Append( KContainersTableIdCol );
    		queryString.Append( KAsc );
    		__LOG_WRITE_INFO("Using order by in query");
        	}

        iUtils.PopulateViewL( view, queryString );

    	TBool found = view.FirstL();

        // Add the IDs in the view to the children array.	
    	while( found )
    		{
    		view.GetL();
    		
    		TUint32 cid = view.ColUint32( idColNum );
    		__LOG_WRITE_FORMAT1_INFO("Id=%x", cid);
    		aChildren.Append( cid );
    		
    		// Search the container for children if this a recursive search and the child count is
    		// greater than 0.
    		if( aRecursive && ( view.ColUint32( childrenCountColNum ) > 0 ) )
    		    {
    		    toBeSearched.Append( view.ColUint32( idColNum ) );
    		    } // end if
    				
    		found = view.NextL();
    		
       		} // end while

        // Most close old view before preparing new one, otherwise a memory leak occurs.
        view.Close();
            
	    } // end while
	
	CleanupStack::PopAndDestroy( &toBeSearched );
	CleanupStack::PopAndDestroy( &view );
		
	} // end ListChildrenL


// ==========================================================================
// FUNCTION: EncryptFirstL
// ==========================================================================
TBool CContainerStoreContainersTable::EncryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER_SUPPRESS("EncryptFirstL")
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KContainersTableIsRowEncryptedCol, EFalse, aNextRow );
    
    return found;
    }

// ==========================================================================
// FUNCTION: EncryptNextL
// ==========================================================================
TBool CContainerStoreContainersTable::EncryptNextL( TDbBookmark& aNextRow, TContainerId& aEncryptedContainerId )
    {
    __LOG_ENTER_SUPPRESS("EncryptNextL")
    
    TBool hasMore = EFalse;
    TContainerId id = KContainerInvalidId;
    
    TRAPD( err, id = GotoL( aNextRow ) );
    if ( err == KErrNotFound )
        {
        hasMore = EncryptFirstL( aNextRow );
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );        
        }
    else
        {
        if ( !IsEncrypted() )
            {
            RBuf8 propertyBuf;
            CleanupClosePushL( propertyBuf );
            
            //read the unencrypted properties and write the enctypted properties back
            ReadUnencryptedPropertiesColumnL( propertyBuf );
            
            PrepareRowForUpdateLC();
            
            EncryptAndWritePropertiesColumnL( propertyBuf );
            
            //update the encrypted flag
            SetEncryptedL( ETrue );
            
            PutRowUpdatesL();
            
            CleanupStack::PopAndDestroy( &propertyBuf );
            
            //set the encryptedConteinerId so that the caller can encrypt its content
            aEncryptedContainerId = id;
            }
        
        hasMore = iTable.NextL();
        if ( hasMore )
            {
            __LOG_WRITE_INFO("hasMore is TRUE")
            aNextRow = iTable.Bookmark();
            }
        }
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: DecryptFirstL
// ==========================================================================
TBool CContainerStoreContainersTable::DecryptFirstL( TDbBookmark& aNextRow )
    {
    __LOG_ENTER("DecryptFirstL")
    
    TBool found = iUtils.FindFirstEncryptedOrUnencryptedL( iTable, KContainersTableIsRowEncryptedCol, ETrue, aNextRow );
    
    __LOG_EXIT
    
    return found;
    }

// ==========================================================================
// FUNCTION: DecryptNextL
// ==========================================================================
TBool CContainerStoreContainersTable::DecryptNextL( TDbBookmark& aNextRow, TContainerId& aDecryptedContainerId )
    {
    __LOG_ENTER_SUPPRESS("DecryptNextL")
    
    TBool hasMore = EFalse;
    TContainerId id = KContainerInvalidId;
    
    TRAPD( err, id = GotoL( aNextRow ) );
    if ( err == KErrNotFound )
        {
        hasMore = DecryptFirstL( aNextRow );
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );        
        }
    else
        {
        if ( IsEncrypted() )
            {
            RBuf8 propertyBuf;
            CleanupClosePushL( propertyBuf );
    
            //the row is currentlly encryptted
            //read the encrypted properties and write the unenctypted properties back
            ReadAndDescrptPropertiesColumnL( propertyBuf );
            
            PrepareRowForUpdateLC();
            
            WriteUnencryptedPropertiesColumnL( propertyBuf );
            
            //update the encrypted flag
            SetEncryptedL( EFalse );
            
            PutRowUpdatesL();
        
            CleanupStack::PopAndDestroy( &propertyBuf );
            
            //set the aDecryptedContainerId so that the caller can decrypt its content
            aDecryptedContainerId = id;
            }
    
    
        hasMore = iTable.NextL();
        if ( hasMore )
            {
            __LOG_WRITE_INFO("hasMore is TRUE")
            aNextRow = iTable.Bookmark();
            }
        }
    
    return hasMore;
    }

// ==========================================================================
// FUNCTION: ReadAndDescrptPropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::ReadAndDescrptPropertiesColumnL( RBuf8& aProperties )
    {
    __LOG_ENTER_SUPPRESS( "ReadPropertiesColumnL" )
    
    RBuf8 encryptedBuffer;
    CleanupClosePushL( encryptedBuffer );

    iUtils.ReadLongColumnL( iTable, iPropertiesColNum, encryptedBuffer );

    if( aProperties.Length() < encryptedBuffer.Length() )
        {        
        aProperties.ReAllocL( encryptedBuffer.Length() );
        } // end if
    
    if( encryptedBuffer.Length() > 0 )
        {        
        iEncryption.DecryptL( encryptedBuffer, aProperties );        
        iEncryption.RemovePaddingL( aProperties );
        } // end if
    
    CleanupStack::PopAndDestroy( &encryptedBuffer );
    }

// ==========================================================================
// FUNCTION: EncryptAndWritePropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::EncryptAndWritePropertiesColumnL( const TDesC8& aProperties )
    {
    __LOG_ENTER_SUPPRESS( "EncryptAndWritePropertiesColumnL" )
    
    if ( aProperties.Length() > 0 )
        {
        RBuf8 encryptedBuffer;
        encryptedBuffer.CreateL( aProperties.Length() + iEncryption.BlockSizeL() );
        CleanupClosePushL( encryptedBuffer );
        
        RBuf8 paddedBuffer;
        paddedBuffer.CreateL( aProperties.Length() + iEncryption.BlockSizeL() );       
        CleanupClosePushL( paddedBuffer );
    
        paddedBuffer.Copy( aProperties );
        iEncryption.AddPaddingL( paddedBuffer, iEncryption.BlockSizeL() );
    
        iEncryption.EncryptL( paddedBuffer, encryptedBuffer );
    
        CleanupStack::PopAndDestroy( &paddedBuffer );
    
        iUtils.WriteLongColumnL( iTable, iPropertiesColNum, encryptedBuffer );
        
        CleanupStack::PopAndDestroy( &encryptedBuffer );
        }
    else
        {
        iTable.SetColNullL( iPropertiesColNum );
        }
    }

// ==========================================================================
// FUNCTION: ReadUnencryptedPropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::ReadUnencryptedPropertiesColumnL( RBuf8& aProperties )
    {
    iUtils.ReadLongColumnL( iTable, iPropertiesColNum, aProperties );
    }

// ==========================================================================
// FUNCTION: WriteUnencryptedPropertiesColumnL
// ==========================================================================
void CContainerStoreContainersTable::WriteUnencryptedPropertiesColumnL( const TDesC8& aProperties )
    {
    iUtils.WriteLongColumnL( iTable, iPropertiesColNum, aProperties );
    }

// ==========================================================================
// FUNCTION: IsEncrypted
// ==========================================================================
TBool CContainerStoreContainersTable::IsEncrypted()
    {
    TBool isEncrypted = static_cast<TBool>( iTable.ColUint8( iIsRowEncryptedColNum ) );
    return isEncrypted;
    }

// ==========================================================================
// FUNCTION: SetEncryptedL
// ==========================================================================
void CContainerStoreContainersTable::SetEncryptedL( TBool aIsEncrypted )
    {
    iTable.SetColL( iIsRowEncryptedColNum, static_cast<TUint8>(aIsEncrypted) );
    }

/*
 * Search based on the parent ID. This will set the index if one exists and use the find if the index does not
 * exists which could happen for a existing DB(IAD case)
 */
TContainerId CContainerStoreContainersTable::FirstChildForDeleteL( TContainerId aId, TDbBookmark& aBookmark )
    {
    __LOG_ENTER( "FirstChildForDeleteL" )
    __LOG_WRITE8_FORMAT1_INFO( "id=%x", aId )

    TContainerId returnValue = KContainerInvalidId;
    User::LeaveIfError( iTable.SetIndex( KContainersTableParentIdIndex ) );
    TRAP_IGNORE( returnValue = GetFirstChildForDeleteL( aId, aBookmark ) );
    //remember to set the index back
    iTable.SetIndex( KContainersTableIdIndex );

    __LOG_EXIT
    return returnValue;   
    }

/*
 * Search based on the parent ID. 
 */
TContainerId CContainerStoreContainersTable::GetFirstChildForDeleteL( TContainerId aId, TDbBookmark& aBookmark )
    {
    __LOG_ENTER( "FirstChildForDeleteL" )
    __LOG_WRITE8_FORMAT1_INFO( "id=%x", aId )
    
    TContainerId returnValue = KContainerInvalidId;
    //Index on parent id, so we can Seek, faster than Find
    if ( !iTable.SeekL( aId ) )
        {
        __LOG_WRITE_INFO( "No match found" )
        returnValue = KContainerInvalidId;
        }
    else
        {
        iTable.GetL();
        returnValue = iTable.ColUint32( iIdColNum );
        aBookmark = Bookmark();
        __LOG_WRITE8_FORMAT1_INFO( "found id=%x", returnValue )
        }
    __LOG_EXIT
    return returnValue;   
    }
