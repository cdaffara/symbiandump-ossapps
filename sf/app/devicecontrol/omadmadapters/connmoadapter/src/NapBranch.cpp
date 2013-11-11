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
#include <featmgr.h>
#include <cmmanagerext.h> 
#include <cmpluginpacketdatadef.h>
#include <cmpluginwlandef.h>
#include <cmconnectionmethoddef.h>
#include <nsmldmtreedbclient.h>
#include "NapBranch.h"
#include "Utils.h"
#include "AddBuffer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "NapBranchTraces.h"
#endif
// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
// NAP Branch
_LIT8( KCMDdfID, "ID" );
_LIT8( KCMDdfName, "Name" );
_LIT8( KCMDdfAddrType, "AddrType" );
_LIT8( KCMDdfAddr, "Addr" );
_LIT8( KCMDdfIP, "IP" );
_LIT8( KCMDdfAuthInfo, "AuthInfo" );
_LIT8( KCMDdfAuthType, "AuthType" );
_LIT8( KCMDdfBearerType, "BearerType" );
_LIT8( KCMDdfBearerParams, "BearerParams" );
_LIT8( KCMDdfIPv4, "IPv4" );
_LIT8( KCMDdfAutoConfig, "AutoConfig" );
_LIT8( KCMDdfIPAddr, "IPAddr" );
_LIT8( KCMDdfNetMask, "NetMask" );
_LIT8( KCMDdfGateway, "Gateway" );
_LIT8( KCMDdfAutoDNS, "AutoDNS" );
_LIT8( KCMDdfDNS, "DNS" );
_LIT8( KCMDdfServers, "Servers" );
_LIT8( KCMDdfServer, "Server" );
_LIT8( KCMDdfAuthName, "AuthName" );
_LIT8( KCMDdfAuthSecret, "AuthSecret" );
// NAP Child URI List
_LIT8( KCMNapFields, "ID/Name/AddrType/Addr/IP/AuthInfo/BearerType" );
_LIT8( KCMNIPFields, "IPv4" );
_LIT8( KCMNIPv4Fields, "AutoConfig/IPAddr/NetMask/Gateway/AutoDNS/DNS" );
_LIT8( KCMNDNSFields, "Servers" );
_LIT8( KCMNAuthInfoFields, "AuthType/AuthName/AuthSecret" );
_LIT8( KCMNDNSServersFields, "Addr");
_LIT8( KSlash, "/" );
_LIT8( KDynamicNapNode, "NAP/*" );
_LIT8( KDynamicNapNodeChildUri, "NAP/*/*" );
_LIT8( KDynamicIPNode, "NAP/*/IP" );
_LIT8( KDynamicIPNodeChildUri, "NAP/*/IP/*" );
_LIT8( KDynamicIPv4Node, "NAP/*/IP/IPv4" );
_LIT8( KDynamicIPv4NodeChildUri, "NAP/*/IP/IPv4/*" );
_LIT8( KDynamicIPv4DnsNode, "NAP/*/IP/IPv4/DNS" );
_LIT8( KDynamicIPv4DnsNodeChildUri, "NAP/*/IP/IPv4/DNS/*" );
_LIT8( KDynamicIPv4DnsServersNode, "NAP/*/IP/IPv4/DNS/Servers" );
_LIT8( KDynamicIPv4DnsServersNodeChildUri, "NAP/*/IP/IPv4/DNS/Servers/*" );
_LIT8( KDynamicIPv4DnsServersNodeChildUriNodes, "NAP/*/IP/IPv4/DNS/Servers/*/*");
_LIT8( KDynamicAuthInfoNode, "NAP/*/AuthInfo");
_LIT8( KDynamicAuthInfoNodeChildUri, "NAP/*/AuthInfo/*");
_LIT8( KNapBearerTypeUri, "NAP/*/BearerType");
_LIT8( KNapAddrUri, "NAP/*/Addr");
_LIT8( KNapDNSAddr1Uri, "NAP/*/Server1/Addr");
_LIT8( KNapDNSAddr2Uri, "NAP/*/Server2/Addr");
_LIT8( KNapIPv4AddrUri, "NAP/*/IPv4/*Addr");
_LIT8( KPacketDataBearerType, "3GPPPS");
_LIT8( KGPRSAddrType, "APN");
_LIT8( KGPRSAuthType, "PAP");


TInt const KInvalidBearerType = 0;
TInt const KBufferExpandSize = 128;
TInt const KNumOfNAPNodes = 8;
TInt const KMaxResultSize = 64;
TInt const KMaxDnsAddr = 2;
_LIT8( KFalse, "False");
_LIT8( KTrue, "True");

_LIT8( KDynamicNodePrefix, "NAPId" );

// ======== LOCAL FUNCTIONS =========
//-----------------------------------------------------------------------------
// TUint32 CNapBranch::CheckForUnmodifiableFieldL()
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CNapBranch::GetUnmodifiableFieldValueL(
        const TDesC8& aUri, CBufBase* aResult)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_GETUNMODIFIABLEFIELDVALUEL_ENTRY,
            "ENTRY: CNapBranch::GetUnmodifiableFieldValueL;aUri=%s", aUri );

    CSmlDmAdapter::TError retval( CSmlDmAdapter::ENotFound );
    TPtrC8 nodeName = CUtils::GetLastURISeg( aUri );

    if ( nodeName.Match( KCMDdfAddrType ) != KErrNotFound )
        {
        aResult->InsertL( 0, KGPRSAddrType );
        retval = CSmlDmAdapter::EOk;
        }
    else if ( nodeName.Match( KCMDdfBearerType ) != KErrNotFound )
        {
        aResult->InsertL( 0, KPacketDataBearerType );
        retval = CSmlDmAdapter::EOk;
        }
    else if ( nodeName.Match( KCMDdfAuthType ) != KErrNotFound )
        {
        aResult->InsertL( 0, KGPRSAuthType );
        retval = CSmlDmAdapter::EOk;
        }
    else if ( nodeName.Match( KCMDdfID ) != KErrNotFound )
        {
        TPtrC8 dynamicNode = GetDynamicNAPNodeUri( aUri );
        aResult->InsertL( 0, dynamicNode );
        retval = CSmlDmAdapter::EOk;
        }

    OstTrace1( TRACE_NORMAL, CNAPBRANCH_GETUNMODIFIABLEFIELDVALUEL_EXIT,
            "EXIT: CNapBranch::GetUnmodifiableFieldValueL;retval=%{TError}", retval );

    return retval;
    }
