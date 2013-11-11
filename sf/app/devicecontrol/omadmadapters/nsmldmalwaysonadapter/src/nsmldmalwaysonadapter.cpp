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
* Description:  Provides handling of Always-on settings in Device management.
*
*/


#include <ecom/implementationproxy.h>

#include "nsmldmalwaysonadapter.h"
#include "logger.h"

// ======== MEMBER FUNCTIONS ========


// --------------------------------------------------------------------------
// Maps plugin UID to its factory function (constructor)
// --------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x101f6d36, 
                               CNSmlDmAOAdapter::NewL)
    };

// --------------------------------------------------------------------------
// For ecom plugin implementation
// -------------------------------------------------------------------------

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::NewL
// --------------------------------------------------------------------------
CNSmlDmAOAdapter* CNSmlDmAOAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    LOGSTRING("CNSmlDmAOAdapter::NewL: Start");
    
    CNSmlDmAOAdapter* self = new (ELeave) CNSmlDmAOAdapter( aDmCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    
    LOGSTRING("CNSmlDmAOAdapter::NewL: End");
    return self;
    }

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::CNSmlDmAOAdapter
// --------------------------------------------------------------------------

CNSmlDmAOAdapter::CNSmlDmAOAdapter( MSmlDmCallback* aDmCallback ) 
    : CSmlDmAdapter( aDmCallback )
    {
    }

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::ConstructL
// --------------------------------------------------------------------------
void CNSmlDmAOAdapter::ConstructL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::CNSmlDmAOAdapter: Start" );
    
    iSettingStore = CNSmlDmAOSettingStore::NewL( this );
    
    LOGSTRING( "CNSmlDmAOAdapter::CNSmlDmAOAdapter: End" );
    }

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::~CNSmlDmAOAdapter
// --------------------------------------------------------------------------
CNSmlDmAOAdapter::~CNSmlDmAOAdapter()
    {
    LOGSTRING( "CNSmlDmAOAdapter::~CNSmlDmAOAdapter: Start" );    
 
    //iBuffer.ResetAndDestroy();
    //iBuffer.Close();
    delete iSettingStore;
 
    LOGSTRING( "CNSmlDmAOAdapter::~CNSmlDmAOAdapter: End" );
    }
    
// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::DDFVersionL
// Returns ddf version nr
// --------------------------------------------------------------------------
void CNSmlDmAOAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    LOGSTRING( "CNSmlDmAOAdapter::DDFVersionL: Start" );
 
    aDDFVersion.InsertL( 0, KNSmlDmAOAdapterDDFVersion );
 
    LOGSTRING( "CNSmlDmAOAdapter::DDFVersionL:End" );
    }
   
// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::DDFStructureL
//  Constructs the DDF structure
// --------------------------------------------------------------------------
void CNSmlDmAOAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    LOGSTRING( "CNSmlDmAOAdapter::DDFStructureL: Start" );
    TSmlDmAccessTypes accessTypes;

    // VENDORCONFIG
    MSmlDmDDFObject& ao = aDDF.AddChildObjectL( KNSmlDmAOAdapterAO );
    
    accessTypes.SetGet();    
    
    ao.SetAccessTypesL( accessTypes );
    ao.SetOccurenceL( MSmlDmDDFObject::EOne );
    ao.SetScopeL( MSmlDmDDFObject::EPermanent );
    ao.SetDFFormatL( MSmlDmDDFObject::ENode );
    ao.AddDFTypeMimeTypeL( KNSmlDmAOAdapterTextPlain );
    ao.SetDescriptionL( KNSmlDmAOAdapterAODescription );
    accessTypes.Reset();

    // VENDORCONFIG/Name
    MSmlDmDDFObject& name = ao.AddChildObjectL( KNSmlDmAOAdapterName );
    
    accessTypes.SetGet();
    accessTypes.SetReplace();
    accessTypes.SetAdd();
    
    name.SetAccessTypesL( accessTypes );
    name.SetOccurenceL( MSmlDmDDFObject::EOne );
    name.SetScopeL( MSmlDmDDFObject::EDynamic );
    name.SetDFFormatL( MSmlDmDDFObject::EChr );
    name.SetDescriptionL( KNSmlDmAOAdapterNameDescription );
    accessTypes.Reset();

    // VENDORCONFIG/AWON-PDPC
    MSmlDmDDFObject& awonPdpc = 
                          ao.AddChildObjectL( KNSmlDmAOAdapterAwonPdpc );
    
    accessTypes.SetGet();
    accessTypes.SetReplace();
    accessTypes.SetAdd();
    
    awonPdpc.SetAccessTypesL( accessTypes );
    awonPdpc.SetOccurenceL( MSmlDmDDFObject::EOne );
    awonPdpc.SetScopeL( MSmlDmDDFObject::EDynamic );
    awonPdpc.SetDFFormatL( MSmlDmDDFObject::EChr );
    awonPdpc.SetDescriptionL( KNSmlDmAOAdapterAwonPdpcDescription );
    accessTypes.Reset();
        
    // VENDORCONFIG/T-RETRY
    MSmlDmDDFObject& tRetry = 
                          ao.AddChildObjectL( KNSmlDmAOAdapterTRetry );
    
    accessTypes.SetGet();
    accessTypes.SetReplace();
    accessTypes.SetAdd();
    
    tRetry.SetAccessTypesL( accessTypes );
    tRetry.SetOccurenceL( MSmlDmDDFObject::EOne );
    tRetry.SetScopeL( MSmlDmDDFObject::EDynamic );
    tRetry.SetDFFormatL( MSmlDmDDFObject::EChr );
    tRetry.SetDescriptionL( KNSmlDmAOAdapterTRetryDescription );
    accessTypes.Reset();
    
    LOGSTRING( "CNSmlDmAOAdapter::DDFStructureL: End" );    
    }

