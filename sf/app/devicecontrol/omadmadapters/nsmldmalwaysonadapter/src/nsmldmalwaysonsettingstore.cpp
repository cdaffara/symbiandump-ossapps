/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides settings management in Central Repository.
*
*/



#include <utf.h>
#include <pdpcontextmanagerinternalcrkeys.h>

#include "nsmldmalwaysonsettingstore.h"
#include "nsmldmalwaysonadapter.h"
#include "logger.h"

// AWON-PDPC
// Values are XY, where X is related to 3G flag and Y is related to 2G flag.
// Possible values are:
// "00", AlwaysOn is OFF for both 3G and 2G
// "10", AlwaysOn is ON for 3G, OFF for 2G
// "01", AlwaysOn is OFF for 3G, ON for 2G
// "11", AlwaysOn is ON for 3G, ON for 2G
const TInt KAwonPdpc_Off3G_Off2G = 00;
const TInt KAwonPdpc_Off3G_On2G  = 01;
const TInt KAwonPdpc_On3G_Off2G  = 10;
const TInt KAwonPdpc_On3G_On2G   = 11;

const TInt KBufSize = 10;

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::NewL
// -----------------------------------------------------------------------------
CNSmlDmAOSettingStore* CNSmlDmAOSettingStore::NewL( CNSmlDmAOAdapter * aAdapter )
    {
    CNSmlDmAOSettingStore* self = new (ELeave) CNSmlDmAOSettingStore( aAdapter );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::CNSmlDmAOSettingStore
// -----------------------------------------------------------------------------
CNSmlDmAOSettingStore::CNSmlDmAOSettingStore( CNSmlDmAOAdapter * aAdapter ) 
    : iAdapter( aAdapter )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ConstructL
// -----------------------------------------------------------------------------
void CNSmlDmAOSettingStore::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::~CNSmlDmAOSettingStore
// -----------------------------------------------------------------------------
CNSmlDmAOSettingStore::~CNSmlDmAOSettingStore()
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ExecuteCmdL
// Executes a single command
// -----------------------------------------------------------------------------
void CNSmlDmAOSettingStore::ExecuteCmdL( CSmlDmAOCommandElement& aCmd, TUint aLuid)
    {   
    if( !aCmd.Leaf() )
        {
        ExecuteVendorConfigCmdL( aCmd, aLuid );
        }
    else 
        {
        CRepository* cenrep = CRepository::NewL( KCRUidPDPContextManager );
        CleanupStack::PushL( cenrep );
        
        // name
        if ( !aCmd.LastUriSeg()->Compare( KNSmlDmAOAdapterName ) )
            {
            ExecuteNameCmdL( aCmd, cenrep );            
            }
        // awon-pdpc
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmAOAdapterAwonPdpc ) )
            {
            ExecuteAwonPdpcCmdL( aCmd, cenrep );
            } 
        // t-retry
        else if ( !aCmd.LastUriSeg()->Compare( KNSmlDmAOAdapterTRetry ) )
            {
            ExecuteTRetryCmdL( aCmd, cenrep ); 
            } 
        else
            {
            // invalid node
            LOGSTRING( "ExecuteCmdL: Error, Invalid node name" );
            aCmd.SetStatus( CSmlDmAdapter::ENotFound );
            }
                         				    
        aCmd.SetExecuted( ETrue );
        
        CleanupStack::PopAndDestroy( cenrep );    
        }   
    }    

	 
// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::GetVendorConfigsL
// Fetches all VENDORCONFIG ids
// -----------------------------------------------------------------------------
void CNSmlDmAOSettingStore::GetVendorConfigsL( RArray<TUint32>& aLUIDArray )
        {
        // Only one VENDORCONFIG node possible at the moment
        aLUIDArray.Append( KDefaultLuid );
        }
 
// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ExecuteVendorConfigCmdL
// Executes a command for VENDORCONFIG node. Either delete or get, adds are handled 
// in StoreVendorConfigL
// -----------------------------------------------------------------------------
void CNSmlDmAOSettingStore::ExecuteVendorConfigCmdL( CSmlDmAOCommandElement& aCmd, 
                                                     TUint /*aLuid*/ )
    {
    
    if ( aCmd.CmdType() == CNSmlDmAOAdapter::EGetCmd )
        {
        aCmd.SetData( KNSmlDmAOAllLeafNodes().AllocL() );
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else if ( aCmd.CmdType() == CNSmlDmAOAdapter::EDeleteCmd )
        {
        // Should not be possible to delete
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else
        {
        LOGSTRING( "ExecuteVendorConfigCmdL: Error, Invalid cmd type" );
        aCmd.SetStatus( CSmlDmAdapter::EError );
        }
    }


// ---------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ExecuteNameCmdL
// Executes NAME command (add or get )
// ---------------------------------------------------------------------------
//
void CNSmlDmAOSettingStore::ExecuteNameCmdL( CSmlDmAOCommandElement& aCmd, 
                                             CRepository*         /*aCenrep*/ )
    {
    if ( aCmd.CmdType() == CNSmlDmAOAdapter::EAddCmd )
        {
        HBufC* name = HBufC::NewLC( aCmd.Data()->Size() );
        TPtr namePtr = name->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( namePtr, *aCmd.Data() );

        // name is not written to the CentralRepository
        // Add write to CR here

        CleanupStack::PopAndDestroy( name );
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        }        		
    else if ( aCmd.CmdType() == CNSmlDmAOAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
        {
        // name is not written/read to/from the CentralRepository
        // Add read from CR here
        
        //HBufC* data = ...read name
        
        //HBufC8* data8 = HBufC8::NewLC( data->Size() );
        //TPtr8 toPtr = data8->Des();        
        //CnvUtfConverter::ConvertFromUnicodeToUtf8( toPtr, *data );
        //aCmd.SetData( data8 );
        //CleanupStack::Pop( data8 );
        //CleanupStack::PopAndDestroy( data );

        // for size command, set the command data to be the 
        // size of the fetched data
        
        //if( aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
        //    {            
        //    HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
        //    aCmd.SetData( size );
        //    }                
        
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else    
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        LOGSTRING( "ExecuteNameCmdL: Error, Only Add, Get and Get size commands supported" );
        }  
    }
    
// ---------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ExecuteAwonPdpcCmdL
// Executes AWON-PDPC command (add or get )
// ---------------------------------------------------------------------------
//
void CNSmlDmAOSettingStore::ExecuteAwonPdpcCmdL( CSmlDmAOCommandElement& aCmd, 
                                                 CRepository*         aCenrep )
    {
    if ( aCmd.CmdType() == CNSmlDmAOAdapter::EAddCmd )
        {
        HBufC* data = HBufC::NewLC( aCmd.Data()->Size() );
        TPtr dataPtr = data->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( dataPtr, *aCmd.Data() );

        // write to the CentralRepository
        TInt value( 0 );
        TInt hplmn( 0 );
        TInt vplmn( 0 );
        
        ParseIntegerL( dataPtr, value );
        ParseAwonPdpcValuesL( value, hplmn, vplmn );
        
        User::LeaveIfError( aCenrep->Set( KPDPContextManagerEnableWhenHome,
        hplmn ) );
        User::LeaveIfError( aCenrep->Set( KPDPContextManagerEnableWhenRoaming,
        vplmn ) );

        CleanupStack::PopAndDestroy( data );
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        }        		
    else if ( aCmd.CmdType() == CNSmlDmAOAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
        {
        // read from CentralRepository
        TInt hplmn( 0 );
        TInt vplmn( 0 );
        
        User::LeaveIfError(
        aCenrep->Get( KPDPContextManagerEnableWhenHome, hplmn ) );
        User::LeaveIfError(
        aCenrep->Get( KPDPContextManagerEnableWhenRoaming, vplmn ) );
        
        HBufC* data    = HBufC::NewLC( KBufSize );
        TPtr   dataPtr = data->Des();
        
        dataPtr.Zero();
        dataPtr.FillZ();
        dataPtr.AppendNum( hplmn );
        dataPtr.AppendNum( vplmn );
        
        HBufC8* data8 = HBufC8::NewLC( data->Size() );
        TPtr8 toPtr = data8->Des();        
        CnvUtfConverter::ConvertFromUnicodeToUtf8( toPtr, *data );
        aCmd.SetData( data8 );
        CleanupStack::Pop( data8 );
        CleanupStack::PopAndDestroy( data );

        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
            {            
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            aCmd.SetData( size );
            }                
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else    
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        LOGSTRING( "ExecuteNameCmdL: Error, Only Add, Get and Get size commands supported" );
        }  
    }
    
 // ---------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ExecuteTRetryCmdL
// Executes T-RETRY command (add or get )
// ---------------------------------------------------------------------------
//
void CNSmlDmAOSettingStore::ExecuteTRetryCmdL( CSmlDmAOCommandElement& aCmd, 
                                               CRepository*            aCenrep )
    {
    if ( aCmd.CmdType() == CNSmlDmAOAdapter::EAddCmd )
        {
        HBufC* data = HBufC::NewLC( aCmd.Data()->Size() );
        TPtr dataPtr = data->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( dataPtr, *aCmd.Data() );

        // write to the CentralRepository
        TInt value( 0 );
        ParseIntegerL( dataPtr, value );
        
        User::LeaveIfError( aCenrep->Set( KPDPContextManagerRetryTimer,
                            value ) );

        CleanupStack::PopAndDestroy( data );
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        }        		
    else if ( aCmd.CmdType() == CNSmlDmAOAdapter::EGetCmd ||
              aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
        {
        // Read from CentralRepository
        TInt value( 0 );
        User::LeaveIfError( aCenrep->Get( KPDPContextManagerRetryTimer,
                            value ) );
        
        HBufC* data    = HBufC::NewLC( KBufSize );
        TPtr   dataPtr = data->Des();
        
        dataPtr.Zero();
        dataPtr.FillZ();
        dataPtr.AppendNum( value );
        
        HBufC8* data8 = HBufC8::NewLC( data->Size() );
        TPtr8 toPtr = data8->Des();        
        CnvUtfConverter::ConvertFromUnicodeToUtf8( toPtr, *data );
        aCmd.SetData( data8 );
        CleanupStack::Pop( data8 );
        CleanupStack::PopAndDestroy( data );

        // for size command, set the command data to be the 
        // size of the fetched data
        if( aCmd.CmdType() == CNSmlDmAOAdapter::EGetSizeCmd )
            {            
            HBufC8* size = iAdapter->IntToDes8L( aCmd.Data()->Size() );
            aCmd.SetData( size );
            }                
        aCmd.SetStatus( CSmlDmAdapter::EOk );
        aCmd.SetExecuted( ETrue );
        }
    else    
        {
        // unsupported command
        // this is checked by framework
        aCmd.SetStatus( CSmlDmAdapter::EError );
        LOGSTRING( "ExecuteNameCmdL: Error, Only Add, Get and Get size commands supported" );
        }  
    }
    
// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ParseIntegerL
// -----------------------------------------------------------------------------
//
void CNSmlDmAOSettingStore::ParseIntegerL( const TDesC& aPtr, TInt& aInt )
    {
    TLex lex( aPtr );
    User::LeaveIfError( lex.Val( aInt ) );
    }

// -----------------------------------------------------------------------------
// CNSmlDmAOSettingStore::ParseAwonPdpcValuesL
// -----------------------------------------------------------------------------
//
void CNSmlDmAOSettingStore::ParseAwonPdpcValuesL( const TInt aInt,
                                                  TInt& aHplmn,
                                                  TInt& aVplmn )
    {
    switch( aInt )
        {
        case KAwonPdpc_Off3G_Off2G:
            aHplmn = 0;
            aVplmn = 0;
            break;
        case KAwonPdpc_Off3G_On2G:
            aHplmn = 0;
            aVplmn = 1;
            break;
        case KAwonPdpc_On3G_Off2G:
            aHplmn = 1;
            aVplmn = 0;
            break;
        case KAwonPdpc_On3G_On2G:
            aHplmn = 1;
            aVplmn = 1;
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }           

