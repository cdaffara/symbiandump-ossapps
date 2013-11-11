/*
 * ==============================================================================
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
 * Description:
 * 
 * ==============================================================================
 */

#include <e32base.h>
#include <implementationproxy.h> // For TImplementationProxy definition
#include <cmmanagerext.h> 

#include "nsmldebug.h"
#include "ConnMoAdapter.h"
#include "Utils.h"
#include "BranchBase.h"
#include "NapBranch.h" 
#include "ProxyBranch.h"
#include "AddBuffer.h" 
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ConnMoAdapterTraces.h"
#endif

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
// DDF version number
_LIT8( KConnMoAdapterDDFversion,    "1.0");

// URI base for ConnMo Nodes
_LIT8( KConnMoBase,                 "");

// ======== LOCAL FUNCTIONS =========

CBranchBase* CConnMoAdapter::GetBranch( const TDesC8& aUri )
    {
    TDesC8 connMoBase( KConnMoBase );
    CBranchBase* retval = NULL;
        
    TPtrC8 connMoTree = 
        ( CUtils::GetEndOfURIFromSeg( aUri, KConnMoBase )).Mid( connMoBase.Size() );
        
    if ( CUtils::GetFirstURISeg( connMoTree ).Match( KCMDdfNAP )!= KErrNotFound )
        {
        retval = iNapBranch;
        }
    else if ( CUtils::GetFirstURISeg( connMoTree ).Match( KCMDdfProxy )!= KErrNotFound )
        {
        retval = iProxyBranch;
        }
    return retval;
    }

// -------------------------------------------------------------------------------------
// CConnMoAdapter::AddBaseUriL( )
// Handles addition of possible base URI to ConnMO DDF structure
// -------------------------------------------------------------------------------------
MSmlDmDDFObject& CConnMoAdapter::AddBaseUriL( MSmlDmDDFObject& aRootDDF )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_ADDBASEURIL_ENTRY );
    
    TDesC8 base(KConnMoBase);
    
    if ( base.Size() > 0)
        {
    
        // Set rest acceptable operations for data itself     
        TSmlDmAccessTypes aclTypesOnlyGet;
        aclTypesOnlyGet.SetGet();
        
        MSmlDmDDFObject& ddf = aRootDDF.AddChildObjectL( KConnMoBase ); // CMO

        _LIT8( KDescription, "ConnMo DDF description" );
        _LIT8( KDFTitle, "ConnMo Settings Base Node" );
    
        CUtils::FillNodeInfoL( ddf, 
                   aclTypesOnlyGet, 
                   MSmlDmDDFObject::EOne, 
                   MSmlDmDDFObject::EPermanent, 
                   MSmlDmDDFObject::ENode, 
                   KDescription );
                   
        ddf.SetDFTitleL( KDFTitle );
        ddf.SetDefaultValueL( KNullDesC8 );
        OstTraceFunctionExit0( CCONNMOADAPTER_ADDBASEURIL_EXIT );
        return ddf;
        }
    else
        {
        OstTraceFunctionExit0( CCONNMOADAPTER_ADDBASEURIL_EXIT_2 );
        return aRootDDF;
        }    
    
    }
// -------------------------------------------------------------------------------------
// CConnMoAdapter::AddBranchL( )
// Adds dynamic branch to given root node with given name
// -------------------------------------------------------------------------------------
MSmlDmDDFObject& CConnMoAdapter::AddBranchL( MSmlDmDDFObject& aRootDDF, 
                                            const TDesC8& aNodeName )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_ADDBRANCHL_ENTRY, "ENTRY: CConnMoAdapter::AddBranchL;aNodeName=%s", aNodeName );
    
    // Set rest acceptable operations for data itself     
    TSmlDmAccessTypes aclTypesOnlyGet;
    aclTypesOnlyGet.SetGet();

    TSmlDmAccessTypes aclTypesAll;
    aclTypesAll.SetGet();
    aclTypesAll.SetReplace();
    aclTypesAll.SetAdd();
    aclTypesAll.SetDelete();
    
    TSmlDmAccessTypes aclTypesNoDelete;
    aclTypesNoDelete.SetGet();
    aclTypesNoDelete.SetReplace();
    aclTypesNoDelete.SetAdd();
    
    MSmlDmDDFObject& nChildRootDdf = aRootDDF.AddChildObjectL( aNodeName ); // Node
    CUtils::FillNodeInfoL( nChildRootDdf, 
                           aclTypesOnlyGet, 
                           MSmlDmDDFObject::EOne,
                           MSmlDmDDFObject::EPermanent, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );
    
    MSmlDmDDFObject& nChildDdf = nChildRootDdf.AddChildObjectGroupL();  // Node/<X>  
    if ( aNodeName == KCMDdfNAP )
        {      
        CUtils::FillNodeInfoL( nChildDdf, 
                               aclTypesAll, 
                               MSmlDmDDFObject::EZeroOrMore, 
                               MSmlDmDDFObject::EDynamic, 
                               MSmlDmDDFObject::ENode, 
                               KNullDesC8 );
        }
    else
        {  
        CUtils::FillNodeInfoL( nChildDdf, 
                               aclTypesNoDelete, 
                               MSmlDmDDFObject::EZeroOrMore, 
                               MSmlDmDDFObject::EDynamic, 
                               MSmlDmDDFObject::ENode, 
                               KNullDesC8 );
        }
    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_ADDBRANCHL_EXIT, "EXIT: CConnMoAdapter::AddBranchL" );
    return nChildDdf;
    }