// -------------------------------------------------------------------------
// CNSmlDmAOAdapter::UpdateLeafObjectL
// Adds or updates leaf node value. 
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                          const TDesC8& aLUID, 
                                          const TDesC8& aObject, 
                                          const TDesC8& /*aType*/, 
                                          TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::UpdateLeafObjectL: Start" );
    LOGSTRING4( "\tUpdateLeafObjectL  \tURI: %S, \n\tLUID: %S,\
                         \tobject: %S ", 
                        &aURI, &aLUID, &aObject );
        
	// execute command and return status  
    CSmlDmAOCommandElement* cmd = 
    CSmlDmAOCommandElement::NewLC( ETrue, 
                                   aStatusRef,
                                   KNSmlDmAOInvalidRef, 
                                   CNSmlDmAOAdapter::EAddCmd,
                                   LastURISeg( aURI ), 
                                   aObject);
    
    TInt luid( KDefaultLuid );
    
    if ( aLUID.Length() > 0 )
        {
        luid = DesToIntL( aLUID );    
        }
                                                                                                     
    iSettingStore->ExecuteCmdL( *cmd, luid );    
    
    LOGSTRING2( "\tCmd executed with status: %d", 
                          cmd->Status() );
    Callback().SetStatusL( aStatusRef, cmd->Status() );     
    CleanupStack::PopAndDestroy( cmd );                
        
    LOGSTRING( "CNSmlDmAOAdapter::UpdateLeafObjectL: End" );    
    }
// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::DeleteObjectL
// Not supported
// --------------------------------------------------------------------------

void CNSmlDmAOAdapter::DeleteObjectL( const TDesC8& /*aURI*/, 
                                      const TDesC8& /*aLUID*/,
						              TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::DeleteObjectL: Start" );
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError); 
    LOGSTRING( "CNSmlDmAOAdapter::DeleteObjectL: End" );
    }
    
