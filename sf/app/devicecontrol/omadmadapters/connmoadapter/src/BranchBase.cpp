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

#include <smldmadapter.h>
#include <cdbcols.h>
#include <cmpluginpacketdatadef.h>
#include "BranchBase.h"

#include "Utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "BranchBaseTraces.h"
#endif

_LIT8( KCMProxyNodeSeparator,   "/" );



// ======== LOCAL FUNCTIONS =========

// ======== MEMBER FUNCTIONS ========
// -------------------------------------------------------------------------------------
// CBranchBase::CBranchBase()
// -------------------------------------------------------------------------------------
CBranchBase::CBranchBase()
    {
    OstTraceFunctionEntry0( CBRANCHBASE_CBRANCHBASE_ENTRY );
    
    OstTraceFunctionExit0( CBRANCHBASE_CBRANCHBASE_EXIT );
    }
// -------------------------------------------------------------------------------------
// CBranchBase::CBranchBase
// -------------------------------------------------------------------------------------
CBranchBase::CBranchBase( MSmlDmCallback* aDmCallback )

    {
    OstTraceFunctionEntry0( CBRANCHBASE_CBRANCHBASE_2_ENTRY );
    iCallback = aDmCallback;
    OstTraceFunctionExit0( DUP1_CBRANCHBASE_CBRANCHBASE_2_EXIT );
    }
// -------------------------------------------------------------------------------------
// CBranchBase::~CBranchBase()
// -------------------------------------------------------------------------------------
CBranchBase::~CBranchBase()
    {
    OstTraceFunctionEntry0( CBRANCHBASE_DELETE_ENTRY );
    
    OstTraceFunctionExit0( CBRANCHBASE_DELETE_EXIT );
    }

// -------------------------------------------------------------------------------------
// CBranchBase::GetAllConnectionMethodsL()
// Returns all connection methods
// -------------------------------------------------------------------------------------
void CBranchBase::GetAllConnectionMethodsL( RArray<TUint32>& aCmArray,
                                           RCmManagerExt& aCmManager )
    {
    OstTraceFunctionEntry0( CBRANCHBASE_GETALLCONNECTIONMETHODSL_ENTRY );
    
    // Get all connection methods
    TBool supportedBearersOnly = EFalse;
    TBool legacyCmsOnly = EFalse;
    TBool includeEasyWlan = EFalse;
        
    aCmManager.ConnectionMethodL( aCmArray, 
                                 supportedBearersOnly,
                                 legacyCmsOnly,
                                 includeEasyWlan );
    
    RCmConnectionMethodExt cm;
    
    // Loop all connection methods and filter away everything else besides packet data.
    // Other bearer types are not supported
    for( TInt i = 0; i < aCmArray.Count(); i++ )
        {
        if ( GetConnectionMethodLC( aCmManager, cm, aCmArray[i] ) )
            {
            TUint bearerType = cm.GetIntAttributeL( CMManager::ECmBearerType );                      
            
            if( bearerType != KUidPacketDataBearerType )
                {
                // Non supported bearer type. Remove
                aCmArray.Remove( i );
                i--;              
                }            
            }        
        CleanupStack::PopAndDestroy( &cm );
        }
    OstTrace1( TRACE_NORMAL, CBRANCHBASE_GETALLCONNECTIONMETHODSL_COUNT, "CBranchBase::GetAllConnectionMethodsL; Found %u connection methods.", aCmArray.Count() );
    
    OstTraceFunctionExit0( CBRANCHBASE_GETALLCONNECTIONMETHODSL_EXIT );
    }