// ======== MEMBER FUNCTIONS ========
//-----------------------------------------------------------------------------
// CConnMoAdapter* CConnMoAdapter::NewL( )
//-----------------------------------------------------------------------------
CConnMoAdapter* CConnMoAdapter::NewL( MSmlDmCallback* aDmCallback )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_NEWL_ENTRY );

    CConnMoAdapter* self = CConnMoAdapter::NewLC( aDmCallback );
    CleanupStack::Pop();

    OstTraceFunctionExit1( CCONNMOADAPTER_NEWL_EXIT, ( TUint )( self ) );
    return self;
    }

//-----------------------------------------------------------------------------
// CConnMoAdapter* CConnMoAdapter::NewLC( )
//-----------------------------------------------------------------------------
CConnMoAdapter* CConnMoAdapter::NewLC( MSmlDmCallback* aDmCallback )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_NEWLC_ENTRY );
       
    CConnMoAdapter* self = new ( ELeave ) CConnMoAdapter( aDmCallback );
    CleanupStack::PushL( self );

    self->iCallBack = aDmCallback;
    self->ConstructL();

    OstTraceFunctionExit1( CCONNMOADAPTER_NEWLC_EXIT, ( TUint )( self ) );
    return self;
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::CConnMoAdapter()
//------------------------------------------------------------------------------
CConnMoAdapter::CConnMoAdapter( TAny* aEcomArguments ) 
    : CSmlDmAdapter( aEcomArguments )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_CCONNMOADAPTER_ENTRY );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_CCONNMOADAPTER_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::~CConnMoAdapter()
//------------------------------------------------------------------------------
CConnMoAdapter::~CConnMoAdapter()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_DELETE_ENTRY );
    
    delete iBuffer;
    delete iNapBranch;
    delete iProxyBranch;
    iCmManager.Close();

    OstTraceFunctionExit0( CCONNMOADAPTER_DELETE_EXIT );
    }

//=============================================
//      CConnMoAdapter::ConstructL()
//      Second phase constructor.
//=============================================
void CConnMoAdapter::ConstructL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_CONSTRUCTL_ENTRY );  
    
    iCmManager.OpenL();
       
    iBuffer = CAddBuffer::NewL();
    iNapBranch = CNapBranch::NewL( iCallBack, iBuffer, &iCmManager );
    iProxyBranch = CProxyBranch::NewL( iCallBack, iBuffer, &iCmManager );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_CONSTRUCTL_EXIT );
    }

void CConnMoAdapter::DDFVersionL( CBufBase& aDDFVersion )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_DDFVERSIONL_ENTRY );
    
    aDDFVersion.InsertL( 0, KConnMoAdapterDDFversion );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_DDFVERSIONL_EXIT );
    }

//------------------------------------------------------------------------------
//  void CConnMoAdapter::DDFStructureL()
//------------------------------------------------------------------------------
void CConnMoAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_DDFSTRUCTUREL_ENTRY );
    
    // Insert Base URI in the beginning of the tree
    MSmlDmDDFObject& ddf = AddBaseUriL( aDDF);
    
    // NAP Branch
    MSmlDmDDFObject& nNapDdf = AddBranchL( ddf, KCMDdfNAP );
    iNapBranch->DDFStructureL( nNapDdf );
    
    // Proxy Branch
    MSmlDmDDFObject& nProxyDdf = AddBranchL( ddf, KCMDdfProxy );
    iProxyBranch->DDFStructureL( nProxyDdf );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_DDFSTRUCTUREL_EXIT );
    }
    
void CConnMoAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
                                        const TDesC8& aLUID,
                                        const TDesC8& aObject, 
                                        const TDesC8& aType,
                                        TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_UPDATELEAFOBJECTL_ENTRY, "ENTRY: CConnMoAdapter::UpdateLeafObjectL;aURI=%s", aURI );
    
    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->UpdateLeafObjectL( aURI, aLUID, aObject, aType, aStatusRef );

    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_UPDATELEAFOBJECTL_EXIT, "EXIT: CConnMoAdapter::UpdateLeafObjectL" );
    }