// -------------------------------------------------------------------------
// CNSmlDmAOAdapter::FetchLeafObjectL
// Fetches leaf object value.
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                         const TDesC8& aLUID, 
                                         const TDesC8& /*aType*/, 
                                         const TInt aResultsRef, 
                                         const TInt aStatusRef )
    {
    LOGSTRING( "NSmlDmAOAdapter::FetchLeafObjectL: Start" );
    LOGSTRING3( "\tFetchLeafObjectL  \tURI: %S, \tLUID: %S,", 
                        &aURI, &aLUID );
    
    CSmlDmAOCommandElement* cmd = 
    CSmlDmAOCommandElement::NewLC( ETrue, 
                                   aStatusRef, 
                                   aResultsRef, 
                                   CNSmlDmAOAdapter::EGetCmd,
                                   LastURISeg( aURI ), 
                                   KNullDesC8);

    TInt luid( KDefaultLuid );
    
    if ( aLUID.Length() > 0 )
        {
        luid = DesToIntL( aLUID );    
        }
        
    iSettingStore->ExecuteCmdL( *cmd, luid );
    LOGSTRING2( "\tCmd executed with status: %d ", 
                          cmd->Status() );

    // if executed return status
    if ( cmd->Executed() ) 
        {
        Callback().SetStatusL( aStatusRef, cmd->Status() );            
        // if successful set results
        if ( cmd->Status() == CSmlDmAdapter::EOk )
            {
            LOGSTRING2( "\tCmd executed with result: %S ", 
                                  cmd->Data() );
            CBufBase* result = CBufFlat::NewL( cmd->Data()->Size() );
            CleanupStack::PushL( result );
            result->InsertL( 0, *cmd->Data() );
            Callback().SetResultsL( aResultsRef, *result, KNullDesC8 );
            CleanupStack::PopAndDestroy( result );
            }
        }
    else
        {
        // failed to execute command
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
        }
    CleanupStack::PopAndDestroy( cmd );
    
    
    LOGSTRING( "NSmlDmAOAdapter::FetchLeafObjectL: End" );
    }

// --------------------------------------------------------------------------
//  CNSmlDmAOAdapter::FetchLeafObjectSizeL
//  Fetches leaf object size.
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                             const TDesC8& aLUID,
									         const TDesC8& /* aType */, 
									         TInt aResultsRef,
									         TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::FetchLeafObjectSizeL: Start" );
    LOGSTRING3( "\tFetchLeafObjectSizeL  \tURI: %S, \tLUID: %S,", 
                         &aURI, &aLUID );
   
    CSmlDmAOCommandElement* cmd = 
    CSmlDmAOCommandElement::NewLC( ETrue, 
                                   aStatusRef, 
                                   aResultsRef, 
                                   CNSmlDmAOAdapter::EGetSizeCmd,
                                   LastURISeg( aURI ), 
                                   KNullDesC8);
                                   
    TInt luid( KDefaultLuid );
    
    if ( aLUID.Length() > 0 )
        {
        luid = DesToIntL( aLUID );    
        }
                                       
    iSettingStore->ExecuteCmdL( *cmd, luid );
    
    LOGSTRING2( "\tCmd executed with status: %d ", 
                          cmd->Status() );
    // if executed get status
    if ( cmd->Executed() ) 
        {
        Callback().SetStatusL( aStatusRef, cmd->Status() );            
        // if successful get results
        if ( cmd->Status() == CSmlDmAdapter::EOk )
            {
            LOGSTRING2( "\tCmd executed with result: %S ", 
                                  cmd->Data() );
            CBufBase* result = CBufFlat::NewL( cmd->Data()->Size() );
            CleanupStack::PushL( result );
            result->InsertL( 0, *cmd->Data() );
            Callback().SetResultsL( aResultsRef, *result, KNullDesC8 );
            CleanupStack::PopAndDestroy( result );
            }
        }
    else
        {
        // failed to execute command
        Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );
        }
    
    CleanupStack::PopAndDestroy( cmd );
        
    LOGSTRING( "CNSmlDmAOAdapter::FetchLeafObjectSizeL: End" );
    }

// ------------------------------------------------------------------------
//  CNSmlDmAOAdapter::ChildURIListL
//  Fetches child nodes of a node. these may be either all VENDORCONFIG 
//  nodes or leaf nodes under a VENDORCONFIG node. 
// ------------------------------------------------------------------------
void CNSmlDmAOAdapter::ChildURIListL( const TDesC8& aURI, 
                                      const TDesC8& aLUID,
					                  const CArrayFix<TSmlDmMappingInfo>& 
					                  /*aPreviousURISegmentList*/,
					                  TInt aResultsRef, 
					                  TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::ChildURIListL: Start" );
    LOGSTRING3( "\tChildURIListL  URI: %S, LUID: %S ", 
                        &aURI, &aLUID );
    
    CBufBase* resultList = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( resultList );
	
	// get all leaf nodes below VENDORCONFIG node
    if ( !aURI.Compare( KNSmlDmAOAdapterAO ) )
        {        
        CSmlDmAOCommandElement* cmd = 
        CSmlDmAOCommandElement::NewLC( EFalse, 
                                       aStatusRef,
                                       aResultsRef, 
                                       CNSmlDmAOAdapter::EGetCmd, 
                                       KNullDesC8, 
                                       KNullDesC8 );
                                       
        TInt luid( KDefaultLuid );
    
        if ( aLUID.Length() > 0 )
            {
            luid = DesToIntL( aLUID );    
            }
                                           
        iSettingStore->ExecuteCmdL( *cmd, luid );
        
        LOGSTRING2( "\tCmd executed with status: %d ", 
                              cmd->Status() );
   
        Callback().SetStatusL( aStatusRef, cmd->Status() );
        if( cmd->Status() == CSmlDmAdapter::EOk )
            {
            resultList->InsertL( 0, *cmd->Data() );
            Callback().SetResultsL( aResultsRef, 
                                    *resultList, 
                                    KNullDesC8 );
            } 
   
        CleanupStack::PopAndDestroy( cmd );               
        }        
   
    CleanupStack::PopAndDestroy( resultList );
    LOGSTRING( "CNSmlDmAOAdapter::ChildURIListL: End" );
    }
 
