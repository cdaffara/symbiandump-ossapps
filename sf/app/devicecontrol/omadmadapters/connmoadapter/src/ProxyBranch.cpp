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
#include <cmconnectionmethoddef.h>
#include <nsmldmtreedbclient.h>

#include "ProxyBranch.h"
#include "Utils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ProxyBranchTraces.h"
#endif

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT8( KCMProxyDdf,             "Proxy" );
_LIT8( KCMProxyDdfProxyId,      "ProxyId" );
_LIT8( KCMProxyDdfName,         "Name" );
_LIT8( KCMProxyDdfAddr,         "Addr" );
_LIT8( KCMProxyDdfToConRef,     "ToConRef" );
_LIT8( KCMProxyDdfConRef,       "ConRef" );
_LIT8( KCMProxyDdfPorts,        "Ports" );
_LIT8( KCMProxyDdfPortNbr,      "PortNbr" );
_LIT8( KCMProxyDdfProxyParams,  "ProxyParams" );
_LIT8( KCMProxyDdfProxyType,    "ProxyType" );
_LIT8( KCMProxyDdfWap,          "WAP" );
_LIT8( KCMProxyDdfStartpg,      "Startpg" );

_LIT8( KCMProxyFields,          "ProxyId/Addr/ToConRef/Ports/ProxyType/ProxyParams" );
_LIT8( KCMProxyParamsFields,    "WAP" );
_LIT8( KCMProxyWapFields,       "Startpg" );
_LIT8( KCMProxyNodeSeparator,   "/" );

_LIT8( KDynamicProxyNodePattern,            "Proxy/*" );
_LIT8( KDynamicProxyNodeChildUri,           "Proxy/*/*" );
_LIT8( KDynamicToConrefNode,                "Proxy/*/ToConRef" ); 
_LIT8( KDynamicToConrefNodeChildUri,        "Proxy/*/ToConRef/*" ); 
_LIT8( KDynamicToConrefNodeChildUriNodes,   "Proxy/*/ToConRef/*/*" );
_LIT8( KDynamicPortsNode,                   "Proxy/*/Ports" );
_LIT8( KDynamicPortsNodeChildUri,           "Proxy/*/Ports/*" );
_LIT8( KDynamicPortsNodeChildUriNodes,      "Proxy/*/Ports/*/*" );
_LIT8( KDynamicProxyParamsNode,             "Proxy/*/ProxyParams" );
_LIT8( KDynamicProxyParamsNodeChildUri,     "Proxy/*/ProxyParams/*" );
_LIT8( KDynamicProxyParamsWapNode,          "Proxy/*/ProxyParams/WAP" );

_LIT8( KProxyAddrUri,                     "Proxy/*/Addr");
_LIT8( KProxyConrefUri,                   "Proxy/*/ConRef");

_LIT8( KRoot,                             "./");

TInt const KBufferExpandSize = 128;
TInt const KNumOfProxyNodes = 8;

_LIT8( KDynamicNodePrefix,                  "ProxyId");

// ======== LOCAL FUNCTIONS =========

