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
* Description:  Container store general table implementation.
*
*/



// ========
// INCLUDES
// ========

#include "ContainerStoreGeneralTable.h"
#include "ContainerStoreUtils.h"

// =========
// CONSTANTS
// =========

const TUint KDefaultMaxMruCount = 50;
const TUint KFirstMruNumber     = 0;

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: OpenL
// ==========================================================================
CContainerStoreGeneralTable* CContainerStoreGeneralTable::OpenL( CContainerStoreUtils& aUtils )
	{
	CContainerStoreGeneralTable* self = new(ELeave) CContainerStoreGeneralTable( aUtils );
	CleanupStack::PushL( self );
	self->OpenTableL();
	CleanupStack::Pop( self );
	return self;
	} // end OpenL

// ==========================================================================
// FUNCTION: CreateL
// ==========================================================================
CContainerStoreGeneralTable* CContainerStoreGeneralTable::CreateL( CContainerStoreUtils& aUtils )
	{
	CContainerStoreGeneralTable* self = new(ELeave) CContainerStoreGeneralTable( aUtils );
	CleanupStack::PushL( self );
	self->CreateTableL();
	CleanupStack::Pop( self );
	return self;
	} // end CreateL

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreGeneralTable::CContainerStoreGeneralTable( CContainerStoreUtils& aUtils ) :
CContainerStoreTable( aUtils )
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreGeneralTable" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor

// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreGeneralTable::~CContainerStoreGeneralTable()
	{
	iUtils.CloseTable( iTable );
	__LOG_DESTRUCT	
	} // end destructor
	
// ==========================================================================
// FUNCTION: OpenTableL
// ==========================================================================
void CContainerStoreGeneralTable::OpenTableL()
	{
	__LOG_ENTER( "OpenTableL" )

	iUtils.OpenTableL( iTable, KGeneralTableName );
	
	CDbColSet* colSet = iTable.ColSetL();
	CleanupStack::PushL( colSet );

	TUint schemaVersionColNum = colSet->ColNo( KGeneralTableDbSchemaVersionCol );

	TBool tableIsEmpty = !iTable.FirstL();

	if( schemaVersionColNum == 0 )
		{
		__LOG_WRITE_ERROR( "schema column not found!" );			
		iUtils.CloseTable( iTable );
		User::Leave( KErrCorrupt );
		}
	else if( !tableIsEmpty )
		{		
		// Check the schema version.  Leave if the schema doesn't match (this will cause the
		// database to be recreated).
		iTable.GetL();
		
		TUint schemaVersion = iTable.ColUint32( schemaVersionColNum );
		
		if( schemaVersion != KDbSchemaVersion )
			{
			__LOG_WRITE8_FORMAT2_ERROR( "schema mismatch (found=%i expected=%i)", schemaVersion, KDbSchemaVersion );			
    		iUtils.CloseTable( iTable );
			User::Leave( KErrCorrupt );
			} // end if

		} // end if

	// Get the column numbers for the folders table.
	
	iNextIdColNum                  = colSet->ColNo( KGeneralTableNextIdCol );
    iAuthenticationRequiredColNum  = colSet->ColNo( KGeneralTableAuthenticationRequiredCol );
	iEncryptionFlagColNum	       = colSet->ColNo( KGeneralTableEncryptionFlagCol );
	iEncryptionStateColNum         = colSet->ColNo( KGeneralTableEncryptionStateCol );
    iAuthenticationDataColNum      = colSet->ColNo( KGeneralTableAuthenticationDataCol );
    iMaxMruCountColNum             = colSet->ColNo( KGeneralTableMaxMruCountCol );
    iCurMruNumberColNum            = colSet->ColNo( KGeneralTableCurMruNumberCol );
	
	CleanupStack::PopAndDestroy( colSet );

	// There is only one entry in this table.  If the table is empty then create the single entry, and
	// initialize column data.

	if( tableIsEmpty )
		{		
		InsertRowLC();
		
		iTable.SetColL( schemaVersionColNum, KDbSchemaVersion );
		iTable.SetColL( iNextIdColNum, KLowestUserCreatedContainerId );
        iTable.SetColL( iAuthenticationRequiredColNum, EFalse);
		iTable.SetColL( iEncryptionFlagColNum, EFalse );  //defaclt to no encryption
		iTable.SetColL( iEncryptionStateColNum, EMsgStoreESIdle );
        iTable.SetColL( iMaxMruCountColNum, KDefaultMaxMruCount );  
        iTable.SetColL( iCurMruNumberColNum, KFirstMruNumber );
		
		PutRowUpdatesL();
		
		} // end if

	__LOG_EXIT	
	} // end OpenTableL
	