//-----------------------------------------------------------------------------
// TBool CBranchBase::CheckPreviousURIList()
//-----------------------------------------------------------------------------
TBool CBranchBase::CheckPreviousURIList( const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                         const TUint32 aCmId,
                                         TDes8& aURISeg )
    {
    OstTraceExt2( TRACE_NORMAL, CBRANCHBASE_CHECKPREVIOUSURILIST_ENTRY, "ENTRY: CBranchBase::CheckPreviousURIList;aURISeg=%s;aCmId=%u", aURISeg, aCmId );
    
    TBool retval = EFalse;
    
    for ( TInt i = 0; i < aPreviousURISegmentList.Count(); i++ )
        {        
        if ( CUtils::IntLUID( aPreviousURISegmentList.At(i).iURISegLUID ) == aCmId )
            {
            aURISeg.Copy( aPreviousURISegmentList.At(i).iURISeg );
            retval = ETrue;
            break;
            }
        }
    OstTrace1( TRACE_NORMAL, CBRANCHBASE_CHECKPREVIOUSURILIST_EXIT, "EXIT: CBranchBase::CheckPreviousURIList;retval=%d", retval );
    
    return retval;
    }


//-----------------------------------------------------------------------------
// CBranchBase::GetAllDynamicNodesL
//-----------------------------------------------------------------------------
void CBranchBase::GetAllDynamicNodesL( const TDesC8& aURI, 
                                       const TDesC8& /*aLUID*/,
                                       const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                       TInt aResultsRef,
                                       TInt aStatusRef,
                                       RCmManagerExt& /*aCmManagerExt*/,
                                       CBufBase& aCurrentURISegmentList,
                                       const TDesC8& aField,
                                       const TInt aNodeCount,
                                       MSmlDmCallback* aCallback )
    {
    OstTraceExt3( TRACE_NORMAL, 
                  CBRANCHBASE_GETALLDYNAMICNODESL_ENTRY, 
                  "ENTRY: CBranchBase::GetAllDynamicNodesL;aURI=%s;aField=%s;aNodeCount=%d", 
                  aURI, 
                  aField, 
                  aNodeCount );
    
    if ( aPreviousURISegmentList.Count() > 0 ) // Already mapped
        {
        for ( TInt i = 0; i < aPreviousURISegmentList.Count(); i++ )
            {
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), 
                                            aPreviousURISegmentList.At(i).iURISeg );
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), 
                                            KCMProxyNodeSeparator );                
            }
        }
    else
        {
        TBuf8<KSmlMaxURISegLen> addNodeName;
   
        // Generate as many nodes as defined
        for ( TInt i = 1; i <= aNodeCount; i++ )
            {
            addNodeName.Format( _L8( "%s%d" ), aField.Ptr(), i );
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), addNodeName );
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), 
                                            KCMProxyNodeSeparator );
            }            
       }
    
    aCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    aCallback->SetResultsL( aResultsRef, aCurrentURISegmentList, KNullDesC8);
    
    OstTrace0( TRACE_NORMAL, 
               CBRANCHBASE_GETALLDYNAMICNODESL_EXIT, 
               "EXIT: CBranchBase::GetAllDynamicNodesL" );
       
    }

//------------------------------------------------------------------------------
//  TBool CBranchBase::GetConnectionMethodLC()
//------------------------------------------------------------------------------ 
TBool CBranchBase::GetConnectionMethodLC( RCmManagerExt& aCmManagerExt,
                                          RCmConnectionMethodExt& aCm,
                                          TUint32 aCmId )
    {
    OstTrace1( TRACE_NORMAL, CBRANCHBASE_GETCONNECTIONMETHODLC_ENTRY, "ENTRY: CBranchBase::GetConnectionMethodLC;aCmId=%d", aCmId );
    TInt error( KErrNone );
    
    TRAP( error, aCm = aCmManagerExt.ConnectionMethodL( aCmId ) );
    
    CleanupClosePushL( aCm );
    
    if ( error != KErrNone )
        {
        OstTrace0( TRACE_NORMAL, 
                   CBRANCHBASE_GETCONNECTIONMETHODLC_EXIT_NOT_FOUND, 
                   "EXIT: CBranchBase::GetConnectionMethodLC;Connection Method NOT found: EFalse." );
        return EFalse;
        }
    
    OstTrace0( TRACE_NORMAL, 
               CBRANCHBASE_GETCONNECTIONMETHODLC_EXIT_FOUND, 
               "EXIT: CBranchBase::GetConnectionMethodLC;Connection Method FOUND: ETrue." );
    
    return ETrue;
    }

// ======== GLOBAL FUNCTIONS ========