//-----------------------------------------------------------------------------
// CSmlDmAdapter::TError CProxyBranch::FetchLeafObjectL()
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CProxyBranch::FetchLeafObjectL( const TDesC8& aURI,
                                                      const TDesC8& aLUID,
                                                      CBufBase& aLeafObject )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTL_ENTRY, 
                  "ENTRY: CProxyBranch::FetchLeafObjectL;aURI=%s", aURI );
    
    RCmConnectionMethodExt cm;    
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotAllowed;
    TPtrC8 leafNode = CUtils::GetLastURISeg( aURI );
    TInt cmId( KErrNotFound );
    if ( aLUID.Length() > 0 )
         {
         cmId = CUtils::IntLUID( aLUID );         
         }
     else
         {
         // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
         cmId = GetCmIdFromURIL( aURI );
         }
    
    if ( cmId == KErrNotFound || !GetConnectionMethodLC( *iCmManager, cm, cmId ) || 
            !cm.GetBoolAttributeL(CMManager::ECmProxyUsageEnabled) )
        {
        if ( cmId != KErrNotFound )
            {
            CleanupStack::PopAndDestroy( &cm );
            }
        status = CSmlDmAdapter::ENotFound;
        
        OstTrace1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTL_NOT_FOUND, 
                   "CProxyBranch::FetchLeafObjectL;status=%{TError}", status );
        
        return status;
        }
    
    if ( leafNode.Compare( KCMProxyDdfAddr ) == 0 )
        {
        HBufC* serverName = cm.GetStringAttributeL( CMManager::ECmProxyServerName );
        
        CleanupStack::PushL( serverName );
        aLeafObject.InsertL( 0, CUtils::ConvertTo8LC( *serverName ) );
        
        CleanupStack::PopAndDestroy(); // CUtils::ConvertTo8LC
        CleanupStack::PopAndDestroy( serverName );
        
        status = CSmlDmAdapter::EOk;
        }
    else if ( leafNode.Compare( KCMProxyDdfConRef ) == 0 )
        {
        TBuf8<KBufferExpandSize> conRef;
        TBuf8<KBufferExpandSize> napChildList;
        CBufBase* napChildListBuf = CBufFlat::NewL( KSmlMaxURISegLen );
        CleanupStack::PushL( napChildListBuf );      
        
        // Get all NAP node childs
        iCallback->FetchLinkL( _L8("./NAP"), *napChildListBuf,  status );
        napChildListBuf->Read( 0, napChildList, napChildListBuf->Size() );
        
        if ( status == CSmlDmAdapter::EOk)
            {
            // Reset operation end status 
            status = CSmlDmAdapter::EError;
            
            // Find correct Uri
            while ( napChildList.Length() > 0 && status != CSmlDmAdapter::EOk )
                {
                conRef.Append( _L("NAP/") );
                CUtils::AppendFirstURISeg( napChildList, conRef );
                HBufC8* napLuid = iCallback->GetLuidAllocL( conRef );
                if ( cmId == CUtils::IntLUID( *napLuid ) )
                    {
                    status = CSmlDmAdapter::EOk;
                    aLeafObject.InsertL( 0, conRef );
                    }
                delete napLuid;
                conRef.Delete( 0, conRef.Length() );
                }
            }
        
        CleanupStack::PopAndDestroy( napChildListBuf );
        }
    else if ( leafNode.Compare( KCMProxyDdfPortNbr ) == 0 )
        {
        TInt proxyPort = cm.GetIntAttributeL( CMManager::ECmProxyPortNumber );
                        
        aLeafObject.InsertL( 0, CUtils::IntToCharL( proxyPort ) );
        CleanupStack::PopAndDestroy();
                
        status = CSmlDmAdapter::EOk;
        }
    else if ( leafNode.Compare( KCMProxyDdfStartpg ) == 0 )
        {
        HBufC* startPage = cm.GetStringAttributeL( CMManager::ECmStartPage );
        
        CleanupStack::PushL( startPage );
        aLeafObject.InsertL( 0, CUtils::ConvertTo8LC( *startPage ) );
        
        CleanupStack::PopAndDestroy(); // CUtils::ConvertTo8LC
        CleanupStack::PopAndDestroy( startPage );
        
        status = CSmlDmAdapter::EOk;
        }
    else if ( leafNode.Compare( KCMProxyDdfProxyType ) == 0 )
        {
        HBufC* protocolName = cm.GetStringAttributeL( CMManager::ECmProxyProtocolName );
        
        CleanupStack::PushL( protocolName );
        aLeafObject.InsertL( 0, CUtils::ConvertTo8LC( *protocolName ) );
        
        CleanupStack::PopAndDestroy(); // CUtils::ConvertTo8LC
        CleanupStack::PopAndDestroy( protocolName );
        
        status = CSmlDmAdapter::EOk;
        }
    else if ( leafNode.Compare( KCMProxyDdfProxyId ) == 0 )
        {
        TPtrC8 dynamicNode = GetDynamicProxyNodeUri( aURI );
        aLeafObject.InsertL( 0, dynamicNode );
        status = CSmlDmAdapter::EOk;
        }

    CleanupStack::PopAndDestroy( &cm );
    
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTL_EXIT, 
               "EXIT: CProxyBranch::FetchLeafObjectL;status=%{TError}", status );
    return status;
    }

//-----------------------------------------------------------------------------
// TUint32 CProxyBranch::CheckForUnmodifiableField()
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CProxyBranch::CheckForUnmodifiableField(const TDesC8& aUri)
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_CHECKFORUNMODIFIABLEFIELD_ENTRY, 
                  "ENTRY: CProxyBranch::CheckForUnmodifiableField;aUri=%s", aUri );
    
    CSmlDmAdapter::TError retval(CSmlDmAdapter::ENotFound);
    TPtrC8 nodeName = CUtils::GetLastURISeg(aUri);
       
    if ( nodeName.Match(KCMProxyDdfName) != KErrNotFound ||
         nodeName.Match(KCMProxyDdfConRef) != KErrNotFound  )
        {
        retval = CSmlDmAdapter::EOk;
        }
    
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_CHECKFORUNMODIFIABLEFIELD_EXIT, 
               "EXIT: CProxyBranch::CheckForUnmodifiableField;retval=%{TError}", retval );
    
    return retval;
    }
//-----------------------------------------------------------------------------
// TBool CProxyBranch::MapUriToCmAttribute()
//-----------------------------------------------------------------------------
TUint32 CProxyBranch::MapUriToCmAttribute( const TDesC8& aUri )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_MAPURITOCMATTRIBUTE_ENTRY, 
                  "ENTRY: CProxyBranch::MapUriToCmAttribute;aUri=%s", aUri );
    
    TUint32 retval(0);
    
    TPtrC8 nodeName = CUtils::GetLastURISeg( aUri );
    
    if ( nodeName.Match( KCMProxyDdfAddr )!= KErrNotFound )
        {
        retval = CMManager::ECmProxyServerName;
        }
    else if ( nodeName.Match( KCMProxyDdfPortNbr ) != KErrNotFound )
        {
        retval = CMManager::ECmProxyPortNumber; 
        }
    else if ( nodeName.Match( KCMProxyDdfStartpg ) != KErrNotFound )
        {
        retval = CMManager::ECmStartPage;
        }
    else if ( nodeName.Match( KCMProxyDdfProxyType ) != KErrNotFound )
            {
            retval = CMManager::ECmProxyProtocolName;
            }
    
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_MAPURITOCMATTRIBUTE_EXIT, 
               "EXIT: CProxyBranch::MapUriToCmAttribute;retval=%{TConnectionMethodCommonAttributes}", 
               retval );
    
    return retval;
    }
