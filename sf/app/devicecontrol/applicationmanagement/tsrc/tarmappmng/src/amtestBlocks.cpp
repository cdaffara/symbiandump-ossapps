/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of applicationmanagement components
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "amtest.h"
#include "applicationmanagementclient.h"
#include <e32math.h>
#include <f32file.h>
#include <swinstapi.h>


using namespace NApplicationManagement;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Camtest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Camtest::Delete() 
    {
    
    }
    
// -----------------------------------------------------------------------------
// Camtest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Camtest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "DDFStructure", Camtest::DDFStructureL),
        ENTRY( "List", Camtest::ExampleL ),
        ENTRY( "Deliver", Camtest::DeliverL ),
        ENTRY( "Details", Camtest::DetailsL ),
        ENTRY( "Install", Camtest::InstallL ),
        ENTRY( "BareInstall", Camtest::BareInstallL ),
        ENTRY( "FetchNode", Camtest::FetchNodeL ),
        ENTRY( "FetchLeaf", Camtest::FetchLeafL ),
        ENTRY( "FetchLeafObjectSizeL", Camtest::FetchLeafObjectSizeL ),
        //ENTRY( "CompleteCommands", Cdmatest::CompleteCommandsL ),
        ENTRY( "AddNode", Camtest::AddNodeL ),
        ENTRY( "UpdateLeaf", Camtest::UpdateLeafL ),
        ENTRY( "UpdateLeafData", Camtest::UpdateLeafDataL ),
        ENTRY( "UpdateLeafDataURI", Camtest::UpdateLeafDataURLL ),
        ENTRY( "Execute", Camtest::ExecuteLeafL ),
        ENTRY( "ExecuteData", Camtest::ExecuteLeafDataL ),
        ENTRY( "DeleteNode", Camtest::DeleteObjectL ),
        ENTRY( "StartAtomic", Camtest::StartAtomicL),
	    ENTRY( "CommitAtomic", Camtest::CommitAtomicL),
	    ENTRY( "RollbackAtomic", Camtest::RollbackAtomicL),
	    //Newly addded
	    ENTRY( "AddLeafNode", Camtest::AddLeafNode),
	    ENTRY( "InstallActivate", Camtest::InstallActivate),
	    //Newly ended
//	    ENTRY( "BareAppInstall", Camtest::BareAppInstallL),
/*        ENTRY( "DDFStructure", Camtest::DDFStructureL),*///BareAppInstallL
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

// -----------------------------------------------------------------------------
// Camtest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Camtest::ExampleL( CStifItemParser& /*aItem*/ )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("amtest"), _L("In Example") );
    // Print to log file
    iLog->Log(_L("In Example"));

    RApplicationManagement ams ;
    TInt err( ams.Connect() );
    
    if ( err == KErrNone )
    	{    	    
	    RElementIdArray array;
	    ams.DeploymentComponentIdsL(array);
		iLog->Log( _L( "Found %d Ids" ), array.Count() );
		for ( TInt i = 0; i < array.Count(); i++ )
			{
			iLog->Log( _L( "  Id %d: %d" ), i, array[i] );
			}
			
			
		TDeploymentComponent comp ;
		comp.iId = _L8( "Iidee" );
		TBuf8<10> buf;
		TTime tm;
		tm.HomeTime();
		TInt64 rn = tm.Int64() ;
		TInt rand = Math::Rand( rn ) ;
		buf.NumFixedWidth( rand, EHex, 10);
	
		comp.iId.Append( buf );
		comp.iVersion = _L8( "2.3" );
		comp.iName = _L8( "Nimi" );
		
		err = ams.DeliverL(/*_L8("Leaf"), */comp.iId/*, comp.iName, comp.iVersion, EDCSDelivered, _L8("datadatadatadata"), _L8("iopts"), _L8("meta") */) ;
		iLog->Log( _L( " Deliver response: %d, state: %d" ), err, comp.iState );
		User::LeaveIfError ( err );
		err = KErrNone;
		
		array.Reset();
		ams.DeploymentComponentIdsL(array);
		iLog->Log( _L( "Found 2ndtime %d Ids" ), array.Count() );
		for ( TInt i = 0; i < array.Count(); i++ )
			{
			iLog->Log( _L( "  Id %d: %d" ), i, array[i] );
			
			TDeploymentComponent comp2 ;
			err = ams.DeploymentComponent(array[i], comp);
			iLog->Log( _L( " DeploymentComponent response: %d, state: %d" ), err, comp.iState );
			if ( err == KErrNone )
				{
				HBufC *b = HBufC::NewLC( comp.iId.Length() );
				TPtr p( b->Des() );
				p.Copy( comp.iId );
				
				iLog->Log( _L( "CompId: %S" ), &p );
				CleanupStack::PopAndDestroy();
				}
			else
				{
				
				}
			}
		ams.Close();
		}
	else
		{
		iLog->Log( _L( "Failed to connect: %d" ), err);
		}

    return err;
    
    }
    