// ==========================================================================
// FUNCTION: CreateTableL
// ==========================================================================
void CContainerStoreGeneralTable::CreateTableL()
	{
	__LOG_ENTER( "CreateTableL" )
	
	// Create table columns
	
	CDbColSet* colSet = CDbColSet::NewLC();
	
	TDbCol schemaVersionIdCol( KGeneralTableDbSchemaVersionCol, EDbColUint32 );
	colSet->AddL( schemaVersionIdCol );	
	
	TDbCol nextIdCol( KGeneralTableNextIdCol, EDbColUint32 );
	colSet->AddL( nextIdCol );	
	
    
    TDbCol authRequiredCol( KGeneralTableAuthenticationRequiredCol, EDbColUint8 );
    colSet->AddL( authRequiredCol );    
    
	TDbCol encryptionFlagCol( KGeneralTableEncryptionFlagCol, EDbColUint8 );
	colSet->AddL( encryptionFlagCol );	

	TDbCol encryptionStateCol( KGeneralTableEncryptionStateCol, EDbColUint8 );
	colSet->AddL( encryptionStateCol );	
    
    TDbCol authDataCol( KGeneralTableAuthenticationDataCol, EDbColBinary );
    colSet->AddL( authDataCol );    
    
    TDbCol maxMruCountCol( KGeneralTableMaxMruCountCol, EDbColUint32 );
    colSet->AddL( maxMruCountCol );    
    
    TDbCol curMruNumCol( KGeneralTableCurMruNumberCol, EDbColUint32 );
    colSet->AddL( curMruNumCol );    
	// Create table.
	
	iUtils.CreateTableL( KGeneralTableName, *colSet );

	CleanupStack::PopAndDestroy( colSet );

	OpenTableL();
		
	__LOG_EXIT
	} // end CreateTableL
	
// ==========================================================================
// FUNCTION: AssignNextIdL
// ==========================================================================
TContainerId CContainerStoreGeneralTable::AssignNextIdL()
	{
	// Read and increment the next folder ID from the general table.
	
  	iTable.FirstL();
	PrepareRowForUpdateLC();
	
	TContainerId id = iTable.ColUint32( iNextIdColNum );

	iTable.SetColL( iNextIdColNum, id+1 );

	PutRowUpdatesL();
	
	//iTable.FirstL();
	
	return id;
	} // end AssignNextIdL

// ==========================================================================
// FUNCTION: IsEncryptionOnL
// ==========================================================================
TBool CContainerStoreGeneralTable::IsEncryptionOnL()
{
	iTable.FirstL();
  	iTable.GetL();

  	return iTable.ColUint8( iEncryptionFlagColNum );
}

// ==========================================================================
// FUNCTION: SetEncryptionFlagL
// ==========================================================================
void CContainerStoreGeneralTable::SetEncryptionFlagL( TBool aNewFlag )
{
	TBool curFlag = IsEncryptionOnL();
	if ( curFlag != aNewFlag )
	{
		iTable.FirstL();
		PrepareRowForUpdateLC();
	
		iTable.SetColL( iEncryptionFlagColNum, aNewFlag );
	
		PutRowUpdatesL();
		
		//iTable.FirstL();
	}
}