//-----------------------------------------------------------------------------
// CSmlDmAdapter::TErrorCNapBranch::GetLeafData()
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CNapBranch::GetLeafDataL(const TDesC8& aURI,
        RCmConnectionMethodExt& aCm, CBufBase* aResult)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_GETLEAFDATAL_ENTRY,
            "ENTRY: CNapBranch::GetLeafDataL;aURI=%s", aURI );

    TUint32 attributeRef( 0 );
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;
    HBufC16* resultTmp;

    attributeRef = MapUriToCmAttribute( aURI );
    switch ( attributeRef )
        {
        case CMManager::ECmName:
        case CMManager::EPacketDataIPNameServer1:
        case CMManager::EPacketDataIPNameServer2:
        case CMManager::EPacketDataAPName:
        case CMManager::EPacketDataIFAuthName:
        case CMManager::EPacketDataIPAddr:
        case CMManager::EPacketDataIPNetmask:
        case CMManager::EPacketDataIPGateway:
        resultTmp = aCm.GetStringAttributeL( attributeRef );
        CleanupStack::PushL( resultTmp );
        aResult->InsertL( 0, CUtils::ConvertTo8LC( *resultTmp ) );
        CleanupStack::PopAndDestroy(); // ConvertTo8LC
        CleanupStack::PopAndDestroy( resultTmp );
        break;
        case CMManager::EPacketDataIPAddrFromServer:
        case CMManager::EPacketDataIPDNSAddrFromServer:
        if ( aCm.GetBoolAttributeL( attributeRef ) )
            {
            aResult->InsertL( 0, KTrue );
            }
        else
            {
            aResult->InsertL( 0, KFalse );
            }

        break;
        default:
        status = GetUnmodifiableFieldValueL( aURI, aResult );
        break;
        }
    OstTrace1( TRACE_NORMAL, CNAPBRANCH_GETLEAFDATAL_EXIT,
            "EXIT: CNapBranch::GetLeafDataL;status=%{TError}", status );

    return status;
    }

//-----------------------------------------------------------------------------
// CNapBranch::GetUnmodifiableFieldAddResult()
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CNapBranch::GetUnmodifiableFieldAddResult(
        const TDesC8& aUri, const TDesC8& aObject)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_GETUNMODIFIABLEFIELDADDRESULT_ENTRY,
            "ENTRY: CNapBranch::GetUnmodifiableFieldAddResult;aUri=%s", aUri );

    CSmlDmAdapter::TError retval( CSmlDmAdapter::ENotFound );
    TPtrC8 nodeName = CUtils::GetLastURISeg( aUri );

    if ( nodeName.Match( KCMDdfAddrType ) != KErrNotFound )
        {
        if ( aObject.Match( KGPRSAddrType ) != KErrNotFound )
            {
            retval = CSmlDmAdapter::EOk;
            }
        else
            {
            retval = CSmlDmAdapter::EInvalidObject;
            }
        }
    else if ( nodeName.Match( KCMDdfAuthType ) != KErrNotFound )
        {
        if ( aObject.Match( KGPRSAuthType ) != KErrNotFound )
            {
            retval = CSmlDmAdapter::EOk;
            }
        else
            {
            retval = CSmlDmAdapter::EInvalidObject;
            }
        }

    OstTrace1( TRACE_NORMAL, DUP1_CNAPBRANCH_GETUNMODIFIABLEFIELDADDRESULT_EXIT,
            "EXIT: CNapBranch::GetUnmodifiableFieldAddResult;retval=%{TError}", retval );

    return retval;
    }