//-----------------------------------------------------------------------------
// TInt32 CProxyBranch::FindConnectionMethodL()
//-----------------------------------------------------------------------------
TInt32 CProxyBranch::FindConnectionMethodL()
    {
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_FINDCONNECTIONMETHODL_ENTRY, 
               "ENTRY: CProxyBranch::FindConnectionMethod" );
    
    TInt32 retval = KErrNotFound;
    
    CConnMoNodeElement* processNode = iBuffer->CacheFind( KProxyConrefUri );
    
    if ( processNode )
        {
        
        HBufC8* luid;
        
        // Ignore root part ("./") of the URI if present in the beginning
        if ( processNode->iObject.Mid( 0, 2 ).Match( KRoot ) != KErrNotFound )
            {
            luid = iCallback->GetLuidAllocL( processNode->iObject.Mid( 2 ) );
            }
        else
            {
            luid = iCallback->GetLuidAllocL( processNode->iObject );
            }
        
        retval = CUtils::IntLUID( *luid );
        
        delete luid;
        
        }
    
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_FINDCONNECTIONMETHODL_EXIT, 
               "EXIT: CProxyBranch::FindConnectionMethod;CmId=%d", retval );
    
    return retval;
    }
//-----------------------------------------------------------------------------
// TBool CProxyBranch::AllMandatoryProxyFieldsFound()
//-----------------------------------------------------------------------------
TBool CProxyBranch::AllMandatoryProxyFieldsFound()
    {
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_ALLMANDATORYPROXYFIELDSFOUND_ENTRY, 
               "ENTRY: CProxyBranch::AllMandatoryProxyFieldsFound" );
    
    TBool retval = ETrue;

    // Check that NAP Address addition is found
    if ( !iBuffer->CacheFind(KProxyAddrUri) )
        {
        retval = EFalse;
        }
    if ( !iBuffer->CacheFind(KProxyConrefUri) )
        {
        retval = EFalse;
        }
    // Check that dynamic nap node addition is found
    if ( !iBuffer->CacheFindDynamicNode(KDynamicProxyNodePattern) )
        {
        retval = EFalse;
        }
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_ALLMANDATORYPROXYFIELDSFOUND_EXIT, 
               "EXIT: CProxyBranch::AllMandatoryProxyFieldsFound;retval=%d", retval );
    
    return retval;
    }
//-----------------------------------------------------------------------------
// TBool CProxyBranch::SetLeafData()
//-----------------------------------------------------------------------------
void CProxyBranch::SetLeafDataL(const TDesC8& aURI, 
                                const TDesC8& /*aLUID*/,
                                const TDesC8& aObject, 
                                 TInt aStatusRef, 
                                 RCmConnectionMethodExt& aCm)
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_SETLEAFDATAL_ENTRY, 
                  "ENTRY: CProxyBranch::SetLeafDataL;aURI=%s", aURI );
    
    TUint32 attributeRef = ( 0 );
    TLex8 lex( aObject );
    TInt value = 0;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    attributeRef = MapUriToCmAttribute( aURI );
    
    switch ( attributeRef )
        {
        case CMManager::ECmProxyServerName:
        case CMManager::ECmStartPage:
        case CMManager::ECmProxyProtocolName:
            aCm.SetStringAttributeL( attributeRef, CUtils::ConvertTo16LC( aObject ) );
            CleanupStack::PopAndDestroy(); // CUtils::ConvertTo16LC
            break;
        case CMManager::ECmProxyPortNumber:        
            if ( lex.Val( value ) == KErrNone )
                {
                aCm.SetIntAttributeL( attributeRef, value );
                }
            else
                {
                status = CSmlDmAdapter::EInvalidObject;
                }
            break;
        default:
            status = CheckForUnmodifiableField( aURI );
            break;
        }
    iCallback->SetStatusL( aStatusRef, status );
    
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_SETLEAFDATAL_EXIT, 
               "EXIT: CProxyBranch::SetLeafDataL;status=%{TError}", status );
    }
//-----------------------------------------------------------------------------
// TBool CProxyBranch::SetNodeData()
//-----------------------------------------------------------------------------
void CProxyBranch::SetNodeDataL(CConnMoNodeElement* aNode, RCmConnectionMethodExt& aCm)
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_SETNODEDATAL_ENTRY, "ENTRY: CProxyBranch::SetNodeDataL;aNode->iUri=%s", aNode->iUri );
    
    if ( aNode->iParentLUID.Length() == 0 )
        {
        TBuf8<16> addLUID;
        _LIT8( KFormat, "%d" );    
        addLUID.Format( KFormat, aCm.GetIntAttributeL( CMManager::ECmId ) );
        
        iCallback->SetMappingL( aNode->iUri, addLUID );
        iCallback->SetStatusL( aNode->iStatusRef, CSmlDmAdapter::EOk );
        }
    else
        {
        iCallback->SetStatusL( aNode->iStatusRef, CSmlDmAdapter::EAlreadyExists );
        }
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_SETNODEDATAL_EXIT, "EXIT: CProxyBranch::SetNodeDataL" );
    
    }