// --------------------------------------------------------------------------
//  CNSmlDmAOAdapter::AddNodeObjectL
//  Not supported
// --------------------------------------------------------------------------
void CNSmlDmAOAdapter::AddNodeObjectL( const TDesC8& /*aURI*/, 
                                       const TDesC8& /*aParentLUID*/,
								       TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::AddNodeObjectL: Start" );
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    LOGSTRING( "CNSmlDmAOAdapter::AddNodeObjectL: End" );
    }

// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::UpdateLeafObjectL
//  Not supported from stream
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::UpdateLeafObjectL( const TDesC8& /* aURI */, 
                                          const TDesC8& /* aLUID */, 
                                          RWriteStream*& /* aStream */, 
                                          const TDesC8& /* aType */, 
                                          TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::UpdateLeafObjectL: Start" );
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);    
    LOGSTRING( "CNSmlDmAOAdapter::UpdateLeafObjectL: End" );
    }



// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::ExecuteCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                        const TDesC8& /* aLUID */, 
                                        const TDesC8& /* aArgument */, 
                                        const TDesC8& /* aType */, 
                                        TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::ExecuteCommandL: Start" );
    Callback().SetStatusL(aStatusRef, CSmlDmAdapter::EError);    
    LOGSTRING( "CNSmlDmAOAdapter::ExecuteCommandL: End" );
    }
// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::ExecuteCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::ExecuteCommandL( const TDesC8& /* aURI */, 
                                        const TDesC8& /* aLUID */, 
                                        RWriteStream*& /* aStream */, 
                                        const TDesC8& /* aType */, 
                                        TInt aStatusRef )
    {
    LOGSTRING( "CNSmlDmAOAdapter::ExecuteCommandL: Start" );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );    
    LOGSTRING( "CNSmlDmAOAdapter::ExecuteCommandL: End" );
    }
 
// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::CopyCommandL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::CopyCommandL( const TDesC8& /* aTargetURI */ , 
                                     const TDesC8& /* aTargetLUID */ , 
                                     const TDesC8& /* aSourceURI */ , 
                                     const TDesC8& /* aSourceLUID */, 
                                     const TDesC8& /* aType */ , 
                                     TInt aStatusRef )
    {
    //not supported
    LOGSTRING( "CNSmlDmAOAdapter::CopyCommandL: Sart" );
    Callback().SetStatusL( aStatusRef, CSmlDmAdapter::EError );    
    LOGSTRING( "CNSmlDmAOAdapter::CopyCommandL: End" );
    }
// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::StartAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::StartAtomicL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::StartAtomicL: Start" );
    LOGSTRING( "CNSmlDmAOAdapter::StartAtomicL: End" );
    }
// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::CommitAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::CommitAtomicL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::CommitAtomicL: Start" );
    LOGSTRING( "CNSmlDmAOAdapter::CommitAtomicL: End" );
    }

// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::RollbackAtomicL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::RollbackAtomicL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::RollbackAtomicL: Start" );
    LOGSTRING( "CNSmlDmAOAdapter::RollbackAtomicL: End" );
    }

TBool CNSmlDmAOAdapter::StreamingSupport( TInt& /* aItemSize */ )
    {
    LOGSTRING( "CNSmlDmAOAdapter::StreamingSupport: Start" );
    LOGSTRING( "CNSmlDmAOAdapter::StreamingSupport: End" );
    return EFalse;
    }
