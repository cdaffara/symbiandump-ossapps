/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of DM adapter test component
* 	This is part of omadmextensions/adapter test application.
*
*/






// INCLUDE FILES
#include "dmatest.h"

#include <StifParser.h>
#include <Stiftestinterface.h>
#include <S32FILE.H>
#include <s32mem.h>
#include <apgcli.h>
#include <e32svr.h>
#include <e32math.h>
#include <f32file.h>
#include <swinstapi.h>

#include "TestDmDDFObject.h"

_LIT8( KEmptyType, "" );
_LIT8( KDefaultType, "text/plain" );
_LIT( KMappingTableFile, "\\dmtestmappings.txt" );
//_LIT8( KNSmlDMSeparator8, "/" );
//const TUint8 KNSmlDMUriSeparator = 0x2f; //forward slash

#define LEAVE_IF_ERROR(x,msg) \
	{ TInt __xres = (x); if ( __xres < 0 ) { if ( iLog ) iLog->Log( (msg), __xres ); User::Leave( __xres );	} }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cdmatest::Cdmatest
// C++ default constructor can NOT contain any code, that
// leave.
// -----------------------------------------------------------------------------
//
Cdmatest::Cdmatest(CTestModuleIf& aTestModuleIf, TUid aUid ):
        CScriptBase( aTestModuleIf ), iMappingTable(2), iUid( aUid )
    {
    
    }

// -----------------------------------------------------------------------------
// Cdmatest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void Cdmatest::ConstructL()
    {    
    Adapter();
	iEmptyMappingInfoArray = new ( ELeave ) CArrayFixFlat<TSmlDmMappingInfo>(1);
	
	TRAPD( err, LoadMappingsL() );
	if (err != KErrEof && err != KErrNone && err != KErrNotFound)
		{
		User::Leave( err );
		}
    }
    
CNSmlDSSettingsAdapter *Cdmatest::Adapter()  
	{
	if ( iAdapter == NULL )
		{
		if ( iLog )
			{
			iLog->Log( _L( "Loading Adapter" ) );
			}
  		
  		TRAPD( err, iAdapter = (CNSmlDSSettingsAdapter*) CSmlDmAdapter::NewL( iUid,*this ) );
	if ( err == KErrNone )
		{
		if (iLog )
			{
			iLog->Log( _L( "Loaded" ) );	
			}
		}
	else
		{
		if (iLog)
			{
			iLog->Log( _L( "Failed to load adapter: %d" ), err );
			}
		}
		}
	return iAdapter;
	}
    
    
void Cdmatest::LoadMappingsL()
	{
	TDataType type; 
	HBufC8 *data = LoadFileLC( KMappingTableFile,  type );
	RDesReadStream buf( *data );
	CleanupClosePushL( buf );

	TInt len( data->Length() );
	while (buf.Source()->TellL( MStreamBuf::ERead ).Offset() < len)
		{
		TUint32 val = buf.ReadUint32L();
		TBuf8<256> uri;
		TBuf8<64> luid;
		buf.ReadL(uri, val);
		val = buf.ReadUint32L();
		buf.ReadL(luid, val);
		TMapping m( uri, luid ) ;
		TInt err( iMappingTable.Append( m )	);
		if ( err == KErrNone )
			{
			iLog->Log( _L8( "Loaded mapping: '%S' : '%S'"), &m.iURI, &m.iLuid );
			}
		else
			{
			iLog->Log( _L8( "FAILED TO Load mapping: '%d' "), err );
			}
		}
	CleanupStack::PopAndDestroy( &buf); // buf
	CleanupStack::PopAndDestroy( data ); // data
	}
    	
	