//------------------------------------------------------------------------------
//  void CProxyBranch::GetAllDynamicProxyNodes()
//------------------------------------------------------------------------------
void CProxyBranch::GetAllDynamicProxyNodesL( const TDesC8& aURI,
                                             const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                             TInt aResultsRef, 
                                             TInt aStatusRef,
                                             CBufBase& aCurrentURISegmentList )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_GETALLDYNAMICPROXYNODESL_ENTRY, 
                  "ENTRY: CProxyBranch::GetAllDynamicProxyNodesL;aURI=%s", aURI );
    
    // Array for connection methods
    RArray <TUint32> cmArray;
    
    CleanupClosePushL( cmArray );
    cmArray.Reset();
    
    GetAllConnectionMethodsL( cmArray, *iCmManager );
    
    // Loop through all connection methods
    for ( TInt i = 0; i < cmArray.Count(); i++ )
        {
        RCmConnectionMethodExt cm = iCmManager->ConnectionMethodL( cmArray[i] );
        CleanupClosePushL( cm );
        
        // If proxy is enabled for connection method
        if ( cm.GetBoolAttributeL( CMManager::ECmProxyUsageEnabled ) )
            {
            OstTrace1( TRACE_NORMAL, CPROXYBRANCH_GETALLDYNAMICPROXYNODESL_PROXY, 
                        "CProxyBranch::GetAllDynamicProxyNodesL;Proxy enabled for Cm Id=%u", 
                        cmArray[i] );
            
            TBuf8<KSmlMaxURISegLen>addCmName;
            // Check if node exists in aPreviousURISegmentList
            if ( !CheckPreviousURIList( aPreviousURISegmentList, cmArray[i], addCmName ) )
                {
                // Connection Method not found in previous URI list                        
                addCmName.Append( aURI );
                addCmName.Append( KCMProxyNodeSeparator );
                addCmName.Append( KDynamicNodePrefix );
                
                addCmName.AppendNumFixedWidth( cmArray[i], EDecimal, 3); 
                
                TBuf8<16> addLUID;
                _LIT8( KFormat, "%d" );    
                addLUID.Format( KFormat, cmArray[i] );
                
                OstTraceExt1( TRACE_NORMAL, 
                           CPROXYBRANCH_GETALLDYNAMICPROXYNODES_ADDED_TO_LIST, 
                           "CProxyBranch::GetAllDynamicProxyNodesL;Added to list: URI=%s", addCmName );
                                             
                iCallback->SetMappingL( addCmName, addLUID );
                }
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), CUtils::GetLastURISeg( addCmName ) );
            aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), KCMProxyNodeSeparator );
            }
        CleanupStack::PopAndDestroy( &cm );
        }
    
    iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    iCallback->SetResultsL( aResultsRef, aCurrentURISegmentList, KNullDesC8 );

    CleanupStack::PopAndDestroy( &cmArray );
    
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_GETALLDYNAMICPROXYNODESL_EXIT, 
               "EXI: CProxyBranch::GetAllDynamicProxyNodesLT" );  
    }

// ======== MEMBER FUNCTIONS ========
//-----------------------------------------------------------------------------
// CProxyBranch* CProxyBranch::NewL( )
//-----------------------------------------------------------------------------
CProxyBranch* CProxyBranch::NewL(MSmlDmCallback* aDmCallback, 
                                 CAddBuffer* aBuffer, 
                                 RCmManagerExt* aCmManagerExt  )
    {
    OstTraceFunctionEntry1( CPROXYBRANCH_NEWL_ENTRY, ( TUint )( aDmCallback ) );

    CProxyBranch* self = new (ELeave) CProxyBranch(aDmCallback, aBuffer, aCmManagerExt );

    OstTraceFunctionExit1( CPROXYBRANCH_NEWL_EXIT, ( TUint )( self ) );
    return self;
    }

//------------------------------------------------------------------------------
// CProxyBranch::CProxyBranch()
//------------------------------------------------------------------------------
CProxyBranch::CProxyBranch(MSmlDmCallback* aDmCallback, 
                           CAddBuffer* aBuffer,
                           RCmManagerExt* aCmManagerExt )
    : CBranchBase( aDmCallback )
    {
    OstTraceFunctionEntry1( CPROXYBRANCH_CONSTRUCTOR_ENTRY, this );
    
    iCallback = aDmCallback;
    iBuffer = aBuffer;
    iCmManager = aCmManagerExt;
    
    OstTraceFunctionExit1( CPROXYBRANCH_CONSTRUCTOR_EXIT, this );
    }

//------------------------------------------------------------------------------
// CProxyBranch::~CProxyBranch()
//------------------------------------------------------------------------------
CProxyBranch::~CProxyBranch()
    {
    OstTraceFunctionEntry1( CPROXYBRANCH_DESTRUCTOR_ENTRY, this );

    OstTraceFunctionExit1( CPROXYBRANCH_DESTRUCTOR_EXIT, this );
    }