//-----------------------------------------------------------------------------
// CNapBranch::MapUriToCmAttribute(()
//-----------------------------------------------------------------------------
TUint32 CNapBranch::MapUriToCmAttribute(const TDesC8& aUri)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_MAPURITOCMATTRIBUTE_ENTRY,
            "ENTRY: CNapBranch::MapUriToCmAttribute;aUri=%s", aUri );

    TUint32 retval( 0 );

    TPtrC8 nodeName = CUtils::GetLastURISeg( aUri );

    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_MAPURITOCMATTRIBUTE_GET_NODE,
            "CNapBranch::MapUriToCmAttribute;nodeName=%s", nodeName );

    if ( nodeName.Match( KCMDdfName ) != KErrNotFound )
        {
        retval = CMManager::ECmName;
        }
    else if ( nodeName.Match( KCMDdfAddr ) != KErrNotFound )
        {
        // IP branch and NAP branch have both "Addr" leaf. Select between them.
        if ( aUri.Match( KNapAddrUri ) != KErrNotFound &&
                aUri.Match( KNapIPv4AddrUri ) == KErrNotFound )
            {
            retval = CMManager::EPacketDataAPName;
            }
        else if ( aUri.Match( KNapDNSAddr1Uri ) != KErrNotFound )
            {
            retval = CMManager::EPacketDataIPNameServer1;
            }
        else if ( aUri.Match( KNapDNSAddr2Uri ) != KErrNotFound )
            {
            retval = CMManager::EPacketDataIPNameServer2;
            }
        }
    else if ( nodeName.Match( KCMDdfBearerType ) != KErrNotFound )
        {
        retval = CMManager::ECmBearerType;
        }
    else if ( nodeName.Match( KCMDdfAuthName ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIFAuthName;
        }
    else if ( nodeName.Match( KCMDdfAuthSecret ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIFAuthPass;
        }
    else if ( nodeName.Match( KCMDdfAutoConfig ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIPAddrFromServer;
        }
    else if ( nodeName.Match( KCMDdfIPAddr ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIPAddr;
        }
    else if ( nodeName.Match( KCMDdfNetMask ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIPNetmask;
        }
    else if ( nodeName.Match( KCMDdfGateway ) != KErrNotFound )
        {
        retval = CMManager::EPacketDataIPGateway;
        }
    else if ( nodeName.Match( KCMDdfAutoDNS )!= KErrNotFound )
        {
        retval = CMManager::EPacketDataIPDNSAddrFromServer;
        }

    OstTrace1( TRACE_NORMAL, CNAPBRANCH_MAPURITOCMATTRIBUTE_EXIT,
            "EXIT: CNapBranch::MapUriToCmAttribute;retval=%{TError}", retval );

    return retval;
    }
//-----------------------------------------------------------------------------
// void CNapBranch::SetLeafData()
//-----------------------------------------------------------------------------
void CNapBranch::SetLeafDataL(const TDesC8& aURI, const TDesC8& aObject,
        TInt aStatusRef, RCmConnectionMethodExt& aCm)
    {
    OstTraceExt1( TRACE_FLOW, CNAPBRANCH_SETLEAFDATAL_ENTRY,
            "ENTRY: CNapBranch::SetLeafDataL;aURI=%s", aURI );

    TUint32 attributeRef( 0 );
    TBool tmpBool = EFalse;
    CSmlDmAdapter::TError status = CSmlDmAdapter::EOk;

    attributeRef = MapUriToCmAttribute( aURI );
    OstTrace1( TRACE_NORMAL, CNAPBRANCH_SETLEAFDATAL_GET_CMATTRIB,
            "CNapBranch::SetLeafDataL;CM Attribute:;attributeRef=%{TConnectionMethodCommonAttributes}",
            attributeRef );

    switch ( attributeRef )
        {
        case CMManager::ECmName:
        case CMManager::EPacketDataIPNameServer1:
        case CMManager::EPacketDataIPNameServer2:
        case CMManager::EPacketDataAPName:
        case CMManager::EPacketDataIFAuthName:
        case CMManager::EPacketDataIFAuthPass:
        case CMManager::EPacketDataIPAddr:
        case CMManager::EPacketDataIPNetmask:
        case CMManager::EPacketDataIPGateway:
        aCm.SetStringAttributeL( attributeRef, CUtils::ConvertTo16LC( aObject ) );
        CleanupStack::PopAndDestroy(); // ConvertTo16LC
        break;
        case CMManager::EPacketDataIPAddrFromServer:
        case CMManager::EPacketDataIPDNSAddrFromServer:
        if ( CUtils::DesToBool( aObject, tmpBool ) == KErrNone )
            {
            aCm.SetBoolAttributeL( attributeRef, tmpBool );
            }
        else
            {
            status = CSmlDmAdapter::EInvalidObject;
            }
        break;
        case CMManager::ECmBearerType:
        // BearerType is already set when CM was created.
        break;
        default:
        // Attribute field was not from CM. Check if value is hard coded to CM.
        status = GetUnmodifiableFieldAddResult( aURI, aObject );
        break;
        }
    iCallback->SetStatusL( aStatusRef, status );
    OstTrace1( TRACE_NORMAL, CNAPBRANCH_SETLEAFDATAL_EXIT,
            "EXIT: CNapBranch::SetLeafDataL;status=%{TError}", status );
    }

//-----------------------------------------------------------------------------
// TBool CNapBranch::SetNodeData()
//-----------------------------------------------------------------------------
void CNapBranch::SetNodeDataL(CConnMoNodeElement* aNode,
        RCmConnectionMethodExt& aCm)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_SETNODEDATAL_ENTRY,
            "ENTRY: CNapBranch::SetNodeDataL;aNode->iUri=%s", aNode->iUri );

    if ( aNode->iParentLUID.Length() == 0 )
        {
        TBuf8<16> addLUID;
        _LIT8( KFormat, "%d" );    
        addLUID.Format( KFormat, aCm.GetIntAttributeL( CMManager::ECmId ) );
        
        iCallback->SetMappingL( aNode->iUri, addLUID );
        iCallback->SetStatusL( aNode->iStatusRef, CSmlDmAdapter::EOk );

        OstTrace0( TRACE_NORMAL, CNAPBRANCH_SETNODEDATAL_OK,
                "CNapBranch::SetNodeDataL; CSmlDmAdapter::EOk" );
        }
    else
        {
        iCallback->SetStatusL( aNode->iStatusRef, CSmlDmAdapter::EAlreadyExists );
        OstTrace0( TRACE_NORMAL, CNAPBRANCH_SETNODEDATAL_EXISTS,
                "CNapBranch::SetNodeDataL; CSmlDmAdapter::EAlreadyExists" );
        }
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_SETNODEDATAL_EXIT,
            "EXIT: CNapBranch::SetNodeDataL" );
    }

//-----------------------------------------------------------------------------
// TBool CNapBranch::FindNewCmType
//-----------------------------------------------------------------------------
TBool CNapBranch::FindNewCmType(TUint32& aBearerType)
    {
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_FINDNEWCMTYPE_ENTRY, "ENTRY: CNapBranch::FindNewCmType" );

    TBool retval = EFalse;
    // Find bearer type for creating a connection methdod
    CConnMoNodeElement* processNode = iBuffer->CacheFind( KNapBearerTypeUri );
    if ( processNode )
        {
        if ( processNode->iObject.Match( KPacketDataBearerType ) != KErrNotFound )
            {
            OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_FINDNEWCMTYPE_3GPP,
                    "CNapBranch::FindNewCmType;Bearer Type: %s", KPacketDataBearerType );

            aBearerType = KUidPacketDataBearerType;
            retval = ETrue;
            }
        else
            {
            aBearerType = KInvalidBearerType;
            }
        }
    OstTrace1( TRACE_NORMAL, CNAPBRANCH_FINDNEWCMTYPE_EXIT,
            "EXIT: CNapBranch::FindNewCmType;retval=%d", retval );

    return retval;
    }