// ==========================================================================
// FUNCTION: AuthenticationDataPresentL
// ==========================================================================
TBool CContainerStoreGeneralTable::AuthenticationDataPresentL()
{
  	iTable.FirstL();
  	iTable.GetL();
  	TInt authenticationDataLength = iTable.ColLength( iAuthenticationDataColNum );

    return ( authenticationDataLength > 0 );    
} // end AuthenticationDataPresentL
    
// ==========================================================================
// FUNCTION: SetAuthenticationDataL
// ==========================================================================
void CContainerStoreGeneralTable::SetAuthenticationDataL( const TDesC8& aBuffer )
{
  	iTable.FirstL();  	
	PrepareRowForUpdateLC();
    
	iTable.SetColL( iAuthenticationDataColNum, aBuffer );

	PutRowUpdatesL();
	
	//iTable.FirstL();
} // end SetAuthenticationDataL

// ==========================================================================
// FUNCTION: GetAuthenticationDataL
// ==========================================================================
void CContainerStoreGeneralTable::GetAuthenticationDataL( RBuf8& aBuffer )
{
  	iTable.FirstL();
  	iTable.GetL();
    
    aBuffer.CreateL( iTable.ColDes8( iAuthenticationDataColNum ) );
} // end GetAuthenticationDataL
		
// ==========================================================================
// FUNCTION: EncryptionStateL
// ==========================================================================
TEncryptionState CContainerStoreGeneralTable::EncryptionStateL()
{
	iTable.FirstL();
	iTable.GetL();

    TEncryptionState encryptionState = static_cast<TEncryptionState>(iTable.ColUint8( iEncryptionStateColNum ));
    return encryptionState;
    
} // end EncryptionStateL

// ==========================================================================
// FUNCTION: SetEncryptionStateL
// ==========================================================================
void CContainerStoreGeneralTable::SetEncryptionStateL( TEncryptionState aEncryptionState )
{
	iTable.FirstL();  	
	PrepareRowForUpdateLC();
    
	iTable.SetColL( iEncryptionStateColNum, static_cast<TUint8>(aEncryptionState) );

	PutRowUpdatesL();
	
	//iTable.FirstL();
} // end SetEncryptionStateL

// ==========================================================================
// FUNCTION: IsAuthenticateRequiredL
// ==========================================================================
TBool CContainerStoreGeneralTable::IsAuthenticateRequiredL()
{
    iTable.FirstL();
    iTable.GetL();

    return iTable.ColUint8( iAuthenticationRequiredColNum );
}

// ==========================================================================
// FUNCTION: SetAuthenticationRequiredL
// ==========================================================================
void CContainerStoreGeneralTable::SetAuthenticationRequiredL( TBool aFlag )
    {
    iTable.FirstL();    
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iAuthenticationRequiredColNum, static_cast<TUint8>(aFlag) );

    PutRowUpdatesL();
    
    //iTable.FirstL();
    }

// ==========================================================================
// FUNCTION: MaxMruCountL
// ==========================================================================
TUint CContainerStoreGeneralTable::MaxMruCountL()
    {
    iTable.FirstL();
    iTable.GetL();

    return iTable.ColUint( iMaxMruCountColNum );
    }

// ==========================================================================
// FUNCTION: SetMaxMruCountL
// ==========================================================================
void CContainerStoreGeneralTable::SetMaxMruCountL( TUint aCount )
    {
    iTable.FirstL();    
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iMaxMruCountColNum, aCount );

    PutRowUpdatesL();
    
    //iTable.FirstL();
    }

TUint CContainerStoreGeneralTable::CurMruNumberL()
    {
    iTable.FirstL();
    iTable.GetL();

    return iTable.ColUint( iCurMruNumberColNum );
    }

void CContainerStoreGeneralTable::SetCurMruNumberL( TUint aNum )
    {
    iTable.FirstL();    
    PrepareRowForUpdateLC();
    
    iTable.SetColL( iCurMruNumberColNum, aNum );

    PutRowUpdatesL();
    
    //iTable.FirstL();
    }