//------------------------------------------------------------------------------
//  void CProxyBranch::DDFStructureL()
//------------------------------------------------------------------------------
void CProxyBranch::DDFStructureL( MSmlDmDDFObject& aDDF )
    {
    OstTraceFunctionEntry0( CPROXYBRANCH_DDFSTRUCTUREL_ENTRY );
        
    //
    // Set rest acceptable operations for data itself
    //
    TSmlDmAccessTypes aclTypesNoDelete;
    aclTypesNoDelete.SetGet();
    aclTypesNoDelete.SetReplace();
    aclTypesNoDelete.SetAdd();

    TSmlDmAccessTypes aclTypesAddGet;
    aclTypesAddGet.SetGet();
    aclTypesAddGet.SetAdd();
    
    TSmlDmAccessTypes aclTypesOnlyGet;
    aclTypesOnlyGet.SetGet();

    MSmlDmDDFObject& nProxyIdDDF = aDDF.AddChildObjectL( KCMProxyDdfProxyId );        // ProxyId
    CUtils::FillNodeInfoL( nProxyIdDDF, 
                           aclTypesOnlyGet, 
                           MSmlDmDDFObject::EOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EChr, 
                           KNullDesC8 );
    
    MSmlDmDDFObject& nAddrDDF = aDDF.AddChildObjectL( KCMProxyDdfAddr );     // Addr
    CUtils::FillNodeInfoL( nAddrDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EChr, 
                           KNullDesC8 ); 
    
    MSmlDmDDFObject& nToConRefDDF = aDDF.AddChildObjectL( KCMProxyDdfToConRef );     // ToConRef
    CUtils::FillNodeInfoL( nToConRefDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );
    
    MSmlDmDDFObject& nToConRefRootDDF = nToConRefDDF.AddChildObjectGroupL();     // ToConRef/<x>
    CUtils::FillNodeInfoL( nToConRefRootDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EOneOrMore, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );
    
    MSmlDmDDFObject& nConRefDDF = nToConRefRootDDF.AddChildObjectL( KCMProxyDdfConRef ); // ToConRef/<x>/ConRef
    CUtils::FillNodeInfoL( nConRefDDF, 
                           aclTypesAddGet, 
                           MSmlDmDDFObject::EOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EChr, 
                           KNullDesC8 );    
       
    MSmlDmDDFObject& nPortsDDF = aDDF.AddChildObjectL( KCMProxyDdfPorts );     // Ports
    CUtils::FillNodeInfoL( nPortsDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );

    MSmlDmDDFObject& nPortsRootDDF = nPortsDDF.AddChildObjectGroupL();     // Ports/<x>
    CUtils::FillNodeInfoL( nPortsRootDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EOneOrMore, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );    
          
    MSmlDmDDFObject& nPortNbrDDF = nPortsRootDDF.AddChildObjectL( KCMProxyDdfPortNbr );  // Ports/<x>/PortNbr
    CUtils::FillNodeInfoL( nPortNbrDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EInt, 
                           KNullDesC8 );

    MSmlDmDDFObject& nProxyParamsDDF = aDDF.AddChildObjectL( KCMProxyDdfProxyParams ); // ProxyParams
    CUtils::FillNodeInfoL( nProxyParamsDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );

    MSmlDmDDFObject& nProxyTypeDDF = aDDF.AddChildObjectL( KCMProxyDdfProxyType ); // ProxyType
    CUtils::FillNodeInfoL( nProxyTypeDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EChr, 
                           KNullDesC8 );

    
    MSmlDmDDFObject& nWapDDF = nProxyParamsDDF.AddChildObjectL( KCMProxyDdfWap ); // WAP
    CUtils::FillNodeInfoL( nWapDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::ENode, 
                           KNullDesC8 );

    MSmlDmDDFObject& nStartpgDDF = nWapDDF.AddChildObjectL( KCMProxyDdfStartpg ); // Startpg
    CUtils::FillNodeInfoL( nStartpgDDF, 
                           aclTypesNoDelete, 
                           MSmlDmDDFObject::EZeroOrOne, 
                           MSmlDmDDFObject::EDynamic, 
                           MSmlDmDDFObject::EChr, 
                           KNullDesC8 );
    
    OstTraceFunctionEntry0( CPROXYBRANCH_DDFSTRUCTUREL_EXIT );
    }

//------------------------------------------------------------------------------
//  void CProxyBranch::UpdateLeafObjectL()
//------------------------------------------------------------------------------
void CProxyBranch::UpdateLeafObjectL( const TDesC8& aURI, 
                                      const TDesC8& aLUID,
                                      const TDesC8& aObject, 
                                      const TDesC8& /*aType*/,
                                      TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_UPDATELEAFOBJECTL_ENTRY, 
                  "ENTRY: CProxyBranch::UpdateLeafObjectL;aURI=%s", aURI );        
    
    // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
    TInt cmId = GetCmIdFromURIL( aURI );

    if ( aLUID.Length() == 0 && cmId == KErrNotFound )
        {
        // New node, add to buffer
        iBuffer->AddNodeToBufferL( aURI, aLUID, aObject, aStatusRef);
        }
    else
        {
        // Existing node, update data immediately
        TInt err( KErrNone );
        RCmConnectionMethodExt cm;
        if ( cmId == KErrNotFound )
            {
            cmId = CUtils::IntLUID( aLUID );
            }
            
        TRAP( err, cm = iCmManager->ConnectionMethodL( cmId ) );
        CleanupClosePushL( cm );
                
        if ( !err )
            {
            OstTrace0( TRACE_NORMAL, DUP1_CPROXYBRANCH_UPDATELEAFOBJECTL, "CProxyBranch::UpdateLeafObjectL; Existing connection method - write value immediately." );            
            SetLeafDataL( aURI, aLUID, aObject, aStatusRef, cm );
            cm.UpdateL();
            }
        else
            {
            //iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            OstTrace0( TRACE_NORMAL, CPROXYBRANCH_UPDATELEAFOBJECTL, "CProxyBranch::UpdateLeafObjectL; Stale LUID - buffer setting." );            
            iBuffer->AddNodeToBufferL(aURI,KNullDesC8,aObject,aStatusRef);
            }

        CleanupStack::PopAndDestroy( &cm );
        }
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_UPDATELEAFOBJECTL_EXIT, 
               "EXIT: CProxyBranch::UpdateLeafObjectL" );
    
    }
    