// -----------------------------------------------------------------------------
// Camtest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Camtest::DetailsL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("amtest"), _L("In DetailsL") );
    // Print to log file
    iLog->Log(_L("In DetailsL"));

    RApplicationManagement ams ;
    TInt err( ams.Connect() );
    
    if ( err == KErrNone )
    	{    	
	    
	    TInt i( 0 );
	    TInt nodename;
	    i = aItem.GetNextInt ( nodename ) ;
	    if ( i != KErrNone ) 
		    {
		    iLog->Log( _L( "ERROR Reading id argument: 0x%X"), i);
		    //return i;
		    }
		else
			{
			iLog->Log( _L("id: %d"), nodename);
			}
	    
	    RElementIdArray array;
	    ams.DeploymentComponentIdsL(array);
		iLog->Log( _L( "Found %d Ids" ), array.Count() );
		for ( TInt i = 0; i < array.Count(); i++ )
			{
			
			iLog->Log( _L( "  Id %d: %d" ), i, array[i] );

			TDeploymentComponent comp2 ;
			err = ams.DeploymentComponent(array[i], comp2);
			iLog->Log( _L( " DeploymentComponent response: %d, state: %d" ), err, comp2.iState );
			if ( err == KErrNone )
				{
				HBufC *b = HBufC::NewLC( comp2.iId.Length() );
				TPtr p( b->Des() );
				p.Copy( comp2.iId );
				
				iLog->Log( _L( "CompId: %S" ), &p );
				CleanupStack::PopAndDestroy();
				}
			else
				{
				
				}
			}
		User::LeaveIfError ( err );
		err = KErrNone;
		ams.Close();
		}
	else
		{
		iLog->Log( _L( "Failed to connect: %d" ), err);
		}
    return err;
    }
    
    
// -----------------------------------------------------------------------------
// Camtest::InstallL
// -----------------------------------------------------------------------------
//
TInt Camtest::InstallL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("amtest"), _L("In InstallL") );
    // Print to log file
    
    iLog->Log(_L("In DetailsL"));
    TCertInfo info ;
	info.iFingerprint =_L8("finger");
	info.iSerialNo = _L8("serial");
	
    RApplicationManagement ams ;
    TInt err( ams.Connect( info ) );
    
    
    if ( err == KErrNone )
    	{    	
    	
        TInt i( 0 );
	    TUint32 nodename;
	    i = aItem.GetNextInt ( (TInt&)nodename ) ;
	    if ( i != KErrNone ) 
		    {
		    iLog->Log( _L( "ERROR Reading id argument: 0x%X"), i);
		    //return i;
		    }
		else
			{
			iLog->Log( _L("id: %d"), nodename);
			}
	    
/*	    RElementIdArray array;
	    ams.DeploymentComponentIdsL(array);
		iLog->Log( _L( "Found %d Ids" ), array.Count() );
		for ( TInt i = 0; i < array.Count(); i++ )
			{
			
			iLog->Log( _L( "  Id %d: %d" ), i, array[i] );

			TDeploymentComponent comp2 ;
			err = ams.DeploymentComponent(array[i], comp2);
			iLog->Log( _L( " DeploymentComponent response: %d, state: %d" ), err, comp2.iState );
			if ( err == KErrNone )
				{
				HBufC *b = HBufC::NewLC( comp2.iId.Length() );
				TPtr p( b->Des() );
				p.Copy( comp2.iId );
				
				iLog->Log( _L( "CompId: %S" ), &p );
				CleanupStack::PopAndDestroy();
				
				}
			else
				{
				
				}
				
			if ( nodename == array[i] )
				{
				iLog->Log( _L( "CompId: %S" ), &p );
				}
			}
		*/
		ams.InstallL( nodename ) ;
		User::LeaveIfError ( err );
		err = KErrNone;

		ams.Close();
		}
	else
		{
		iLog->Log( _L( "Failed to connect: %d" ), err);
		}
    return err;
    }
    
// -----------------------------------------------------------------------------
// Camtest::InstallL
// -----------------------------------------------------------------------------
//
TInt Camtest::BareInstallL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("amtest"), _L("In InstallL") );
    // Print to log file
    iLog->Log(_L("In DetailsL"));