//------------------------------------------------------------------------------
// void CConnMoAdapter::UpdateLeafObjectL
//------------------------------------------------------------------------------
void CConnMoAdapter::UpdateLeafObjectL( const TDesC8& /*aURI*/, 
                                        const TDesC8& /*aLUID*/,
                                        RWriteStream*& /*aStream*/, 
                                        const TDesC8& /*aType*/,
                                        TInt aStatusRef )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_UPDATELEAFOBJECTL_STREAM_ENTRY );
    
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_UPDATELEAFOBJECTL_STREAM_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::UpdateLeafObjectL;Unsupported method call. CSmlDmAdapter::EError." );
   
    OstTraceFunctionExit0( CCONNMOADAPTER_UPDATELEAFOBJECTL_STREAM_EXIT );
    }

//------------------------------------------------------------------------------
// void CConnMoAdapter::DeleteObjectL
//------------------------------------------------------------------------------
void CConnMoAdapter::DeleteObjectL( const TDesC8& aURI, 
                                    const TDesC8& aLUID,
                                    TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_DELETEOBJECTL_ENTRY, "ENTRY: CConnMoAdapter::DeleteObjectL;aURI=%s", aURI );

    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->DeleteObjectL( aURI, aLUID, aStatusRef );
    
    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_DELETEOBJECTL_EXIT, "EXIT: CConnMoAdapter::DeleteObjectL" );
    }

//------------------------------------------------------------------------------
// void CConnMoAdapter::FetchLeafObjectL
//------------------------------------------------------------------------------
void CConnMoAdapter::FetchLeafObjectL( const TDesC8& aURI, 
                                       const TDesC8& aLUID,
                                       const TDesC8& aType, 
                                       TInt aResultsRef,
                                       TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_FETCHLEAFOBJECTL_ENTRY, "ENTRY: CConnMoAdapter::FetchLeafObjectL;aURI=%s", aURI );
    
    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->FetchLeafObjectL( aURI, aLUID, aType, aResultsRef, aStatusRef );
       
    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_FETCHLEAFOBJECTL_EXIT, "EXIT: CConnMoAdapter::FetchLeafObjectL" );
    }

//------------------------------------------------------------------------------
// void CConnMoAdapter::FetchLeafObjectSizeL
//------------------------------------------------------------------------------
void CConnMoAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                           const TDesC8& aLUID,
                                           const TDesC8& aType, 
                                           TInt aResultsRef,
                                           TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_FETCHLEAFOBJECTSIZEL_ENTRY, "ENTRY: CConnMoAdapter::FetchLeafObjectSizeL;aURI=%s", aURI );
    
    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->FetchLeafObjectSizeL( aURI, 
                                          aLUID, 
                                          aType, 
                                          aResultsRef, 
                                          aStatusRef );
    
    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_FETCHLEAFOBJECTSIZEL_EXIT, "EXIT: CConnMoAdapter::FetchLeafObjectSizeL" );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::ChildURIListL
// Return childs of the requested node.
//------------------------------------------------------------------------------
void CConnMoAdapter::ChildURIListL( const TDesC8& aURI, 
                        const TDesC8& aLUID,
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                        TInt aResultsRef,
						TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_CHILDURILISTL_ENTRY, "ENTRY: CConnMoAdapter::ChildURIListL;aURI=%s", aURI );
    
    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->ChildURIListL( aURI, aLUID, aPreviousURISegmentList, 
                                   aResultsRef, aStatusRef );

    
    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_CHILDURILISTL_EXIT, "EXIT: CConnMoAdapter::ChildURIListL" );
    }
    
//------------------------------------------------------------------------------
// CConnMoAdapter::AddNodeObjectL
//------------------------------------------------------------------------------
void CConnMoAdapter::AddNodeObjectL( const TDesC8& aURI, 
                                     const TDesC8& aParentLUID,
                                     TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CCONNMOADAPTER_ADDNODEOBJECTL_ENTRY, "ENTRY: CConnMoAdapter::AddNodeObjectL;aURI=%s", aURI );
    
    CBranchBase* implementation = GetBranch( aURI ); 
    
    implementation->AddNodeObjectL( aURI, aParentLUID, aStatusRef );

    OstTrace0( TRACE_NORMAL, CCONNMOADAPTER_ADDNODEOBJECTL_EXIT, "EXIT: CConnMoAdapter::AddNodeObjectL" );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::ExecuteCommandL