//------------------------------------------------------------------------------
//  void CProxyBranch::DeleteObjectL()
//------------------------------------------------------------------------------
void CProxyBranch::DeleteObjectL( const TDesC8& /*aURI*/, 
                                  const TDesC8& /*aLUID*/,
                                  TInt /*aStatusRef*/ )
    {
 
    }
    
//------------------------------------------------------------------------------
//  void CProxyBranch::FetchLeafObjectL()
//------------------------------------------------------------------------------
void CProxyBranch::FetchLeafObjectL( const TDesC8& aURI, 
                                     const TDesC8& aLUID,
                                     const TDesC8& aType, 
                                     TInt aResultsRef,
                                     TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTL_PUPLIC_ENTRY, 
                  "ENTRY: CProxyBranch::FetchLeafObjectL;aURI=%s", aURI );
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;
    
    CBufBase *leafObject = CBufFlat::NewL( KBufferExpandSize );
    CleanupStack::PushL( leafObject );
    leafObject->Reset();
   
    status = FetchLeafObjectL( aURI, aLUID, *leafObject );
    
    if ( status == CSmlDmAdapter::EOk )
        {
        iCallback->SetResultsL( aResultsRef, *leafObject, aType );
        OstTrace0( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECT_OK, 
                   "CProxyBranch::FetchLeafObjectL; EOk" );
        }
    
    CleanupStack::PopAndDestroy( leafObject );   

    iCallback->SetStatusL( aStatusRef, status );
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTL_PUPLIC_EXIT, 
               "EXIT: CProxyBranch::FetchLeafObjectL;status=%{TError}", status );
    
    }
    
//------------------------------------------------------------------------------
//  void CProxyBranch::FetchLeafObjectSizeL()
//------------------------------------------------------------------------------
void CProxyBranch::FetchLeafObjectSizeL( const TDesC8& aURI, 
                                         const TDesC8& aLUID,
                                         const TDesC8& aType, 
                                         TInt aResultsRef,
                                         TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTSIZEL_ENTRY, 
                  "ENTRY: CProxyBranch::FetchLeafObjectSizeL;aURI=%s", aURI );
    
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;
 
    CBufBase *leafObject = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( leafObject );
    leafObject->Reset();

    status = FetchLeafObjectL( aURI, aLUID, *leafObject );

    if ( status == CSmlDmAdapter::EOk )
        {
        leafObject->Compress();
        TBuf8<8> size;
        size.Num( leafObject->Size() );
        leafObject->Reset();
        leafObject->InsertL(0, size );
        iCallback->SetResultsL( aResultsRef, *leafObject, aType );
        }
    
    CleanupStack::PopAndDestroy( leafObject );

    iCallback->SetStatusL( aStatusRef, status );
    OstTrace1( TRACE_NORMAL, CPROXYBRANCH_FETCHLEAFOBJECTSIZEL_EXIT, 
               "EXIT: CProxyBranch::FetchLeafObjectSizeL;status=%{TError}", status );
    
    }