/*void CAmAdapter::InstallL( 
	TUint aLuidi, 
	const TDesC8& aURI, 
	const TDesC8& aLUID, 
	const TDeploymentComponentState aTargetState,
	TError &aRet
    TInt aluidi( 0 );
    TUint32 aLuidInt;
    aluidi = aItem.GetNextInt ( (TInt&)aLuidInt ) ;
	
	
    TPtrC8 nodename( GetNextStringLC ( aItem, _L("Node name") )->Des() ) ;
    
	
    SetURIL( nodename );
    
    HBufC8 *luid = GetLuidAllocLC( *iURI );
    TError ret( EError );
	
	TRAPD( err, Adapter()->InstallL(aluidi,*iURI, *luid,3 ,ret));*/

	//using namespace SwiUI;
    SwiUI::RSWInstLauncher ams ;
    TInt err( ams.Connect() );
    
    if ( err == KErrNone )
    	{    	
    	CleanupClosePushL( ams );
	    iLog->Log( _L( "Connected!" ));
	    TPtrC filename ;
	    User::LeaveIfError( aItem.GetNextString( filename ) );
	    
	   	
	   		
   		TRequestStatus stat;
   		
   		ams.Install( stat, filename  );
   		User::WaitForRequest( stat );
   		CleanupStack::Pop();
   		
		ams.Close();
		}
	else
		{
		iLog->Log( _L( "Failed to connect: %d" ), err);
		}
    return err;
    }
        
        
HBufC8 *Camtest::GetNextStringLC ( CStifItemParser& aItem, const TDesC &aName )
	{
	TPtrC nodename;
	nodename.Set( KNullDesC );

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
	
	
HBufC8 *LoadFileL( const TDesC8 &aDataFile )
	{
	RFs fs ;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs ); // 1
	RFile file ;
	TFileName fn; 
	fn.Copy( aDataFile );
	User::LeaveIfError ( file.Open( fs, fn, EFileRead ) );
	CleanupClosePushL( file ); // 2
	TInt size;
	User::LeaveIfError( file.Size( size ) );
	
	HBufC8 *res = HBufC8::NewLC( size ); //3
	TPtr8 p( res->Des() );
	User::LeaveIfError( file.Read( p ) );
	CleanupStack::Pop(); // res
	CleanupStack::PopAndDestroy( 2 ); // file, fs
	CleanupStack::PushL( res );
	return res ;
	}
	
// -----------------------------------------------------------------------------
// Camtest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Camtest::DeliverL( CStifItemParser& aItem )
    {
    
    // Print to UI
    TestModuleIf().Printf( 0, _L("amtest"), _L("In DeliverL") );
    // Print to log file
    iLog->Log(_L("In DeliverL"));

    RApplicationManagement ams ;
    TInt err( ams.Connect() );
    
    if ( err == KErrNone )
    	{    	
    	CleanupClosePushL( ams );
	    HBufC8 *id = GetNextStringLC ( aItem, _L("Id") ) ;
	    HBufC8 *name = GetNextStringLC( aItem, _L( "Name" )  ) ;
	    HBufC8 *version = GetNextStringLC (  aItem, _L( "Version" ) ) ;
	    HBufC8 *data = GetNextStringLC ( aItem, _L( "data" ) ) ;
	    HBufC8 *instopt = GetNextStringLC (aItem, _L( "InstOpts" ) ) ;
	    if ( data != NULL )
	    	{
	    	//HBufC8 *datat = NULL;	
		    HBufC8 *datat = LoadFileL( *data );
		    RElementIdArray array;
		    ams.DeploymentComponentIdsL(array);
			iLog->Log( _L( "Found %d Ids" ), array.Count() );
			for ( TInt i = 0; i < array.Count(); i++ )
				{
				iLog->Log( _L( "  Id %d: %d" ), i, array[i] );
				}
								
			TDeploymentComponent comp ;
			comp.iId = *id;
			comp.iVersion = *version;
			comp.iName = *name;
			
			err = ams.DeliverL(/*_L8("Jee"),*/ comp.iId/*, comp.iName, comp.iVersion, EDCSDelivered , *datat, *instopt, _L8("meta") */) ;
			iLog->Log( _L( " Deliver response: %d, state: %d" ), err, comp.iState );
			User::LeaveIfError ( err );
			err = KErrNone;
			
			array.Reset();
			ams.DeploymentComponentIdsL(array);
			iLog->Log( _L( "Found 2ndtime %d Ids" ), array.Count() );
			for ( TInt i = 0; i < array.Count(); i++ )
				{
				iLog->Log( _L( "  Id %d: %d" ), i, array[i] );
				}
				
			CleanupStack::PopAndDestroy( datat ); 
	    	}
		CleanupStack::PopAndDestroy( instopt ); 
		CleanupStack::PopAndDestroy( data ); 
		CleanupStack::PopAndDestroy( version );
		CleanupStack::PopAndDestroy( name );
		CleanupStack::PopAndDestroy( id ); 
		CleanupStack::PopAndDestroy( ); //ams
		}
	else
		{
		iLog->Log( _L( "Failed to connect: %d" ), err);
		}

    return err;
    
    }        
   

//  End of File
