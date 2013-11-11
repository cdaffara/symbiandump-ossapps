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
* Description:  Container store table implementation.
*
*/



#include "ContainerStoreTable.h"
#include "ContainerStoreUtils.h"

// ==================================
// LOCAL STATIC FUNCTION DECLARATIONS
// ==================================

static void CancelRowUpdates( TAny* aObject );

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreTable::CContainerStoreTable( CContainerStoreUtils& aUtils )
: iUtils( aUtils )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreTable" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreTable::~CContainerStoreTable()
	{
	__LOG_DESTRUCT
	} // end destructor

// ==========================================================================
// FUNCTION: PrepareRowForUpdateLC
// ==========================================================================
void CContainerStoreTable::PrepareRowForUpdateLC()
	{
	iTable.UpdateL();
	
	CleanupStack::PushL(TCleanupItem(&CancelRowUpdates, &iTable) );
		
	} // end PrepareRowForUpdateLC

// ==========================================================================
// FUNCTION:InsertRowLC
// ==========================================================================
void CContainerStoreTable::InsertRowLC()
	{
	iTable.InsertL();
	
	CleanupStack::PushL(TCleanupItem(&CancelRowUpdates, &iTable) );
		
	} // end InsertRowLC

// ==========================================================================
// FUNCTION: PutRowUpdatesL
// ==========================================================================
void CContainerStoreTable::PutRowUpdatesL()
	{
	iTable.PutL();
	
	CleanupStack::Pop( &iTable );
	} // end PutRowUpdatesL
	
// ==========================================================================
// FUNCTION: CreateIndexL
// ==========================================================================
void CContainerStoreTable::CreateIndexL( const TDesC& aTableName, const TDesC& aIndexName, const TDesC& aColumn1, const TDesC& aColumn2 )
    {
    __LOG_ENTER( "CreateIndexL" )

    CDbKey* index = CDbKey::NewLC();
    
    TDbKeyCol keyCol1( aColumn1 );
    index->AddL( keyCol1 );
    
    if ( aColumn2 != KNullDesC )
        {
        TDbKeyCol keyCol1( aColumn2 );
        index->AddL( keyCol1 );
        }
    
    iUtils.CreateIndexL( aIndexName, aTableName, *index );   
    CleanupStack::PopAndDestroy( index );

    __LOG_EXIT
    }

// ===================================
// LOCAL STATIC METHOD IMPLEMENTATIONS
// ===================================

// ==========================================================================
// FUNCTION: CancelRowUpdates
// ==========================================================================
static void CancelRowUpdates( TAny* aObject )
	{
	__LOG_STATIC_ENTER( "msg", "CancelRowUpdates" )
	
	RDbTable* table = reinterpret_cast<RDbTable*>(aObject);
	table->Cancel();		
	
	__LOG_STATIC_EXIT
	} // end CancelRowUpdates