//------------------------------------------------------------------------------
//  void CProxyBranch::ChildURIListL()
//------------------------------------------------------------------------------
void CProxyBranch::ChildURIListL( const TDesC8& aURI, 
                                  const TDesC8& aLUID,
                                  const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                                  TInt aResultsRef,
                                  TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_CHILDURILISTL_ENTRY, 
                  "ENTRY: CProxyBranch::ChildURIListL;aURI=%s", aURI );
        
    TPtrC8 proxyDdfTree = CUtils::GetEndOfURIFromSeg( aURI, KCMProxyDdf );
    
    CBufBase* currentURISegmentList = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( currentURISegmentList );
    
    // Proxy (handle as special case)   
    if ( proxyDdfTree.Match( KCMProxyDdf ) != KErrNotFound )
        {
        GetAllDynamicProxyNodesL( aURI,
                                  aPreviousURISegmentList,
                                  aResultsRef, 
                                  aStatusRef,
                                  *currentURISegmentList );
        CleanupStack::PopAndDestroy( currentURISegmentList );
        return;
        }
    // Then check if the CM exists. Use LUID or predefined ID    
    TInt cmId( KErrNotFound );
    if ( aLUID.Length() > 0 )
        {
        cmId = CUtils::IntLUID( aLUID );
        }
    else
        {
        // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
        cmId = GetCmIdFromURIL( aURI );
        }
    // If after this cmId is still not found then the branch does not exist
    if ( cmId == KErrNotFound )
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        CleanupStack::PopAndDestroy( currentURISegmentList );
        OstTrace0( TRACE_NORMAL, DUP1_CPROXYBRANCH_CHILDURILISTL, "CNapBranch::ChildURIListL; CSmlDmAdapter::ENotFound" );
        return;
        }
    
    RCmConnectionMethodExt cm;
    // Check it the method actually can be found      
    if ( !GetConnectionMethodLC( *iCmManager, cm, cmId ) || !cm.GetBoolAttributeL(CMManager::ECmProxyUsageEnabled ) )       
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        CleanupStack::PopAndDestroy( &cm );
        CleanupStack::PopAndDestroy( currentURISegmentList );        
        OstTrace0( TRACE_NORMAL, DUP2_CPROXYBRANCH_CHILDURILISTL, "CNapBranch::ChildURIListL; CSmlDmAdapter::ENotFound" );
        return;
        }            
    CleanupStack::PopAndDestroy( &cm );    
    
    
    // Proxy/<x>
    if ( proxyDdfTree.Match( KDynamicProxyNodePattern ) != KErrNotFound &&
              proxyDdfTree.Match( KDynamicProxyNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMProxyFields ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );        
        }
    
    // Proxy/<x>/ToConRef/<x>
    else if ( proxyDdfTree.Match( KDynamicToConrefNodeChildUri ) != KErrNotFound &&
              proxyDdfTree.Match( KDynamicToConrefNodeChildUriNodes ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMProxyDdfConRef ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    
    // Proxy/<x>/ToConRef
    else if ( proxyDdfTree.Match( KDynamicToConrefNode ) != KErrNotFound )
        {        
        GetAllDynamicNodesL( aURI,
                             aLUID,
                             aPreviousURISegmentList,
                             aResultsRef, 
                             aStatusRef,
                             *iCmManager,
                             *currentURISegmentList,
                             KCMProxyDdfToConRef,
                             1,
                             iCallback );
        }
    
    // Proxy/<x>/Ports/<x>
    else if ( proxyDdfTree.Match( KDynamicPortsNodeChildUri ) != KErrNotFound &&
              proxyDdfTree.Match( KDynamicPortsNodeChildUriNodes ) == KErrNotFound )
        {      
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMProxyDdfPortNbr ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );        
        }
    
    // Proxy/<x>/Ports
    else if ( proxyDdfTree.Match( KDynamicPortsNode ) != KErrNotFound )
        {
        GetAllDynamicNodesL( aURI, 
                             aLUID,
                             aPreviousURISegmentList,
                             aResultsRef, 
                             aStatusRef,
                             *iCmManager,
                             *currentURISegmentList,
                             KCMProxyDdfPorts,
                             1,
                             iCallback );      
        }
    
    // Proxy/<x>/ProxyParams
    else if ( proxyDdfTree.Match( KDynamicProxyParamsNode ) != KErrNotFound &&
              proxyDdfTree.Match( KDynamicProxyParamsNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMProxyParamsFields ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );        
        }
    
    // Proxy/<x>/ProxyParams/WAP
    else if ( proxyDdfTree.Match( KDynamicProxyParamsWapNode ) != KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMProxyWapFields ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );  
        }
    
    // Not found
    else
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        OstTrace0( TRACE_NORMAL, CPROXYBRANCH_CHILDURILISTL_NOT_FOUND, 
                   "CProxyBranch::ChildURIListL;CSmlDmAdapter::ENotFound" );
        
        }
    
    CleanupStack::PopAndDestroy( currentURISegmentList );
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_CHILDURILISTL_EXIT, 
               "EXIT: CProxyBranch::ChildURIListL" );
        
    }

//------------------------------------------------------------------------------
//  void CProxyBranch::AddNodeObjectL()
//------------------------------------------------------------------------------    
void CProxyBranch::AddNodeObjectL( const TDesC8& aURI, 
                                   const TDesC8& aParentLUID,
                                   TInt aStatusRef )
    {
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_ADDNODEOBJECTL_ENTRY, 
                  "ENTRY: CProxyBranch::AddNodeObjectL;aURI=%s", aURI );
    
    if ( aParentLUID.Length() == 0 )
        {
        // If this is for pre-defined node then get CM id xxx from URI (Proxy/ProxyIdxxx)
        TInt cmId = GetCmIdFromURIL( aURI );
        RCmConnectionMethodExt cm;
        // Check if CM exists
        if ( cmId != KErrNotFound && GetConnectionMethodLC( *iCmManager, cm, cmId ) 
             && cm.GetBoolAttributeL(CMManager::ECmProxyUsageEnabled))
            {
            CleanupStack::PopAndDestroy( &cm );
            // Pre-defined CM already exists for this ID.            
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
            return;
            }
        if ( cmId != KErrNotFound )
            {
            CleanupStack::PopAndDestroy( &cm );
            }
        // New node, add to buffer
        iBuffer->AddNodeToBufferL( aURI, aParentLUID, aStatusRef );
        OstTrace0( TRACE_NORMAL, CPROXYBRANCH_ADDNODEOBJECTL_NEW_NODE, 
                   "CProxyBranch::AddNodeObjectL;New node -> Add to buffer." );
        }
    else
        {
        // Sanity check: If the dynamic name of the Proxy node has is the same as deleted previously and
        // DM Framework hasn't updated the tree, it will give the LUID when calling this method. Thus,
        // it's needed to check that the connection method really exists.
        RCmConnectionMethodExt cm;
    
        
        if ( !GetConnectionMethodLC( *iCmManager, cm, CUtils::IntLUID( aParentLUID ) ) )
            {
            // New node, add to buffer
            iBuffer->AddNodeToBufferL( aURI, KNullDesC8, aStatusRef );
            OstTrace0( TRACE_NORMAL, CPROXYBRANCH_ADDNODEOBJECTL_NEW_NODE2, 
                       "CProxyBranch::AddNodeObjectL;New node -> Add to buffer." );
            }
        else
            {
            // Proxies must be added in one message. If luid is given add is done to existing AP or node
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
            OstTrace0( TRACE_NORMAL, CPROXYBRANCH_ADDNODEOBJECTL_ALREADY_EXISTS, 
                       "CProxyBranch::AddNodeObjectL;CSmlDmAdapter::EAlreadyExists" );
            } 
       
        CleanupStack::PopAndDestroy( &cm );
        }
        
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_ADDNODEOBJECTL_EXIT, "EXIT: CProxyBranch::AddNodeObjectL" );   
    }