// -------------------------------------------------------------------------
//  CNSmlDmAOAdapter::StreamCommittedL
//  Not supported
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::StreamCommittedL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::StreamCommittedL: Start" );
    LOGSTRING( "CNSmlDmAOAdapter::StreamCommittedL: End" );
    }

// -------------------------------------------------------------------------
// CNSmlDmAOAdapter::CompleteOutstandingCmdsL
// Buffering not needed because nodes can't be added
// -------------------------------------------------------------------------
void CNSmlDmAOAdapter::CompleteOutstandingCmdsL()
    {
    LOGSTRING( "CNSmlDmAOAdapter::CompleteOutstandingCmdsL: Start");
    LOGSTRING( "CNSmlDmAOAdapter::CompleteOutstandingCmdsL: End" );    
    }

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::IntToDes8L
// Converts integer to 8bit descriptor
// --------------------------------------------------------------------------
HBufC8* CNSmlDmAOAdapter::IntToDes8L( TInt aLuid ) const
    {
    HBufC8* buf = HBufC8::NewL( 10 ); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num( aLuid );
    return buf;
    }
    
// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::DesToIntL
// Converts 8bit descriptor to int
// --------------------------------------------------------------------------
TUint CNSmlDmAOAdapter::DesToIntL( const TDesC8& aLuid ) const
    {
    TLex8 lex( aLuid );
    TUint value( 0 );
    User::LeaveIfError( lex.Val( value ) );
    return value;
    }        

// --------------------------------------------------------------------------
// CNSmlDmAOAdapter::LastURISeg
// Returns the last uri segemnt of a uri. 
// --------------------------------------------------------------------------
TPtrC8 CNSmlDmAOAdapter::LastURISeg( const TDesC8& aURI ) const
    {
    TInt i;
    for ( i=aURI.Length()-1; i >= 0; i-- )
        {
        if ( aURI[i]=='/' )
            {
            break;
            }
        }
        
    if ( i==0 )
        {
        return aURI;
        }
    else
        {
        return aURI.Mid( i + 1 );
        }
    }


//---------------------- CSmlDmAOCommandElement -------------------------//

// --------------------------------------------------------------------------
// CSmlDmAOCommandElement* CSmlDmAOCommandElement::NewLC
// --------------------------------------------------------------------------

CSmlDmAOCommandElement* CSmlDmAOCommandElement::NewLC( TBool aLeaf,
                                                       TInt aStatusRef, 
                                                       TInt aResultRef, 
                                                       CNSmlDmAOAdapter::TCommandType aCmdType,
                                                       const TDesC8& aLastUriSeg, 
                                                       const TDesC8& aData )
    {
    CSmlDmAOCommandElement* self = new (ELeave) CSmlDmAOCommandElement( aLeaf, 
                                                                        aStatusRef, 
                                                                        aResultRef, 
                                                                        aCmdType );
    CleanupStack::PushL(self);
    self->ConstructL( aLastUriSeg, aData );
    return self;
    }    
    
// --------------------------------------------------------------------------
// CSmlDmAOCommandElement::~CSmlDmAOCommandElement
// --------------------------------------------------------------------------
CSmlDmAOCommandElement::~CSmlDmAOCommandElement()
    {
    delete iLastUriSeg;
    delete iData;
    }
    
// --------------------------------------------------------------------------
// CSmlDmAOCommandElement::CSmlDmAOCommandElement
// --------------------------------------------------------------------------
CSmlDmAOCommandElement::CSmlDmAOCommandElement( TBool aLeaf,
                                                TInt aStatusRef, 
                                                TInt aResultRef, 
                                                CNSmlDmAOAdapter::TCommandType  aCmdType )
                                                : iStatus( CSmlDmAdapter::EError ),
                                                iLeaf( aLeaf ),
                                                iStatusRef( aStatusRef ),
                                                iResultRef( aResultRef ),
                                                iCmdType( aCmdType ),
                                                iData( NULL )
                                                
    {
    }

// --------------------------------------------------------------------------
// CSmlDmAOCommandElement::ConstructL
// --------------------------------------------------------------------------
void CSmlDmAOCommandElement::ConstructL( const TDesC8& aLastUriSeg, 
                                         const TDesC8& aData )
    {
    iLastUriSeg = aLastUriSeg.AllocL();    
    iData = aData.AllocL();
    }