// Not supported. Returns EError.
//------------------------------------------------------------------------------
void CConnMoAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                      const TDesC8& /*aLUID*/,
                                      const TDesC8& /*aArgument*/, 
                                      const TDesC8& /*aType*/,
                                      TInt aStatusRef )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_EXECUTECOMMANDL_ENTRY );
    
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );

    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_EXECUTECOMMANDL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::ExecuteCommandL;Unsupported method call. CSmlDmAdapter::EError." );

    OstTraceFunctionExit0( CCONNMOADAPTER_EXECUTECOMMANDL_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::ExecuteCommandL
// Not supported. Returns EError.
//------------------------------------------------------------------------------
void CConnMoAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, 
                                      const TDesC8& /*aLUID*/,
                                      RWriteStream*& /*aStream*/, 
                                      const TDesC8& /*aType*/,
                                      TInt aStatusRef )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_EXECUTECOMMANDL_STREAM_ENTRY );
    
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
    
    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_EXECUTECOMMANDL_STREAM_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::ExecuteCommandL;Unsupported method call. CSmlDmAdapter::EError." );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_EXECUTECOMMANDL_STREAM_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::CopyCommandL
// Not supported. Returns EError.
//------------------------------------------------------------------------------
void CConnMoAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, 
                                   const TDesC8& /*aTargetLUID*/,
                                   const TDesC8& /*aSourceURI*/, 
                                   const TDesC8& /*aSourceLUID*/,
                                   const TDesC8& /*aType*/, 
                                   TInt aStatusRef )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_COPYCOMMANDL_ENTRY );
    
    iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EError );

    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_COPYCOMMANDL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::CopyCommandL;Unsupported method call. CSmlDmAdapter::EError." );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_COPYCOMMANDL_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::StartAtomicL
// Not supported.
//------------------------------------------------------------------------------
void CConnMoAdapter::StartAtomicL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_STARTATOMICL_ENTRY );
    
    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_STARTATOMICL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::StartAtomicL;Unsupported method call." );

    OstTraceFunctionExit0( CCONNMOADAPTER_STARTATOMICL_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::CommitAtomicL
// Not supported.
//------------------------------------------------------------------------------
void CConnMoAdapter::CommitAtomicL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_COMMITATOMICL_ENTRY );

    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_COMMITATOMICL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::CommitAtomicL;Unsupported method call." );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_COMMITATOMICL_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::RollbackAtomicL
// Not supported.
//------------------------------------------------------------------------------
void CConnMoAdapter::RollbackAtomicL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_ROLLBACKATOMICL_ENTRY );

    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_ROLLBACKATOMICL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::RollbackAtomicL;Unsupported method call." );
    
    OstTraceFunctionExit0( CCONNMOADAPTER_ROLLBACKATOMICL_EXIT );
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::StreamingSupport
// Not supported. Returns EFalse.
//------------------------------------------------------------------------------
TBool CConnMoAdapter::StreamingSupport( TInt& /*aItemSize*/ )
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_STREAMINGSUPPORT_ENTRY );

    
    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_STREAMINGSUPPORT_NOT_SUPPORTED, 
               "CConnMoAdapter::StreamingSupport;Not supported." );
    
    OstTraceFunctionExitExt( CCONNMOADAPTER_STREAMINGSUPPORT_EXIT, this, EFalse );
    return EFalse;
    }

//------------------------------------------------------------------------------
// CConnMoAdapter::StreamCommittedL
// Not supported. Returns EError.
//------------------------------------------------------------------------------
void CConnMoAdapter::StreamCommittedL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_STREAMCOMMITTEDL_ENTRY);
    
    OstTrace0( TRACE_NORMAL, 
               CCONNMOADAPTER_STREAMCOMMMITTEDL_UNSUPPORTED_METHOD_CALL, 
               "CConnMoAdapter::StreamCommittedL;Unsupported method call." );

    OstTraceFunctionExit0( CCONNMOADAPTER_STREAMCOMMITTEDL_EXIT );
    }

void CConnMoAdapter::CompleteOutstandingCmdsL()
    {
    OstTraceFunctionEntry0( CCONNMOADAPTER_COMPLETEOUTSTANDINGCMDSL_ENTRY );
    
    iNapBranch->ProcessBufferL();
    iProxyBranch->ProcessBufferL();
    
    iBuffer->Reset();
    
    OstTraceFunctionExit0( CCONNMOADAPTER_COMPLETEOUTSTANDINGCMDSL_EXIT );
    }

// ======== GLOBAL FUNCTIONS ========
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr )  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KConnMoAdapterUid, CConnMoAdapter::NewL)
    };

//------------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    OstTraceFunctionEntry1( CONNMOADAPTER_IMPLEMENTATIONGROUPPROXY_ENTRY, aTableCount );

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    OstTraceFunctionExit0( CONNMOADAPTER_IMPLEMENTATIONGROUPPROXY_EXIT );
    return ImplementationTable;
    }