//-----------------------------------------------------------------------------
// TBool CNapBranch::AllMandatoryNapFieldsFound()
//-----------------------------------------------------------------------------
TBool CNapBranch::AllMandatoryNapFieldsFound()
    {
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_ALLMANDATORYNAPFIELDSFOUND_ENTRY,
            "ENTRY: CNapBranch::AllMandatoryNapFieldsFound" );

    TBool retval = ETrue;

    // Check that NAP Address addition is found
    if ( !iBuffer->CacheFind( KNapAddrUri ) )
        {
        retval = EFalse;
        }
    // Check that dynamic nap node addition is found
    if ( !iBuffer->CacheFindDynamicNode( KDynamicNapNode ) )
        {
        retval = EFalse;
        }

    OstTrace1( TRACE_NORMAL, CNAPBRANCH_ALLMANDATORYNAPFIELDSFOUND_EXIT,
            "EXIT: CNapBranch::AllMandatoryNapFieldsFound;retval=%d", retval );

    return retval;
    }
//-----------------------------------------------------------------------------
// TBool CNapBranch::NapChildURIListL
// TBool CNapBranch::NapChildURIList( )
//-----------------------------------------------------------------------------
void CNapBranch::NapChildURIListL(const TDesC8& aURI, const CArrayFix<
        TSmlDmMappingInfo>& aPreviousURISegmentList, TInt aResultsRef,
        TInt aStatusRef, CBufBase& aCurrentURISegmentList)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_NAPCHILDURILISTL_ENTRY,
            "ENTRY: CNapBranch::NapChildURIListL;aURI=%s", aURI );

    RArray<TUint32> CmArray;
    CleanupClosePushL( CmArray );

    TPtrC8 napTree = CUtils::GetEndOfURIFromSeg( aURI, KCMDdfNAP );

    GetAllConnectionMethodsL( CmArray, *iCmManager );

    for (TInt i = 0; i < CmArray.Count(); i++ )
        {
        TBuf8<KBufferExpandSize> addURISeg;
        if ( !CheckPreviousURIList( aPreviousURISegmentList, CmArray[i], addURISeg ) )
            {
            // Connection Method not found in previous URI list                        
            addURISeg.Append( aURI );
            addURISeg.Append( KSlash );
            addURISeg.Append( KDynamicNodePrefix );

            addURISeg.AppendNumFixedWidth( CmArray[i], EDecimal, 3);

            TBuf8<16> addLUID;
            _LIT8( KFormat, "%d" );    
            addLUID.Format( KFormat, CmArray[i] );

            OstTraceExt1( TRACE_NORMAL,
                    CNAPBRANCH_NAPCHILDURILISTL_ADDED_TO_LIST,
                    "CNapBranch::NapChildURIListL;Added to list: URI=%s", addURISeg );

            iCallback->SetMappingL( addURISeg, addLUID );
            }

        // Child is the last segment in uri
        aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(),
                CUtils::GetLastURISeg(addURISeg) );
        aCurrentURISegmentList.InsertL( aCurrentURISegmentList.Size(), KSlash );
        }
    iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
    iCallback->SetResultsL( aResultsRef, aCurrentURISegmentList, KNullDesC8 );

    CleanupStack::PopAndDestroy( &CmArray );

    OstTrace0( TRACE_NORMAL, CNAPBRANCH_NAPCHILDURILISTL_EXIT,
            "EXIT: CNapBranch::NapChildURIListL" );
    }
//-----------------------------------------------------------------------------
// CSmlDmAdapter::TError CNapBranch::DeleteObjectL
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CNapBranch::DeleteObjectL(const TDesC8& aURI,
        const TDesC8& aLUID)
    {
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_DELETEOBJECTL_PRIVATE_ENTRY,
            "ENTRY: CNapBranch::DeleteObjectL" );

    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;

    // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
    TInt cmId = GetCmIdFromURIL( aURI );
    
    RCmConnectionMethodExt cm;
    
    if( aLUID.Length() > 0 )
        {       
        cmId = CUtils::IntLUID( aLUID );
        }

    // Check, if referred connection method exists           
    if ( GetConnectionMethodLC( *iCmManager, cm, cmId ) )
        {
        cm.DeleteL();
        status = CSmlDmAdapter::EOk;
        OstTrace1( TRACE_NORMAL, CNAPBRANCH_DELETEOBJECTL_PRIVATE_DELETE,
                "CNapBranch::DeleteObjectL; Connection Method=%u deleted.", cmId );
        }

    CleanupStack::PopAndDestroy( &cm );

    OstTrace1( TRACE_NORMAL, CNAPBRANCH_DELETEOBJECTL_PRIVATE_EXIT,
            "EXIT: CNapBranch::DeleteObjectL;status=%{TError}", status );

    return status;
    }

// ======== MEMBER FUNCTIONS ========
//-----------------------------------------------------------------------------
// CNapBranch* CNapBranch::NewL( )
//-----------------------------------------------------------------------------
CNapBranch* CNapBranch::NewL(MSmlDmCallback* aDmCallback,
        CAddBuffer* aBuffer, RCmManagerExt* aCmManagerExt)
    {
    OstTraceFunctionEntry1( CNAPBRANCH_NEWL_ENTRY, ( TUint )( aDmCallback ) );

    CNapBranch* self = new (ELeave) CNapBranch( aDmCallback );
    CleanupStack::PushL(self);

    self->ConstructL( aBuffer, aCmManagerExt );

    CleanupStack::Pop(self);

    OstTraceFunctionExit1( CNAPBRANCH_NEWL_EXIT, ( TUint )( self ) );
    return self;
    }

//------------------------------------------------------------------------------
// CNapBranch::CNapBranch()
//------------------------------------------------------------------------------
CNapBranch::CNapBranch(MSmlDmCallback* aDmCallback) :
    CBranchBase(aDmCallback)
    {
    OstTraceFunctionEntry1( CNAPBRANCH_CONSTRUCTOR_ENTRY, this );

    iCallback = aDmCallback;

    OstTraceFunctionExit1( CNAPBRANCH_CONSTRUCTOR_EXIT, this );
    }