void Cdmatest::SaveMappingsL()
    {
	TInt c( iMappingTable.Count() );
	if ( c > 0 )
	{
    	RFs fs;
    	User::LeaveIfError( fs.Connect() );
    	CleanupClosePushL( fs );
    	RFileWriteStream buf;
    	User::LeaveIfError( buf.Replace( fs, KMappingTableFile, EFileWrite ) );
    	CleanupClosePushL( buf );
		
		TInt i( 0 ) ;
		do 
			{
			buf.WriteUint32L( iMappingTable[i].iURI.Length() );
			buf.WriteL( iMappingTable[i].iURI );
			buf.WriteUint32L( iMappingTable[i].iLuid.Length() );
			buf.WriteL( iMappingTable[i].iLuid );
			}
		while ( ++i < c )	;
		buf.CommitL();
		buf.Close();
	
		CleanupStack::PopAndDestroy(); // buf
		CleanupStack::PopAndDestroy(); // fs
		}
    }
    	
    
// Destructor
Cdmatest::~Cdmatest()
    {
    // Delete resources allocated from test methods
    TRAPD(err, SaveMappingsL() );
    if ( err != KErrNone )
    	{
    	if(iLog)
    			iLog->Log( _L8( "Failed to save mappings!: %d"), err );
    	}
    Delete();
    
    // Delete logger
    delete iLog;  
    delete iEmptyMappingInfoArray;
    delete iAdapter;
    delete iURI;
    iMappingTable.Reset();
    REComSession::FinalClose();
    }


// -----------------------------------------------------------------------------
// Camatest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cdmatest::Delete() 
    {
    
    }

// -----------------------------------------------------------------------------
// Cdmatest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------


        
HBufC8 *Cdmatest::GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName )
	{
	TPtrC nodename( KNullDesC );

    TInt i( aItem.GetNextString ( nodename ) );
    if ( i != KErrNone ) 
	    {
	    iLog->Log( _L( "ERROR Reading '%S' argument: 0x%X" ), &aName, i );
	    }
	else
		{
		iLog->Log( _L("%S: %S"), &aName, &nodename);
		}
	
	HBufC8 *buf = HBufC8::NewLC( nodename.Length() ) ;
	buf->Des().Copy( nodename );
	return buf;
	}
	