// -------------------------------------------------------------------------------------
// CProxyBranch::ProcessNodeBufferL()
// -------------------------------------------------------------------------------------
void CProxyBranch::ProcessBufferL()
    {
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_PROCESSBUFFERL_ENTRY, "ENTRY: CProxyBranch::ProcessBufferL" );
    
    RPointerArray<CConnMoNodeElement> proxyNodes( KNumOfProxyNodes );
    CleanupClosePushL( proxyNodes );
    while ( iBuffer->GetGroup( KCMDdfProxy, &proxyNodes ) != KErrNotFound )
        {
        TInt32 cmId = FindConnectionMethodL();
    
        if ( AllMandatoryProxyFieldsFound() && cmId != KErrNotFound )
            {
            // Open connection method, enable proxy and update nodes
            RCmConnectionMethodExt cm;
            TRAPD( err, cm = iCmManager->ConnectionMethodL( cmId ) );
            CleanupClosePushL( cm );
            
            if ( !err )
                {
                cm.SetBoolAttributeL( CMManager::ECmProxyUsageEnabled, ETrue );
        
                for ( TInt i = 0; i < proxyNodes.Count(); ++i )
                    {
                    if ( proxyNodes[i]->iLeaf  )
                        {
                    // Leaf data found
                    SetLeafDataL( proxyNodes[i]->iUri, 
                                  proxyNodes[i]->iParentLUID, 
                                  proxyNodes[i]->iObject, 
                                  proxyNodes[i]->iStatusRef, cm );
                    proxyNodes[i]->iProcessed = ETrue;
                        }
                    else
                        {
                        // Node has no leaf data
                        SetNodeDataL( proxyNodes[i], cm );
                        proxyNodes[i]->iProcessed = ETrue;
                        }
                    }
                cm.UpdateL();
                }
            else
                {
                for ( TInt i = 0; i < proxyNodes.Count(); ++i )
                    {
                    iCallback->SetStatusL( proxyNodes[i]->iStatusRef, CSmlDmAdapter::EError );
                    proxyNodes[i]->iProcessed = ETrue;
                    }
                }
            CleanupStack::PopAndDestroy( &cm );
            }
        else
            {
            for ( TInt i = 0; i < proxyNodes.Count(); ++i )
                {
                iCallback->SetStatusL( proxyNodes[i]->iStatusRef, CSmlDmAdapter::EError );
                proxyNodes[i]->iProcessed = ETrue;
                }
            }
        proxyNodes.Reset();
        }
    CleanupStack::Pop( &proxyNodes );
    proxyNodes.Close();
 
    OstTrace0( TRACE_NORMAL, CPROXYBRANCH_PROCESSBUFFERL_EXIT, "EXIT: CProxyBranch::ProcessBufferL" );  
    }


//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetDynamicAPNodeUri( const TDesC8& aURI )
// returns NAP/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CProxyBranch::GetDynamicProxyNodeUri( const TDesC8& aURI )
    {    
    OstTraceExt1( TRACE_NORMAL, CPROXYBRANCH_GETDYNAMICPROXYNODEURI, "CProxyBranch::GetDynamicProxyNodeUri;aURI=%s", aURI );
    TInt i= 0;
    for ( i = aURI.Find( KCMDdfProxy ) + 6; i < aURI.Length(); i++ )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }           
    OstTraceExt1( TRACE_NORMAL, DUP1_CPROXYRANCH_GETDYNAMICPROXYNODEURI, "CProxyBranch::GetDynamicProxyNodeUri;return URI=%s", aURI.Left( i ) );
    return aURI.Left( i );
    }

//--------------------------------------------------------------------
//TInt CProxyBranch::IsNAPUriFormatMatchPredefined(const TDesC8 & aURI)
//
//-------------------------------------------------------------------


TBool CProxyBranch::IsProxyUriFormatMatchPredefined(const TDesC8 & aURI)
{
    if( aURI.Match( _L8( "Proxy/ProxyId*" ) ) != KErrNotFound )
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
//------------------------------------------------------------------------------
// TUint32 CProxyBranch::GetAPIdFromURIL(const TDesC8& aURI)
//         Get NAP id for given aURI
//------------------------------------------------------------------------------
TInt CProxyBranch::GetCmIdFromURIL(const TDesC8& aURI)
    {
    TInt ret(KErrNotFound);
    
    if( !IsProxyUriFormatMatchPredefined( aURI ) )
        {
        // Cannot get CmIds from any other format URIs than predefined
        return ret;
        }
   
    TLex8 lex;
    //  Find xxx from ./Proxy/ProxyIdxxx/...    
    TInt start = aURI.Find( KDynamicNodePrefix ) + KDynamicNodePrefix().Length();
    lex = aURI.Mid( start, 3 );
    lex.Val( ret );
    
    return ret;
    }