//------------------------------------------------------------------------------
// CNapBranch::~CNapBranch()
//------------------------------------------------------------------------------
CNapBranch::~CNapBranch()
    {
    OstTraceFunctionEntry1( CNAPBRANCH_DESTRUCTOR_ENTRY, this );

    OstTraceFunctionExit1( CNAPBRANCH_DESTRUCTOR_EXIT, this );
    }

//=============================================
//      CNapBranch::ConstructL()
//      Second phase constructor.
//=============================================
void CNapBranch::ConstructL(CAddBuffer* aBuffer, RCmManagerExt* aCmManagerExt)
    {
    OstTraceFunctionEntry1( CNAPBRANCH_CONSTRUCTL_ENTRY, this );

    iBuffer = aBuffer;
    iCmManager = aCmManagerExt;

    OstTraceFunctionExit1( CNAPBRANCH_CONSTRUCTL_EXIT, this );
    }

//------------------------------------------------------------------------------
//  void CNapBranch::DDFStructureL()
//------------------------------------------------------------------------------
void CNapBranch::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    OstTraceFunctionEntry0( CNAPBRANCH_DDFSTRUCTUREL_ENTRY );

    //
    // Set rest acceptable operations for data itself
    //
    TSmlDmAccessTypes aclTypesNoDelete;
    aclTypesNoDelete.SetGet();
    aclTypesNoDelete.SetReplace();
    aclTypesNoDelete.SetAdd();

    TSmlDmAccessTypes aclTypesNoGet;
    aclTypesNoGet.SetReplace();
    aclTypesNoGet.SetAdd();
    aclTypesNoGet.SetDelete();

    TSmlDmAccessTypes aclTypesOnlyGet;
    aclTypesOnlyGet.SetGet();

    MSmlDmDDFObject& nIDDDF = aDDF.AddChildObjectL( KCMDdfID ); // ID
    CUtils::FillNodeInfoL( nIDDDF,
            aclTypesOnlyGet,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nNameDDF = aDDF.AddChildObjectL( KCMDdfName ); // Name
    CUtils::FillNodeInfoL( nNameDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nAddrTypeDDF = aDDF.AddChildObjectL( KCMDdfAddrType ); // AddrType
    CUtils::FillNodeInfoL( nAddrTypeDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nAddrDDF = aDDF.AddChildObjectL( KCMDdfAddr ); // Addr
    CUtils::FillNodeInfoL( nAddrDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nIPDDF = aDDF.AddChildObjectL( KCMDdfIP ); // IP
    CUtils::FillNodeInfoL( nIPDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    MSmlDmDDFObject& nIPv4DDF = nIPDDF.AddChildObjectL( KCMDdfIPv4 ); // IPv4
    CUtils::FillNodeInfoL( nIPv4DDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );
    IpBranchDDFStructureL( nIPv4DDF );

    MSmlDmDDFObject& nAuthInfoDDF = aDDF.AddChildObjectL( KCMDdfAuthInfo ); // AuthInfo
    CUtils::FillNodeInfoL( nAuthInfoDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    MSmlDmDDFObject& nAuthNameDDF = nAuthInfoDDF.AddChildObjectL( KCMDdfAuthName ); // AuthName
    CUtils::FillNodeInfoL( nAuthNameDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nAuthTypeDDF = nAuthInfoDDF.AddChildObjectL( KCMDdfAuthType ); // AuthType
    CUtils::FillNodeInfoL( nAuthTypeDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nAuthSecretDDF = nAuthInfoDDF.AddChildObjectL( KCMDdfAuthSecret ); // AuthSecret
    CUtils::FillNodeInfoL( nAuthSecretDDF,
            aclTypesNoGet,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nBearerTypeDDF = aDDF.AddChildObjectL( KCMDdfBearerType ); // BearerType
    CUtils::FillNodeInfoL( nBearerTypeDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nBearerParamsDDF = aDDF.AddChildObjectL( KCMDdfBearerParams ); // BearerParams
    CUtils::FillNodeInfoL( nBearerParamsDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    OstTraceFunctionExit0( CNAPBRANCH_DDFSTRUCTUREL_EXIT );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::IpBranchDDFStructureL
// -------------------------------------------------------------------------------------
void CNapBranch::IpBranchDDFStructureL(MSmlDmDDFObject& nIPvXDDF)
    {
    OstTraceFunctionEntry0( CNAPBRANCH_IPBRANCHDDFSTRUCTUREL_ENTRY );

    //
    // Set rest acceptable operations for data itself
    //
    TSmlDmAccessTypes aclTypesNoDelete;
    aclTypesNoDelete.SetGet();
    aclTypesNoDelete.SetReplace();
    aclTypesNoDelete.SetAdd();

    MSmlDmDDFObject& nAutoConfigDDF = nIPvXDDF.AddChildObjectL( KCMDdfAutoConfig ); // AutoConfig
    CUtils::FillNodeInfoL( nAutoConfigDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nIPAddrDDF = nIPvXDDF.AddChildObjectL( KCMDdfIPAddr ); // IPAddr
    CUtils::FillNodeInfoL( nIPAddrDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nNetMaskDDF = nIPvXDDF.AddChildObjectL( KCMDdfNetMask ); // NetMask
    CUtils::FillNodeInfoL( nNetMaskDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nGatewayDDF = nIPvXDDF.AddChildObjectL( KCMDdfGateway ); // Gateway
    CUtils::FillNodeInfoL( nGatewayDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    MSmlDmDDFObject& nAutoDNSDDF = nIPvXDDF.AddChildObjectL( KCMDdfAutoDNS ); // AutoDNS
    CUtils::FillNodeInfoL( nAutoDNSDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNullDesC8 );

    MSmlDmDDFObject& nDNSDDF = nIPvXDDF.AddChildObjectL( KCMDdfDNS ); // DNS
    CUtils::FillNodeInfoL( nDNSDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    MSmlDmDDFObject& nServersDDF = nDNSDDF.AddChildObjectL( KCMDdfServers ); // Servers
    CUtils::FillNodeInfoL( nServersDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    MSmlDmDDFObject& nServersRootDDF = nServersDDF.AddChildObjectGroupL(); // Servers/<x>
    CUtils::FillNodeInfoL( nServersRootDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOneOrMore,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode,
            KNullDesC8 );

    MSmlDmDDFObject& nAddrDDF = nServersRootDDF.AddChildObjectL( KCMDdfAddr ); // Addr
    CUtils::FillNodeInfoL( nAddrDDF,
            aclTypesNoDelete,
            MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNullDesC8 );

    OstTraceFunctionExit0( CNAPBRANCH_IPBRANCHDDFSTRUCTUREL_EXIT );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::UpdateLeafObjectL
// -------------------------------------------------------------------------------------
void CNapBranch::UpdateLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        const TDesC8& aObject, const TDesC8& /*aType*/, TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_UPDATELEAFOBJECTL_ENTRY,
            "ENTRY: CNapBranch::UpdateLeafObjectL;aURI=%s", aURI );

    // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
    TInt cmId = GetCmIdFromURIL( aURI );
    
    if ( aLUID.Length() == 0 && cmId == KErrNotFound )
        {
            // New node, add to buffer
            iBuffer->AddNodeToBufferL( aURI, aLUID, aObject, aStatusRef );    
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
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_UPDATELEAFOBJECTL, "CNapBranch::UpdateLeafObjectL; Update existing node data immediately." );
            SetLeafDataL( aURI, aObject, aStatusRef, cm );
            cm.UpdateL();
            }
        else
            {
            OstTrace0( TRACE_NORMAL, DUP1_CNAPBRANCH_UPDATELEAFOBJECTL, "CNapBranch::UpdateLeafObjectL; Stale LUID - Buffer setting." );
            iBuffer->AddNodeToBufferL( aURI, KNullDesC8, aObject, aStatusRef );
            }

        CleanupStack::PopAndDestroy( &cm );
        }

    OstTrace0( TRACE_NORMAL, CNAPBRANCH_UPDATELEAFOBJECTL_EXIT,
            "EXIT: CNapBranch::UpdateLeafObjectL" );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::DeleteObjectL
// -------------------------------------------------------------------------------------
void CNapBranch::DeleteObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_DELETEOBJECTL_ENTRY,
            "ENTRY: CNapBranch::DeleteObjectL;aURI=%s", aURI );

    CSmlDmAdapter::TError status = CSmlDmAdapter::EError;

    TPtrC8 napTree = CUtils::GetEndOfURIFromSeg( aURI, KCMDdfNAP );

    if ( napTree.Match( KDynamicNapNode ) != KErrNotFound &&
            napTree.Match( KDynamicNapNodeChildUri ) == KErrNotFound )
        {

        status = DeleteObjectL( aURI, aLUID );

        }

    iCallback->SetStatusL( aStatusRef, status );

    OstTrace1( TRACE_NORMAL, CNAPBRANCH_DELETEOBJECTL_EXIT,
            "EXIT: CNapBranch::DeleteObjectL;status=%{TError}", status );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::FetchLeafObjectL
// -------------------------------------------------------------------------------------
void CNapBranch::FetchLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        const TDesC8& aType, TInt aResultsRef, TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTL_ENTRY,
            "ENTRY: CNapBranch::FetchLeafObjectL;aURI=%s", aURI );
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
    if ( cmId != KErrNotFound )
        {
        CBufBase* result = CBufFlat::NewL( KBufferExpandSize );
        CleanupStack::PushL( result );

        // Open connection method.
        RCmConnectionMethodExt cm;
        TRAPD(err, cm = iCmManager->ConnectionMethodL( cmId ) );
        if (err != KErrNone )
            {
            CleanupStack::PopAndDestroy( result );
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_DUP_FETCHLEAFOBJECTL_NOT_FOUND, "CSmlDmAdapter::ENotFound" );
            return;
            }
        CleanupClosePushL( cm );

        // Find data from CM
        CSmlDmAdapter::TError status = GetLeafDataL( aURI,
                cm,
                result );

        OstTrace1( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTL_STATUS,
                "CNapBranch::FetchLeafObjectL;status=%{TError}", status );

        if ( status == CSmlDmAdapter::EOk )
            {
            iCallback->SetResultsL( aResultsRef, *result, aType );
            }
        iCallback->SetStatusL( aStatusRef, status );

        CleanupStack::PopAndDestroy( &cm );
        CleanupStack::PopAndDestroy( result );
        }
    else
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        OstTrace0( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTL_NOT_FOUND, "CSmlDmAdapter::ENotFound" );
        }
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTL_EXIT, "EXIT: CNapBranch::FetchLeafObjectL" );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::FetchLeafObjectSizeL
// -------------------------------------------------------------------------------------
void CNapBranch::FetchLeafObjectSizeL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef,
        TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTSIZEL_ENTRY,
            "ENTRY: CNapBranch::FetchLeafObjectSizeL;aURI=%s", aURI );
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
    if ( cmId != KErrNotFound )
        {
        CBufBase* result = CBufFlat::NewL( KBufferExpandSize );
        CleanupStack::PushL(result);

        // Open connection method.
        RCmConnectionMethodExt cm;
        TRAPD(err, cm = iCmManager->ConnectionMethodL( cmId ) );
        if (err != KErrNone )
            {
            CleanupStack::PopAndDestroy( result );
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_DUP1_FETCHLEAFOBJECTL_NOT_FOUND, "CSmlDmAdapter::ENotFound" );
            return;
            }
        CleanupClosePushL( cm );

        // Find data from CM
        CSmlDmAdapter::TError status = GetLeafDataL( aURI,
                cm,
                result );

        OstTrace1( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTSIZEL_STATUS,
                "CNapBranch::FetchLeafObjectSizeL;status=%{TError}", status );

        if ( status == CSmlDmAdapter::EOk )
            {
            TInt size = result->Size();
            TBuf8<KMaxResultSize> sizeStr;
            sizeStr.AppendNum( size );

            result->Reset();
            result->InsertL( 0, sizeStr );

            iCallback->SetResultsL( aResultsRef, *result, aType);
            }
        iCallback->SetStatusL( aStatusRef, status );

        CleanupStack::PopAndDestroy( &cm );
        CleanupStack::PopAndDestroy( result );
        }
    else
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        OstTrace0( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTSIZEL_NOT_FOUND,
                "CNapBranch::FetchLeafObjectSizeL;CSmlDmAdapter::ENotFound" );
        }
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_FETCHLEAFOBJECTSIZEL_EXIT,
            "EXIT: CNapBranch::FetchLeafObjectSizeL" );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::ChildURIListL
// -------------------------------------------------------------------------------------
void CNapBranch::ChildURIListL(const TDesC8& aURI, const TDesC8& aLUID,
        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
        TInt aResultsRef, TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_CHILDURILISTL_ENTRY,
            "ENTRY: CNapBranch::ChildURIListL;aURI=%s", aURI );
    
       
    TPtrC8 napTree = CUtils::GetEndOfURIFromSeg( aURI,KCMDdfNAP );

    CBufBase* currentURISegmentList = CBufFlat::NewL( KSmlMaxURISegLen );
    CleanupStack::PushL( currentURISegmentList );    
    
    // First handle ./NAP as a special case
    if ( napTree.Match( KCMDdfNAP ) != KErrNotFound )
        {
        NapChildURIListL( aURI,
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
        OstTrace0( TRACE_NORMAL, DUP1_CNAPBRANCH_CHILDURILISTL, "CNapBranch::ChildURIListL; CSmlDmAdapter::ENotFound" );
        return;
        }
    
    RCmConnectionMethodExt cm;
    // Check it the method actually can be found      
    if ( !GetConnectionMethodLC( *iCmManager, cm, cmId ) )       
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        CleanupStack::PopAndDestroy( &cm );
        CleanupStack::PopAndDestroy( currentURISegmentList );        
        OstTrace0( TRACE_NORMAL, DUP2_CNAPBRANCH_CHILDURILISTL, "CNapBranch::ChildURIListL; CSmlDmAdapter::ENotFound" );
        return;
        }            
    CleanupStack::PopAndDestroy( &cm );    
            
    if ( napTree.Match( KDynamicNapNode ) != KErrNotFound &&
            napTree.Match( KDynamicNapNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNapFields ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );        
        }
    // ./NAP/*/IP

    else if ( napTree.Match( KDynamicIPNode ) != KErrNotFound &&
            napTree.Match( KDynamicIPNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNIPFields );
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    // ./NAP/*/IP/IPv4

    else if ( napTree.Match( KDynamicIPv4Node ) != KErrNotFound &&
            napTree.Match( KDynamicIPv4NodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNIPv4Fields );
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    // ./NAP/*/IP/IPv4/DNS

    else if ( napTree.Match( KDynamicIPv4DnsNode ) != KErrNotFound &&
            napTree.Match( KDynamicIPv4DnsNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNDNSFields );
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    // ./NAP/*/IP/IPv4/DNS/Servers

    else if ( napTree.Match( KDynamicIPv4DnsServersNode ) != KErrNotFound )
        {
        GetAllDynamicNodesL( aURI,
                aLUID,
                aPreviousURISegmentList,
                aResultsRef,
                aStatusRef,
                *iCmManager,
                *currentURISegmentList,
                KCMDdfServer,
                KMaxDnsAddr,
                iCallback );
        }
    // ./NAP/*/IP/IPv4/DNS/Servers*

    else if ( napTree.Match( KDynamicIPv4DnsServersNodeChildUri ) != KErrNotFound &&
            napTree.Match( KDynamicIPv4DnsServersNodeChildUriNodes ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNDNSServersFields ); 
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    // ./NAP/*/AuthInfo

    else if ( napTree.Match( KDynamicAuthInfoNode ) != KErrNotFound &&
            napTree.Match( KDynamicAuthInfoNodeChildUri ) == KErrNotFound )
        {
        currentURISegmentList->InsertL( currentURISegmentList->Size(), KCMNAuthInfoFields );
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        iCallback->SetResultsL( aResultsRef, *currentURISegmentList, KNullDesC8 );
        }
    else
        {
        iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::ENotFound );
        OstTrace0( TRACE_NORMAL, CNAPBRANCH_CHILDURILISTL,
                "CNapBranch::ChildURIListL; CSmlDmAdapter::ENotFound" );
        }

    CleanupStack::PopAndDestroy( currentURISegmentList );

    OstTrace0( TRACE_NORMAL, DUP1_CNAPBRANCH_CHILDURILISTL_EXIT,
            "EXIT: CNapBranch::ChildURIListL" );
    }

// -------------------------------------------------------------------------------------
// CNapBranch::AddNodeObjectL
// -------------------------------------------------------------------------------------
void CNapBranch::AddNodeObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, TInt aStatusRef)
    {
    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL_ENTRY,
            "ENTRY: CNapBranch::AddNodeObjectL;aURI=%s", aURI );

    if ( aParentLUID.Length() == 0 )
        {        
        // If this is for pre-defined node then get CM id xxx from URI (NAP/NAPIdxxx)
        TInt cmId = GetCmIdFromURIL( aURI );
        RCmConnectionMethodExt cm;
        // Check if CM exists
        if ( cmId != KErrNotFound && GetConnectionMethodLC( *iCmManager, cm, cmId ) )
            {
            CleanupStack::PopAndDestroy( &cm );
            // Pre-defined CM already exists for this ID.
            // NAPs must be added in one message. If luid is given add is done to existing AP or node
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL, 
                    "CNapBranch::AddNodeObjectL; CSmlDmAdapter::EAlreadyExists because tried to add in predefined node format that exists" );
            return;
            }                          
        if ( cmId != KErrNotFound )
            {
            CleanupStack::PopAndDestroy( &cm );
            }

        // New node, add to buffer
        iBuffer->AddNodeToBufferL( aURI, aParentLUID, aStatusRef );
        //iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EOk );
        OstTrace0( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL_NEW_NODE,
                "CNapBranch::AddNodeObjectL;New node -> Add to buffer." );
        }
    else
        {
        // Sanity check: If the dynamic name of the NAP node has is the same as deleted previously and
        // DM Framework hasn't updated the tree, it will give the LUID when calling this method. Thus,
        // it's needed to check that the connection method really exists.
        TBuf8<KSmlMaxURISegLen> parentLUID;
        parentLUID.Zero();
        parentLUID = aParentLUID;
        RCmConnectionMethodExt cm;               
        if ( !GetConnectionMethodLC( *iCmManager, cm,  CUtils::IntLUID( parentLUID) ) )
            {            
			if(aParentLUID.Length() > 0)
				{
				TInt ret = iCallback->RemoveMappingL(KConnMoAdapterUid,
							GetDynamicNAPNodeUri( aURI ), ETrue );
				if(ret)
					{
					iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EError );
					CleanupStack::PopAndDestroy( &cm );
					return;
					}				
				}
			// New node, add to buffer
            iBuffer->AddNodeToBufferL( aURI, KNullDesC8, aStatusRef );
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL_NEW_NODE2,
                    "CNapBranch::AddNodeObjectL;New node -> Add to buffer." );
            }
        else
            {
            // NAPs must be added in one message. If luid is given add is done to existing AP or node
            iCallback->SetStatusL( aStatusRef, CSmlDmAdapter::EAlreadyExists );
            OstTrace0( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL_ALLREADY_EXISTS,
                    "CNapBranch::AddNodeObjectL; CSmlDmAdapter::EAlreadyExists" );
            }

        CleanupStack::PopAndDestroy( &cm );
        }
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_ADDNODEOBJECTL_EXIT, "EXIT: CNapBranch::AddNodeObjectL" );

    }

// -------------------------------------------------------------------------------------
// CNapBranch::ProcessNodeBufferL()
// -------------------------------------------------------------------------------------
void CNapBranch::ProcessBufferL()
    {
    OstTrace0( TRACE_NORMAL, CNAPBRANCH_PROCESSBUFFERL_ENTRY, "ENTRY: CNapBranch::ProcessBufferL" );

    TUint32 bearerType;

    RPointerArray<CConnMoNodeElement> napNodes( KNumOfNAPNodes );
    CleanupClosePushL( napNodes );

    while ( iBuffer->GetGroup( KCMDdfNAP, &napNodes ) != KErrNotFound )
        {

        if ( FindNewCmType( bearerType ) && AllMandatoryNapFieldsFound() )
            {
            // Create new connection method and update nodes
            RCmConnectionMethodExt cm = iCmManager->CreateConnectionMethodL( bearerType );
            CleanupClosePushL( cm );
            cm.UpdateL();

            for ( TInt i = 0; i < napNodes.Count(); ++i )
                {
                if ( napNodes[i]->iLeaf )
                    {
                    // Leaf data found
                    SetLeafDataL( napNodes[i]->iUri,
                            napNodes[i]->iObject,
                            napNodes[i]->iStatusRef, cm );
                    napNodes[i]->iProcessed = ETrue;
                    }
                else
                    {
                    // Node has no leaf data
                    SetNodeDataL( napNodes[i], cm);
                    napNodes[i]->iProcessed = ETrue;
                    }
                }
            cm.UpdateL();
            CleanupStack::PopAndDestroy( &cm );
            }
        else
            {
            for ( TInt i = 0; i < napNodes.Count(); ++i )
                {
                iCallback->SetStatusL( napNodes[i]->iStatusRef, CSmlDmAdapter::EError );
                napNodes[i]->iProcessed = ETrue;
                }
            }
        napNodes.Reset();
        }
    CleanupStack::Pop( &napNodes );
    napNodes.Close();

    OstTrace0( TRACE_NORMAL, CNAPBRANCH_PROCESSBUFFERL_EXIT, "EXIT: CNapBranch::ProcessBufferL" );

    }

//------------------------------------------------------------------------------
// TPtrC8 CUtils::GetDynamicAPNodeUri( const TDesC8& aURI )
// returns NAP/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CNapBranch::GetDynamicNAPNodeUri(const TDesC8& aURI)
    {

    OstTraceExt1( TRACE_NORMAL, CNAPBRANCH_GETDYNAMICNAPNODEURI, "CNapBranch::GetDynamicNAPNodeUri;aURI=%s", aURI );

    TInt i= 0;
    for ( i = aURI.Find( KCMDdfNAP ) + 4; i < aURI.Length(); i++ )
        {
        if( aURI[i] == '/' )
            {
            break;
            }
        }
    OstTraceExt1( TRACE_NORMAL, DUP1_CNAPBRANCH_GETDYNAMICNAPNODEURI, "CNapBranch::GetDynamicNAPNodeUri;return URI=%s", aURI.Left( i ) );

    return aURI.Left( i );
    }

//--------------------------------------------------------------------
//TInt CNapBranch::IsNAPUriFormatMatchPredefined(const TDesC8 & aURI)
//
//-------------------------------------------------------------------


TBool CNapBranch::IsNAPUriFormatMatchPredefined(const TDesC8 & aURI)
{
    if( aURI.Match( _L8( "NAP/NAPId*" ) ) != KErrNotFound )
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
//------------------------------------------------------------------------------
// TUint32 CNapBranch::GetAPIdFromURIL(const TDesC8& aURI)
//         Get NAP id for given aURI
//------------------------------------------------------------------------------
TInt CNapBranch::GetCmIdFromURIL(const TDesC8& aURI)
    {
    TInt ret(KErrNotFound);
    
    if( !IsNAPUriFormatMatchPredefined( aURI ) )
        {
        // Cannot get CmIds from any other format URIs than predefined
        return ret;
        }
   
    TLex8 lex;
    //  Find xxx from ./NAP/NAPIdxxx/...    
    TInt start = aURI.Find( KDynamicNodePrefix ) + KDynamicNodePrefix().Length();
    lex = aURI.Mid( start, 3 );
    lex.Val( ret );
    
    return ret;
    }