TInt Cdmatest::FetchNodeL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Cdmatest"), _L("FetchNodeL") );

	iResultsFunction = FetchNodeResultsL;
	
    TPtrC8 nodename( GetNextStringLC( aItem, _L(" nodename" ) )->Des() ) ;
		
	SetURIL(nodename) ;//
	HBufC8 *luid = GetLuidAllocLC( *iURI );

	Adapter()->ChildURIListL( *iURI, *luid, *iEmptyMappingInfoArray, 4, 5) ;

	if ( iStatus == MSmlDmAdapter::EOk )
		{
		}
	else
		{
		iLog->Log( _L("FetchNodeL: ChildUriList Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy( luid ) ; 
	CleanupStack::PopAndDestroy() ; // nodename
	iLog->Log( _L("FetchNodeL: Test Complete with status %d" ), ret );	

    return ret;
	}
	
TInt Cdmatest::StartAtomicL( CStifItemParser& /*aItem*/ )	
	{
	TRAPD( err, Adapter()->StartAtomicL() ) ;
	iLog->Log( _L("StartAtomicL: Atomic started resulting error %d" ), err );	
    return err;
	}

TInt Cdmatest::RollbackAtomicL( CStifItemParser& /*aItem*/ )	
	{
	TRAPD( err, Adapter()->RollbackAtomicL() ) ;
	iLog->Log( _L("RollbackAtomicL: Atomic rolled back resulting error %d" ), err );	
    return err;
	}
	
TInt Cdmatest::CommitAtomicL( CStifItemParser& /*aItem*/ )	
	{
	TRAPD( err, Adapter()->CommitAtomicL() ) ;
	iLog->Log( _L("RollbackAtomicL: Atomic commited resulting error %d" ), err );	
    return err;
	}
	

TInt Cdmatest::DDFStructureL( CStifItemParser& /*aItem*/ )	
	{
	CTestDmDDFObject* ddfRoot = CTestDmDDFObject::NewLC( iLog ); //, aNodeName );

	TRAPD( err, iAdapter->DDFStructureL( *ddfRoot ) ) ;
	CleanupStack::PopAndDestroy( ddfRoot );
	iLog->Log( _L("DDFStructureL: method called resulting error %d" ), err );	
    return err;
	}
		
		
TInt Cdmatest::AddNodeL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Cdmatest"), _L("AddNodeL") );

    
    TPtrC8 nodename( GetNextStringLC ( aItem, _L("nodename" ) )->Des() ) ;
    SetURIL( nodename );

	Adapter()->AddNodeObjectL( *iURI, KEmptyType, 8 ) ;
	
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("AddNodeL: AddNodeObjectL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("AddNodeL: AddNodeObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
		
	CleanupStack::PopAndDestroy() ; // nodename
	iLog->Log( _L("AddNodeL Test Complete with status %d" ), ret );	

    return ret;
	}	
		
TInt Cdmatest::UpdateLeafL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Cdmatest"), _L("UpdateLeafL") );

    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Node name") )->Des() ) ;
    TPtrC8 data (GetNextStringLC( aItem, _L("datafile"))->Des() );
    
	HBufC8 *mime = GetNextStringLC( aItem, _L("mime") ) ;
    SetURIL( nodename );
    
    TPtrC8 parentURI(RemoveLastSeg(nodename));
    HBufC8 *luid = GetLuidAllocLC( parentURI );
    
    TDataType type; 

	TPtrC8 mimePtr( *mime == KNullDesC8 ? type.Des8() : mime->Des() );

    /**
    virtual void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
									const TDesC8& aObject, const TDesC8& aType,
									TInt aStatusRef ) = 0;
    */
    
	Adapter()->UpdateLeafObjectL( *iURI , *luid, data, mimePtr, 3);

	if ( iStatus == MSmlDmAdapter::EOk )
	{
		iLog->Log( _L("UpdateLeafL: UpdateLeafObjectL Successful! %d" ), iStatus );	
	}
	else
	{
		iLog->Log( _L("UpdateLeafL UpdateLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
	}

	CleanupStack::PopAndDestroy(); // loadfile
	CleanupStack::PopAndDestroy(); // luid
	CleanupStack::PopAndDestroy(); // mime
	CleanupStack::PopAndDestroy(); // nodename

	iLog->Log( _L("UpdateLeafL Test Complete with status %d" ), ret );	

    return ret;
	}	
	

TInt Cdmatest::UpdateLeafDataURLL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Cdmatest"), _L("UpdateLeafDataL") );

    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Node name") )->Des() ) ;
    TPtrC8 http (GetNextStringLC( aItem, _L("http"))->Des() );
    TPtrC8 url (GetNextStringLC( aItem, _L("rest of url"))->Des() );
	HBufC8 *mime = GetNextStringLC( aItem, _L("mime") ) ;
    SetURIL( nodename );
    
    _LIT8( KTag, "://" );
    
    HBufC8 *fullurl = HBufC8::NewLC( http.Length() + KTag().Length() + url.Length() );
    TPtr8 pfullurl( fullurl->Des() );
    pfullurl.Copy( http ) ;
    pfullurl.Append( KTag );
    pfullurl.Append( url );
    TPtrC8 mimePtr( *mime == KNullDesC8 ? KDefaultType() : mime->Des() );
    
    HBufC8 *luid = GetLuidAllocLC( *iURI );
    /**
    virtual void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
									const TDesC8& aObject, const TDesC8& aType,
									TInt aStatusRef ) = 0;
    */
	Adapter()->UpdateLeafObjectL( *iURI , *luid, pfullurl, mimePtr, 3);
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("UpdateLeafDataL: UpdateLeafObjectL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("UpdateLeafDataL UpdateLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy( luid ); // 
	CleanupStack::PopAndDestroy( mime ); // mime
	CleanupStack::PopAndDestroy(); // url
	CleanupStack::PopAndDestroy(); // http
	CleanupStack::PopAndDestroy(); // nodename
	iLog->Log( _L("UpdateLeafDataL Test Complete with status %d" ), ret );	

    return ret;
	}
    
TInt Cdmatest::UpdateLeafDataL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
   TestModuleIf().Printf( 0, _L("Camtest"), _L("UpdateLeafDataL") );

    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Node name") )->Des() ) ;
    TPtrC8 data (GetNextStringLC( aItem, _L("data"))->Des() );
	HBufC8 *mime = GetNextStringLC( aItem, _L("mime") ) ;
	
    SetURIL( nodename );
    
        
    TPtrC8 mimePtr( *mime == KNullDesC8 ? KDefaultType() : mime->Des() );
    
    TPtrC8 parentURI(RemoveLastSeg(nodename));
    HBufC8 *luid = GetLuidAllocLC( parentURI );
     
//    
//    virtual void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
//									const TDesC8& aObject, const TDesC8& aType,
//									TInt aStatusRef ) = 0;
//   
	Adapter()->UpdateLeafObjectL( *iURI , *luid, data, mimePtr, 3);
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("UpdateLeafDataL: UpdateLeafObjectL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("UpdateLeafDataL UpdateLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	
	CleanupStack::PopAndDestroy(); // mime
	CleanupStack::PopAndDestroy(); // luid
	CleanupStack::PopAndDestroy(); // data
	CleanupStack::PopAndDestroy(); // nodename
	iLog->Log( _L("UpdateLeafDataL Test Complete with status %d" ), ret );	

    return ret;
	}	
			
TInt Cdmatest::FetchLeafL( CStifItemParser& aItem )	
	{

    TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Camtest"), _L("FetchLeafL") );

	iResultsFunction = NULL;
	
    TInt i( 0 );
    TPtrC8 nodename ( GetNextStringLC( aItem, _L( "nodename" ) )->Des() ) ;
    
    //TPtrC datafile;
    TPtrC datafile( KNullDesC );
    i = aItem.GetNextString ( datafile ) ;
    if ( i != KErrNone ) 
	    {
	    iLog->Log(_L("FetchLeafL: ERROR Reading outfile argument: 0x%X"), i );
	    //return i;
	    }
	else
		{
		iSaveFileName = datafile;
		iLog->Log( _L( " Save file nameis '%S'" ), &iSaveFileName );
		iResultsFunction = SaveDataL;
		}
    
	SetURIL(nodename) ;
		
		/*
			void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
								   const TDesC8& aType, TInt aResultsRef,
								   TInt aStatusRef );
		*/
    TPtrC8 parentURI(RemoveLastSeg(nodename));
    HBufC8 *luid = GetLuidAllocLC( parentURI );

	Adapter()->FetchLeafObjectL( *iURI, *luid, KEmptyType, 7, 8 ) ;
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("FetchLeafL: FetchLeafObjectL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("FetchLeafL: FetchLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy( luid );
	CleanupStack::PopAndDestroy(  ); // nodename
	iLog->Log( _L("FetchLeafL Test Complete with status %d" ), ret );	
    return ret;
	}	


TInt Cdmatest::ExecuteLeafL( CStifItemParser& aItem )		
	{
	TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Camtest"), _L("ExecuteLeafL") );

	iResultsFunction = NULL;
	
    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Nodename") )->Des() ) ;
    TPtrC8 data( GetNextStringLC ( aItem, _L("Input file") )->Des() ) ;
	
	SetURIL(nodename) ;

		/*
			virtual void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
							      const TDesC8& aArgument, const TDesC8& aType,
								  TInt aStatusRef ) = 0;
		*/
	TDataType type;
	
    TPtrC8 parentURI(RemoveLastSeg(nodename));
    HBufC8 *luid = GetLuidAllocLC( parentURI );

	Adapter()->ExecuteCommandL( *iURI, *luid, data, KEmptyType, 11 ) ;
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("ExecuteLeafL: ExecuteCommandL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("ExecuteLeafL: ExecuteCommandL FetchLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy( luid ); // luid
	CleanupStack::PopAndDestroy(); // data
	CleanupStack::PopAndDestroy(); // nodename
	
	iLog->Log( _L("ExecuteLeafL: Test Complete with status %d" ), ret );	

    return ret;
	}
	
TInt Cdmatest::CompleteCommandsL( CStifItemParser& /*aItem*/ )
	{
	TRAPD( err, Adapter()->CompleteOutstandingCmdsL() );
	delete iAdapter;
	iAdapter = NULL;
	return err;
	}
TInt Cdmatest::DeleteObjectL( CStifItemParser& aItem )
{
	TInt ret( KErrNone );
    // Print to UI
    TestModuleIf().Printf( 0, _L("Camtest"), _L("DeleteObjectL") );

    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Nodename") )->Des() ) ;

	SetURIL(nodename) ;

	HBufC8 *luid = GetLuidAllocLC( *iURI );
	Adapter()->DeleteObjectL( *iURI, *luid, 11 ) ;
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("DeleteNode: DeleteObjectL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("DeleteNode: DeleteObjectL FetchLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy( luid ); // luid
	CleanupStack::PopAndDestroy(); // nodename	
	iLog->Log( _L("ExecuteLeafDataL Test Complete with status %d" ), ret );	

    return ret;	
}
	
TInt Cdmatest::ExecuteLeafDataL( CStifItemParser& aItem )		
	{
	TInt ret( KErrNone );

    // Print to UI
    TestModuleIf().Printf( 0, _L("Camtest"), _L("ExecuteLeafL") );

	iResultsFunction = NULL;
	
    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Nodename") )->Des() ) ;
    TPtrC8 data( GetNextStringLC ( aItem, _L("Input data") )->Des() ) ;
    
	SetURIL(nodename) ;

		/*
			virtual void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
							      const TDesC8& aArgument, const TDesC8& aType,
								  TInt aStatusRef ) = 0;
		*/
	HBufC8 *luid = GetLuidAllocLC( *iURI );
	Adapter()->ExecuteCommandL( *iURI, *luid, data, KEmptyType, 11 ) ;
	if ( iStatus == MSmlDmAdapter::EOk )
		{
		iLog->Log( _L("ExecuteLeafDataL: ExecuteCommandL Successful! %d" ), iStatus );	
		}
	else
		{
		iLog->Log( _L("ExecuteLeafDataL: ExecuteCommandL FetchLeafObjectL Error ! %d" ), iStatus );	
		ret = KErrGeneral ;
		}
	CleanupStack::PopAndDestroy(); // luid
	CleanupStack::PopAndDestroy(); // data		
	CleanupStack::PopAndDestroy(); // nodename	
	iLog->Log( _L("ExecuteLeafDataL Test Complete with status %d" ), ret );	

    return ret;
	}


HBufC8 *Cdmatest::LoadFileLC( const TDesC &aFileName, TDataType &aType )	
	{
	RFs fs ;
	LEAVE_IF_ERROR( fs.Connect(), _L( "Could not connect fileserver: %d"  ) );
	
	CleanupClosePushL( fs );
	RFile file ;
	LEAVE_IF_ERROR( file.Open(fs,aFileName,EFileRead), _L( "Could not open file: %d" ) );
	
	
	
	CleanupClosePushL( file );
	TInt dataSize ;
	LEAVE_IF_ERROR( file.Size( dataSize ), _L( "Could not get file size: %d" ) );
	HBufC8 *nodedata = HBufC8::NewL ( dataSize );
	CleanupStack::PushL( nodedata );
	TPtr8 nodedataptr( nodedata->Des() );
	LEAVE_IF_ERROR( file.Read( nodedataptr ), _L( "Could not read file: %d" ) );
	TDataRecognitionResult aDataType;
	RApaLsSession ls ;
	TInt err( ls.Connect() );
	if ( err == KErrNone )
		{
		CleanupClosePushL( ls );
		err = ls.RecognizeData(aFileName, nodedataptr, aDataType)  ;
		if ( err == KErrNone )
			{
			aType = aDataType.iDataType;
			}
		else
			{
			iLog->Log( _L("LoadFileLC: WARNING Failed to get type: %d" ), err );
			aType = TDataType( KDefaultType );
			}	
		CleanupStack::PopAndDestroy( &ls ); 
		}
	else
		{
		iLog->Log( _L("LoadFileLC: WARNING Failed to connect rapalssession: %d" ), err );
		}	
	CleanupStack::Pop( nodedata );
	CleanupStack::PopAndDestroy( &file ); 
	CleanupStack::PopAndDestroy( &fs ); 
	CleanupStack::PushL( nodedata );
	return nodedata ;
	}

HBufC8 *Cdmatest::LoadFileLC( const TDesC8 &aFileName, TDataType &aType )	
	{
	TFileName fn ;
	fn.Copy( aFileName );
	return LoadFileLC( fn, aType );
	}
	

void Cdmatest::SaveDataL( TInt /*aResultsRef*/, CBufBase& aObject,
							  const TDesC8& aType ) 
	{
	iLog->Log( _L8( "Saving data of type: '%S'" ), &aType );
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	RFile file;
	User::LeaveIfError( file.Replace ( fs, iSaveFileName, EFileWrite ) );
	CleanupClosePushL( file );
	TPtrC8 p( aObject.Ptr( 0 ) );
	User::LeaveIfError( file.Write( p ) );
	CleanupStack::PopAndDestroy( 2 ); // file, fs
	}
							  
		
	
void Cdmatest::FetchNodeResultsL( TInt /*aResultsRef*/, CBufBase& aObject,
							  const TDesC8& /*aType*/ )
	{
	TPtrC8 ptr( aObject.Ptr( 0 ) );	
	iLog->Log( _L8("FetchNodeResultsL for '%S': '%S'" ), iURI, &ptr );
	
	if ( ptr.Length() > 0 )
		{
		TPtrC8 last( LastURISeg( ptr ) );
		HBufC8 *oldUri = HBufC8::NewL( iURI->Length() );
		(*oldUri) = *iURI; 
		do 
			{
			iLog->Log ( _L8( " Node: '%S' "), &last );
			HBufC8 *nUri = HBufC8::NewLC( oldUri->Length() + 1 + last.Length() );
			nUri->Des().Copy( *oldUri ) ;
			nUri->Des().Append( '/' );
			nUri->Des().Append( last );
			
			SetURIL( nUri );
			//iResultsFunction = FetchNodeResultsL;
			
		    //TPtrC8 parentURI(RemoveLastSeg(*nUri));
		    //HBufC8 *luid = GetLuidAllocLC( parentURI );
		    CleanupStack::Pop( nUri );
			
			HBufC8 *luid = GetLuidAllocLC( *iURI );
			
			Adapter()->ChildURIListL( *nUri, KNullDesC8, *iEmptyMappingInfoArray, 4, 5 );//Dipak
			
			CleanupStack::PopAndDestroy( luid ); 			
			
			ptr.Set( RemoveLastURISeg( ptr ) );
			last.Set( LastURISeg( ptr ) );
			
			}
		while (last != KNullDesC8);	
		}
	
	}
	

	
TPtrC8 Cdmatest::LastURISeg( const TDesC8& aURI )
	{
	TInt i;
	for( i = aURI.Length() - 1; i >= 0; i-- ) 
		{
		if( aURI[i] == '/' )
			{
			break;
			}
		}

	if( i == 0 )
		{
		return aURI;
		}
	else
		{
		return aURI.Mid( i+1 );
		}
	}
TPtrC8 Cdmatest::RemoveLastSeg(const TDesC8& aURI)
	{
	TInt i;
	for(i=aURI.Length()-1;i>=0;i--)
		{
		if(aURI[i]==KNSmlDMUriSeparator)
			{
			break;
			}
		}

	if(i>0)
		{
		return aURI.Left(i);
		}
	else
		{
		return KNullDesC8();
		}
	}

// ------------------------------------------------------------------------------------------------
// TPtrC8 Cdmatest::RemoveLastURISeg(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// ------------------------------------------------------------------------------------------------
TPtrC8 Cdmatest::RemoveLastURISeg( const TDesC8& aURI )
	{
	TInt i;
	for ( i = aURI.Length() - 1; i >= 0 ; i-- )
		{
		if( aURI[i] == '/' )
			{
			break;
			}
		}
	if ( i > -1 )
		{
		return aURI.Left( i );	
		}
	else
		{
		return KNullDesC8();
		}
	}	
	
TPtrC Cdmatest::RemoveLastURISeg( const TDesC& aURI )
	{
	TInt i;
	for ( i = aURI.Length() - 1; i >= 0 ; i-- )
		{
		if( aURI[i] == '/' )
			{
			break;
			}
		}
	if ( i > -1 )
		{
		return aURI.Left( i );	
		}
	else

		{
		return KNullDesC();
		}
	}		
	
void Cdmatest::SetURIL( const TDesC& aURI )
	{
	if ( iURI != NULL )
		{
		delete iURI ;
		iURI = NULL;
		}
	iURI = HBufC8::NewL( aURI.Length() ) ;
	iURI->Des().Copy( aURI );
	}
	
void Cdmatest::SetURIL( const TDesC8& aURI )
	{
	if ( iURI != NULL )
		{
		delete iURI ;
		iURI = NULL;
		}
	iURI = HBufC8::NewL( aURI.Length() ) ;
	iURI->Des().Copy( aURI );
	}	

void Cdmatest::SetURIL( HBufC8* aURI )
	{
	if ( iURI != NULL )
		{
		delete iURI ;
		iURI = NULL;
		}
	iURI = aURI ;
	}
		
		
		
void Cdmatest::SetResultsL( 
	TInt aResultsRef, 
	CBufBase& aObject,
	const TDesC8& aType )
	{
	TPtrC8 ptr( aObject.Ptr(0) );
	iLog->Log( _L8( "SetResults, ref=%d, object='%S', type='%S'" ), aResultsRef, &ptr, &aType );
	if ( iResultsFunction )
		{
		(this->*iResultsFunction)( aResultsRef, aObject, aType );
		iResultsFunction = NULL ;
		}
	
	}


void Cdmatest::SetStatusL( TInt aStatusRef,
							 MSmlDmAdapter::TError aErrorCode ) 
	{
	iStatus = aErrorCode ;
	iLog->Log( _L( "SetStatusL, ref=%d, code=%d" ), aStatusRef, aErrorCode );
	
	
	}
	
void Cdmatest::SetMappingL( const TDesC8& aURI, const TDesC8& aLUID ) 
	{
	iLog->Log( _L8( "SetMappingL, aURI='%s', aLUID='%s'" ), aURI.Ptr(), aLUID.Ptr() );	
	
	iMappingTable.Append(TMapping( aURI, aLUID ) )	;
	}
	
HBufC8* Cdmatest::GetLuidAllocL( const TDesC8& aURI ) 
	{
	iLog->Log( _L8( "GetLuidAllocL, aURI='%S'" ), &aURI );		
	HBufC8 *res = NULL;		
	for( TInt i(0); i < iMappingTable.Count(); i++ )	
		{
		if ( aURI == iMappingTable[i].iURI )
			{
			res = iMappingTable[i].iLuid.AllocL();
			}
		}
	if ( res == NULL )
		{
		res = HBufC8::NewL( 0 );
		}
	iLog->Log( _L8( "GetLuidAllocL, response='%S'" ), res );
	return res;
	}	    
    
HBufC8* Cdmatest::GetLuidAllocLC( const TDesC8& aURI ) 
	{
	iLog->Log( _L8( "GetLuidAllocL, aURI='%S'" ), &aURI );
	HBufC8 *res = NULL;		
	for( TInt i(0); i < iMappingTable.Count(); i++ )	
		{
		if ( aURI == iMappingTable[i].iURI )
			{
			res = iMappingTable[i].iLuid.AllocLC();
			break;
			}
		}
	if ( res == NULL )
		{
		res = HBufC8::NewLC( 0 );
		}
	iLog->Log( _L8( "GetLuidAllocLC, response='%S'" ), res );
	return res ;
	}	
    	
#ifdef __TARM_SYMBIAN_CONVERGENCY
    
void Cdmatest::GetMappingInfoListL( const TDesC8& /*aURI*/,
								CArrayFix<TSmlDmMappingInfo>& /*aSegmentList*/ )
	{
	// do nothing
	}								

#else
// nothing
#endif
//***************************************************************************
//  End of File
