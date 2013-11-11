/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Internet-settings 
 *
 */

#include <commdb.h>
#include <cdbstore.h>
#include <cdbcols.h>             // CommsDB columname defs
#include <implementationproxy.h> // For TImplementationProxy definition
#include <cdbpreftable.h>
#include <etelpckt.h>   // ISP enumerations
#include <etelmm.h>     // GPRS ProtocolType
#include <nifvar.h>     // CallbackType
#include <etelqos.h>    // GPRS enumeration
#include <wappdef.h>    // WAP  enumeration
#include <sysutil.h>    // OOD checking
#include <uriutils.h>   // ipv4 + ipv6 checking
#include <featmgr.h>
#include "NSmlInternetAdapter.h"
#include "NSmlWLanAdapter.h"
#include "nsmldebug.h"
#include "nsmldmtreedbclient.h"
#include <comms-infras/commdb/protection/protectdb.h>
#include <utf.h>

#include <metadatabase.h>
#include <cmmanagerext.h>
#include <cmconnectionmethoddef.h>
#include <cmconnectionmethodext.h>
#include  <cmpluginbaseeng.h>
#include <centralrepository.h>
#include <pdpcontextmanagerinternalcrkeys.h>

const TUint KNSmlInternetAdapterImplUid = 0x101F6DE2;

const TUint KIapColumn = 0x00000100;
const TUint KLingerColumn = 0x00000200;
const TUint KColumnMask = 0xFFFFFF00;
const TUint KRowMask = 0x000000FF;

#include <SettingEnforcementInfo.h> // VSettingEnforcementInfo
class CNSmlWLanAdapter;

_LIT( KDaemonManagerName, "NetCfgExtnDhcp" );
_LIT( KConfigDaemonName, "!DhcpServ" );

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TInt KBeginTransRetryDelay = 1000000; // Delay for comms db begintransaction retry (microseconds)
const TInt KBeginTransRetryCount = 7; // Maximum number of retries
//-----------------------------------------------------------------------------
// CSmlInternetAdapter* CSmlInternetAdapter::NewL( )
//-----------------------------------------------------------------------------

CNSmlInternetAdapter* CNSmlInternetAdapter::NewL(MSmlDmCallback* aDmCallback)
    {
    _DBG_FILE("CNSmlInternetAdapter::NewL(): begin");

    CNSmlInternetAdapter* self = NewLC(aDmCallback);
    CleanupStack::Pop();

    _DBG_FILE("CNSmlInternetAdapter::NewL(): end");
    return self;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter* CNSmlInternetAdapter::NewLC( )
//------------------------------------------------------------------------------
CNSmlInternetAdapter* CNSmlInternetAdapter::NewLC(MSmlDmCallback* aDmCallback)
    {
    _DBG_FILE("CNSmlInternetAdapter::NewLC(): begin");
    CNSmlInternetAdapter* self = new (ELeave) CNSmlInternetAdapter(
            aDmCallback);
    CleanupStack::PushL(self);

    self->iCallBack = aDmCallback;
    self->ConstructL();

    _DBG_FILE("CNSmlInternetAdapter::NewLC(): end");
    return self;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::CNSmlInternetAdapter()
//------------------------------------------------------------------------------
CNSmlInternetAdapter::CNSmlInternetAdapter(TAny* aEcomArguments) :
    CSmlDmAdapter(aEcomArguments)
    {
    _DBG_FILE("CNSmlInternetAdapter::CNSmlInternetAdapter(): begin");
    _DBG_FILE("CNSmlInternetAdapter::CNSmlInternetAdapter(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::~CNSmlInternetAdapter(TAny* aEcomArguments)
//------------------------------------------------------------------------------
CNSmlInternetAdapter::~CNSmlInternetAdapter()
    {
    _DBG_FILE("CNSmlInternetAdapter::~CNSmlInternetAdapter(): begin");

    delete iField;

    delete iPrevURI;

    // Command buffer cleaning if leave happend
    if (iBuffer)
        {
        for (TInt i = 0; i < iBuffer->Count(); i++)
            {
            delete iBuffer->At(i).iMappingName;
            delete iBuffer->At(i).iName;

            ClearBuffer(iBuffer->At(iExecutionIndex).iNodeBuf);
            delete iBuffer->At(i).iNodeBuf;

            ClearBuffer(iBuffer->At(iExecutionIndex).iWlanNodeBuf);
            delete iBuffer->At(i).iWlanNodeBuf;
            }

        iBuffer->Reset();
        delete iBuffer;
        }

    if (iWlanSupported)
        {
        delete iWlanAdapter;
        }

    delete iDatabase;
    FeatureManager::UnInitializeLib();
    _DBG_FILE("CNSmlInternetAdapter::~CNSmlInternetAdapter(): end");
    }

//=============================================
//      CNSmlInternetAdapter::ConstructL()
//      Second phase constructor.
//=============================================
void CNSmlInternetAdapter::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iField = HBufC8::NewL(KNSmlMaxURLLength);

    iPrevURI = HBufC8::NewL(KNSmlMaxURLLength);

    iDatabase = CCommsDatabase::NewL();
    // iDatabase->BeginTransaction(); //+++
    iTransactionCommitted = ETrue;
    i3GPPPS = EFalse;

    iLeafType = EDMUnset; // Default for leaf handling

    //  Reset identification data

    iDirection = ECommDbConnectionDirectionUnknown;

    //
    //  Command buffering used for AddNode + AddLeaf
    //
    iBuffer = new (ELeave) CArrayFixFlat<TNSmlAPBufferElement> (
            KNSmlAPGranularity);

    // checks if Wlan feature is supported
    iWlanSupported = FeatureManager::FeatureSupported(KFeatureIdProtocolWlan);

    // WlanAdapter 
    //  
    if (iWlanSupported)
        {
        iWlanAdapter = CNSmlWLanAdapter::NewL(iCallBack, *iDatabase);
        }

    iNetworkId = KErrNotFound;
    iLingerValue = NULL;
    iLingerFlag = ETrue;
    isAdd = EFalse;
    }

//------------------------------------------------------------------------------
// void CNSmlInternetAdapter::DDFVersionL()
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    aDDFVersion.InsertL(0, KNSmlInternetAdapterDDFversion);
    }
//------------------------------------------------------------------------------
//  void CNSmlInternetAdapter::DDFStructureL()
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    _DBG_FILE("CNSmlInternetAdapter::DDFStructureL(): begin");

    //
    // Set rest acceptable operations for data itself
    //

    TSmlDmAccessTypes aclTypesAddGetDel;
    aclTypesAddGetDel.SetGet();
    aclTypesAddGetDel.SetAdd();
    aclTypesAddGetDel.SetDelete();

    TSmlDmAccessTypes aclTypesAddGet;
    aclTypesAddGet.SetGet();
    aclTypesAddGet.SetAdd();

    TSmlDmAccessTypes aclTypesNoDelete;
    aclTypesNoDelete.SetGet();
    aclTypesNoDelete.SetReplace();
    aclTypesNoDelete.SetAdd();

    TSmlDmAccessTypes aclTypesNoGet;
    aclTypesNoGet.SetReplace();
    aclTypesNoGet.SetAdd();
    aclTypesNoGet.SetDelete();

    TSmlDmAccessTypes aclTypesAddReplace;
    aclTypesAddReplace.SetReplace();
    aclTypesAddReplace.SetAdd();

    TSmlDmAccessTypes aclTypesOnlyGet;
    aclTypesOnlyGet.SetGet();

    TSmlDmAccessTypes aclTypesAll;
    aclTypesAll.SetGet();
    aclTypesAll.SetReplace();
    aclTypesAll.SetAdd();
    aclTypesAll.SetDelete();

    MSmlDmDDFObject* ddfRoot = &aDDF;
    MSmlDmDDFObject& ddf = ddfRoot->AddChildObjectL(KNSmlDdfAP);

    _LIT8(KDescription, "AP-Settings DDF description");
    _LIT8(KDFTitle, "AP-settings title");

    FillNodeInfoL(ddf, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode, KDescription);

    ddf.SetDFTitleL(KDFTitle);
    ddf.SetDefaultValueL(KNullDesC8);

    MSmlDmDDFObject& nApDDF = ddf.AddChildObjectGroupL(); // For AP/<X>

    FillNodeInfoL(nApDDF, aclTypesAll, MSmlDmDDFObject::EZeroOrMore,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNAPDefRootDDF = nApDDF.AddChildObjectL(KNSmlDdfNAPDef); // NAPDef
    FillNodeInfoL(nNAPDefRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, // EZeroOrMore
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNAPDefDDF = nNAPDefRootDDF.AddChildObjectGroupL(); // For NAPDef<x>
    FillNodeInfoL(nNAPDefDDF, aclTypesAddGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nPxRootDDF = nApDDF.AddChildObjectL(KNSmlDdfPx); // Px
    FillNodeInfoL(nPxRootDDF, aclTypesAddGet, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nPxDDF = nPxRootDDF.AddChildObjectGroupL(); // For Px<x>
    FillNodeInfoL(nPxDDF,
            aclTypesAddGetDel,//aclTypesAddGet
            MSmlDmDDFObject::EOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);
    //
    //  NAPDef-node fields
    //
    MSmlDmDDFObject& nNameDDF = nNAPDefDDF.AddChildObjectL(KNSmlDdfNAPName); // Name
    FillNodeInfoL(nNameDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfNAPNameDesc);

    MSmlDmDDFObject& nNAPIDDDF = nNAPDefDDF.AddChildObjectL(KNSmlDdfNAPID); // NAPID
    FillNodeInfoL(nNAPIDDDF, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfNAPIDDesc);

    MSmlDmDDFObject& nNAPLingerDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfNAPLinger); // Linger
    FillNodeInfoL(nNAPLingerDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EInt, KNSmlDdfNAPLingerDesc);

    MSmlDmDDFObject& nNAPStartpgDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfStartpg); // Startpg
    FillNodeInfoL(nNAPStartpgDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfStartpgDesc);

    MSmlDmDDFObject& nBearerRootDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfBearer); // Bearer 
    FillNodeInfoL(nBearerRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nBearerDDF = nBearerRootDDF.AddChildObjectGroupL(); // Bearer/<x>
    FillNodeInfoL(nBearerDDF, aclTypesAddGet, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nBearerLDDF =
            nBearerDDF.AddChildObjectL(KNSmlDdfBearerL); // BearerL
    FillNodeInfoL(nBearerLDDF, aclTypesAddGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfBearerLDesc);

    MSmlDmDDFObject& nDirectionDDF = nBearerDDF.AddChildObjectL(
            KNSmlDdfDirection); // Direction
    FillNodeInfoL(nDirectionDDF, aclTypesAddGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDirectionDesc);

    MSmlDmDDFObject& nNAPAddrDDF =
            nNAPDefDDF.AddChildObjectL(KNSmlDdfNAPAddr); // NAPAddr
    FillNodeInfoL(nNAPAddrDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfNAPAddrDesc);

    MSmlDmDDFObject& nNAPAddrTyDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfNAPAddrTy); // NAPAddrTy
    FillNodeInfoL(nNAPAddrTyDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfNAPAddrTyDesc);

    MSmlDmDDFObject& nDNSAddrRootDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfDNSAddr); // DNSAddr
    FillNodeInfoL(nDNSAddrRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nDNSAddrDDF = nDNSAddrRootDDF.AddChildObjectGroupL(); // DNSAddr/<x>
    FillNodeInfoL(nDNSAddrDDF, aclTypesAddGet, MSmlDmDDFObject::EOneOrMore,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nDNSAddrLDDF = nDNSAddrDDF.AddChildObjectL(
            KNSmlDdfDNSAddrL); // DNSAddrL
    FillNodeInfoL(nDNSAddrLDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDNSAddrLDesc);

    MSmlDmDDFObject& nDNSAddrTyDDF = nDNSAddrDDF.AddChildObjectL(
            KNSmlDdfDNSAddrTy); // DNSAddrTy
    FillNodeInfoL(nDNSAddrTyDDF, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDNSAddrTyDesc);

    MSmlDmDDFObject& nDNSPriorityDDF = nDNSAddrDDF.AddChildObjectL(
            KNSmlDdfDNSPriority); // DNSAddrPriority
    FillNodeInfoL(nDNSPriorityDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDNSPriorityDesc);

    MSmlDmDDFObject& nNAPAuthInfRootDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfNAPAuthInf); // NAPAuthInf 
    FillNodeInfoL(nNAPAuthInfRootDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNAPAuthInfDDF =
            nNAPAuthInfRootDDF.AddChildObjectGroupL(); // NAPAuthInf/<x>
    FillNodeInfoL(nNAPAuthInfDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nAuthNameDDF = nNAPAuthInfDDF.AddChildObjectL(
            KNSmlDdfAuthName); // AuthName
    FillNodeInfoL(nAuthNameDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfAuthNameDesc);

    MSmlDmDDFObject& nAuthSecrDDF = nNAPAuthInfDDF.AddChildObjectL(
            KNSmlDdfAuthSecr); // AuthSecr
    FillNodeInfoL(nAuthSecrDDF, aclTypesAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfAuthSecrDesc);

    MSmlDmDDFObject& nDefGWDDF = nNAPDefDDF.AddChildObjectL(KNSmlDdfDefGW); // DefGW
    FillNodeInfoL(nDefGWDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDefGWDesc);

    MSmlDmDDFObject& nNetworkMaskDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfNetworkMask); // NetworkMask
    FillNodeInfoL(nNetworkMaskDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfNetworkMaskDesc);

    MSmlDmDDFObject& nUsePTxtLogDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfUsePTxtLog); // UsePTxtLog
    FillNodeInfoL(nUsePTxtLogDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlDdfUsePTxtLogDesc);

    MSmlDmDDFObject& nNetworksRootDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfNetworks); // Networks 
    FillNodeInfoL(nNetworksRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNetworksDDF = nNetworksRootDDF.AddChildObjectGroupL(); // Networks/<x>
    FillNodeInfoL(nNetworksDDF, aclTypesAddGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNetworkNameDDF = nNetworksDDF.AddChildObjectL(
            KNSmlDdfNetworkName); // NetworkName
    FillNodeInfoL(nNetworkNameDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfNetworkNameDesc);

    MSmlDmDDFObject& nNetworkIDDDF = nNetworksDDF.AddChildObjectL(
            KNSmlDdfNetworkID); // NetworkID
    FillNodeInfoL(nNetworkIDDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt,
            KNSmlDdfNetworkIDDesc);

    MSmlDmDDFObject& nPPPCompDDF =
            nNAPDefDDF.AddChildObjectL(KNSmlDdfPPPComp); // PPPComp
    FillNodeInfoL(nPPPCompDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EBool,
            KNSmlDdfPPPCompDesc);

    MSmlDmDDFObject& nGPRSPDPDDF =
            nNAPDefDDF.AddChildObjectL(KNSmlDdfGPRSPDP); // GPRSPDP
    FillNodeInfoL(nGPRSPDPDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfGPRSPDPDesc);

    MSmlDmDDFObject& nIPAddrFromServerDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIPAddrFromServer); // IPAddrFromServer
    FillNodeInfoL(nIPAddrFromServerDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlDdfIPAddrFromServerDesc);

    MSmlDmDDFObject& nIPAddrDDF = nNAPDefDDF.AddChildObjectL(KNSmlDdfIPAddr); // IPAddr
    FillNodeInfoL(nIPAddrDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfIPAddrDesc);

    MSmlDmDDFObject& nDNSAddrFromServerDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfDNSAddrFromServer); // DNSAddrFromServer
    FillNodeInfoL(nDNSAddrFromServerDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlDdfDNSAddrFromServerDesc);

    MSmlDmDDFObject& nIPv6DNSAddrFromServerDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIPv6DNSAddrFromServer); // IPv6DNSAddrFromServer
    FillNodeInfoL(nIPv6DNSAddrFromServerDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlDdfIPv6DNSAddrFromServerDesc);

    MSmlDmDDFObject& nIfNetworksDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIfNetworks); // IfNetworks
    FillNodeInfoL(nIfNetworksDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfIfNetworksDesc);

    MSmlDmDDFObject& nIAPServiceDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIAPService); // IAPService
    FillNodeInfoL(nIAPServiceDDF, aclTypesOnlyGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfIAPServiceDesc);

    MSmlDmDDFObject& nIAPSeamlessnessDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIAPSeamlessness); // IAPService
    FillNodeInfoL(nIAPSeamlessnessDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EInt, KNSmlDdfIAPSeamlessnessDesc);
    MSmlDmDDFObject& nIAPMetaDataDDF = nNAPDefDDF.AddChildObjectL(
            KNSmlDdfIAPMetaData); // IAPService
    FillNodeInfoL(nIAPMetaDataDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EInt, KNSmlDdfIAPMetaDataDesc);

    if (iWlanSupported)
        {
        iWlanAdapter->DDFStructureL(nNAPDefDDF); // Adds WLAN fields to DDF
        }

    //
    //  Px-node fields
    //
    MSmlDmDDFObject& nPxNameDDF = nPxDDF.AddChildObjectL(KNSmlDdfPxName); // Name
    FillNodeInfoL(nPxNameDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlDdfPxDesc);

    MSmlDmDDFObject& nPxIdDDF = nPxDDF.AddChildObjectL(KNSmlDdfPxId); // PxId
    FillNodeInfoL(nPxIdDDF, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfPxIdDesc);

    MSmlDmDDFObject& nDomainRootDDF = nPxDDF.AddChildObjectL(KNSmlDdfDomain); // Domain 
    FillNodeInfoL(nDomainRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlDdfDomainDesc);

    MSmlDmDDFObject& nDomainDDF = nDomainRootDDF.AddChildObjectGroupL(); // Domain<x> 
    FillNodeInfoL(nDomainDDF, aclTypesAddGet, MSmlDmDDFObject::EOneOrMore,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nDomainLDDF =
            nDomainDDF.AddChildObjectL(KNSmlDdfDomainL); // DomainL (Get)
    FillNodeInfoL(nDomainLDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfDomainLDesc);

    MSmlDmDDFObject& nPxPWDDF = nPxDDF.AddChildObjectL(KNSmlDdfPxPW); // PxPW
    FillNodeInfoL(nPxPWDDF, aclTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfPxPWDesc);

    MSmlDmDDFObject& nPxAddrDDF = nPxDDF.AddChildObjectL(KNSmlDdfPxAddr); // PxAddr
    FillNodeInfoL(nPxAddrDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfPxAddrDesc);

    MSmlDmDDFObject& nPortRootDDF = nPxDDF.AddChildObjectL(KNSmlDdfPort); // Port 
    FillNodeInfoL(nPortRootDDF, aclTypesAddGet, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode,
            KNSmlDdfPortDesc);

    MSmlDmDDFObject& nPortDDF = nPortRootDDF.AddChildObjectGroupL(); // Port<x> 
    FillNodeInfoL(nPortDDF, aclTypesAddGet, MSmlDmDDFObject::EOneOrMore,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nPortNbrDDF = nPortDDF.AddChildObjectL(KNSmlDdfPortNbr); // PortNbr (Get)
    FillNodeInfoL(nPortNbrDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt,
            KNSmlDdfPortNbrDesc);

    MSmlDmDDFObject& nToNAPIDRootDDF =
            nPxDDF.AddChildObjectL(KNSmlDdfToNAPID); // ToNAPID 
    FillNodeInfoL(nToNAPIDRootDDF, aclTypesOnlyGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlDdfToNAPIDDesc);

    MSmlDmDDFObject& nToNAPIDDDF = nToNAPIDRootDDF.AddChildObjectGroupL(); // ToNAPID<x> 
    FillNodeInfoL(nToNAPIDDDF, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nToNAPIDLDDF = nToNAPIDDDF.AddChildObjectL(
            KNSmlDdfToNAPIDL); // ToNAPIDL (Get)
    FillNodeInfoL(nToNAPIDLDDF, aclTypesOnlyGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfToNAPIDLDesc);

    MSmlDmDDFObject& nPxStartpgDDF = nPxDDF.AddChildObjectL(KNSmlDdfStartpg); // Startpg
    FillNodeInfoL(nPxStartpgDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfStartpgDesc);

    MSmlDmDDFObject& nPxAuthInfRootDDF = nPxDDF.AddChildObjectL(
            KNSmlDdfPxAuthInf); // PxAuthInf 
    FillNodeInfoL(nPxAuthInfRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nPxAuthInfDDF = nPxAuthInfRootDDF.AddChildObjectGroupL(); // PxAuthInf/<x>
    FillNodeInfoL(nPxAuthInfDDF, aclTypesAddGet, MSmlDmDDFObject::EOneOrMore,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nPxAuthIdDDF = nPxAuthInfDDF.AddChildObjectL(
            KNSmlDdfPxAuthId); // PxAuthId
    FillNodeInfoL(nPxAuthIdDDF, aclTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfPxAuthIdDesc);

    MSmlDmDDFObject& nPxAuthPWDDF = nPxAuthInfDDF.AddChildObjectL(
            KNSmlDdfPxAuthPW); // PxAuthPW
    FillNodeInfoL(nPxAuthPWDDF, aclTypesAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlDdfPxAuthPWDesc);

    MSmlDmDDFObject& nNoPxForRootDDF =
            nPxDDF.AddChildObjectL(KNSmlDdfNoPxFor); // NoPxFor 
    FillNodeInfoL(nNoPxForRootDDF, aclTypesAddGet,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlDdfNoPxForDesc);

    MSmlDmDDFObject& nNoPxForDDF = nNoPxForRootDDF.AddChildObjectGroupL(); // NoPxFor<x> 
    FillNodeInfoL(nNoPxForDDF, aclTypesAddGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNullDesC8);

    MSmlDmDDFObject& nNoPxForLDDF = nNoPxForDDF.AddChildObjectL(
            KNSmlDdfNoPxForL); // NoPxForL (Get)
    FillNodeInfoL(nNoPxForLDDF, aclTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr,
            KNSmlDdfNoPxForLDesc);

    _DBG_FILE("CNSmlInternetAdapter::DDFStructureL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::LingerValueL()
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::LingerValueL(const TDesC8& aObject)
    {

    TBuf<100> desObj;
    CnvUtfConverter::ConvertToUnicodeFromUtf8(desObj, aObject);
    TInt err(KErrNone);
    //const TDesC& value = desObj;
    if (!Notalpha(desObj))
        {
        TLex lex(desObj);
        TUint Linger = 0;
        TInt lingerint;
        err = lex.Val(Linger, EDecimal);
        if (err == KErrNone)
            {
            lingerint = Linger;
            if (lingerint >= 0)
                {
                iLingerValue = lingerint;
                iLingerFlag = EFalse;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPAPNapdef::UdateligerValueL
// 
// -----------------------------------------------------------------------------
//
TBool CNSmlInternetAdapter::Notalpha(const TDesC& aValue)
    {
    TBool NotAlpha = EFalse;
    TChar character;
    for (TInt i = 0; i < aValue.Length(); i++)
        {
        character = aValue[i];
        NotAlpha = character.IsAlpha();
        if (NotAlpha)
            break;
        }

    return NotAlpha;
    }
// CNSmlInternetAdapter::AddLeafObjectL()
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::AddLeafObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, const TDesC8& aObject,
        const TDesC8& aType, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): begin");
    DBG_ARGS8(_S8("AP:add aURI AddLeafObjectL   - %S - %S"), &aURI,
            &aParentLUID);
    DBG_ARGS8(_S8("AP:Object %S"), &aObject);

    isAdd = ETrue;

    TInt parentLUID(0);
    if (aParentLUID.Length() <= 0)
        {
        if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound)
            {
            if (!GetProxyIdL(aURI))
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                return;
                }
            }
        if (!iLUID)
            iLUID = IntLUID(aParentLUID);
        parentLUID = GetAPIdFromURIL(aURI);
        if (parentLUID == 0)
            {
            if (IsAPUriFormatMatchPredefined(aURI))
                {
                iLUID = ConstructTreeL(aURI);
                parentLUID = GetAPIdFromURIL(aURI);
                }
            }

        }
    else
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): PParentLUID set to parentLUID");
        parentLUID = IntLUID(aParentLUID);
        }
    // Always buffer DNSAddr/<X>/*
    if ((parentLUID == 0 || aURI.Find(KNSmlDdfDNSPriority) >= 0 || aURI.Find(
            KNSmlDdfDNSAddrL) >= 0) && !iExecutingBuffer)
    // Update as add
        {
        iLeafType = EDMUnset; // Default for leaf handling
        AddLeafBufferL(aURI, aParentLUID, aObject, aType, aStatusRef);
        _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): end");
        return;
        }

    if (parentLUID == KNSmlIncompleteAP)
        {
        _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        return;
        }

    //check if Stale and add leaf buffer
    if (parentLUID > 0 && !iExecutingBuffer)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): parentLUID > 0 && !iExecutingBuffer");
        TUint32 apID = GetAPIdFromURIL(aURI);
        if (!APExistsL(apID))
            {
            AddLeafBufferL(aURI, aParentLUID, aObject, aType, aStatusRef);
            iStaleMapUpdate = ETrue;
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): end");
            return;
            }
        }
    TInt pushed = 0;
    //
    //  Check which field going to be handled
    //
    SetField(aURI);
    if ((iField->Compare(KNSmlDdfNAPLinger) == 0))
        {
        TUint32 apID = GetAPIdFromURIL(aURI);
        if (aObject.Size() == 0)
            {
            DeleteLingerL(apID);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            return;
            }
        LingerValueL(aObject);

        if (!iLingerFlag)
            {
            iLingerFlag = ETrue;
            TRAPD(error, AddLingerL(apID, iLingerValue));
            if (error == !KErrNone)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                }
            }
        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            }
        return;
        }

    //
    //  On leaf handling and add, must be checked that value not set before
    // 
    if (iLeafType != EDMUpdate && iLeafType != EDMDelete)
        {
        iLeafType = EDMAdd;
        }
    if (iStaleMapUpdate)
        {
        iLeafType = EDMAdd;
        }

    TPtrC qTable = TPtrC(KNullDesC);
    TPtrC qColumn = TPtrC(KNullDesC);
    TPtrC qDB = TPtrC(COMMDB_ID);

    if (aURI.Find(KNSmlDdfPortNbr) >= 0)
        {
        IsWapPort(aObject);
        }
    //
    //  Get Database field type 
    //

    if (parentLUID > 0)
        {
        // Get IAP nbr for servicetype
        TUint32 iapIDs = GetAPIdFromURIL(aURI);

        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL(): checkluid is %d and iapId %d "),
                parentLUID, iapIDs);

        //  IAP-table search serviceType (=Bearer)
        CCommsDbTableView* iapViews = iDatabase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), iapIDs);

        TBuf<KCommsDbSvrMaxFieldLength> serviceType;

        TInt errorCode = iapViews->GotoFirstRecord();

        // Read iapService from (IAP)
        if (errorCode == KErrNone)
            {
            iapViews->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
            CleanupStack::PopAndDestroy(); // iapViews
            qTable.Set(serviceType);
            }
        else
            {
            CleanupStack::PopAndDestroy(); // iapViews
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): ENotFound 1 end");
            return;
            }
        }

    if (iWlanSupported)
        {
        TBool wLANfield = IsWLANfield(aURI);
        if (wLANfield)
            {
            iWlanAdapter->AddLeafObjectL(aURI, aParentLUID, aObject, aType,
                    aStatusRef);
            CleanupStack::PopAndDestroy(pushed);
            return;
            }
        }

    TInt fType = GetAPFieldType(aURI);
    if (fType == EWrong) // Error if leaf not supported
        {
        _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        return;
        }

    TBool fieldOK = GetAPField(qTable, qColumn);
    if (!fieldOK) // OK if column for table not supported
        {
        _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EOk end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        return;
        }

    iISPId = parentLUID;
    if (aURI.Match(_L8("AP/*/Px/*/*")) != KErrNotFound)
        {
        if (iProxyId == 0)
            iProxyId = parentLUID;
        }

    if ((fType == CNSmlInternetAdapter::EStr) || (fType
            == CNSmlInternetAdapter::EInt && aURI.Find(KNSmlDdfGPRSPDP) >= 0)
            || // KNSmlDdfNAPAddrTy
            (fType == CNSmlInternetAdapter::EInt && aURI.Find(
                    KNSmlDdfNAPAddrTy) >= 0)) // KNSmlDdfNAPAddrTy
        { // Name + NAPAddr + DNSAddr +  Networks/*/Name

        if (aURI.Match(_L8("AP/*/Networks/*/Name")) != KErrNotFound)
            {
            TUint32 checkLUID = 0;
            TUint32 iapID1 = GetAPIdFromURIL(aURI);

            //  IAP-table NetworkId Update
            CCommsDbTableView* iapUpdate = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iapID1);
            TInt iapExists = iapUpdate->GotoFirstRecord();

            if (iapExists == KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL, Get network id");
                iapUpdate->ReadUintL(TPtrC(IAP_NETWORK), checkLUID);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL(): networks is %d"),
                        checkLUID);
                _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL, IAP exists");
                if (checkLUID > 0)
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL, network id > 0");
                    //check if network exist in NETWORK TABLE 
                    CCommsDbTableView* checknetView;
                    checknetView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                            NETWORK), TPtrC(COMMDB_ID), checkLUID);
                    TInt error = checknetView->GotoFirstRecord();
                    if (error == KErrNotFound)
                        {
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL: Network does not exist, create it");
                        AddNetworkL(checkLUID, aObject);
                        if (checkLUID > 0)
                            {
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL: network generation ok");
                            DBG_ARGS8(
                                    _S8(
                                            "CNSmlInternetAdapter::AddLeafObjectL(): network id is %d"),
                                    checkLUID);
                            //iapUpdate->WriteUintL(TPtrC(IAP_NETWORK),checkLUID); 
                            }
                        else
                            {
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL(): network generation failed");
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::EError);
                            return;
                            }
                        }
                    // Network was found, update the name to the given name.
                    else
                        {
                        if (checknetView->UpdateRecord() == KErrNone)
                            {
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL: network found, update name");
                            checknetView->WriteTextL(TPtrC(COMMDB_NAME),
                                    ConvertTo16LC(aObject));
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL: network name updated");
                            checknetView->PutRecordChanges();
                            CleanupStack::PopAndDestroy(); //ConvertTo16LC
                            }
                        // writing failed for some reason
                        else
                            {
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL(): network generation failed");
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::EError);
                            return;
                            }
                        }
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL: network ok");
                    CleanupStack::PopAndDestroy(); //checknetView
                    }
                }
            // IAP does not exist yet, create network, network id will be updated later using checkLUID
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL: network does not exist, creation 2");
                AddNetworkL(checkLUID, aObject);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL(): network id is %d"),
                        checkLUID);
                }
            CleanupStack::PopAndDestroy(); // iapUpdate
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL: network addition handled");

            //  IAP-table NetworkId Update
            CCommsDbTableView* iapView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iapID1);
            TInt iapFound = iapView->GotoFirstRecord();
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL: network id update begin");
            if (iapFound == KErrNone)
                {
                TBool iapReLock = EFalse;
                if (FeatureManager::FeatureSupported(
                        KFeatureIdSapPolicyManagement))
                    {
                    TBool apEnforce = EFalse;
                    TRAPD(eError,apEnforce=CheckEnforcementL())
                    DBG_ARGS8(_S8("CNSmlInternetAdapter::AddLeafObjectL(): EError %d , APEnforcement is %d"),eError,apEnforce);
                    if (eError == KErrNone && apEnforce)
                        {
                        DoLockIAPTablesL(EFalse);
                        ((CCommsDbProtectTableView*) iapView)->UnprotectRecord();
                        iWAPRelock = ETrue;
                        }
                    }

                if (IsIAPRecordLockedL(iapID1))
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                    TInt
                            reclockerr =
                                    ((CCommsDbProtectTableView*) iapView)->UnprotectRecord();
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        iapReLock = ETrue;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                    }

                if (iapView->UpdateRecord() == KErrNone)
                    {
                    iapView->WriteUintL(TPtrC(IAP_NETWORK), checkLUID);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObject(): checkLUID is %d"),
                            checkLUID);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObject, IAP_NETWORK set to checkLUID ");
                    iapView->PutRecordChanges();
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObject, iap found, set iNetworkId to checkLUID");
                    iNetworkId = checkLUID;
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                    }
                else
                    {
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    }

                if (FeatureManager::FeatureSupported(
                        KFeatureIdSapPolicyManagement))
                    {
                    if (iWAPRelock)
                        {
                        DoLockIAPTablesL(ETrue);
                        ((CCommsDbProtectTableView*) iapView)->ProtectRecord();
                        iWAPRelock = EFalse;
                        }
                    }
                if (iapReLock)
                    {
                    TInt reclockerr = DoProtectIAPRecordL(iapID1, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,Protecting IAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        iapReLock = EFalse;
                    }
                }
            else if (iapFound == KErrNotFound)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObject, iap not found, set iNetworkId");
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObject(): checkLUID is %d"),
                        checkLUID);
                iNetworkId = checkLUID;
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                _DBG_FILE("CNSmlInternetAdapter::AddLeafObject, iap found");
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                }

            CleanupStack::PopAndDestroy(); // iapView
            CleanupStack::PopAndDestroy(pushed);

            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): OK end");
            return;
            }

        else if (((aURI.Match(_L8("AP/*/Px/*/Name")) != KErrNotFound || // ProxyName
                aURI.Match(_L8("AP/*/Px/*/PxAddr")) != KErrNotFound || // ProxyAddr
                aURI.Match(_L8("AP/*/Px/*/DomainL")) != KErrNotFound) // DomainL (PROXY_PROTOCOL_NAME)
                && iLeafType == EDMUpdate)
                || ((aURI.Match(_L8("AP/*/Px/*/PxAddr")) != KErrNotFound
                        || aURI.Match(_L8("AP/*/Px/*/DomainL"))
                                != KErrNotFound) && iLeafType == EDMAdd))
            {
            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(PROXIES), TPtrC(COMMDB_ID), iProxyId);
            TBool proxyReLock = EFalse;
            if (IsProxyRecordLockedL(iProxyId))
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): proxy record is LOCKED");
                TInt
                        reclockerr =
                                ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting ProxyRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    proxyReLock = ETrue;
                }
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                }

            TInt errorCode = tableView->GotoFirstRecord();
            if (errorCode == KErrNone)
                {
                if (tableView->UpdateRecord() == KErrNone)
                    {
                    if (aURI.Match(_L8("AP/*/Px/*/PxAddr")) != KErrNotFound)
                        {
                        tableView->WriteLongTextL(TPtrC(PROXY_SERVER_NAME),
                                ConvertTo16LC(aObject));
                        pushed++;
                        }
                    else if (aURI.Match(_L8("AP/*/Px/*/DomainL"))
                            != KErrNotFound)
                        {
                        // Accepted values are (http, https, ftp, ftps)
                        _LIT8(KProxyProtocol, "http, https, ftp, ftps");

                        TInt protocolFound = 0;
                        if (aObject.Length() > 0)
                            {
                            protocolFound = KProxyProtocol().Find(aObject);
                            }
                        if (protocolFound == KErrNotFound && aObject.Length()
                                > 0)
                            {
                            CleanupStack::PopAndDestroy(); // tableView
                            CleanupStack::PopAndDestroy(pushed);
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::EError);
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                            return;
                            }
                        if (aObject.Length() == 0)
                            {
                            tableView->SetNullL(TPtrC(PROXY_PROTOCOL_NAME));
                            }
                        else
                            {
                            tableView->WriteTextL(TPtrC(PROXY_PROTOCOL_NAME),
                                    ConvertTo16LC(aObject));
                            CleanupStack::PopAndDestroy(); //ConvertTo16LC
                            }
                        }
                    tableView->PutRecordChanges();
                    CleanupStack::PopAndDestroy(); // tableView
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(); // tableView
                    CleanupStack::PopAndDestroy(pushed);
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                    return;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // tableView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): ENotFound end");
                return;
                }
            if (proxyReLock)
                {
                TInt reclockerr = DoProtectProxyRecordL(iProxyId, ETrue);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting Proxy returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    proxyReLock = EFalse;
                }
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EOk end");
            return;
            }

        if (iLeafType == EDMUpdate || (iLeafType == EDMAdd && (aURI.Find(
                KNSmlDdfNAPName) >= 0 && aURI.Find(KNSmlDdfNAPDef) >= 0)
                || aURI.Find(KNSmlDdfNAPAddr) >= 0 || aURI.Find(
                KNSmlDdfGPRSPDP) >= 0 || aURI.Find(KNSmlDdfDNSAddrL) >= 0
                || aURI.Find(KNSmlDdfAuthSecr) >= 0 || aURI.Find(
                KNSmlDdfDefGW) >= 0 || aURI.Find(KNSmlDdfNetworkMask) >= 0
                || aURI.Find(KNSmlDdfIPAddr) >= 0 || aURI.Find(
                KNSmlDdfNAPAddrTy) >= 0))
            {
            // Get IAP nbr for servicetype

            TUint32 iapID2 = GetAPIdFromURIL(aURI);

            //  IAP-table search serviceType (=Bearer)
            CCommsDbTableView
                    * serviceView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                            IAP), qDB, iapID2);

            TBuf<KCommsDbSvrMaxFieldLength> serviceType;

            TInt errorCode = serviceView->GotoFirstRecord();

            // Read serviceType from (IAP)
            if (errorCode == KErrNone)
                {
                serviceView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
                serviceView->ReadTextL(TPtrC(IAP_SERVICE_TYPE),
                        iProxyServiceType); // for locking

                serviceView->ReadUintL(TPtrC(IAP_SERVICE), iISPId);

                CleanupStack::PopAndDestroy(); // serviceView
                }
            else
                {
                CleanupStack::PopAndDestroy(); // serviceView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): ENotFound end");
                return;
                }
            qTable.Set(serviceType);

            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    serviceType, qDB, iISPId);

            errorCode = tableView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                TBool serviceReLock = EFalse;
                if (IsServiceRecordLockedL(iISPId))
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                    TInt
                            reclockerr =
                                    ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        serviceReLock = ETrue;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                    }

                if (tableView->UpdateRecord() != KErrNone)
                    {
                    //database locked
                    CleanupStack::PopAndDestroy(); // tableView
                    CleanupStack::PopAndDestroy(pushed);
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                    return;
                    }
                if (fType == CNSmlInternetAdapter::EInt)
                    {
                    TUint32 object32;
                    if (aURI.Find(KNSmlDdfGPRSPDP) >= 0 || /* GPRSPDP handling */
                    aURI.Find(KNSmlDdfNAPAddrTy) >= 0) /* NAPAddrTy handling */
                        {
                        if (aObject.MatchF(KNSmlDmApValIpv4) != KErrNotFound)
                            {
                            object32 = RPacketContext::EPdpTypeIPv4;
                            }
                        else if (aObject.MatchF(KNSmlDmApValIpv6)
                                != KErrNotFound)
                            {
                            object32 = RPacketContext::EPdpTypeIPv6;
                            }
                        else if (aObject.MatchF(_L8("PPP")) != KErrNotFound)
                            {
                            object32 = RPacketContext::EPdpTypePPP;
                            }
                        else
                            { // NotValid value
                            tableView->CancelRecordChanges();
                            CleanupStack::PopAndDestroy(); // tableView
                            CleanupStack::PopAndDestroy(pushed);
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::EError);
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                            return;
                            }
                        }
                    else
                        {
                        object32 = GetIntObject8(aObject);
                        }

                    tableView->WriteUintL(qColumn, object32);
                    }
                else
                    {
                    // This should be executed only when executing buffered cmds
                    if (aURI.Find(KNSmlDdfDNSAddrL) >= 0)
                    // DNS address 
                        {
                        TPtrC8 parentUri = RemoveLastSeg(aURI);
                        TInt dnsPri = 0;
                        TBool found = InitializeDNSParamsFromBuffL(parentUri,
                                dnsPri);
                        UriUtils::TUriHostType dnsAddrTy =
                                UriUtils::HostType(aObject);

                        if (!found)
                            {
                            UriUtils::TUriHostType dnsLuidType;
                            TInt dnsLuidPriority;
                            if (LuidToDns(dnsLuidType, dnsLuidPriority,
                                    parentLUID) != KErrNone)
                                {
                                if (dnsAddrTy == UriUtils::EIPv4Host)
                                    {
                                    iDnsIpv4Pri = (iDnsIpv4Pri % 2) + 1;
                                    dnsPri = iDnsIpv4Pri;
                                    }
                                else if (dnsAddrTy == UriUtils::EIPv6Host)
                                    {
                                    iDnsIpv6Pri = (iDnsIpv6Pri % 2) + 1;
                                    dnsPri = iDnsIpv6Pri;
                                    }
                                }
                            else
                                {
                                // update
                                dnsPri = dnsLuidPriority;
                                }

                            }
                        if (dnsPri != 0)
                            {
                            if (dnsPri == 1)
                                {
                                if (dnsAddrTy == UriUtils::EIPv4Host)
                                    {
                                    qColumn.Set(
                                            TPtrC(SERVICE_IP_NAME_SERVER1));
                                    }
                                else
                                    {
                                    qColumn.Set(TPtrC(
                                            SERVICE_IP6_NAME_SERVER1));

                                    }
                                }
                            else
                                {
                                if (dnsAddrTy == UriUtils::EIPv4Host)
                                    {
                                    qColumn.Set(
                                            TPtrC(SERVICE_IP_NAME_SERVER2));
                                    }
                                else
                                    {
                                    qColumn.Set(TPtrC(
                                            SERVICE_IP6_NAME_SERVER2));

                                    }
                                }
                            tableView->WriteTextL(qColumn, ConvertTo16LC(
                                    aObject));
                            pushed++;
                            TInt dnsLuid = DnsToLuid(dnsAddrTy, dnsPri);
                            iCallBack->SetMappingL(parentUri, SetIntObjectLC(
                                    dnsLuid));
                            pushed++;
                            }
                        else
                            {
                            CleanupStack::PopAndDestroy(); // tableView
                            CleanupStack::PopAndDestroy(pushed);
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::EError);
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                            return;
                            }

                        }

                    else
                        {
                        if ((serviceType == TPtrC(LAN_SERVICE))
                                && (iField->Compare(KNSmlDdfNAPAddr) == 0))
                            {
                            if (!IsValidIPv4AddressL(aObject))
                                {
                                iCallBack->SetStatusL(aStatusRef,
                                        CSmlDmAdapter::EInvalidObject);
                                _DBG_FILE(
                                        "CNSmlInternetAdapter::AddLeafObjectL(): EInvalidObject end");
                                CleanupStack::PopAndDestroy(); // tableView
                                CleanupStack::PopAndDestroy(pushed);
                                return;
                                }

                            }
                        tableView->WriteTextL(qColumn, ConvertTo16LC(aObject));
                        pushed++;

                        }
                    // DNSAddrL <> 0.0.0.0 or empty => 
                    if (aURI.Find(KNSmlDdfDNSAddrL) >= 0) /* DNS serviceFlag handling */
                        {
                        if (aObject.Match(_L8("fec0:0:0:ffff::1"))
                                == KErrNotFound || aObject.Match(_L8(
                                "fec0:0:0:ffff::2")) == KErrNotFound)
                            {
                            tableView->WriteBoolL(TPtrC(
                                    SERVICE_IP6_DNS_ADDR_FROM_SERVER), EFalse);
                            }
                        else
                            {
                            tableView->WriteBoolL(TPtrC(
                                    SERVICE_IP6_DNS_ADDR_FROM_SERVER), ETrue);
                            }

                        }
                    }
                TInt putOk = tableView->PutRecordChanges();
                if (putOk != KErrNone)
                    {
                    TInt retry = KBeginTransRetryCount;
                    while (retry > 0 && putOk == KErrLocked)
                        {
                        User::After(KBeginTransRetryDelay);
                        putOk = tableView->PutRecordChanges();
                        retry--;
                        }
                    if (putOk != KErrNone)
                        {
                        CleanupStack::PopAndDestroy(); // tableView
                        CleanupStack::PopAndDestroy(pushed);
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                        DBG_ARGS(
                                _S16(
                                        "CNSmlInternetAdapter::AddLeafObjectL(): Update failed with Code %d"),
                                putOk);
                        return;
                        }
                    }
                if (aURI.Find(KNSmlDdfNAPName) >= 0 && aURI.Find(
                        KNSmlDdfAuthName) <= 0) /* NAPName */
                    {
                    if (UpdateIAPnameL(iapID2, aObject) != KErrNone)
                        {
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);
                        }
                    }
                if (serviceReLock)
                    {
                    TInt reclockerr = DoProtectServiceRecordL(iISPId, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        serviceReLock = EFalse;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // tableView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): ENotFound end");
                return;
                }

            CleanupStack::PopAndDestroy(); // tableView
            }

        }
    else if (fType == CNSmlInternetAdapter::EBool)
        { // UseCB + UseIPSec
        TUint32 iapID3 = GetAPIdFromURIL(aURI);

        //  IAP-table search serviceType (=Bearer)
        CCommsDbTableView* boolView = iDatabase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), iapID3);
        TInt errorCode = boolView->GotoFirstRecord();

        TBuf<KCommsDbSvrMaxFieldLength> serviceType;

        errorCode = boolView->GotoFirstRecord();

        // Read serviceType from (IAP)
        if (errorCode == KErrNone)
            {
            boolView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
            boolView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), iProxyServiceType); // for locking
            boolView->ReadUintL(TPtrC(IAP_SERVICE), iISPId);
            }
        else
            {
            CleanupStack::PopAndDestroy(); // boolView
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): ENotFound end");
            return;
            }

        CleanupStack::PopAndDestroy(); // boolView

        qTable.Set(serviceType);
        TBool ret = GetAPField(qTable, qColumn);
        if (!ret)
            {
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EInvalidObject);
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): EInvalidObject end");
            return;
            }

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                serviceType, TPtrC(COMMDB_ID), iISPId);

        errorCode = tableView->GotoFirstRecord();

        if (errorCode == KErrNone)
            {
            TBool serviceReLock = EFalse;
            if (IsServiceRecordLockedL(iISPId))
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                TInt
                        reclockerr =
                                ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    serviceReLock = ETrue;
                }
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                }
            if (tableView->UpdateRecord() != KErrNone)
                {
                //database locked
                CleanupStack::PopAndDestroy(); // tableView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                return;
                }
            TBool tObject = EFalse;

            if (aObject.CompareF(KNSmlDmApValTrue) == 0)
                {
                tObject = ETrue;
                }
            if (aURI.Right(10).Compare(KNSmlDdfUsePTxtLog) == 0)
                {
                tObject = !tObject;
                }
            if (qColumn.Match(TPtrC(SERVICE_IP_ADDR_FROM_SERVER))
                    != KErrNotFound)
                {
                if (serviceType == TPtrC(LAN_SERVICE)) // Is WLAN service
                    {
                    if (tObject)
                        {
                        tableView->WriteTextL(TPtrC(
                                SERVICE_CONFIG_DAEMON_MANAGER_NAME),
                                KDaemonManagerName);
                        tableView->WriteTextL(TPtrC(
                                SERVICE_CONFIG_DAEMON_NAME),
                                KConfigDaemonName);
                        }
                    else
                        {
                        tableView->WriteTextL(TPtrC(
                                SERVICE_CONFIG_DAEMON_MANAGER_NAME),
                                KNullDesC);
                        tableView->WriteTextL(TPtrC(
                                SERVICE_CONFIG_DAEMON_NAME), KNullDesC);
                        }
                    }
                } // Is WLAN service

            tableView->WriteBoolL(qColumn, tObject);
            tableView->PutRecordChanges();
            CleanupStack::PopAndDestroy(); // tableView
            if (serviceReLock)
                {
                TInt reclockerr = DoProtectServiceRecordL(iISPId, ETrue);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting ServiceRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    serviceReLock = EFalse;
                }
            }
        else
            {
            tableView->CancelRecordChanges();
            CleanupStack::PopAndDestroy(); // tableView
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
            return;
            }
        }
    else if (fType == CNSmlInternetAdapter::EInt)
        { // UsePTxtLog + PortNbr + seamlessness and metadata
        iObject = GetIntObject8(aObject);

        if (aURI.Find(KNSmlDdfIAPSeamlessness) >= 0)
            {
            // TUint32 setSeam = GetIntObject8(aObject);
            TUint32 apMetaID = GetAPIdFromURIL(aURI);
            SetIAPSeamlessnessL(apMetaID, iObject, aStatusRef);
            return;
            }

        else if (aURI.Find(KNSmlDdfIAPMetaData) >= 0)
            {
            //  TUint32 setMeta = GetIntObject8(aObject);
            TUint32 apMetaID = GetAPIdFromURIL(aURI);
            SetIAPMetaDataL(apMetaID, iObject, aStatusRef);
            return;
            }

        if (aURI.Find(KNSmlDdfUsePTxtLog) >= 0)
            {
            TUint32 iapID4 = GetAPIdFromURIL(aURI);

            //  IAP-table search serviceType (=Bearer)
            CCommsDbTableView* iapView4 = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iapID4);
            TInt errorCode = iapView4->GotoFirstRecord();

            TBuf<KCommsDbSvrMaxFieldLength> serviceType;

            errorCode = iapView4->GotoFirstRecord();

            // Read serviceType from (IAP)
            if (errorCode == KErrNone)
                {
                iapView4->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
                iapView4->ReadTextL(TPtrC(IAP_SERVICE_TYPE),
                        iProxyServiceType); // for locking
                iapView4->ReadUintL(TPtrC(IAP_SERVICE), iISPId);
                CleanupStack::PopAndDestroy(); // iapView4
                }
            else
                {
                CleanupStack::PopAndDestroy(); // iapView4
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): ENotFound end");
                return;
                }

            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    serviceType, TPtrC(COMMDB_ID), iISPId);

            errorCode = tableView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                TBool serviceReLock = EFalse;
                if (IsServiceRecordLockedL(iISPId))
                    {

                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                    TInt
                            reclockerr =
                                    ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        serviceReLock = ETrue;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                    }
                if (tableView->UpdateRecord() != KErrNone)
                    {
                    //database locked
                    CleanupStack::PopAndDestroy(); // tableView
                    CleanupStack::PopAndDestroy(pushed);
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                    return;
                    }
                TBool tObject = EFalse;
                if (iObject)
                    {
                    tObject = ETrue; // <> 0
                    }
                tableView->WriteBoolL(TPtrC(SERVICE_DISABLE_PLAIN_TEXT_AUTH),
                        tObject);
                tableView->PutRecordChanges();
                if (serviceReLock)
                    {
                    TInt reclockerriap = DoProtectIAPRecordL(iapID4, ETrue);
                    TInt reclockerr = DoProtectServiceRecordL(iISPId, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAP returned code = %d"),
                            reclockerriap);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting ServiceRecord returned code = %d"),
                            reclockerr);
                    if ((reclockerr == KErrNone) && (reclockerriap == KErrNone))
                        serviceReLock = EFalse;
                    }
                CleanupStack::PopAndDestroy(); // tableView
                }
            else
                {
                tableView->CancelRecordChanges();
                CleanupStack::PopAndDestroy(); // tableView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                return;
                }
            }
        else if (aURI.Find(KNSmlDdfPortNbr) >= 0)
            {
            if (GetProxyIdL(aURI))
                {
                iProxyISP = GetAPIdFromURIL(aURI);

                CCommsDbTableView* tableView =
                        iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES),
                                TPtrC(COMMDB_ID), iProxyId);
                TInt errorCode = tableView->GotoFirstRecord();
                TBool proxyReLock = EFalse;
                // Read all columns needed.
                if (errorCode == KErrNone)
                    {
                    if (IsProxyRecordLockedL(iProxyId))
                        {
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): proxy record is LOCKED");
                        TInt
                                reclockerr =
                                        ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                        DBG_ARGS8(
                                _S8(
                                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting Proxy Record returned code = %d"),
                                reclockerr);
                        if (reclockerr == KErrNone)
                            proxyReLock = ETrue;
                        }
                    else
                        {
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                        }
                    if (tableView->UpdateRecord() != KErrNone)
                        {
                        //database locked
                        CleanupStack::PopAndDestroy(); // tableView
                        CleanupStack::PopAndDestroy(pushed);
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): Update failed end");
                        return;
                        }
                    iObject = GetIntObject8(aObject);

                    tableView->WriteUintL(TPtrC(PROXY_PORT_NUMBER), iObject);
                    }
                tableView->PutRecordChanges();
                if (proxyReLock)
                    {
                    TInt reclockerr = DoProtectProxyRecordL(iProxyId, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting Proxy Record returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        proxyReLock = EFalse;
                    }
                CleanupStack::PopAndDestroy(); // tableView
                }
            else
                {
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): PortNbr Error end");
                return;
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): PortNbr Error end");
            return;
            }

        }

    else if (fType == CNSmlInternetAdapter::EWap)
        { // Startpg + PxAuthId + PxAuthPW (WAP-settings) (BasAuthId + BasAuthPW)
        TUint32 iapID5 = GetAPIdFromURIL(aURI);
        //
        // Without iapID5 no insert possible, set on buffer
        //
        if (iapID5 < 1)
            {
            AddLeafBufferL(aURI, aParentLUID, aObject, aType, aStatusRef);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): end");
            return;
            }

        GetAPField(qTable, qColumn);
        if (WapAPExistsL(iapID5))
            { // Update handled
            TBool iapReLock = EFalse;
            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TBool apEnforce = EFalse;
                TRAPD(eError,apEnforce=CheckEnforcementL())
                DBG_ARGS8(_S8("CNSmlInternetAdapter::AddLeafObjectL(): EError %d , APEnforcement is %d"),eError,apEnforce);
                if (eError == KErrNone && apEnforce)
                    {
                    DoLockIAPTablesL(EFalse);
                    iWAPRelock = ETrue;
                    }
                }
            if (IsIAPRecordLockedL(iapID5))
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                TInt reclockerr = DoProtectIAPRecordL(iapID5, EFalse);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    iapReLock = ETrue;
                }
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                }
            CCommsDbTableView* wapView;
            TBool wapaccesspointTableLocked = EFalse;

            if (qColumn == TPtrC(WAP_START_PAGE))
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): record to be added WAP_START_PAGE, check for lock status");
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL Get WapAccessPoint record access for ID = %d"),
                        iWapId);
                if (IsWAPAccessPointRecordLockedL(iWapId))
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): wapaccesspoint record is LOCKED");
                    TInt reclockerr = DoProtectWAPAccessRecordL(iWapId,
                            EFalse);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        wapaccesspointTableLocked = ETrue;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): wapaccesspoint record is not write protected");
                    }
                wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                        WAP_ACCESS_POINT), TPtrC(COMMDB_ID), iWapId);
                pushed++;
                }
            else // Other WAP-data
                {
                wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                        WAP_IP_BEARER), TPtrC(WAP_ACCESS_POINT_ID), iWapId);
                pushed++;
                }

            TInt errorCode = wapView->GotoFirstRecord();

            // Read all columns needed.
            if (errorCode == KErrNone)
                {
                if (wapView->UpdateRecord() != KErrNone)
                    {
                    //database locked
                    CleanupStack::PopAndDestroy(pushed);
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): Wapview Update failed end");
                    if (wapaccesspointTableLocked)
                        {
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): wapaccesspoint record to be locked again ");
                        DoProtectWAPAccessRecordL(iWapId, ETrue);
                        wapaccesspointTableLocked = EFalse;
                        }
                    if (FeatureManager::FeatureSupported(
                            KFeatureIdSapPolicyManagement))
                        {
                        if (iWAPRelock)
                            {
                            DoLockIAPTablesL(ETrue);
                            iWAPRelock = EFalse;
                            }
                        }
                    if (iapReLock)
                        {
                        TInt reclockerr = DoProtectIAPRecordL(iapID5, ETrue);
                        DBG_ARGS8(
                                _S8(
                                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                                reclockerr);
                        if (reclockerr == KErrNone)
                            iapReLock = EFalse;
                        }
                    return;
                    }

                if (qColumn == TPtrC(WAP_PROXY_PORT))
                    {
                    TBool security = EFalse; // => 9200 + 9201, ETrue 9202 + 9203
                    TUint32 object32;
                    TPtrC object16 = ConvertTo16LC(aObject);
                    pushed++;
                    if (object16.Match(KWappPort9200) != KErrNotFound)
                        {
                        object32 = KWAPP_PORT_9200; // wappdef.h
                        }
                    else if (object16.Match(KWappPort9201) != KErrNotFound)
                        {
                        object32 = KWAPP_PORT_9201;
                        }
                    else if (object16.Match(KWappPort9202) != KErrNotFound)
                        {
                        object32 = KWAPP_PORT_9202;
                        security = ETrue;
                        }
                    else if (object16.Match(KWappPort9203) != KErrNotFound)
                        {
                        object32 = KWAPP_PORT_9203;
                        security = ETrue;
                        }
                    else
                        {
                        wapView->CancelRecordChanges();
                        CleanupStack::PopAndDestroy(pushed);
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                        if (FeatureManager::FeatureSupported(
                                KFeatureIdSapPolicyManagement))
                            {
                            if (iWAPRelock)
                                {
                                DoLockIAPTablesL(ETrue);
                                iWAPRelock = EFalse;
                                }
                            }
                        if (iapReLock)
                            {
                            TInt reclockerr = DoProtectIAPRecordL(iapID5,
                                    ETrue);
                            DBG_ARGS8(
                                    _S8(
                                            "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                                    reclockerr);
                            if (reclockerr == KErrNone)
                                iapReLock = EFalse;
                            }
                        return;
                        }
                    wapView->WriteUintL(qColumn, object32);
                    wapView->WriteBoolL(TPtrC(WAP_SECURITY), security);
                    }
                else
                    {
                    wapView->WriteTextL(TPtrC(qColumn),
                            ConvertTo16LC(aObject));
                    pushed++;
                    }
                }
            wapView->PutRecordChanges();
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): wapaccesspoint record Changes DONE successfully ");
            if (wapaccesspointTableLocked)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): wapaccesspoint record to be locked again ");
                TInt reclockerr = DoProtectWAPAccessRecordL(iWapId, ETrue);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,Protecting WAPAccessRecord %d , returned code = %d"),
                        iWapId, reclockerr);
                wapaccesspointTableLocked = EFalse;
                }
            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                if (iWAPRelock)
                    {
                    DoLockIAPTablesL(ETrue);
                    iWAPRelock = EFalse;
                    }
                }
            if (iapReLock)
                {
                TInt reclockerr = DoProtectIAPRecordL(iapID5, ETrue);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    iapReLock = EFalse;
                }
            CleanupStack::PopAndDestroy(pushed);
            pushed = 0;
            }
        else
            { // Insert handled
            CCommsDbTableView* insView;
            TInt wapInitOK = KErrCancel;
            TBool iapReLock = EFalse;

            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                TBool apEnforce = EFalse;
                TRAPD(eError,apEnforce=CheckEnforcementL())
                DBG_ARGS8(_S8("CNSmlInternetAdapter::AddLeafObjectL(): EError %d , APEnforcement is %d"),eError,apEnforce);
                if (eError == KErrNone && apEnforce)
                    {
                    DoLockIAPTablesL(EFalse);
                    iWAPRelock = ETrue;
                    }
                }
            if (iapReLock)
                {
                TInt reclockerr = DoProtectIAPRecordL(iapID5, ETrue);
                DBG_ARGS8(
                        _S8(
                                "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                        reclockerr);
                if (reclockerr == KErrNone)
                    iapReLock = EFalse;
                }
            insView = iDatabase->OpenTableLC(TPtrC(WAP_ACCESS_POINT));
            iWapId = 0;
            wapInitOK = insView->InsertRecord(iWapId);
            if (wapInitOK == KErrNone)
                {
                iWAPAccessPoint.Zero();
                iWapUserName.Zero();
                iWapPassword.Zero();

                if (qColumn == TPtrC(WAP_START_PAGE))
                    {
                    iWAPAccessPoint = aObject;
                    }
                else if (qColumn == TPtrC(WAP_PROXY_LOGIN_NAME))
                    {
                    iWapUserName = aObject;
                    }
                else if (qColumn == TPtrC(WAP_PROXY_LOGIN_PASS))
                    {
                    iWapPassword = aObject;
                    }

                iWapBearer = TPtrC(WAP_IP_BEARER);
                iWapName = FirstURISeg(aURI); // Name needed !!!

                wapInitOK = InitializeWAPIAPL(insView, iIAPName);
                if (wapInitOK == KErrNone)
                    {
                    insView->PutRecordChanges();
                    CleanupStack::PopAndDestroy(); // insView

                    CCommsDbTableView* updView;
                    TUint32 iWapId2 = 0;
                    iISPId = iapID5; // IAP number set
                    updView = iDatabase->OpenTableLC(TPtrC(WAP_IP_BEARER));
                    if (updView->InsertRecord(iWapId2) == KErrNone)
                        {
                        wapInitOK = InitializeWAPL(updView);
                        }
                    else
                        {
                        wapInitOK = KErrGeneral;
                        }
                    if (wapInitOK == KErrNone)
                        {
                        if (!PxExistsL(iapID5))
                            {

                            // Set mapping-data

                            TPtrC8 addURI = GetAddURISeg(aURI,
                                    KNSmlDdfStartpg);
                            TBuf8<16> addLUID;
                            _LIT8(KFormat, "%d");
                            addLUID.Format(KFormat, iapID5);
                            iCallBack->SetMappingL(addURI, addLUID);
                            }
                        else if (!NAPDefExistsL(iapID5))
                            {

                            // Set mapping-data

                            TPtrC8 addURI = GetAddURISeg(aURI,
                                    KNSmlDdfStartpg);
                            TBuf8<16> addLUID;
                            _LIT8(KFormat, "%d");
                            addLUID.Format(KFormat, iapID5);
                            iCallBack->SetMappingL(addURI, addLUID);
                            }

                        updView->PutRecordChanges();
                        CleanupStack::PopAndDestroy(); // updView
                        CleanupStack::PopAndDestroy(pushed);
                        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);

                        if (FeatureManager::FeatureSupported(
                                KFeatureIdSapPolicyManagement))
                            {
                            if (iWAPRelock)
                                {
                                DoLockIAPTablesL(ETrue);
                                iWAPRelock = EFalse;
                                }
                            }
                        if (iapReLock)
                            {
                            TInt reclockerr = DoProtectIAPRecordL(iapID5,
                                    ETrue);
                            DBG_ARGS8(
                                    _S8(
                                            "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                                    reclockerr);
                            if (reclockerr == KErrNone)
                                iapReLock = EFalse;
                            }
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): EOk end");
                        return;
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(); // updView
                        CleanupStack::PopAndDestroy(pushed);
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);

                        if (FeatureManager::FeatureSupported(
                                KFeatureIdSapPolicyManagement))
                            {
                            if (iWAPRelock)
                                {
                                DoLockIAPTablesL(ETrue);
                                iWAPRelock = EFalse;
                                }
                            }
                        if (iapReLock)
                            {
                            TInt reclockerr = DoProtectIAPRecordL(iapID5,
                                    ETrue);
                            DBG_ARGS8(
                                    _S8(
                                            "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                                    reclockerr);
                            if (reclockerr == KErrNone)
                                iapReLock = EFalse;
                            }
                        _DBG_FILE(
                                "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                        return;
                        }
                    }
                else
                    {
                    insView->CancelRecordChanges();
                    CleanupStack::PopAndDestroy(); // insView
                    CleanupStack::PopAndDestroy(pushed);
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);

                    if (FeatureManager::FeatureSupported(
                            KFeatureIdSapPolicyManagement))
                        {
                        if (iWAPRelock)
                            {
                            DoLockIAPTablesL(ETrue);
                            iWAPRelock = EFalse;
                            }
                        }
                    if (iapReLock)
                        {
                        TInt reclockerr = DoProtectIAPRecordL(iapID5, ETrue);
                        DBG_ARGS8(
                                _S8(
                                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                                reclockerr);
                        if (reclockerr == KErrNone)
                            iapReLock = EFalse;
                        }
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                    return;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // insView
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                if (FeatureManager::FeatureSupported(
                        KFeatureIdSapPolicyManagement))
                    {
                    if (iWAPRelock)
                        {
                        DoLockIAPTablesL(ETrue);
                        iWAPRelock = EFalse;
                        }
                    }
                if (iapReLock)
                    {
                    TInt reclockerr = DoProtectIAPRecordL(iapID5, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        iapReLock = EFalse;
                    }
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                return;
                }
            }
        }
    else if (fType == CNSmlInternetAdapter::ESpec)
        { // BearerL + NoPxForL + IAPService + Direction
        if (aURI.Find(KNSmlDdfDirection) >= 0)
            {
            _LIT8(KDirection, "Outgoing");
            if (KDirection().Find(aObject) != KErrNotFound)
                {
                iDirection = ECommDbConnectionDirectionOutgoing;
                }
            else
                {
                iDirection = ECommDbConnectionDirectionIncoming;
                if (iBearer == TPtrC(OUTGOING_GPRS))
                    {
                    iBearer = TPtrC(INCOMING_GPRS);
                    }
                }
            }
        else if (aURI.Find(KNSmlDdfIAPService) >= 0)
            {
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
            return;
            }
        else if (aURI.Find(KNSmlDdfNoPxForL) >= 0) /* No proxies used field handling */
            {
            TBool proxyFound = GetProxyIdL(aURI);
            if (!proxyFound)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): EError end");
                CleanupStack::PopAndDestroy(pushed);
                return;
                }

            CCommsDbTableView* specView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(PROXIES), TPtrC(COMMDB_ID), iProxyId);
            TInt errorCode = specView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                TBool proxyReLock = EFalse;
                if (IsProxyRecordLockedL(iProxyId))
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
                    TInt
                            reclockerr =
                                    ((CCommsDbProtectTableView*) specView)->UnprotectRecord();
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting ProxyAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        proxyReLock = ETrue;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
                    }
                if (specView->UpdateRecord() == KErrNone)
                    {
                    //database not locked
                    specView->WriteLongTextL(TPtrC(PROXY_EXCEPTIONS),
                            ConvertTo16LC(aObject));
                    specView->PutRecordChanges();
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                    CleanupStack::PopAndDestroy(); // ConvertTo16LC
                    }
                else
                    {
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    }
                if (proxyReLock)
                    {
                    TInt reclockerr = DoProtectProxyRecordL(iProxyId, ETrue);
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting ProxyAccessRecord returned code = %d"),
                            reclockerr);
                    if (reclockerr == KErrNone)
                        proxyReLock = EFalse;
                    }
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                }
            CleanupStack::PopAndDestroy(); // specView

            CleanupStack::PopAndDestroy(pushed);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
            return;
            }
        else if (aURI.Find(KNSmlDdfBearerL) >= 0) /* Bearer handling */
            {
            if (aObject.Match(KNSmlDmApValGsmGprs) != KErrNotFound)
                {
                if (iDirection == ECommDbConnectionDirectionIncoming)
                    {
                    iBearer = TPtrC(INCOMING_GPRS);
                    }
                else
                    {
                    iBearer = TPtrC(OUTGOING_GPRS);
                    }
                }

            else if (aObject.Match(KNSmlDmApVal3Gppps) != KErrNotFound)
                {
                i3GPPPS = ETrue;
                if (iDirection == ECommDbConnectionDirectionIncoming)
                    {
                    iBearer = TPtrC(INCOMING_GPRS);
                    }
                else
                    {
                    iBearer = TPtrC(OUTGOING_GPRS);
                    }
                }
            else if (aObject.Match(KNSmlDmApValVpn) != KErrNotFound)
                {
                iBearer = TPtrC(VPN_SERVICE);
                }
            else if ((aObject.Match(KNSmlDmApValWlan) != KErrNotFound)
                    && iWlanSupported)
                {
                iBearer = TPtrC(LAN_SERVICE);
                }
            else
                {
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddLeafObjectL(): UNKNOWN Bearer Error end");
                return;
                }
            }
        else if (aURI.Find(KNSmlDdfNAPName) >= 0)
            {
            iIAPName.Copy(aObject);
            TUint32 iapID2 = GetAPIdFromURIL(aURI);
            if (iapID2 > 0 && UpdateIAPnameL(iapID2, aObject) != KErrNone)
                {
                CleanupStack::PopAndDestroy(pushed);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            }
        }

    CleanupStack::PopAndDestroy(pushed);
    pushed = 0;

    //  Update CommsDB IAP + insert iBearer Table

    if (IsInsertAllowedL() && iLeafType == EDMAdd && fType
            == CNSmlInternetAdapter::ESpec) //iIAPExists  
        {
        TInt initOK = KErrCancel;

        iISPId = GetMaxIdL() + 1; // For new data

        SaveIAPDataL(aURI); // Level 1 Mapping inside

        CCommsDbTableView* newAPView;

        newAPView = iDatabase->OpenTableLC(iBearer);
        // 3GPPPS  = OUTGOING/INCOMING_GPRS
        if (iBearer == TPtrC(OUTGOING_GPRS) || iBearer
                == TPtrC(INCOMING_GPRS))
            {
            if (newAPView->InsertRecord(iISPId) == KErrNone)
                {
                initOK = InitializeGPRSL(newAPView);
                }
            else
                {
                initOK = KErrGeneral;
                }
            }

        else if (iBearer == TPtrC(VPN_SERVICE))
            {
            if (newAPView->InsertRecord(iISPId) == KErrNone)
                {
                initOK = InitializeVPNServiceL(newAPView);
                }
            else
                {
                initOK = KErrGeneral;
                }
            }
        else if ((iBearer == TPtrC(LAN_SERVICE)) && iWlanSupported)
            {
            if (newAPView->InsertRecord(iISPId) == KErrNone)
                {
                initOK = InitializeLANServiceL(newAPView);
                }
            else
                {
                initOK = KErrGeneral;
                }
            }
        else
            {
            initOK = KErrCancel;
            }

        if (initOK == KErrNone)
            {
            TUint32 newId = 0;
            newAPView->PutRecordChanges();
            newAPView->ReadUintL(TPtrC(COMMDB_ID), newId);

            // To update right Id

            iISPId = newId;

            SaveIAPDataL(aURI);

            TUint32 iapID2 = GetAPIdFromURIL(aURI);
            UpdateIAPnameL(iapID2, iIAPName);
            //
            // Set mapping-data (levels 2 and 3 )
            //
            TPtrC8 addURI = GetAddURISeg(aURI, KNSmlDdfNAPDef);

            TBuf8<16> addLUID;
            _LIT8(KFormat, "%d");
            addLUID.Format(KFormat, newId);

            // Also NAPDef  (level 2)
            iCallBack->SetMappingL(addURI, addLUID);
            DBG_ARGS8(_S8("AP:level2 URI - <%S> <%S> <%d>"), &addURI,
                    &addLUID, newId);

            // Also BearerL (level 3)
            TPtrC8 addURI2 = GetAddURISeg(aURI, KNSmlDdfBearer);
            if (i3GPPPS)
                {
                newId = KNSmlAp3gpppsLowerBase + newId;
                addLUID.Format(KFormat, newId);
                i3GPPPS = EFalse;
                }
            iCallBack->SetMappingL(addURI2, addLUID);
            DBG_ARGS8(_S8("AP:level3 URI - <%S> <%S> <%d>"), &addURI2,
                    &addLUID, newId);
            }
        else
            {
            if (initOK == KErrCancel)
                {
                newAPView->CancelRecordChanges();
                }
            CleanupStack::PopAndDestroy(newAPView);
            CleanupStack::PopAndDestroy(pushed);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): EError end");
            return;
            }

        CleanupStack::PopAndDestroy(newAPView);
        }
    else if (IsInsertAllowedL() && iLeafType == EDMAdd && aURI.Find(
            KNSmlDdfBearerL) >= 0)

        {
        CleanupStack::PopAndDestroy(pushed);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EAlreadyExists);
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): EAlreadyExists BearerL end");
        return;
        }
    CleanupStack::PopAndDestroy(pushed);
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
    _DBG_FILE("CNSmlInternetAdapter::AddLeafObjectL(): OK end");
    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType )
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::UpdateLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType,
        const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::UpdateLeafObjectL(): begin");

    DBG_ARGS8(_S8("AP:update aURI UpdateLeafObjectL   - %S - %S"), &aURI,
            &aLUID);
    DBG_ARGS8(_S8("AP:Object %S"), &aObject);
    if (iTransactionCommitted)
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("UpdateLeafObjectL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("UpdateLeafObjectL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        iTransactionCommitted = EFalse;
        }
    if ((aURI.Match(_L8("*/WLAN*")) != KErrNotFound) && iWlanSupported)
        {
        AddLeafBufferL(aURI, aLUID, aObject, aType, aStatusRef, ETrue);
        iDatabase->CommitTransaction();
        iTransactionCommitted = ETrue;
        _DBG_FILE("CNSmlInternetAdapter::UpdateLeafObjectL(): WLAN end");

        return;
        }
    //
    //  No direct update allowed only update as add
    //
    if (aLUID.Length() > 0 && aURI.Find(KNSmlDdfNAPDef) >= 0)
    /*( aURI.Find(KNSmlDdfBearerL)   >= 0 ||
     aURI.Find(KNSmlDdfDirection) >= 0 || 
     aURI.Find(KNSmlDdfNAPName) >= 0 ) */
        {
        _DBG_FILE(
                "Bearer | direction CNSmlInternetAdapter::UpdatelaefObject ( ): EError end");

        AddLeafBufferL(aURI, aLUID, aObject, aType, aStatusRef, EFalse);
        iDatabase->CommitTransaction();
        iTransactionCommitted = ETrue;
        return;
        }

    iLeafType = EDMUpdate;

    TRAP_IGNORE(AddLeafObjectL(aURI, aLUID, aObject, aType, aStatusRef));
    iDatabase->CommitTransaction();
    iTransactionCommitted = ETrue;
    //
    //  If try to update field that has value which is not default errorvalue should be returned
    //
    iLeafType = EDMUnset;

    _DBG_FILE("CNSmlInternetAdapter::UpdateLeafObjectL(): end");

    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType )
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::UpdateLeafObjectL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/,
        const TDesC8& /*aType*/, TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::UpdateLeafObjectL(RWriteStream): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlInternetAdapter::UpdateLeafObjectL(RWriteStream): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::DeleteObjectL( const TDesC& aURI, const TDesC& aLUID, 
// const TInt aStatusRef )
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::DeleteObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL( ): begin");
    if (iTransactionCommitted)
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("DeleteObjectL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("DeleteObjectL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        iTransactionCommitted = EFalse;
        }

    if ((aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*")) != KErrNotFound)
            && iWlanSupported)
        {
        iWlanAdapter->DeleteObjectL(aURI, aLUID, aStatusRef);
        _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL( ): end");
        return;
        }

    //
    //  Check which field going to be handled
    //
    SetField(aURI);

    iLUID = IntLUID(aLUID);

    TUint32 checkLUID = IntLUID(aLUID);
    //
    //  Check which kind node to be deleted (Leaf delete not supported)
    //
    if (aURI.Match(_L8("AP/*")) != KErrNotFound && aURI.Match(_L8("AP/*/*"))
            == KErrNotFound)
        {
        // Check if AP exists
        if (!APExistsL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): ENotFound end");
            return;
            }
        if (DeleteAPL(checkLUID))
            {
            iPrevURI->Des().Format(KNullDesC8);
            iPrevLUID = 0;
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EOk end");
            return;
            }
        else
            {
            iPrevURI->Des().Format(KNullDesC8);
            iPrevLUID = 0;
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EError end");
            return;
            }
        }

    if (aURI.Match(_L8("AP/*/Px/*/Startpg")) != KErrNotFound) //Key for WAP-delete  
        {
        // Check if WAPAp exists
        if (!WapAPExistsL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): ENotFound end");
            return;
            }
        if (DeleteWapAPL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EOk end");
            return;
            }
        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EError end");
            return;
            }
        }

    if (aURI.Match(_L8("AP/*/NAPDef/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/NAPDef/*/*")) == KErrNotFound)
        {
        // Check if NAPDef exists
        if (!NAPDefExistsL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): ENotFound end");
            return;
            }
        if (DeleteNAPDefL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EOk end");
            return;
            }
        }

    if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Px/*/*")) == KErrNotFound)
        {
        // Check if PX exists
        if (!PxExistsL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): ENotFound end");
            return;
            }
        if (DeleteProxyL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL(): EOk end");
            return;
            }
        }

    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlInternetAdapter::DeleteObjectL( ): end");
    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::FetchLeafObjectSizeL()
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::FetchLeafObjectSizeL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef,
        TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectSizeL(): begin");

    DBG_ARGS8(_S8("AP:Fetch aURI - %S - %S"), &aURI, &aLUID);
    CBufBase *lObject = CBufFlat::NewL(128);
    CleanupStack::PushL(lObject);

    // call to internal fetchleafobj.
    CSmlDmAdapter::TError status = FetchLeafObjectL(aURI, aLUID, aType,
            *lObject);

    if (status == CSmlDmAdapter::EOk)
        {
        lObject->Compress();
        TBuf8<8> size;
        size.Num(lObject->Size());
        lObject->Reset();
        lObject->InsertL(0, size);
        iCallBack->SetResultsL(aResultsRef, *lObject, aType);
        }
    iCallBack->SetStatusL(aStatusRef, status);

    CleanupStack::PopAndDestroy(lObject);
    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectSizeL(): end");

    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::FetchLeafObjectL()
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::FetchLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aType, const TInt aResultsRef,
        const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectL(): begin");

    DBG_ARGS8(_S8("AP:Fetch aURI - %S - %S"), &aURI, &aLUID);
    CBufBase *lObject = CBufFlat::NewL(128);
    CleanupStack::PushL(lObject);

    // Add call to internal fetchleafobj
    CSmlDmAdapter::TError status = FetchLeafObjectL(aURI, aLUID, aType,
            *lObject);
    if (status == CSmlDmAdapter::EOk)
        {
        iCallBack->SetResultsL(aResultsRef, *lObject, aType);
        }
    iCallBack->SetStatusL(aStatusRef, status);

    CleanupStack::PopAndDestroy(); //lObject 
    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectL(): end");
    return;
    }

//------------------------------------------------------------------------------
// CSmlDmInternetAdapter::FetchLeafObjectL()
//------------------------------------------------------------------------------

CSmlDmAdapter::TError CNSmlInternetAdapter::FetchLeafObjectL(
        const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType,
        CBufBase& aObject)
    {
    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectL(internal)(): begin");

    TInt pushed = 0;
    TUint32 iapID = 0;
    TBool treeConstructed(EFalse);
    //
    // If no proxy no fetch allowed 
    //
    if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound)
        {
        if (!GetProxyIdL(aURI))
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
            return CSmlDmAdapter::ENotFound;
            }
        }
    if (!iLUID)
        iLUID = IntLUID(aLUID);

    iapID = GetAPIdFromURIL(aURI);

    if (iapID == 0)
        {
        if (IsAPUriFormatMatchPredefined(aURI))
            {
            iLUID = ConstructTreeL(aURI);
            iapID = GetAPIdFromURIL(aURI);
            treeConstructed = ETrue;
            }

        }
    // Check if values for Given LUID
    if (!iLUID && (!IsWLANfield(aURI) || !iWlanSupported))
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
        return CSmlDmAdapter::ENotFound;
        }
    //
    //  Check which field going to be handled
    //
    SetField(aURI);

    aObject.Reset();

    //Linger value 
    if ((iField->Compare(KNSmlDdfNAPLinger) == 0))
        {
        TInt lingerInterval;
        TRAPD(err, GetLingerL(iapID, lingerInterval));
        if (err == KErrNotFound)
            {

            return CSmlDmAdapter::ENotFound;
            }
        else if (err != KErrNone)
            {

            return CSmlDmAdapter::EError;

            }
        else
            {
            TBuf<10> slinger;
            slinger.AppendNum(lingerInterval);

            aObject.InsertL(aObject.Size(), ConvertTo8LC(slinger));
            CleanupStack::PopAndDestroy();
            return CSmlDmAdapter::EOk;
            }
        }
    TPtrC qTable = TPtrC(KNullDesC);
    TPtrC qColumn = TPtrC(KNullDesC);
    TPtrC qDB = TPtrC(COMMDB_ID);

    if (aURI.Find(KNSmlDdfPortNbr) >= 0) // Check if WAP or PROXY port asked
        {
        _LIT8(KWap, "/WAP");
        if (aURI.Find(KWap) >= 0)
            {
            iWapPort = ETrue;
            }
        else
            {
            iWapPort = EFalse;
            }
        }

    if (iWlanSupported)
        {
        TBool wLANfield = IsWLANfield(aURI);
        if (wLANfield)
            {
            //
            //      WLAN-adapter handles all WLAN fields
            //
            return iWlanAdapter->FetchLeafObjectL(aURI, aLUID, aType, aObject);
            }
        }

    //
    //  Get Database field type 
    //
    TInt fType = GetAPFieldType(aURI);

    if (fType == EWrong)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EError end");
        return CSmlDmAdapter::EError;
        }
    //
    iISPId = iLUID;
    //
    //  IAP-table search serviceType (=Bearer)

    CCommsDbTableView* serviceView = iDatabase->OpenViewMatchingUintLC(TPtrC(
            IAP), qDB, iapID);

    TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TUint32 serviceId = 0;

    TInt errorCode = serviceView->GotoFirstRecord();

    // Read serviceType from (IAP)
    if (errorCode == KErrNone)
        {
        serviceView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        serviceView->ReadUintL(TPtrC(IAP_SERVICE), serviceId);
        CleanupStack::PopAndDestroy(); // serviceView
        }
    else
        {
        CleanupStack::PopAndDestroy(); // serviceView
        _DBG_FILE(
                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
        CleanupStack::PopAndDestroy(pushed);
        return CSmlDmAdapter::ENotFound;
        }
    qTable.Set(serviceType);

    TBool allowed = GetAPField(qTable, qColumn);

    if (!allowed)
        {
        if (aURI.Find(KNSmlDdfNAPAddrTy) >= 0) /* NAPAddrTy handling */
            {
            if (serviceType == TPtrC(OUTGOING_GPRS) || serviceType == TPtrC(
                    INCOMING_GPRS))
                {
                _LIT8(KApn, "APN");
                aObject.InsertL(aObject.Size(), KApn);

                }
            CleanupStack::PopAndDestroy(pushed);
            return CSmlDmAdapter::EOk;

            }
        _DBG_FILE(
                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): FieldNot allowed");
        CleanupStack::PopAndDestroy(pushed);
        return CSmlDmAdapter::EError;
        }

    if (fType == CNSmlInternetAdapter::EStr)
        { // Name + NAPAddr + DNSAddrL
        if (aURI.Match(_L8("AP/*/Networks/*/Name")) != KErrNotFound) // Networks-name
            {
            // TUint32 iapID8 = IntLUID(aLUID);

            //  IAP-table NetworkId Fetch
            CCommsDbTableView* networkView =
                    iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(
                            COMMDB_ID), iapID);
            errorCode = networkView->GotoFirstRecord();
            if (errorCode == KErrNone)
            {
            	TRAPD(leavecode, networkView->ReadUintL(TPtrC(IAP_NETWORK),
                    iISPId));
            	CleanupStack::PopAndDestroy(); // networkView
            	if (leavecode != 0)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                CleanupStack::PopAndDestroy(pushed);
                return CSmlDmAdapter::ENotFound;
                }
             }
             else
             	{
             		CleanupStack::PopAndDestroy(); // networkView
             		_DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                CleanupStack::PopAndDestroy(pushed);
                return CSmlDmAdapter::ENotFound;
             	}

            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(NETWORK), TPtrC(COMMDB_ID), iISPId);

            errorCode = tableView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                TBuf<KCommsDbSvrMaxFieldLength> columnValue;

                tableView->ReadTextL(qColumn, columnValue);

                aObject.InsertL(aObject.Size(), ConvertTo8LC(columnValue));
                pushed++;
                }
            else
                {
                CleanupStack::PopAndDestroy(); // tableView
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                CleanupStack::PopAndDestroy(pushed);
                return CSmlDmAdapter::ENotFound;
                }
            }
        else if (aURI.Match(_L8("AP/*/Px/*/Name")) != KErrNotFound || // ProxyName
                aURI.Match(_L8("AP/*/Px/*/PxAddr")) != KErrNotFound || // ProxyAddr
                aURI.Match(_L8("AP/*/Px/*/DomainL")) != KErrNotFound) // DomainL (PROXY_PROTOCOL_NAME)
            {
            CCommsDbTableView* proxyView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(PROXIES), TPtrC(COMMDB_ID), iProxyId);

            errorCode = proxyView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                if (aURI.Match(_L8("AP/*/Px/*/DomainL")) != KErrNotFound)
                    {
                    TBuf<KCommsDbSvrMaxFieldLength> columnValue;

                    proxyView->ReadTextL(TPtrC(PROXY_PROTOCOL_NAME),
                            columnValue);

                    aObject.InsertL(aObject.Size(), ConvertTo8LC(columnValue));
                    pushed++;
                    }

                else if (aURI.Match(_L8("AP/*/Px/*/PxAddr")) != KErrNotFound)
                    {
                    HBufC* serverName = proxyView->ReadLongTextLC(TPtrC(
                            PROXY_SERVER_NAME));
                    pushed++;

                    aObject.InsertL(aObject.Size(), ConvertTo8LC(*serverName));
                    pushed++;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // proxyView
                CleanupStack::PopAndDestroy(pushed);
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                return CSmlDmAdapter::ENotFound;
                }
            }
        else
            {
            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    serviceType, qDB, serviceId);

            errorCode = tableView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                if (aURI.Find(KNSmlDdfDNSPriority) >= 0) /* DNSPriority */
                    {
                    UriUtils::TUriHostType theType;
                    TInt dnsPri;

                    // This resets iLUID to match aLUID, which is needed in this case.
                    // Otherwise iLUID has a different value
                    if (!treeConstructed)
                        iLUID = IntLUID(aLUID);

                    if (LuidToDns(theType, dnsPri, iLUID) == KErrNone)
                        {
                        if (dnsPri == 1)
                            {
                            aObject.InsertL(aObject.Size(), KNSmlFirstDNSPri);
                            }
                        else
                            {
                            aObject.InsertL(aObject.Size(), KNSmlSecondDNSPri);
                            }
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(tableView); // tableView
                        CleanupStack::PopAndDestroy(pushed);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                        return CSmlDmAdapter::ENotFound;
                        }
                    }
                else if (aURI.Find(KNSmlDdfDNSAddrTy) >= 0) /* DNSAddrTy */
                    {
                    UriUtils::TUriHostType theType;
                    TInt dnsPri;
                    // This resets iLUID to match aLUID, which is needed in this case.
                    // Otherwise iLUID has a different value
                    if (!treeConstructed)
                        iLUID = IntLUID(aLUID);
                    if (LuidToDns(theType, dnsPri, iLUID) == KErrNone)
                        {
                        if (theType == UriUtils::EIPv4Host)
                            {
                            aObject.InsertL(aObject.Size(), KNSmlDNSIp);
                            }
                        else
                            {
                            aObject.InsertL(aObject.Size(), KNSmlDNSIp6);
                            }
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(tableView); // tableView
                        CleanupStack::PopAndDestroy(pushed);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                        return CSmlDmAdapter::ENotFound;
                        }
                    }
                else
                    {
                    TBuf<KCommsDbSvrMaxFieldLength> columnValue;
                    if (aURI.Find(KNSmlDdfDNSAddrL) >= 0)
                        {
                        UriUtils::TUriHostType theType;
                        TInt dnsPri;

                        // This resets iLUID to match aLUID, which is needed in this case.
                        // Otherwise iLUID has a different value
                        if (!treeConstructed)
                            iLUID = IntLUID(aLUID);
                        if (LuidToDns(theType, dnsPri, iLUID) == KErrNone)
                            {
                            if (dnsPri == 1)
                                {
                                if (theType == UriUtils::EIPv4Host)
                                    {
                                    qColumn.Set(
                                            TPtrC(SERVICE_IP_NAME_SERVER1));
                                    }
                                else
                                    {
                                    qColumn.Set(TPtrC(
                                            SERVICE_IP6_NAME_SERVER1));

                                    }
                                }
                            else
                                {
                                if (theType == UriUtils::EIPv4Host)
                                    {
                                    qColumn.Set(
                                            TPtrC(SERVICE_IP_NAME_SERVER2));
                                    }
                                else
                                    {
                                    qColumn.Set(TPtrC(
                                            SERVICE_IP6_NAME_SERVER2));

                                    }
                                }
                            }
                        else
                            {
                            CleanupStack::PopAndDestroy(tableView); // tableView
                            CleanupStack::PopAndDestroy(pushed);
                            _DBG_FILE(
                                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                            return CSmlDmAdapter::ENotFound;
                            }
                        }
                    TRAPD(leavecode, tableView->ReadTextL(qColumn,
                            columnValue));
                    if (leavecode != 0)
                        {
                        CleanupStack::PopAndDestroy(); // tableView
                        CleanupStack::PopAndDestroy(pushed);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                        return CSmlDmAdapter::ENotFound;
                        }
                    aObject.InsertL(aObject.Size(), ConvertTo8LC(columnValue));
                    pushed++;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // tableView
                CleanupStack::PopAndDestroy(pushed);
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                return CSmlDmAdapter::ENotFound;
                }
            }
        CleanupStack::PopAndDestroy(); // tableView
        }
    else if (fType == CNSmlInternetAdapter::EBool)
        { // UseCB
        CCommsDbTableView* boolView = iDatabase->OpenViewMatchingUintLC(
                serviceType, qDB, serviceId);

        errorCode = boolView->GotoFirstRecord();

        TBool objectBool = EFalse;
        if (errorCode == KErrNone)
            {
            TRAPD(leavecode, boolView->ReadBoolL(qColumn, objectBool));
            if (leavecode == KErrUnknown) // value is null
                {
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(); // boolView
            CleanupStack::PopAndDestroy(pushed);
            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EError end");
            return CSmlDmAdapter::EError;
            }

        if (objectBool)
            {
            aObject.InsertL(aObject.Size(), KNSmlDmApValTrue);
            }
        else
            {
            aObject.InsertL(aObject.Size(), KNSmlDmApValFalse);
            }

        CleanupStack::PopAndDestroy(); // boolView
        }
    else if (fType == CNSmlInternetAdapter::EInt)
        { // NAPId +Bearer + NAPAddrTy + PxID + PortNbr + UsePTxtLog
        // CBTy + LnkSpeed

        if (aURI.Match(_L8("AP/*/Px/*/PxID")) != KErrNotFound)
            {
            aObject.InsertL(aObject.Size(), SetIntObjectLC(iISPId));
            pushed++;
            }

        else if (aURI.Find(KNSmlDdfIAPSeamlessness) >= 0) /* Seamlessness handling */
            {
            TInt seam = GetIAPSeamlessnessL(iapID);
            aObject.InsertL(aObject.Size(), SetIntObjectLC(seam));
            pushed++;
            }
        else if (aURI.Find(KNSmlDdfIAPMetaData) >= 0)
            {
            TInt meta = GetIAPMetaDataL(iapID);
            aObject.InsertL(aObject.Size(), SetIntObjectLC(meta));
            pushed++;
            }

        else if (aURI.Match(_L8("AP/*/Networks/*/ID")) != KErrNotFound)
            {

            iISPId = IntLUID(aLUID);

            CCommsDbTableView* nwidView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iISPId);
            if (nwidView->GotoFirstRecord() == KErrNone)
            {						
            	nwidView->ReadUintL(TPtrC(IAP_NETWORK), iISPId); 
            	aObject.InsertL(aObject.Size(), SetIntObjectLC(iISPId));
            	pushed++;
            }
            CleanupStack::PopAndDestroy(); // nwidView
          	}
        else if (aURI.Match(_L8("AP/*/Px/*/PortNbr")) != KErrNotFound)
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): PortNbr");
            DBG_ARGS8(_S8("iISPId = %d "), iISPId);
            DBG_ARGS8(_S8("proxyid = %d "), iProxyId);
            CCommsDbTableView* portView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(PROXIES), TPtrC(COMMDB_ID), iProxyId);

            errorCode = portView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): Portview opened");
                TUint32 object32 = 0;

                TRAPD(leavecode, portView->ReadUintL(qColumn, object32));
                if (leavecode == KErrNone)
                    {
                    aObject.InsertL(aObject.Size(), SetIntObjectLC(object32));
                    pushed++;
                    }
                else
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): db read failed");
                    CleanupStack::PopAndDestroy(); // portView
                    CleanupStack::PopAndDestroy(pushed);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EError end");
                    return CSmlDmAdapter::EError;
                    }
                }
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): Portview opening failed");
                CleanupStack::PopAndDestroy(); // portView
                CleanupStack::PopAndDestroy(pushed);
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                return CSmlDmAdapter::ENotFound;
                }
            CleanupStack::PopAndDestroy(); // portView
            }
        else if (aURI.Match(_L8("AP/*/NAPID")) != KErrNotFound || aURI.Match(
                _L8("AP/*/ToNAPIDL")) != KErrNotFound)
            {
            iObject = iapID;
            TPtrC8 napidUri = FirstURISeg(aURI);
            aObject.InsertL(aObject.Size(), napidUri);

            }
        else
            {
            iObject = 0;

            CCommsDbTableView* colView = iDatabase->OpenViewMatchingUintLC(
                    serviceType, qDB, serviceId);

            errorCode = colView->GotoFirstRecord();

            TUint32 object32 = 0;
            if (errorCode == KErrNone)
                {
                if (aURI.Find(KNSmlDdfUsePTxtLog) >= 0)
                    {
                    TBool objectBool = EFalse;
                    TRAPD(leavecode, colView->ReadBoolL(qColumn, objectBool));
                    errorCode = leavecode;
                    object32 = objectBool;
                    }
                else
                    {
                    TRAPD(leavecode, colView->ReadUintL(qColumn, object32));
                    errorCode = leavecode;
                    }
                }

            if (errorCode == KErrNone)
                {

                if (aURI.Find(KNSmlDdfNAPAddrTy) >= 0 || /* NAPAddrTy handling */
                aURI.Find(KNSmlDdfGPRSPDP) >= 0) /* NAPAddrTy handling */
                    {
                    if (object32 == RPacketContext::EPdpTypeIPv4)
                        {
                        aObject.InsertL(aObject.Size(), KNSmlDmApValIpv4);
                        }
                    else if (object32 == RPacketContext::EPdpTypeIPv6)
                        {
                        aObject.InsertL(aObject.Size(), KNSmlDmApValIpv6);
                        }
                    else if (object32 == RPacketContext::EPdpTypePPP)
                        {
                        _LIT8(KPpp, "PPP");
                        aObject.InsertL(aObject.Size(), KPpp);
                        }
                    else
                        {
                        aObject.InsertL(aObject.Size(), KNullDesC8);
                        }
                    }
                else
                    {
                    aObject.InsertL(aObject.Size(), SetIntObjectLC(object32));
                    pushed++;
                    }
                }
            else
                {
                CleanupStack::PopAndDestroy(); // colView
                CleanupStack::PopAndDestroy(pushed);
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                return CSmlDmAdapter::ENotFound;
                }
            CleanupStack::PopAndDestroy(); // colView
            }
        }

    else if (fType == CNSmlInternetAdapter::EWap)
        { // Startpg + PxAuthId + PxAuthPW + PortNbr (BasAuthId + BasAuthPW)
        CCommsDbTableView* wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                WAP_IP_BEARER), TPtrC(WAP_IAP), iapID);

        errorCode = wapView->GotoFirstRecord();

        if (errorCode == KErrNone)
            {
            TBuf<KCommsDbSvrMaxFieldLength> columnValue;

            if (qColumn == TPtrC(WAP_START_PAGE))
                {
                TRAPD(leavecode, wapView->ReadUintL(
                        TPtrC(WAP_ACCESS_POINT_ID), iWapId));
                CleanupStack::PopAndDestroy(); // wapView
                if (leavecode != KErrNone)
                    {
                    CleanupStack::PopAndDestroy(pushed);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EError end");
                    return CSmlDmAdapter::EError;

                    }

                CCommsDbTableView* wapView2 =
                        iDatabase->OpenViewMatchingUintLC(TPtrC(
                                WAP_ACCESS_POINT), TPtrC(COMMDB_ID), iWapId);

                errorCode = wapView2->GotoFirstRecord();
                if (errorCode != KErrNone)
                    {
                    CleanupStack::PopAndDestroy(); // wapView2
                    CleanupStack::PopAndDestroy(pushed);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                    return CSmlDmAdapter::ENotFound;
                    }

                HBufC* tmpVal = wapView2->ReadLongTextLC(qColumn);

                aObject.InsertL(aObject.Size(), ConvertTo8LC(tmpVal->Des()));
                CleanupStack::PopAndDestroy(); // ReadLongTextLC
                CleanupStack::PopAndDestroy(); // wapView2
                pushed++;
                }
            else
                {
                if (qColumn == TPtrC(WAP_PROXY_PORT))
                    {
                    TUint32 object32;
                    wapView->ReadUintL(qColumn, object32);
                    if (object32 == KWAPP_PORT_9200) // wappdef.h
                        {
                        aObject.InsertL(aObject.Size(), ConvertTo8LC(
                                KWappPort9200));
                        }
                    else if (object32 == KWAPP_PORT_9201)
                        {
                        aObject.InsertL(aObject.Size(), ConvertTo8LC(
                                KWappPort9201));
                        }
                    else if (object32 == KWAPP_PORT_9202)
                        {
                        aObject.InsertL(aObject.Size(), ConvertTo8LC(
                                KWappPort9202));
                        }
                    else if (object32 == KWAPP_PORT_9203)
                        {
                        aObject.InsertL(aObject.Size(), ConvertTo8LC(
                                KWappPort9203));
                        }
                    else
                        {
                        CleanupStack::PopAndDestroy(); // wapView
                        CleanupStack::PopAndDestroy(pushed);
                        _DBG_FILE(
                                "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                        return CSmlDmAdapter::ENotFound;
                        }

                    pushed++;
                    }
                else
                    {
                    wapView->ReadTextL(qColumn, columnValue);
                    aObject.InsertL(aObject.Size(), ConvertTo8LC(columnValue));
                    pushed++;
                    }
                CleanupStack::PopAndDestroy(); // wapView
                }
            }
        else
            {
            CleanupStack::PopAndDestroy(); // wapView
            CleanupStack::PopAndDestroy(pushed);
            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
            return CSmlDmAdapter::ENotFound;
            }
        }
    else if (fType == CNSmlInternetAdapter::ESpec)
        {
        // name
        if (aURI.Find(KNSmlDdfNAPName) >= 0)
            {
            TUint32 iapID10 = IntLUID(aLUID);
            /*  CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(serviceType,
             qDB,
             serviceId);*/
            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iapID);

            errorCode = tableView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                TBuf<KCommsDbSvrMaxFieldLength> columnValue;
                TRAPD(leavecode, tableView->ReadTextL(TPtrC(COMMDB_NAME),
                        columnValue));
                if (leavecode != 0)
                    {
                    CleanupStack::PopAndDestroy(); // tableView
                    CleanupStack::PopAndDestroy(pushed);
                    _DBG_FILE(
                            "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                    return CSmlDmAdapter::ENotFound;
                    }
                aObject.InsertL(aObject.Size(), ConvertTo8LC(columnValue));
                pushed++;
                }
            CleanupStack::PopAndDestroy(); // tableView
            CleanupStack::PopAndDestroy(pushed);
            return CSmlDmAdapter::EOk;
            }
        // BearerL + NoPxForL + IAPService
        else if (aURI.Find(KNSmlDdfIAPService) >= 0)
            {
            aObject.InsertL(aObject.Size(), SetIntObjectLC(serviceId));
            pushed++;
            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EOk end");
            CleanupStack::PopAndDestroy(pushed);
            return CSmlDmAdapter::EOk;
            }
        else if (aURI.Find(KNSmlDdfNoPxForL) >= 0) /* No proxies used handling */
            {
            TBool proxyFound = GetProxyIdL(aURI);
            if (!proxyFound)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                CleanupStack::PopAndDestroy(pushed);
                return CSmlDmAdapter::ENotFound;
                }

            CCommsDbTableView* exeptionView =
                    iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES), TPtrC(
                            COMMDB_ID), iProxyId);
            errorCode = exeptionView->GotoFirstRecord();

            if (errorCode == KErrNone)
                {
                HBufC* proxyExceptions = exeptionView->ReadLongTextLC(TPtrC(
                        PROXY_EXCEPTIONS));
                pushed++;

                aObject.InsertL(aObject.Size(),
                        ConvertTo8LC(*proxyExceptions));
                pushed++;
                }
            else
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): ENotFound end");
                CleanupStack::PopAndDestroy(pushed);
                return CSmlDmAdapter::ENotFound;
                }
            CleanupStack::PopAndDestroy(); // exeptionView

            _DBG_FILE(
                    "CNSmlInternetAdapter::FetchLeafObjectL(internal)()(): EOk end");
            CleanupStack::PopAndDestroy(pushed);
            return CSmlDmAdapter::EOk;
            }

        if (aURI.Find(KNSmlDdfDirection) >= 0)
            {
            if (serviceType == TPtrC(OUTGOING_GPRS))
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValOutgoing);
                }
            else if (serviceType == TPtrC(INCOMING_GPRS))
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValIncoming);
                }
            else if (serviceType == TPtrC(LAN_SERVICE)) // Wlan ADD
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValOutgoing);
                }
            else if (serviceType == TPtrC(VPN_SERVICE))
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValOutgoing);
                }
            else
                {
                _LIT8(KDirectionUnk, "DirectionUnknown");
                aObject.InsertL(aObject.Size(), KDirectionUnk);
                }
            }
        else
            {
            if (serviceType == TPtrC(OUTGOING_GPRS) || serviceType == TPtrC(
                    INCOMING_GPRS))
                {
                if (i3GPPPS)
                    {
                    aObject.InsertL(aObject.Size(), KNSmlDmApVal3Gppps);
                    i3GPPPS = EFalse;
                    }
                else
                    {
                    aObject.InsertL(aObject.Size(), KNSmlDmApValGsmGprs);
                    }

                }

            else if (serviceType == TPtrC(OUTGOING_GPRS) || serviceType
                    == TPtrC(INCOMING_GPRS))
                {
                if (i3GPPPS)
                    {
                    aObject.InsertL(aObject.Size(), KNSmlDmApVal3Gppps);
                    i3GPPPS = EFalse;
                    }
                else
                    {
                    aObject.InsertL(aObject.Size(), KNSmlDmApValGsmGprs);
                    }

                }
            else if (serviceType == TPtrC(VPN_SERVICE))
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValVpn);
                }

            else if ((serviceType == TPtrC(LAN_SERVICE) && iWlanSupported))
                {
                aObject.InsertL(aObject.Size(), KNSmlDmApValWlan);
                }
            else
                {
                _LIT8(KUnkBearer, "Unknown Bearer");
                aObject.InsertL(aObject.Size(), KUnkBearer);
                }
            }
        }
    else
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::FetchLeafObjectL(internal)(): ENotFound end");
        CleanupStack::PopAndDestroy(pushed);
        return CSmlDmAdapter::ENotFound;
        }

    _DBG_FILE("CNSmlInternetAdapter::FetchLeafObjectL(internal)(): end");
    CleanupStack::PopAndDestroy(pushed);
    return CSmlDmAdapter::EOk;

    }

//------------------------------------------------------------------------------
// CSmlDmAdapter::ChildURIListL()
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::ChildURIListL(const TDesC8& aURI,
        const TDesC8& aLUID,
        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
        const TInt aResultsRef, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): begin");
    DBG_ARGS8(_S8("AP:ChildURIListL URI - <%S> <%S>"), &aURI, &aLUID);

    CBufBase* currentURISegmentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentURISegmentList);

    TUint32 checkLUID = IntLUID(aLUID);

    // If this is command to proxy node we'll need to make sure that
    // the LUID is for proxy node and not AP node.
    // If this command comes to predefined AP node then 
    // the LUID passed is inherited from that node and is not
    // proper proxy node.
    if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound)
        {
        TLex8 lex(aLUID);
        TUint tempLUID;
        if (!((lex.Val(tempLUID) == KErrNone) && (tempLUID
                > KNSmlApProxyLowerBase)
                && (tempLUID < KNSmlApProxyUpperBase)))
            {
            // Non-proxy LUID         
            checkLUID = 0;
            }
        }

    // Fetch all data for child
    if (!checkLUID && (!IsWLANfield(aURI) || !iWlanSupported))
        {
        if (aURI.Match(KNSmlDdfAP) == KErrNotFound) // Not for AP
            {
            CleanupStack::PopAndDestroy(); // currentURISegmentList
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
            return;
            }
        }

    TUint32 apId = GetAPIdFromURIL(aURI);

    if (aURI.Match(_L8("AP/*")) != KErrNotFound && !APExistsL(apId))
        {
        if (aLUID.Length() > 0)
            {
            // Stale data, remove mapping. 
            TInt ret = iCallBack->RemoveMappingL(KNSmlInternetAdapterImplUid,
                    GetDynamicAPNodeUri(aURI), ETrue);
            }

        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE(
                "CNSmlInternetAdapter::ChildURIListL(): AP doesn't exist: ENotFound end");
        return;
        }

    if (aURI.Match(_L8("AP/*")) != KErrNotFound && aURI.Match(_L8("AP/*/*"))
            == KErrNotFound)
        {
        // Check if AP exists
        if (!APExistsL(checkLUID))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            CleanupStack::PopAndDestroy(); // currentURISegmentList
            _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
            return;
            }
        currentURISegmentList->InsertL(0, KNSmlAPnode);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): end");
        return;
        }

    // Check that queried proxy exists
    if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound && !PxExistsL(checkLUID))
        {
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
        return;
        }

    if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Px/*/*")) == KErrNotFound)
        {
        // Check if PX exists
        if (!PxExistsL(checkLUID))
            {
            // Only WAP-data  
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            CleanupStack::PopAndDestroy(); // currentURISegmentList
            _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
            return;
            }
        // List all Px
        currentURISegmentList->InsertL(0, KNSmlPxnode);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): OK end");
        return;
        }

    if (aURI.Match(_L8("AP/*/NAPDef/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/NAPDef/*/*")) == KErrNotFound)
        {
        // Check if IAP has luid mapped
        HBufC8* iapIdBuf = iCallBack->GetLuidAllocL(RemoveLastSeg(aURI));
        TInt iapIdInt = GetIntObject8(iapIdBuf->Des());

        delete iapIdBuf;
        iapIdBuf = NULL;

        if (iapIdInt > 0)
            {
            CCommsDbTableView* iapView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), iapIdInt);
            TInt error = iapView->GotoFirstRecord();
            TBuf<KCommsDbSvrMaxFieldLength> serviceType;
            if (error == KErrNone)
                {
                iapView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
                }
            CleanupStack::PopAndDestroy(iapView);

            // Check if NAPDef exists
            if (!NAPDefExistsL(checkLUID))
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                CleanupStack::PopAndDestroy(); // currentURISegmentList
                _DBG_FILE(
                        "CNSmlInternetAdapter::ChildURIListL(): ENotFound end");
                return;
                }
            // List all NAPDef
            if (IsWlanL(aURI) && iWlanSupported)
                {
                iWlanAdapter->ChildURIListL(aURI, aLUID,
                        aPreviousURISegmentList, aResultsRef, aStatusRef);
                CleanupStack::PopAndDestroy(); // currentURISegmentList
                _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): WLAN end");
                return;
                }
            _LIT(KGprs, "*GPRS*");
            _LIT(K3gppps, "*3GPPPS*");
            if (serviceType.Match(KGprs) != KErrNotFound
                    || serviceType.Match(K3gppps) != KErrNotFound)
                {
                currentURISegmentList->InsertL(0, KNSmlNAPDefGPRSnode);
                }

#ifdef __SYNCML_DM_LSCRIPT
            currentURISegmentList->InsertL(currentURISegmentList->Size(),KNSmlLoginscriptnode);
#endif

            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                    KNullDesC8);
            }
        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            }
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): end");
        return;
        }

    if ((aURI.Match(_L8("AP/*/WLAN")) != KErrNotFound || aURI.Match(_L8(
            "AP/*/WLAN/*")) != KErrNotFound) && iWlanSupported)
        {
        iWlanAdapter->ChildURIListL(aURI, aLUID, aPreviousURISegmentList,
                aResultsRef, aStatusRef);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): WLAN end");
        return;
        }
    //
    //  Get all AP numbers from IAP-table
    //
    if (aURI.Match(KNSmlDdfAP) != KErrNotFound)
        {
        TBool inDatabase = EFalse;

        CCommsDbTableView* apView = iDatabase->OpenTableLC(TPtrC(IAP));

        TInt errorCode = apView->GotoFirstRecord();

        while (errorCode == KErrNone)
            {
            TUint32 lValue;
            TBool validService = EFalse;
            TInt lLine(0);
            apView->ReadUintL(TPtrC(COMMDB_ID), lValue);

            TBuf<KCommsDbSvrMaxFieldLength> serviceType;
            apView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
            TUint32 bearerId = 0;
            apView->ReadUintL(TPtrC(IAP_BEARER), bearerId);
            //
            // Skip other service types than GSM outgoing
            //
            if (serviceType == TPtrC(OUTGOING_GPRS) || serviceType == TPtrC(
                    INCOMING_GPRS))
                {
                validService = ETrue;
                }

            else if (serviceType == TPtrC(LAN_SERVICE) && bearerId > 1)
                {
                validService = ETrue;
                }
            else if (serviceType == TPtrC(VPN_SERVICE))
                {
                validService = ETrue;
                }
            else if ((serviceType == TPtrC(LAN_SERVICE)) && iWlanSupported)
                {
                validService = ETrue;
                }

            if (validService)
                {
                // Skip others
                //
                //  Check if in aPreviousURISegmentList
                // 
                while (!inDatabase && lLine < aPreviousURISegmentList.Count())
                    {
                    TUint32 list = GetIntObject8(aPreviousURISegmentList.At(
                            lLine).iURISegLUID);
                    if (list == lValue)
                        {
                        inDatabase = ETrue;
                        }
                    else
                        {
                        lLine++;
                        }
                    }

                if (inDatabase)
                    {
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(),
                            aPreviousURISegmentList.At(lLine).iURISeg);
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    inDatabase = EFalse;
                    DBG_ARGS8(_S8("InList: Id = %d Name %S"), lValue,
                            &aPreviousURISegmentList.At(lLine).iURISeg);

                    }
                else
                    {
                    _LIT8(Kprev, "APId");
                    TBuf8<9> addNAME(Kprev); // APIdnnn , nnn = profileid
                    addNAME.AppendNumFixedWidth(lValue, EDecimal, 3);

                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), addNAME);
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    DBG_ARGS8(_S8("NotInList: Id = %d Name %S"), lValue,
                            &addNAME);

                    // KNSmlDMStart includes start text for URISeg
                    TBuf8<20> addURI; // AP/APIdnnn , nnn = profileid
                    addURI.Append(_L8("AP/APId"));
                    addURI.AppendNumFixedWidth(lValue, EDecimal, 3);

                    TBuf8<16> addLUID;
                    _LIT8(KFormat, "%d");
                    addLUID.Format(KFormat, lValue);

                    // Also added to mapping
                    iCallBack->SetMappingL(addURI, addLUID);
                    }
                } // Skip
            errorCode = apView->GotoNextRecord();
            }

        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(2); // apView, currentURISegmentList

        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): AP end");
        return;
        }

    iISPId = IntLUID(aLUID);

    //
    //  Get all NAPDef numbers
    //
    if (aURI.Match(_L8("AP/*/NAPDef")) != KErrNotFound)
        {
        TBool inDatabase = EFalse;
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;
        TUint32 serviceId;

        CCommsDbTableView* napdefView = iDatabase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), iISPId);

        TInt errorCode = napdefView->GotoFirstRecord();

        if (errorCode != KErrNone)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE(
                    "CNSmlInternetAdapter::ChildURIListL(): NAPDef error end");
            CleanupStack::PopAndDestroy(2); // napdefView, currentURISegmentList
            return;
            }
        else
            {
            napdefView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
            napdefView->ReadUintL(TPtrC(IAP_SERVICE), serviceId); // ID  
            CleanupStack::PopAndDestroy(); // napdefView
            }

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                serviceType, TPtrC(COMMDB_ID), serviceId);

        errorCode = tableView->GotoFirstRecord();

        while (errorCode == KErrNone)
            {
            TUint32 lValue;
            TInt lLine(0);
            tableView->ReadUintL(TPtrC(COMMDB_ID), lValue);
            //
            //  Check if in aPreviousURISegmentList
            // 
            while (!inDatabase && lLine < aPreviousURISegmentList.Count())
                {
                TUint32 list = GetIntObject8(
                        aPreviousURISegmentList.At(lLine).iURISegLUID);
                if (list == lValue)
                    {
                    inDatabase = ETrue;
                    }
                else
                    {
                    lLine++;
                    }
                }

            if (inDatabase)
                {
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        aPreviousURISegmentList.At(lLine).iURISeg);
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KNSmlDmApValFSlash);
                inDatabase = EFalse;
                }
            else
                {
                _LIT8(Kprev, "NAPId");
                TBuf8<15> addNAME(Kprev); // APIdnnn , nnn = profileid
                addNAME.AppendNumFixedWidth(lValue, EDecimal, 3);

                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        addNAME);
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KNSmlDmApValFSlash);

                _LIT8(Kprev2, "/NAPId");
                TBuf8<80> addURI; // AP/xxx/NAPDef/NAPIdnnn , nnn = id nbr
                addURI.Append(aURI);
                addURI.Append(Kprev2);
                addURI.AppendNumFixedWidth(lValue, EDecimal, 3);

                TBuf8<16> addLUID;
                _LIT8(KFormat, "%d");
                addLUID.Format(KFormat, lValue);

                // Also added to mapping
                iCallBack->SetMappingL(addURI, addLUID);
                }
            errorCode = tableView->GotoNextRecord();
            }

        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(2); // tableView, currentURISegmentList

        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): AP end");
        return;
        }

    //  Get all Px numbers 
    //  Check also AP-nbr + serviceType and nbr for Proxy
    //  To get right data for AP
    //
    if (aURI.Match(_L8("AP/*/Px")) != KErrNotFound)
        {
        TBool inDatabase = EFalse;
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;
        TUint32 ServiceIsp;

        CCommsDbTableView* pxView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                IAP), TPtrC(COMMDB_ID), iISPId);

        TInt errorCode = pxView->GotoFirstRecord();

        if (errorCode != KErrNone)
            {
            CleanupStack::PopAndDestroy(2); // pxView, currentURISegmentList
            _DBG_FILE(
                    "CNSmlInternetAdapter::ChildURIListL(): NAPDef error end");
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            return;
            }
        else
            {
            pxView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType); // Type
            pxView->ReadUintL(TPtrC(IAP_SERVICE), ServiceIsp); // ID  
            CleanupStack::PopAndDestroy(); // pxView
            }

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingTextLC(
                TPtrC(PROXIES), TPtrC(PROXY_SERVICE_TYPE), serviceType);

        errorCode = tableView->GotoFirstRecord();

        TBool proxyFound(false);
        while (errorCode == KErrNone)
            {
            TUint32 lProxyIspId;
            TUint32 lProxyId;
            TInt lLine(0);
            tableView->ReadUintL(TPtrC(PROXY_ISP), lProxyIspId);
            tableView->ReadUintL(TPtrC(COMMDB_ID), lProxyId);
            if (lProxyIspId == ServiceIsp) // Right value ISP
                {
                proxyFound = true;
                tableView->ReadUintL(TPtrC(COMMDB_ID), lProxyId);
                //
                //  Check if in aPreviousURISegmentList
                // 
                while (!inDatabase && lLine < aPreviousURISegmentList.Count())
                    {
                    TUint32 list = GetIntObject8(aPreviousURISegmentList.At(
                            lLine).iURISegLUID) - KNSmlApProxyLowerBase;
                    if (list == lProxyId)
                        {
                        inDatabase = ETrue;
                        }
                    else
                        {
                        lLine++;
                        }
                    }

                if (inDatabase)
                    {
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(),
                            aPreviousURISegmentList.At(lLine).iURISeg);
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    inDatabase = EFalse;
                    DBG_ARGS8(_S8("InList: Id = %d Name %S"), lProxyId
                            + KNSmlApProxyLowerBase,
                            &aPreviousURISegmentList.At(lLine).iURISeg);
                    }
                else
                    {
                    _LIT8(Kprev, "PXId");
                    TBuf8<9> addNAME(Kprev); // PXIdnnn , nnn = profileid
                    addNAME.AppendNumFixedWidth(lProxyId, EDecimal, 3);

                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), addNAME);
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    DBG_ARGS8(_S8("NotInList: Id = %d Name %S"), lProxyId,
                            &addNAME);
                    TBuf8<80> addURI; // AP/xxx/Px/PXIdnnn , nnn = id nbr
                    addURI.Append(aURI);
                    _LIT8(KPxid, "/PXId");
                    addURI.Append(KPxid);
                    addURI.AppendNumFixedWidth(lProxyId, EDecimal, 3);

                    TBuf8<16> addLUID;
                    _LIT8(KFormat, "%d");
                    addLUID.Format(KFormat, KNSmlApProxyLowerBase + lProxyId); // 100000 + lProxyId

                    // Also added to mapping
                    iCallBack->SetMappingL(addURI, addLUID);
                    }
                }
            errorCode = tableView->GotoNextRecord();
            }
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): AP end");
        if (proxyFound)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            }
        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            }
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(2); //  tableView, currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/Bearer/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Bearer/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(), (_L8(
                "BearerL/Direction"))); // Direction
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): BearerL end");
        return;
        }

    if (aURI.Match(_L8("AP/*/Bearer")) != KErrNotFound)
        {
        if (aPreviousURISegmentList.Count() > 0) // Allready mapped
            {
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(0).iURISeg);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);
            }
        else
            {
            _LIT8(Kprev, "BId");
            TBuf8<9> addNAME(Kprev); // Id
            addNAME.AppendNumFixedWidth(checkLUID, EDecimal, 3);

            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    addNAME);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);

            TBuf8<80> addURI; // AP/xxx/NapDef/Bearer/BIdn , n=aLUID
            addURI.Append(aURI);
            _LIT8(KBid, "/BId");
            addURI.Append(KBid);
            addURI.AppendNumFixedWidth(checkLUID, EDecimal, 3);

            TBuf8<16> addLUID;
            _LIT8(KFormat, "%d");
            addLUID.Format(KFormat, checkLUID);

            // Also added to mapping
            iCallBack->SetMappingL(addURI, addLUID);
            }
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Bearer end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/DNSAddr/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/DNSAddr/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                KNSmlDNSAddrNode);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): DNSAddrL end");
        return;
        }

    if (aURI.Match(_L8("AP/*/DNSAddr")) != KErrNotFound)
        {

        TBuf<KCommsDbSvrMaxFieldLength> ServType;
        TUint32 servId;

        CCommsDbTableView* napdefView = iDatabase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), iISPId);

        TInt errorCode = napdefView->GotoFirstRecord();

        if (errorCode != KErrNone)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE(
                    "CNSmlInternetAdapter::ChildURIListL(): DNSAddr error end");
            CleanupStack::PopAndDestroy(2); // napdefView, currentURISegmentList
            return;
            }
        else
            {
            napdefView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), ServType);
            napdefView->ReadUintL(TPtrC(IAP_SERVICE), servId); // ID  
            CleanupStack::PopAndDestroy(napdefView); // napdefView
            }

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                ServType, TPtrC(COMMDB_ID), servId);

        errorCode = tableView->GotoFirstRecord();

        if (errorCode != KErrNone)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE(
                    "CNSmlInternetAdapter::ChildURIListL(): DNSAddr error end");
            CleanupStack::PopAndDestroy(2); // tableView, currentURISegmentList
            return;
            }
        else
            {
            // Now we can read DNSAddrL values
            TInt leavecode;
            TBuf<KCommsDbSvrMaxFieldLength> columnValue;
            TBuf8<16> addLUID;
            TBuf8<80> addURI; // AP/xxx/NAPDef/DNSAddr/DNSx
            _LIT8(KFormat, "%d");
            TRAP(leavecode, tableView->ReadTextL(TPtrC(
                    SERVICE_IP_NAME_SERVER1), columnValue));
            if ((leavecode == KErrNone) && (columnValue.Length() > 0))
                {
                addURI.Append(aURI);
                addURI.Append(KNSmlDNS1);
                _LIT8(KDns1, "DNS1");
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KDns1);
                addLUID.Format(KFormat, DnsToLuid(UriUtils::EIPv4Host, 1));
                iCallBack->SetMappingL(addURI, addLUID);
                addURI.Zero();
                }

            TRAP(leavecode, tableView->ReadTextL(TPtrC(
                    SERVICE_IP_NAME_SERVER2), columnValue));
            if (leavecode == KErrNone && (columnValue.Length() > 0))
                {
                if (currentURISegmentList->Size() > 0)
                    {
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    }
                addURI.Append(aURI);
                addURI.Append(KNSmlDNS2);
                _LIT8(KDns2, "DNS2");
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KDns2);
                addLUID.Format(KFormat, DnsToLuid(UriUtils::EIPv4Host, 2));
                iCallBack->SetMappingL(addURI, addLUID);
                addURI.Zero();
                }

            TRAP(leavecode, tableView->ReadTextL(TPtrC(
                    SERVICE_IP6_NAME_SERVER1), columnValue));
            if (leavecode == KErrNone && (columnValue.Length() > 0))
                {
                if (currentURISegmentList->Size() > 0)
                    {
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    }
                addURI.Append(aURI);
                addURI.Append(KNSmlDNS3);
                _LIT8(KDns3, "DNS3");
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KDns3);
                addLUID.Format(KFormat, DnsToLuid(UriUtils::EIPv6Host, 1));
                iCallBack->SetMappingL(addURI, addLUID);
                addURI.Zero();
                }

            TRAP(leavecode, tableView->ReadTextL(TPtrC(
                    SERVICE_IP6_NAME_SERVER2), columnValue));
            if (leavecode == KErrNone && (columnValue.Length() > 0))
                {
                if (currentURISegmentList->Size() > 0)
                    {
                    currentURISegmentList->InsertL(
                            currentURISegmentList->Size(), KNSmlDmApValFSlash);
                    }
                addURI.Append(aURI);
                addURI.Append(KNSmlDNS4);
                _LIT8(KDns4, "DNS4");
                currentURISegmentList->InsertL(currentURISegmentList->Size(),
                        KDns4);
                addLUID.Format(KFormat, DnsToLuid(UriUtils::EIPv6Host, 2));
                iCallBack->SetMappingL(addURI, addLUID);
                }

            _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): DNSAddr end");
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                    KNullDesC8);

            CleanupStack::PopAndDestroy(tableView);
            CleanupStack::PopAndDestroy(currentURISegmentList); //  currentURISegmentList
            }
        return;
        }

    if (aURI.Match(_L8("AP/*/Port/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Port/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                KNSmlDdfPortNbr);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Port end");
        return;
        }

    if (aURI.Match(_L8("AP/*/Port")) != KErrNotFound)
        {

        TUint32 iapID10 = GetAPIdFromURIL(aURI);

        if (WapAPExistsL(iapID10)) // WapPort
            {
            TBuf8<80> addURI; // AP/xxx/Px/Port/WAP
            addURI.Append(aURI);
            _LIT8(KWap, "/WAP");
            addURI.Append(KWap);
            _LIT8(KWap2, "WAP/");
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KWap2);
            }

        if (PxExistsL(checkLUID)) // ProxyPort
            {
            TBuf8<80> addURI; // AP/xxx/Px/Port/PROXY
            addURI.Append(aURI);
            _LIT8(KProxy, "/PROXY");
            addURI.Append(KProxy);
            _LIT8(KProxy2, "PROXY/");
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KProxy2);
            }

        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Port end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/NAPAuthInf/*")) != KErrNotFound && aURI.Match(
            _L8("AP/*/NAPAuthInf/*/*")) == KErrNotFound)
        {
        _LIT8(KAuthFields, "AuthName/AuthSecr");
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KAuthFields));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NAPAuthInfL end");
        return;
        }

    if (aURI.Match(_L8("AP/*/NAPAuthInf")) != KErrNotFound)
        {
        _LIT8(Kprev, "AUId");
        TBuf8<9> addNAME(Kprev); // Id
        addNAME.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        currentURISegmentList->InsertL(currentURISegmentList->Size(), addNAME);
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                KNSmlDmApValFSlash);

        TBuf8<80> addURI; // AP/xxx/NapDef/NAPAuthInf/AUIdn , n=aLUID
        addURI.Append(aURI);
        _LIT8(KAuid, "/AUId");
        addURI.Append(KAuid);
        addURI.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        TBuf8<16> addLUID;
        _LIT8(KFormat, "%d");
        addLUID.Format(KFormat, checkLUID);

        // Also added to mapping
        iCallBack->SetMappingL(addURI, addLUID);
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NAPAuthInf end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/PxAuthInf/*")) != KErrNotFound && aURI.Match(
            _L8("AP/*/PxAuthInf/*/*")) == KErrNotFound)
        {
        _LIT8(KPxAuthFields, "PxAuthId/PxAuthPW");
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KPxAuthFields));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NAPAuthInfL end");
        return;
        }

    if (aURI.Match(_L8("AP/*/PxAuthInf")) != KErrNotFound)
        {
        _LIT8(Kprev, "AUPxId");
        TBuf8<11> addNAME(Kprev); // Id
        addNAME.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        currentURISegmentList->InsertL(currentURISegmentList->Size(), addNAME);
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                KNSmlDmApValFSlash);

        TBuf8<80> addURI; // AP/xxx/Px/PxAuthInf/AUPxIdn , n=aLUID
        addURI.Append(aURI);
        _LIT8(KAupxid, "/AUPxId");
        addURI.Append(KAupxid);
        addURI.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NAPAuthInf end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/ToNAPID/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/ToNAPID/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KNSmlDdfToNAPIDL));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ToNAPIDL end");
        return;
        }

    if (aURI.Match(_L8("AP/*/ToNAPID")) != KErrNotFound)
        {
        _LIT8(Kprev, "TId");
        TBuf8<9> addNAME(Kprev); // Id
        addNAME.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        currentURISegmentList->InsertL(currentURISegmentList->Size(), addNAME);
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                KNSmlDmApValFSlash);

        TBuf8<80> addURI; // AP/xxx/NapDef/ToNAPID/TIdn , n=aLUID
        addURI.Append(aURI);
        _LIT8(KTid, "/TId");
        addURI.Append(KTid);
        addURI.AppendNumFixedWidth(checkLUID, EDecimal, 3);

        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): ToNAPID end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/Networks/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Networks/*/*")) == KErrNotFound)
        {
        _LIT8(KNameId, "Name/ID");
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KNameId));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Networks end");
        return;
        }

    if (aURI.Match(_L8("AP/*/Networks")) != KErrNotFound)
        {
        checkLUID = GetAPIdFromURIL(aURI);

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), checkLUID);

        TInt errorCode = tableView->GotoFirstRecord();

        if (errorCode == KErrNone)
            {
            TUint32 nwLUID = 0;
            tableView->ReadUintL(TPtrC(IAP_NETWORK), nwLUID); // Networks luid
            if (!NetworkExistsL(nwLUID))
                {
                errorCode = KErrNotFound;
                }
            }

        CleanupStack::PopAndDestroy(); // tableView

        if (errorCode != KErrNone)
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter::ChildURIListL(): Networks error end");
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            CleanupStack::PopAndDestroy(); //  currentURISegmentList
            return;
            }

        if (aPreviousURISegmentList.Count() > 0) // Allready mapped
            {
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(0).iURISeg);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);
            }
        else
            {
            _LIT8(Kprev, "NwId");
            TBuf8<9> addNAME(Kprev); // Id
            addNAME.AppendNumFixedWidth(checkLUID, EDecimal, 3);

            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    addNAME);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);

            TBuf8<80> addURI; // AP/xxx/NapDef/Networks/NwIdn , n=aLUID
            addURI.Append(aURI);
            _LIT8(KNwid, "/NwId");
            addURI.Append(KNwid);
            addURI.AppendNumFixedWidth(checkLUID, EDecimal, 3);

            TBuf8<16> addLUID;
            _LIT8(KFormat, "%d");
            addLUID.Format(KFormat, checkLUID);

            // Also added to mapping
            iCallBack->SetMappingL(addURI, addLUID);
            }
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Networks end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/NoPxFor/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/NoPxFor/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KNSmlDdfNoPxForL));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NoPxFor end");
        return;
        }

    if (aURI.Match(_L8("AP/*/NoPxFor")) != KErrNotFound)
        {
        if (aPreviousURISegmentList.Count() > 0) // Allready mapped
            {
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(0).iURISeg);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);
            }
        else
            {
            _LIT8(KexepPrev, "eXC");
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KexepPrev);
            }
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): NoPxFor end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    if (aURI.Match(_L8("AP/*/Domain/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/Domain/*/*")) == KErrNotFound)
        {
        currentURISegmentList->InsertL(currentURISegmentList->Size(),
                (KNSmlDdfDomainL));
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);
        CleanupStack::PopAndDestroy(); // currentURISegmentList
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Domain end");
        return;
        }

    if (aURI.Match(_L8("AP/*/Domain")) != KErrNotFound)
        {
        if (aPreviousURISegmentList.Count() > 0) // Allready mapped
            {
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    aPreviousURISegmentList.At(0).iURISeg);
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KNSmlDmApValFSlash);
            }
        else
            {
            _LIT8(KexepPrev, "dOM");
            currentURISegmentList->InsertL(currentURISegmentList->Size(),
                    KexepPrev);
            }
        _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): Domain end");
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentURISegmentList,
                KNullDesC8);

        CleanupStack::PopAndDestroy(); //  currentURISegmentList

        return;
        }

    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
    iCallBack->SetResultsL(aResultsRef, *currentURISegmentList, KNullDesC8);
    _DBG_FILE("CNSmlInternetAdapter::ChildURIListL(): end");

    CleanupStack::PopAndDestroy(); // currentURISegmentList

    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::AddNodeBufferL()
// Inserts new AP-data to database
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::AddNodeBufferL(const TDesC8& aURI,
        const TDesC8& /*aParentLUID*/, const TInt aStatusRef,
        const TBool aDnsUpd, const TBool aWlan)
    {
    TInt index = -1;

    for (TInt i = 0; i < iBuffer->Count(); i++)
        {
        TPtrC8 parentUri = FirstURISeg(aURI);
        if (iBuffer->At(i).iMappingName->Compare(parentUri) == 0)
            {
            index = i;
            break;
            }
        }

    if (index < 0)
        {
        TNSmlAPBufferElement newNode;
        newNode.iMappingName = aURI.AllocLC();

        NextAPNameL(*LastURISeg(aURI).AllocLC());
        CleanupStack::PopAndDestroy();

        newNode.iName = iIAPName.AllocLC();
        newNode.iNodeBuf = new (ELeave) CArrayFixFlat<TNSmlAPAddElement> (
                KNSmlAPGranularity);
        newNode.iWlanNodeBuf
                = new (ELeave) CArrayFixFlat<TNSmlAPAddElement> (
                        KNSmlAPGranularity);
        newNode.iExecuted = EFalse;
        newNode.iBearer = EFalse;
        newNode.iDirection = EFalse;
        newNode.iNameReceived = EFalse;
        if (aDnsUpd)
            {
            newNode.iDnsUpdateBuf = ETrue;
            }
        else
            {
            newNode.iDnsUpdateBuf = EFalse;
            }
        newNode.iLuid = 0;
        iBuffer->AppendL(newNode);
        CleanupStack::Pop(2); //newNode.iMappingName,newNode.iName

        index = iBuffer->Count() - 1;
        }

    if (!aDnsUpd)
        {
        TNSmlAPAddElement newCommand;
        newCommand.iUri = aURI.AllocLC();
        newCommand.iData = 0;
        newCommand.iStatusRef = aStatusRef;
        newCommand.iLeaf = EFalse;
        newCommand.iDone = EFalse;

        TPtrC8 parentUri = FirstURISeg(aURI);
        newCommand.iLuid = iCallBack->GetLuidAllocL(parentUri);

        if (aWlan)
            {
            iBuffer->At(index).iWlanNodeBuf->AppendL(newCommand);
            }
        else
            {
            iBuffer->At(index).iNodeBuf->AppendL(newCommand);
            }
        CleanupStack::Pop(); //newCommand.iUri
        }
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::AddLeafBufferL()
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::AddLeafBufferL(const TDesC8& aURI,
        const TDesC8& aParentLUID, const TDesC8& aObject,
        const TDesC8& /*aType*/, const TInt aStatusRef, const TBool aWlan)
    {
    TInt index = -1;

    for (TInt i = 0; i < iBuffer->Count(); i++)
        {
        TPtrC8 parentUri = FirstURISeg(aURI);
        if (iBuffer->At(i).iMappingName->Compare(parentUri) == 0)
            {
            index = i;
            break;
            }
        }

    if (index < 0)
        {
        if (aParentLUID.Length() < 0)
            {
            DBG_ARGS8(_S8("AP:AddLeafBufferL URI - <%S> <%S> NOTFOUND"),
                    &aURI, &aParentLUID);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            return;
            }
        else
            {
            // this means update to DNSAddr/<X>
            TPtrC8 apURI = FirstURISeg(aURI);
            HBufC8* luid = iCallBack->GetLuidAllocL(apURI);
            CleanupStack::PushL(luid);

            AddNodeBufferL(apURI, luid->Des(), -2, ETrue);
            iDnsIpv4Pri = 0;
            iDnsIpv6Pri = 0;

            CleanupStack::PopAndDestroy(luid);
            index = 0;
            }
        }

    TNSmlAPAddElement newCommand;

    newCommand.iUri = aURI.AllocLC(); //aURI.AllocLC();
    newCommand.iData = aObject.AllocLC();
    newCommand.iStatusRef = aStatusRef;
    newCommand.iLuid = aParentLUID.AllocLC();
    newCommand.iLeaf = ETrue;
    newCommand.iDone = EFalse;

    if (aWlan)
        {
        iBuffer->At(index).iWlanNodeBuf->AppendL(newCommand);
        }
    else
        {
        iBuffer->At(index).iNodeBuf->AppendL(newCommand);
        }

    CleanupStack::Pop(3); //newCommand.iLastUriSeg, newCommand.iData, newCommand.iLuid

    if (aURI.Find(KNSmlDdfBearerL) >= 0) // Bearer added
        {
        if (aObject.Match(KNSmlDmApValGsmGprs) != KErrNotFound)
            {
            iBuffer->At(index).iBearer = ETrue;
            }

        else if (aObject.Match(KNSmlDmApVal3Gppps) != KErrNotFound
                || aObject.Match(KNSmlDmApValVpn) != KErrNotFound)
            {
            iBuffer->At(index).iBearer = ETrue;
            }
        if ((aObject.Match(KNSmlDmApValWlan) != KErrNotFound)
                && iWlanSupported) // Valid Bearer
            {
            iBuffer->At(index).iBearer = ETrue;
            }
        }

    if (aURI.Find(KNSmlDdfDirection) >= 0) // Direction added
        {
        iBuffer->At(index).iDirection = ETrue;
        }

    if (aURI.Match(_L8("AP/*/NAPDef/*/*")) != KErrNotFound && aURI.Match(_L8(
            "AP/*/NAPDef/*/*/*")) == KErrNotFound
            && LastURISeg(aURI).Compare(KNSmlDdfNAPName) == 0)
        {
        iIAPExists = EFalse;
        iBuffer->At(index).iNameReceived = ETrue;

        delete iBuffer->At(index).iName;
        iBuffer->At(index).iName = 0;

        iBuffer->At(index).iName = aObject.AllocLC();
        iIAPName.Copy(aObject);
        CleanupStack::Pop();

        // Check if Name and LUID match
        TPtrC qTable = TPtrC(IAP); // Check if IAP-table free
        TPtrC qDB = TPtrC(COMMDB_ID);
        TPtrC8 parentUri = FirstURISeg(aURI);
        HBufC8* luid = iCallBack->GetLuidAllocL(parentUri);
        CleanupStack::PushL(luid);

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                qTable, qDB, IntLUID(*luid));

        TInt errorCode = tableView->GotoFirstRecord();
        if (errorCode != KErrNotFound)
            {
            // Check the name
            TBuf8<KCommsDbSvrMaxFieldLength> name;
            tableView->ReadTextL(TPtrC(COMMDB_NAME), name);
            if (name.Compare(aObject) != 0)
                {
                // Not correct name                                    
                // Remove all mappings from AP/xxx level
                if (isAdd)
                    {
                    TInt ret = iCallBack->RemoveMappingL(
                            KNSmlInternetAdapterImplUid, GetDynamicAPNodeUri(
                                    aURI), ETrue);
                    iPrevURI->Des().Format(KNullDesC8);
                    iPrevLUID = 0;
                    }
                else
                    {
                    iIAPExists = ETrue;
                    iLeafType = EDMUpdate;
                    }

                }
            else
                {
                iIAPExists = ETrue;
                iLeafType = EDMUpdate;
                }
            }
        CleanupStack::PopAndDestroy(tableView);
        CleanupStack::PopAndDestroy(luid);
        }
    //    

    //  If Bearer data + Direction + name => add is possible for AP
    //
    if (iBuffer->At(index).iDirection && iBuffer->At(index).iBearer
            && iBuffer->At(index).iNameReceived && !iExecutingBuffer)
        {
        iBearer.Zero();
        iDirection = ECommDbConnectionDirectionUnknown;
        ExecuteBufferL(aURI);

        }
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::AddNodeObjectL()
// Inserts new AP-data to database
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::AddNodeObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): begin");

    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::AddNodeObjectL(): aURI=<%S>, aParentLUID=<%S>"),
            &aURI, &aParentLUID);

    if (iLeafType == EDMUpdate && iExecutingBuffer)
        {
        // This is an update to an old IAP. All node additions return KErrAlreadyExists.
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EAlreadyExists); // EError
        _DBG_FILE(
                "CNSmlInternetAdapter::AddNodeObjectL(): EAlreadyExists end");
        return;
        }

    // Save LUID to local variable to be able to reset the value, if needed.
    TBuf8<KSmlMaxURISegLen> parentLUID;
    parentLUID.Zero();
    parentLUID = aParentLUID;

    TUint32 apId = GetAPIdFromURIL(aURI);

    if (parentLUID.Length() > 0 && IntLUID(parentLUID) > 0
            && !APExistsL(apId))
        {
        // AP has been deleted. Handle as new data.
        _DBG_FILE(
                "CNSmlInternetAdapter::AddNodeObjectL(): parentLUID.Length() > 0 && !APExistsL() -> Remove mapping ");
        TInt ret = iCallBack->RemoveMappingL(KNSmlInternetAdapterImplUid,
                GetDynamicAPNodeUri(aURI), ETrue);
        if (ret)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            return;
            }
        parentLUID.Zero();
        iPrevURI->Des().Format(KNullDesC8);
        iPrevLUID = 0;
        }

    if (parentLUID.Length() <= 0)
        {
        // Check OOD before saving new 
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);

        if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs,
                KNSmlInternetAdapterFatMinSize))
            {
            CleanupStack::PopAndDestroy(); // fs
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EDiskFull);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): DISK FULL end");
            return;
            // do not write
            }
        CleanupStack::PopAndDestroy(); //fs
        }
    //
    //  Check if AP-add
    //
    if ((aURI.Match(_L8("AP/*")) != KErrNotFound && aURI.Match(_L8("AP/*/*"))
            == KErrNotFound) || (aURI.Match(_L8("AP/*/NAPDef/*"))
            != KErrNotFound && aURI.Match(_L8("AP/*/NAPDef/*/*"))
            == KErrNotFound) || aURI.Match(_L8("AP/*/NAPDef"))
            != KErrNotFound || aURI.Match(_L8("AP/*/NAPDef/*/Bearer"))
            != KErrNotFound || (aURI.Match(_L8("AP/*/NAPDef/*/Bearer/*"))
            != KErrNotFound && aURI.Match(_L8("AP/*/NAPDef/*/Bearer/*/*"))
            == KErrNotFound) || aURI.Match(_L8("*/WLAN*")) != KErrNotFound)
        {
        if ((parentLUID.Length() <= 0) && (aURI.Match(_L8("*/WLAN*"))
                != KErrNotFound))
            {
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse, ETrue);
            return;
            }
        if (parentLUID.Length() <= 0)
            {
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            return;
            }

        if (iTransactionCommitted && !iDatabase->InTransaction())
            {

            _DBG_FILE(
                    "CNSmlInternetAdapter::AddNode object, begin commsdb transaction ");
            TInt codeBegin = iDatabase->BeginTransaction();
            TInt retry = KBeginTransRetryCount;
            if (codeBegin == KErrNone)
                {
                iTransactionCommitted = EFalse;
                }
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::AddNode object iDatabase->Begintransaction start of addobjectL return code = %d"),
                    codeBegin);
            if (codeBegin == KErrLocked)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::Addnodeobject Database->Begintransaction has returned KErrLocked");
                while (retry > 0 && codeBegin == KErrLocked)
                    {
                    User::After(KBeginTransRetryDelay);
                    codeBegin = iDatabase->BeginTransaction();
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::AddNode object iDatabase->Begintransaction after 2 seconds of wait of addobjectL return code = %d"),
                            codeBegin);
                    retry--;
                    }

                if (codeBegin == KErrNone)
                    {
                    iTransactionCommitted = EFalse;
                    _DBG_FILE(
                            "CNSmlInternetAdapter::AddNode object transaction commited is false ");
                    }

                }

            }
        if ((aURI.Match(_L8("*/WLAN*")) != KErrNotFound) && iWlanSupported)
            {
            DBG_ARGS8(
                    _S8(
                            "To WlanAdapter::AddNodeObjectL - uri: <%S> to parentLUID: <%S>"),
                    &aURI, &parentLUID);
            iWlanAdapter->AddNodeObjectL(aURI, parentLUID, aStatusRef);
            return;
            }

        TInt pluid = IntLUID(parentLUID);

        if (pluid < 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): end");
            return;
            }
        iIAPId = pluid;

        TPtrC qTable = TPtrC(IAP); // Check if IAP-table free
        TPtrC qDB = TPtrC(COMMDB_ID);

        //Check if URI is /AP/*/NAPDef/* then it should be check IAP ID in DB not IAPService
        if ((iIAPId >= 0)
                && ((aURI.Match(_L8("AP/*/NAPDef/*")) != KErrNotFound)
                        || (aURI.Match(_L8("AP/*/NAPDef/*/Bearer/*"))
                                != KErrNotFound) || (aURI.Match(_L8("AP/*"))
                        != KErrNotFound) || (aURI.Match(_L8("AP/*/NAPDef"))
                        != KErrNotFound)))
            {
            //Get parent UID and check if that exist if not add 
            iIAPId = GetAPIdFromURIL(aURI);

            }
        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                qTable, qDB, iIAPId);

        TInt errorCode = tableView->GotoFirstRecord();

        CleanupStack::PopAndDestroy(tableView);

        // Node allready exists => No adding possible
        if (errorCode == KErrNone)
            {
            if (!iExecutingBuffer)
                {
                AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::EAlreadyExists); // EError
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddNodeObjectL(): EAlreadyExists end");
                }
            return;
            }
        else if (iIAPId == 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): ENotFound end");
            return;
            }
        else if (iIAPId > 0 && errorCode == KErrNotFound) // Stale data, needs update
            {

            _DBG_FILE(
                    "CNSmlInternetAdapter::AddNodeObjectL(): stale data Add to buffer");
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            iStaleMapUpdate = ETrue;
            return;
            }

        }
    else if (aURI.Match(_L8("AP/*/Px/*")) != KErrNotFound && // How to check if already added
            aURI.Match(_L8("AP/*/Px/*/*")) == KErrNotFound) // dm-tree knows last LUID
        {
        if (parentLUID.Length() <= 0)
            {
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            return;
            }

        // Check if already exists on database (=> only One possible for AP)
        if (GetProxyIdL(aURI))
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EAlreadyExists);
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddNodeObjectL(): EAlreadyExists end");
            return;
            }

        TUint32 iapID = GetAPIdFromURIL(aURI);

        //  IAP-table search serviceType (=Bearer)
        CCommsDbTableView* iapView = iDatabase->OpenViewMatchingUintLC(TPtrC(
                IAP), TPtrC(COMMDB_ID), iapID);

        TInt errorCode = iapView->GotoFirstRecord();

        // Read serviceType from (IAP)
        if (errorCode == KErrNone)
            {
            iapView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), iProxyServiceType);
            iapView->ReadUintL(TPtrC(IAP_SERVICE), iProxyISP);
            CleanupStack::PopAndDestroy(); // iapView
            }
        else if (iapID > 0 && errorCode == KErrNotFound) // Stale data, needs update
            {

            CleanupStack::PopAndDestroy(); // iapViewv
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddNodeObjectL(): stale data for proxy this as new AP ");
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            iStaleMapUpdate = ETrue;
            return;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // iapView
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): ENotFound end");
            return;
            }

        CCommsDbTableView* proxyTable;

        proxyTable = iDatabase->OpenTableLC(TPtrC(PROXIES));

        TInt initOK = proxyTable->InsertRecord(iapID);

        TInt retry = KBeginTransRetryCount;
        while (retry > 0 && initOK != KErrNone)
            {
            User::After(KBeginTransRetryDelay);
            initOK = proxyTable->InsertRecord(iapID);
            retry--;
            }

        if (initOK == KErrNone)
            {
            iProxyServerName = LastURISeg(aURI); // Name from URI
            initOK = InitializeProxyL(proxyTable);

            User::After(KBeginTransRetryDelay);
            if (initOK == KErrNone)
                {
                TUint32 newId = 0;
                initOK = proxyTable->PutRecordChanges();

                retry = KBeginTransRetryCount;
                while (retry > 0 && initOK != KErrNone)
                    {
                    User::After(KBeginTransRetryDelay);
                    initOK = proxyTable->PutRecordChanges();
                    retry--;
                    }

                proxyTable->ReadUintL(TPtrC(COMMDB_ID), newId);
                //
                // Set mapping-data
                //
                TBuf8<16> addLUID;
                _LIT8(KFormat, "%d");
                addLUID.Format(KFormat, KNSmlApProxyLowerBase + newId); // 100000 + lProxyId

                // Also added to mapping
                iCallBack->SetMappingL(aURI, addLUID);
                }
            else
                {
                proxyTable->CancelRecordChanges();
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddNodeObjectL(): EError end");
                }
            }
        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddNodeObjectL(): EError end Insert failed");
            }
        CleanupStack::PopAndDestroy(); // proxyTable

        }
    else if (aURI.Match(_L8("AP/*/Px/*/*")) != KErrNotFound)
        {
        if (parentLUID.Length() <= 0)
            {
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): end");
            return;
            }

        TInt pluid = IntLUID(parentLUID);

        if (pluid < 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): end");
            return;
            }
        else
            {
            TUint32 apID = GetAPIdFromURIL(aURI);
            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), apID);
            TInt errorCode = tableView->GotoFirstRecord();
            CleanupStack::PopAndDestroy(tableView);
            if (apID > 0 && errorCode == KErrNotFound) // Stale data, needs update
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddNodeObjectL(): stale data for proxy this as new AP ");
                AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
                iStaleMapUpdate = ETrue;
                return;
                }

            }
        }

    else if (aURI.Match(_L8("AP/*/Px")) != KErrNotFound)
        {
        if (parentLUID.Length() <= 0)
            {
            AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
            return;
            }

        TInt pluid = IntLUID(parentLUID);

        if (pluid < 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): end");
            return;
            }
        else if (pluid == 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): EError");
            return;
            }
        else
            {
            TUint32 apID = GetAPIdFromURIL(aURI);
            CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                    TPtrC(IAP), TPtrC(COMMDB_ID), apID);
            TInt errorCode = tableView->GotoFirstRecord();
            CleanupStack::PopAndDestroy(tableView);
            if (apID > 0 && errorCode == KErrNotFound) // Stale data, needs update
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter::AddNodeObjectL(): stale data for proxy this as new AP ");
                AddNodeBufferL(aURI, parentLUID, aStatusRef, EFalse);
                iStaleMapUpdate = ETrue;
                return;
                }
            }
        }

    else if (aURI.Match(_L8("AP/*/Networks/*")) != KErrNotFound
            && aURI.Match(_L8("AP/*/Networks/*/*")) == KErrNotFound)
        {
        iIAPId = IntLUID(parentLUID);
        }

    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
    _DBG_FILE("CNSmlInternetAdapter::AddNodeObjectL(): end");
    return;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::ExecuteCommandL
// not supported
//------------------------------------------------------------------------------


void CNSmlInternetAdapter::ExecuteCommandL(const TDesC8&/* aURI*/,
        const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/,
        const TDesC8& /*aType*/, TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::ExecuteCommandL(): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlInternetAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::ExecuteCommandL ( .. RWriteStream ..)
// not supported
//------------------------------------------------------------------------------


void CNSmlInternetAdapter::ExecuteCommandL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/,
        const TDesC8& /*aType*/, TInt aStatusref)
    {
    _DBG_FILE("CNSmlInternetAdapter::ExecuteCommandL(): begin");
    iCallBack->SetStatusL(aStatusref, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlInternetAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::CopyCommandL
// not supported
//------------------------------------------------------------------------------


void CNSmlInternetAdapter::CopyCommandL(const TDesC8& /*aTargetURI*/,
        const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/,
        const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/,
        TInt aStatusRef)
    {
    _DBG_FILE("CNSmlInternetAdapter::CopyCommandL(): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlInternetAdapter::CopyCommandL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::StartAtomicL
// not supported
//------------------------------------------------------------------------------


void CNSmlInternetAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlInternetAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlInternetAdapter::StartAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::CommitAtomicL
// not supported
//------------------------------------------------------------------------------


void CNSmlInternetAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlInternetAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlInternetAdapter::CommitAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::RollbackAtomicL
// not supported
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlInternetAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlInternetAdapter::RollbackAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::StreamingSupport
// returns EFalse
//------------------------------------------------------------------------------

TBool CNSmlInternetAdapter::StreamingSupport(TInt& /*aItemSize*/)
    {
    _DBG_FILE("CNSmlInternetAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlInternetAdapter::StreamingSupport(): end");
    return EFalse;
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::StreamCommittedL
// not used in this adapter
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlInternetAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlInternetAdapter::StreamCommittedL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::CompleteOutstandingCmdsL
// 
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::CompleteOutstandingCmdsL()
    {

    _DBG_FILE("CNSmlInternetAdapter::CompleteOutstandingCmdsL(): begin");
    //
    //  Handle possible unhandled buffered nodes
    //
    if (!iDatabase->InTransaction())
        {
        _DBG_FILE("CompleteOutstandingCmds: Try first begintransaction.");
        TInt err = iDatabase->BeginTransaction();
        if (err == KErrLocked)
            {
            _DBG_FILE("CompleteOutstandingCmds: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && err == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE(
                        "CompleteOutstandingCmds: Slept 1 second. Try again");
                err = iDatabase->BeginTransaction();
                retry--;
                }
            if (err != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(err);
                }
            }
        }
    _DBG_FILE("CompleteOutstandingCmds: BeginTransaction was successful");

    iDnsIpv4Pri = 0;
    iDnsIpv6Pri = 0;
    _LIT8(KDummy, "/Dummy");
    for (TInt h = iBuffer->Count() - 1; h >= 0; h--) // Through buffers
        {
        TBuf8<80> commandURI;
        commandURI.Append(iBuffer->At(h).iMappingName->Des());
        commandURI.Append(KDummy); // Removed on execution
        ExecuteBufferL(commandURI, ETrue);
        }
    iDnsUpdate = EFalse;
    iDnsIpv4Pri = 0;
    iDnsIpv6Pri = 0;
    iBuffer->Reset();

    if (iWlanSupported)
        {
        iWlanAdapter->CompleteOutstandingCmdsL(); // Execute possible unhandled WLAN commands 
        }

    iNetworkId = KErrNotFound;

    iDatabase->CommitTransaction();
    iTransactionCommitted = ETrue;
    _DBG_FILE("CNSmlInternetAdapter::CompleteOutstandingCmdsL(): end");
    }

//=============================================
//      CNSmlInternetAdapter::GetAPField()
//      Match URI fieldname to 
//      CommsDb table and column
//      
//=============================================
TBool CNSmlInternetAdapter::GetAPField(TPtrC& aTableName, TPtrC& aColumn) const
    {
    _DBG_FILE("CNSmlInternetAdapter::GetAPField(): Start");

    TBool fRet = EFalse;

    aColumn.Set(TPtrC(KNullDesC));

    if (iField->Compare(KNSmlDdfAuthName) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(SERVICE_IF_AUTH_NAME));
            }
        else
            {
            return fRet;
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNAPName) == 0)
        {
        aColumn.Set(TPtrC(COMMDB_NAME));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfToNAPIDL) == 0)
        {
        aColumn.Set(TPtrC(COMMDB_ID));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNAPID) == 0)
        {
        aColumn.Set(TPtrC(COMMDB_ID));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPxId) == 0)
        {
        aColumn.Set(TPtrC(COMMDB_ID));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPxAddr) == 0)
        {
        aTableName.Set(TPtrC(PROXIES));
        aColumn.Set(TPtrC(PROXY_SERVER_NAME));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNAPAddrTy) == 0) // Is valid
        {
        return fRet;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrL) == 0)
        {
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfDNSPriority) == 0)
        {
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrTy) == 0)
        {
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNAPAddr) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(GPRS_APN)); // GPRS_PDP_ADDRESS
            }
        else if (aTableName == TPtrC(LAN_SERVICE)) // Wlan ADD
            {
            aColumn.Set(TPtrC(ISP_IP_ADDR));
            // Leni: Should this be LAN_IP_ADDR  ??
            }
        else
            {
            return fRet;
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfBearerL) == 0)
        {
        aTableName.Set(TPtrC(IAP));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfDirection) == 0)
        {
        aTableName.Set(TPtrC(IAP));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfAuthSecr) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(SERVICE_IF_AUTH_PASS));
            }
        else
            {
            return fRet; // NotValid
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPortNbr) == 0)
        {
        if (iWapPort)
            {
            aTableName.Set(TPtrC(WAP_IP_BEARER));
            aColumn.Set(TPtrC(WAP_PROXY_PORT));
            }
        else
            {
            aTableName.Set(TPtrC(PROXIES));
            aColumn.Set(TPtrC(PROXY_PORT_NUMBER));
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfStartpg) == 0)
        {
        aTableName.Set(TPtrC(WAP_ACCESS_POINT));
        aColumn.Set(TPtrC(WAP_START_PAGE));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPxAuthId) == 0) // No KNSmlDdfBasAuthId
        {
        aTableName.Set(TPtrC(WAP_IP_BEARER));
        aColumn.Set(TPtrC(WAP_PROXY_LOGIN_NAME));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPxAuthPW) == 0) // No KNSmlDdfBasAuthPW
        {
        aTableName.Set(TPtrC(WAP_IP_BEARER));
        aColumn.Set(TPtrC(WAP_PROXY_LOGIN_PASS));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfUsePTxtLog) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(SERVICE_DISABLE_PLAIN_TEXT_AUTH));
            fRet = ETrue;
            }
        else
            {
            return fRet; // NotValid
            }
        }
    else if (iField->Compare(KNSmlDdfDefGW) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP_GATEWAY));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNetworkMask) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP_NETMASK));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNetworkID) == 0)
        {
        aColumn.Set(TPtrC(COMMDB_ID));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfPPPComp) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(GPRS_DATA_COMPRESSION));
            }
        else
            {
            return fRet; // NotValid
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfGPRSPDP) == 0)
        {
        if (aTableName == TPtrC(OUTGOING_GPRS) || aTableName == TPtrC(
                INCOMING_GPRS))
            {
            aColumn.Set(TPtrC(GPRS_PDP_TYPE));
            }
        else
            {
            return fRet; // NotValid
            }
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfNoPxForL) == 0)
        {
        aColumn.Set(TPtrC(PROXY_EXCEPTIONS));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfDomainL) == 0)
        {
        aColumn.Set(TPtrC(PROXY_PROTOCOL_NAME));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIPAddrFromServer) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP_ADDR_FROM_SERVER));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIPAddr) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP_ADDR));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrFromServer) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP_DNS_ADDR_FROM_SERVER));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIPv6DNSAddrFromServer) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IP6_DNS_ADDR_FROM_SERVER));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIfNetworks) == 0)
        {
        aColumn.Set(TPtrC(SERVICE_IF_NETWORKS));
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIAPService) == 0)
        {
        aColumn.Set(TPtrC(IAP_SERVICE));
        fRet = ETrue;
        }

    else if (iField->Compare(KNSmlDdfIAPSeamlessness) == 0)
        {
        fRet = ETrue;
        }
    else if (iField->Compare(KNSmlDdfIAPMetaData) == 0)
        {
        fRet = ETrue;
        }

    DBG_ARGS(_S16("GetAPField: aURI    - %S - %S - %S"), &aTableName,
            &aColumn, &*iField);

    _DBG_FILE("CNSmlInternetAdapter::GetAPField(): EOk");

    return fRet;
    }

//=============================================
//      CNSmlInternetAdapter::GetAPFieldType()
//      Match fieldtype to Commsdb-database 
//      
//=============================================
CNSmlInternetAdapter::TNSmlAPFieldType CNSmlInternetAdapter::GetAPFieldType(
        const TDesC8& aURI) const
    {
    if (iField->Compare(KNSmlDdfBearerL) == 0)
        {
        return CNSmlInternetAdapter::ESpec;
        }
    else if (iField->Compare(KNSmlDdfDirection) == 0)
        {
        return CNSmlInternetAdapter::ESpec;
        }
    else if (iField->Compare(KNSmlDdfBearer) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfToNAPIDL) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfNAPID) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfPxId) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfAuthName) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfNAPName) == 0 || iField->Compare(
            KNSmlDdfPxName) == 0 || iField->Compare(KNSmlDdfNetworkName) == 0)
        {
        if (aURI.Match(_L8("AP/*/Px/*/Name")) != KErrNotFound || aURI.Match(
                _L8("AP/*/Networks/*/Name")) != KErrNotFound)
            {
            return CNSmlInternetAdapter::EStr;
            }
        else
            {
            return CNSmlInternetAdapter::ESpec; // For NAPName
            }
        }
    else if (iField->Compare(KNSmlDdfNAPAddrTy) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfNAPAddr) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfStartpg) == 0)
        {
        return CNSmlInternetAdapter::EWap;
        }
    else if (iField->Compare(KNSmlDdfPxAuthId) == 0)
        {
        return CNSmlInternetAdapter::EWap;
        }
    else if (iField->Compare(KNSmlDdfPxAuthPW) == 0)
        {
        return CNSmlInternetAdapter::EWap;
        }
    else if (iField->Compare(KNSmlDdfPxAddr) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrFromServer) == 0)
        {
        return CNSmlInternetAdapter::EBool;
        }
    else if (iField->Compare(KNSmlDdfIPv6DNSAddrFromServer) == 0)
        {
        return CNSmlInternetAdapter::EBool;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrL) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfDNSAddrTy) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfDNSPriority) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfPxName) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfAuthSecr) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfPxId) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfPxPW) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfPortNbr) == 0)
        {
        if (iWapPort) // Different handlig for WAP and Proxy PortNbr
            {
            return CNSmlInternetAdapter::EWap;
            }
        else
            {
            return CNSmlInternetAdapter::EInt;
            }
        }
    else if (iField->Compare(KNSmlDdfUsePTxtLog) == 0)
        {
        return CNSmlInternetAdapter::EBool;
        }
    else if (iField->Compare(KNSmlDdfDefGW) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfNetworkMask) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfNetworkID) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfPPPComp) == 0)
        {
        return CNSmlInternetAdapter::EBool;
        }
    else if (iField->Compare(KNSmlDdfGPRSPDP) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfDomainL) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfNoPxForL) == 0)
        {
        return CNSmlInternetAdapter::ESpec;
        }
    else if (iField->Compare(KNSmlDdfIPAddrFromServer) == 0)
        {
        return CNSmlInternetAdapter::EBool;
        }
    else if (iField->Compare(KNSmlDdfIPAddr) == 0)
        {
        return EStr;
        }
    else if (iField->Compare(KNSmlDdfIfNetworks) == 0)
        {
        return CNSmlInternetAdapter::EStr;
        }
    else if (iField->Compare(KNSmlDdfIAPService) == 0)
        {
        return CNSmlInternetAdapter::ESpec;
        }

    else if (iField->Compare(KNSmlDdfIAPSeamlessness) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }
    else if (iField->Compare(KNSmlDdfIAPMetaData) == 0)
        {
        return CNSmlInternetAdapter::EInt;
        }

    _DBG_FILE("CNSmlInternetAdapter::GetAPFieldType(): Error");

    return EWrong;
    }

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)  {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x101F6DE2, CNSmlInternetAdapter::NewL)
    };

//------------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy()
//------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount)
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlInternetAdapter: begin");

    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    _DBG_FILE("ImplementationGroupProxy() for CNSmlInternetAdapter: end");
    return ImplementationTable;
    }

//=============================================
//      CNSmlInternetAdapter::IntLUID()
//      Returns IntValue for aLUID
//=============================================
TInt CNSmlInternetAdapter::IntLUID(const TDesC8& aLUID)
    {
    TLex8 lex(aLUID);

    if (lex.Val(iLUID) == KErrNone)
        {
        if ((iLUID > KNSmlApProxyUpperBase) && (iLUID
                < KNSmlApDnsLuidUpperBase))
            {
            iLUID = iLUID - KNSmlApProxyUpperBase;
            }
        else if ((iLUID > KNSmlApProxyLowerBase) && (iLUID
                < KNSmlApProxyUpperBase))
            {
            iProxyId = iLUID - KNSmlApProxyLowerBase; // Proxy 
            iLUID = iProxyId;
            }
        else if ((iLUID > KNSmlAp3gpppsLowerBase) && (iLUID
                <= KNSmlApProxyLowerBase))
            {
            iLUID = iLUID - KNSmlAp3gpppsLowerBase;
            i3GPPPS = ETrue;
            }
        else
            {
            iProxyId = 0;
            }
        return iLUID;
        }
    else
        {
        return 0;
        }
    }

//=============================================
//      CNSmlInternetAdapter::GetIntObject8()
//      Returns IntValue for aObject
//=============================================
TInt CNSmlInternetAdapter::GetIntObject8(const TDesC8& aObject)
    {
    TLex8 lex(aObject);

    TInt lValue = 0;

    if (lex.Val(lValue) == KErrNone)
        {
        return lValue;
        }
    else
        {
        return lValue;
        }
    }
//=============================================
//      CNSmlInternetAdapter::SetIntObjectLC()
//      Returns IntValue for aObject
//=============================================
TDesC8& CNSmlInternetAdapter::SetIntObjectLC(const TInt& aObject)
    {
    HBufC8* buf = HBufC8::NewLC(8);
    TPtr8 ptrBuf = buf->Des();

    ptrBuf.Num(aObject);

    return *buf;
    }

//=============================================
//      CNSmlInternetAdapter::NotValidStrLenght()
//      False if datalength is valid
//=============================================
TBool CNSmlInternetAdapter::NotValidStrLenght(const TDesC& aSource)
    {
    TInt len = aSource.Length();
    TBool theBool = EFalse;

    if (len > KCommsDbSvrMaxFieldLength)
        {
        theBool = ETrue;
        }
    return theBool;
    }

//=============================================
//      CNSmlInternetAdapter::SetField()
//      String after last '/' is returned
//      
//=============================================
TInt CNSmlInternetAdapter::SetField(const TDesC8& aSource)
    {
    if (aSource.LocateReverse('/') == KErrNotFound)
        {
        iField->Des().Format(aSource);
        }
    else
        {
        iField->Des().Format(aSource.Mid(aSource.LocateReverse('/') + 1));
        }

    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::ConvertTo8LC()
//      Converts string value to 8-bit
//      
//=============================================
TDesC8& CNSmlInternetAdapter::ConvertTo8LC(const TDesC& aSource)
    {
    HBufC8* buf = HBufC8::NewLC(aSource.Length() * 2);
    TPtr8 bufPtr = buf->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufPtr, aSource);

    return *buf;
    }

//=============================================
//      CNSmlInternetAdapter::ConvertTo16LC()
//      Converts string value to 16-bit
//      
//=============================================
TDesC16& CNSmlInternetAdapter::ConvertTo16LC(const TDesC8& aSource)
    {
    HBufC16* buf16 = HBufC16::NewLC(aSource.Length());
    TPtr bufPtr16 = buf16->Des();

    CnvUtfConverter::ConvertToUnicodeFromUtf8(bufPtr16, aSource);

    return *buf16;
    }

//=============================================
//      CNSmlInternetAdapter::InitializeGPRSL()
//      Initialize GPRS-data before insert
//      
//=============================================
TInt CNSmlInternetAdapter::InitializeGPRSL(CCommsDbTableView* aTable)
    {
    TBuf<KCommsDbSvrMaxFieldLength> user;
    TBuf<KCommsDbSvrMaxFieldLength> pass;

    _LIT(Kdns6Address, "0:0:0:0:0:0:0:0");
    _LIT(KgifNetworks, "ip");

    // COMMON DATA 
    aTable->WriteBoolL(TPtrC(SERVICE_DISABLE_PLAIN_TEXT_AUTH), EFalse);

    aTable->WriteTextL(TPtrC(GPRS_IF_NETWORKS), KgifNetworks);

    aTable->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(iIAPName)); // Same as IAP
    CleanupStack::PopAndDestroy(); //ConvertTo16LC

    aTable->WriteBoolL(TPtrC(GPRS_IF_PROMPT_FOR_AUTH), EFalse);
    aTable->WriteTextL(TPtrC(GPRS_IF_AUTH_NAME), user);
    aTable->WriteTextL(TPtrC(GPRS_IF_AUTH_PASS), pass);

    aTable->WriteBoolL(TPtrC(GPRS_IP_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(GPRS_IP_ADDR), KNSmlEmptyIpv4Address);

    aTable->WriteBoolL(TPtrC(GPRS_IP_DNS_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(GPRS_IP_NAME_SERVER1), KNSmlEmptyIpv4Address);
    aTable->WriteTextL(TPtrC(GPRS_IP_NAME_SERVER2), KNSmlEmptyIpv4Address);

    aTable->WriteBoolL(TPtrC(GPRS_IP6_DNS_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(GPRS_IP6_NAME_SERVER1), Kdns6Address);
    aTable->WriteTextL(TPtrC(GPRS_IP6_NAME_SERVER2), Kdns6Address);
    // GPRS DATA

    aTable->WriteTextL(TPtrC(GPRS_APN), ConvertTo16LC(iIAPName)); // Same as IAP
    CleanupStack::PopAndDestroy(); //ConvertTo16LC

    aTable->WriteUintL(TPtrC(GPRS_REQ_PRECEDENCE), 0);
    aTable->WriteUintL(TPtrC(GPRS_REQ_DELAY), 0);
    aTable->WriteUintL(TPtrC(GPRS_REQ_RELIABILITY), 0);
    aTable->WriteUintL(TPtrC(GPRS_REQ_PEAK_THROUGHPUT), 0);
    aTable->WriteUintL(TPtrC(GPRS_REQ_MEAN_THROUGHPUT), 0);
    aTable->WriteUintL(TPtrC(GPRS_MIN_PRECEDENCE), 0);
    aTable->WriteUintL(TPtrC(GPRS_MIN_DELAY), 0);
    aTable->WriteUintL(TPtrC(GPRS_MIN_RELIABILITY), 0);
    aTable->WriteUintL(TPtrC(GPRS_MIN_PEAK_THROUGHPUT), 0);
    aTable->WriteUintL(TPtrC(GPRS_MIN_MEAN_THROUGHPUT), 0);

    aTable->WriteUintL(TPtrC(GPRS_IF_AUTH_RETRIES), 0);

    aTable->WriteBoolL(TPtrC(GPRS_DATA_COMPRESSION), EFalse);
    aTable->WriteUintL(TPtrC(GPRS_PDP_TYPE), RPacketContext::EPdpTypeIPv4); // IPV4

    aTable->WriteBoolL(TPtrC(GPRS_ENABLE_LCP_EXTENSIONS), EFalse);
    aTable->WriteBoolL(TPtrC(GPRS_ANONYMOUS_ACCESS), EFalse);
    aTable->WriteBoolL(TPtrC(GPRS_HEADER_COMPRESSION), EFalse);

    aTable->WriteBoolL(TPtrC(GPRS_IP_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(GPRS_IP_GATEWAY), KNSmlEmptyIpv4Address);

    aTable->WriteTextL(TPtrC(GPRS_IP_ADDR), KNSmlEmptyIpv4Address);
    aTable->WriteTextL(TPtrC(GPRS_IF_PARAMS), KNullDesC);
    aTable->WriteTextL(TPtrC(GPRS_IP_NETMASK), KNullDesC);

    aTable->WriteUintL(TPtrC(GPRS_QOS_WARNING_TIMEOUT), 0xffffffff);

    aTable->WriteUintL(TPtrC(GPRS_AP_TYPE), (TUint32) EIspTypeInternetAndWAP);

    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::InitializeProxyL()
//      Initialize Proxy-data before insert
//      
//=============================================
TInt CNSmlInternetAdapter::InitializeProxyL(CCommsDbTableView* aTable)
    {
    _LIT(KprotocolName, "http"); // http, https, ftp, ftps
    const TUint32 KPortNumber(80);

    aTable->WriteUintL(TPtrC(PROXY_ISP), iProxyISP); // ISP with which these proxies are associated 

    aTable->WriteTextL(TPtrC(PROXY_SERVICE_TYPE), iProxyServiceType); // Type ISP/GPRS
    aTable->WriteBoolL(TPtrC(PROXY_USE_PROXY_SERVER), ETrue);

    TPtr proxyName16 = HBufC::NewLC(iProxyServerName.Length())->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(proxyName16, iProxyServerName);

    aTable->WriteLongTextL(TPtrC(PROXY_SERVER_NAME), proxyName16); // Name of the proxy server
    aTable->WriteTextL(TPtrC(PROXY_PROTOCOL_NAME), KprotocolName); // also https + ftps

    aTable->WriteUintL(TPtrC(PROXY_PORT_NUMBER), KPortNumber);

    aTable->WriteLongTextL(TPtrC(PROXY_EXCEPTIONS), KNullDesC);

    CleanupStack::PopAndDestroy(); //proxyName16
    return KErrNone;
    }

//      CNSmlInternetAdapter::IsInsertAllowedL
//      Check if possible to Add new AP
//
//=============================================
TBool CNSmlInternetAdapter::IsInsertAllowedL()
    {
    TBool theBool = EFalse;
    //
    //  Id + Name + Bearer needed for Insert
    //
    if (iIAPName.Length() > 0 && iBearer.Length() > 0 && iDirection
            != ECommDbConnectionDirectionUnknown)
        {
        theBool = ETrue;
        }
    //
    //  Should check also if Name or iISPId in USE !!!
    //
    if (theBool)
        {
        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingTextLC(
                iBearer, TPtrC(COMMDB_NAME), iIAPName);
        TInt errorCode = tableView->GotoFirstRecord();

        // Node allready exists => No adding possible
        if (errorCode == KErrNone)
            {
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::NODE NAME <%S> ALLREADY(): error"),
                    &iIAPName);
            theBool = EFalse;
            }
        CleanupStack::PopAndDestroy(tableView);
        }
    return theBool;
    }

//=============================================
//      CNSmlInternetAdapter::SaveIAPDataL
//      Saves data to IAP-table
//      
//=============================================
void CNSmlInternetAdapter::SaveIAPDataL(const TDesC8& aURI)
    {
    TInt errorCode;
    CCommsDbTableView* tableView;

    TBool apEnforce = EFalse;
    TBool iapReLock = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TRAPD(eError,apEnforce=CheckEnforcementL())
        DBG_ARGS8(_S8("CNSmlInternetAdapter::SaveIAPDataL(): EError %d , APEnforcement is %d"),eError,apEnforce);
        if (eError == KErrNone && apEnforce)
            {
            DoLockIAPTablesL(EFalse);
            iWAPRelock = ETrue;
            }
        }
    if (iIAPExists)
        {
        tableView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(
                COMMDB_ID), iIAPId);
        if (IsIAPRecordLockedL(iIAPId))
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
            TInt
                    reclockerr =
                            ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                    reclockerr);
            if (reclockerr == KErrNone)
                iapReLock = ETrue;
            }
        else
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
            }

        errorCode = tableView->GotoFirstRecord();

        if (errorCode == KErrNone)
            {
            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                // if enforcement is on and rec exists, it needs to be unprotected
                if (apEnforce)
                    {
                    ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                    }
                }
            errorCode = tableView->UpdateRecord();
            }
        }
    else
        {
        tableView = iDatabase->OpenTableLC(TPtrC(IAP));

        errorCode = tableView->InsertRecord(iIAPId);
        }

    if (errorCode == KErrNone)
        {
        TUint32 modemISPId = 1; // Default
        TBuf<KCommsDbSvrMaxFieldLength> modemBearer = TPtrC(MODEM_BEARER);
        if (iBearer == TPtrC(LAN_SERVICE))
            {
            modemBearer = TPtrC(LAN_BEARER);
            }
        else if (iBearer == TPtrC(VPN_SERVICE))
            {
            modemBearer = TPtrC(VIRTUAL_BEARER);
            }
        else
            {
            modemBearer = TPtrC(MODEM_BEARER);
            }

        if (modemBearer == TPtrC(MODEM_BEARER))
            {
            GetModemBearerL(modemISPId);
            }
        else if (modemBearer == TPtrC(VIRTUAL_BEARER))
            {
            modemISPId = 1;
            }
        else if (iWlanSupported)
            {
            GetLanBearerL(modemISPId);
            }
        tableView->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(iIAPName));
        CleanupStack::PopAndDestroy(); //ConvertTo16LC

        // ISP id must be written
        tableView->WriteUintL(TPtrC(IAP_SERVICE), iISPId);
        tableView->WriteTextL(TPtrC(IAP_SERVICE_TYPE), iBearer);

        tableView->WriteUintL(TPtrC(IAP_BEARER), modemISPId);
        tableView->WriteTextL(TPtrC(IAP_BEARER_TYPE), modemBearer);
        if (iNetworkId != KErrNotFound)
            {
            tableView->WriteUintL(TPtrC(IAP_NETWORK), iNetworkId); // First name on network table    
            }
        tableView->WriteUintL(TPtrC(IAP_NETWORK_WEIGHTING), 0);
        tableView->WriteUintL(TPtrC(IAP_LOCATION), 2); // Minimum=2


        errorCode = tableView->PutRecordChanges();
        }
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWAPRelock)
            {
            DoLockIAPTablesL(ETrue);
            ((CCommsDbProtectTableView*) tableView)->ProtectRecord();
            iWAPRelock = EFalse;
            }
        }
    if (iapReLock)
        {
        TInt reclockerr = DoProtectIAPRecordL(iIAPId, ETrue);
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                reclockerr);
        if (reclockerr == KErrNone)
            iapReLock = EFalse;
        }

    if (errorCode == KErrNone)
        {
        if (!iIAPExists)
            {
            TUint32 lValue;
            tableView->ReadUintL(TPtrC(COMMDB_ID), lValue);

            // Add AP
            TPtrC8 addURI = GetAddURISeg(aURI, KNSmlDdfAP);

            TBuf8<16> addLUID;
            _LIT8(KFormat, "%d");
            addLUID.Format(KFormat, lValue);

            // Also added to mapping
            iCallBack->SetMappingL(addURI, addLUID);
            DBG_ARGS8(_S8("AP:level1 URI - <%S> <%S> <%d>"), &addURI,
                    &addLUID, lValue);
            }

        iIAPExists = ETrue;
        }

    if (apEnforce)
        apEnforce = EFalse; // to rget rid of warnings

    CleanupStack::PopAndDestroy(); // tableView
    User::LeaveIfError(errorCode);
    }

//=============================================
//      CNSmlInternetAdapter::GetMaxIdL
//      Get max id from given bearer
//      
//=============================================
TUint32 CNSmlInternetAdapter::GetMaxIdL()
    {
    CCommsDbTableView* checkView;
    TUint32 maxGPRS(0);
    checkView = iDatabase->OpenTableLC(iBearer);
    TInt errorCodem = checkView->GotoFirstRecord();
    while (errorCodem == KErrNone)
        {
        TUint32 lValue;
        checkView->ReadUintL(TPtrC(COMMDB_ID), lValue);
        if (lValue > maxGPRS)
            {
            maxGPRS = lValue;
            }
        errorCodem = checkView->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy(); // checkView

    return (maxGPRS);
    }

//=============================================
//      CNSmlInternetAdapter::GetProxyIdL
//      Get proxy id for given IAP
//      
//=============================================
TBool CNSmlInternetAdapter::GetProxyIdL(const TDesC8& aURI)
    {
    TBool proxyFound = EFalse;

    CCommsDbTableView* checkView;

    // Get servicetype for IAP nbr  

    TUint32 iapID = GetAPIdFromURIL(aURI);
    if (iapID == 0)
        {
        if (IsAPUriFormatMatchPredefined(aURI))
            {
            iLUID = ConstructTreeL(aURI);
            iapID = GetAPIdFromURIL(aURI);
            }

        }
    //  IAP-table search serviceType + serviceId
    CCommsDbTableView* iapView = iDatabase->OpenViewMatchingUintLC(
            TPtrC(IAP), TPtrC(COMMDB_ID), iapID);

    TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TUint32 serviceId;

    TInt errorCode = iapView->GotoFirstRecord();

    // Read serviceType from (IAP)
    if (errorCode == KErrNone)
        {
        iapView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        iapView->ReadUintL(TPtrC(IAP_SERVICE), serviceId);
        CleanupStack::PopAndDestroy(); // iapView
        }
    else
        {
        CleanupStack::PopAndDestroy(); // iapView
        return proxyFound;
        }

    checkView = iDatabase->OpenViewMatchingTextLC(TPtrC(PROXIES), TPtrC(
            PROXY_SERVICE_TYPE), serviceType);

    TInt errorCodem = checkView->GotoFirstRecord();

    while (errorCodem == KErrNone)
        {
        TUint32 lValue;
        checkView->ReadUintL(TPtrC(PROXY_ISP), lValue);
        if (lValue == serviceId)
            {
            checkView->ReadUintL(TPtrC(COMMDB_ID), iProxyId);
            proxyFound = ETrue;
            break;
            }
        errorCodem = checkView->GotoNextRecord();
        }

    CleanupStack::PopAndDestroy(); // checkView

    return proxyFound;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::APExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::APExistsL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;

    checkView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP),
            TPtrC(COMMDB_ID), aId);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy(); // checkView

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::NetworkExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::NetworkExistsL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;

    checkView = iDatabase->OpenViewMatchingUintLC(TPtrC(NETWORK), TPtrC(
            COMMDB_ID), aId);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy(); // checkView

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::NetworkNameExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::NetworkNameExistsL(TUint32& aId,
        const TDesC8& aObject)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;

    checkView = iDatabase->OpenViewMatchingTextLC(TPtrC(NETWORK), TPtrC(
            COMMDB_NAME), ConvertTo16LC(aObject));
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        checkView->ReadUintL(TPtrC(COMMDB_ID), aId);
        ret = ETrue;
        }

    CleanupStack::PopAndDestroy(2); // checkView ConvertTo16LC

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::AddNetworkL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::AddNetworkL(TUint32& aId, const TDesC8& aObject)
    {
    TBool ret = EFalse;
    TUint32 newId;
    CCommsDbTableView* networkView = iDatabase->OpenTableLC(TPtrC(NETWORK));

    if (networkView->InsertRecord(newId) == KErrNone)
        {
        networkView->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(aObject));
        if (networkView->PutRecordChanges() == KErrNone)
            {
            networkView->ReadUintL(TPtrC(COMMDB_ID), aId); // New networks Id
            ret = ETrue;
            }
        CleanupStack::PopAndDestroy(); //ConvertTo16LC
        }
    CleanupStack::PopAndDestroy(); // networkView
    DBG_ARGS8(_S8("NsmlInternetAdapter: AddNetworkL:networkId %d"), aId);

    return ret;
    }
//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::NAPDefExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::NAPDefExistsL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;

    checkView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(
            IAP_SERVICE), aId);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;

        checkView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                serviceType, TPtrC(COMMDB_ID), aId);
        error = tableView->GotoFirstRecord();

        if (error == KErrNone)
            {
            ret = ETrue;
            }
        CleanupStack::PopAndDestroy(); // tableView
        }

    CleanupStack::PopAndDestroy(); // checkView

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::PxExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::PxExistsL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* pxView;

    pxView = iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES), TPtrC(
            COMMDB_ID), aId);

    TInt errorCode = pxView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy(); // pxView

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::WapAPExistsL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::WapAPExistsL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* wapView;

    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_IP_BEARER), TPtrC(
            WAP_IAP), aId);

    TInt errorCode = wapView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        wapView->ReadUintL(TPtrC(WAP_ACCESS_POINT_ID), iWapId);
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy(); // wapView

    return ret;
    }

//------------------------------------------------------------------------------
// TUint32 CNSmlInternetAdapter::GetAPIdFromURIL(const TDesC8& aURI)
//         Get AP luid for given aURI
//------------------------------------------------------------------------------
TUint32 CNSmlInternetAdapter::GetAPIdFromURIL(const TDesC8& aURI)
    {
    HBufC8* apURI = HBufC8::NewLC(aURI.Length());

    _DBG_FILE("CNSmlInternetAdapter::GetAPIdFromURIL(): begin");
    DBG_ARGS8(_S8("AP:add aURI GetAPIdFromURIL   - %S "), &aURI);
    //
    //  Second slash after AP
    //
    TInt i = 0;
    for (i = aURI.Find(KNSmlDdfAP) + 3; i < aURI.Length(); i++)
        {
        if (aURI[i] == '/')
            {
            break;
            }
        }
    DBG_ARGS8(_S8("AP:add aURI GetAPIdFromURIL i is %d "), i);
    if (i > 0)
        {
        apURI->Des().Format(aURI.Left(i));
        }
    //
    //  Same profile as previous
    //
    if (iPrevURI->Match(*apURI) != KErrNotFound)
        {
        _DBG_FILE("CNSmlInternetAdapter::GetAPIdFromURIL(): previous uri");
        CleanupStack::PopAndDestroy(); // apURI
        return iPrevLUID;
        }
    //
    //  Fetch AP-id for URI
    //
    HBufC8* apLUID = iCallBack->GetLuidAllocL(*apURI);
    CleanupStack::PushL(apLUID);

    TUint32 resLUID = IntLUID(*apLUID);
    DBG_ARGS8(_S8("AP:add aURI GetAPIdFromURIL luidallocation is %d "),
            resLUID);

    if (resLUID > 0)
        {
        iPrevURI->Des().Format(*apURI);
        iPrevLUID = resLUID;
        }

    CleanupStack::PopAndDestroy(2); // apURI, apLUID

    return resLUID;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::DeleteProxyL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::DeleteProxyL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* pxView;

    pxView = iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES), TPtrC(
            COMMDB_ID), aId);

    TInt errorCode = pxView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        errorCode = pxView->DeleteRecord();
        if (errorCode == KErrNone)
            {
            ret = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(); // pxView
    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::DeleteWapAPL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::DeleteWapAPL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* wapView;

    TUint32 wapId = 0;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TBool apEnforce = EFalse;
        TRAPD(eError,apEnforce=CheckEnforcementL())
        DBG_ARGS8(_S8("CNSmlInternetAdapter::DeleteWAPAPL(): EError %d , APEnforcement is %d"),eError,apEnforce);
        if (eError == KErrNone && apEnforce)
            {
            DoLockIAPTablesL(EFalse);
            iWAPRelock = ETrue;
            }
        }
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_IP_BEARER), TPtrC(
            WAP_IAP), aId);

    TInt errorCode = wapView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        wapView->ReadUintL(TPtrC(WAP_ACCESS_POINT_ID), wapId); // WapIAP id
        errorCode = wapView->DeleteRecord();
        if (errorCode == KErrNone)
            {
            ret = ETrue;
            }
        }

    if (ret)
        {
        CCommsDbTableView* wapApView = iDatabase->OpenViewMatchingUintLC(
                TPtrC(WAP_ACCESS_POINT), TPtrC(COMMDB_ID), wapId);

        errorCode = wapApView->GotoFirstRecord();

        if (errorCode == KErrNone)
            {
            errorCode = wapApView->DeleteRecord();
            if (errorCode == KErrNone)
                {
                ret = ETrue;
                }
            }
        CleanupStack::PopAndDestroy(); // wapApView
        }

    CleanupStack::PopAndDestroy(); // wapView
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWAPRelock)
            {
            DoLockIAPTablesL(ETrue);
            iWAPRelock = EFalse;
            }
        }
    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::DeleteNAPDefL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::DeleteNAPDefL(TUint32 aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;
    TBool apEnforce = EFalse;
    TBool iapReLock = EFalse;

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TRAPD( eError,apEnforce=CheckEnforcementL() )
        DBG_ARGS8(_S8("CNSmlInternetAdapter::DeleteNAPDefL(): EError %d , APEnforcement is %d"),eError,apEnforce);
        if (eError == KErrNone && apEnforce)
            {
            DoLockIAPTablesL(EFalse);
            iWAPRelock = ETrue;
            }
        }

    checkView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(
            IAP_SERVICE), aId);
    if (IsIAPRecordLockedL(aId))
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
        TInt reclockerr =
                ((CCommsDbProtectTableView*) checkView)->UnprotectRecord();
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                reclockerr);
        if (reclockerr == KErrNone)
            iapReLock = ETrue;
        }
    else
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
        }
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;

        checkView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);

        CCommsDbTableView* tableView = iDatabase->OpenViewMatchingUintLC(
                serviceType, TPtrC(COMMDB_ID), aId);
        error = tableView->GotoFirstRecord();

        if (error == KErrNone)
            {
            if (FeatureManager::FeatureSupported(
                    KFeatureIdSapPolicyManagement))
                {
                if (apEnforce)
                    {
                    ((CCommsDbProtectTableView*) tableView)->UnprotectRecord();
                    }
                }

            error = tableView->DeleteRecord();
            if (error == KErrNone)
                {
                ret = ETrue;
                }
            }
        CleanupStack::PopAndDestroy(); // tableView
        }

    CleanupStack::PopAndDestroy(); // checkView

    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWAPRelock)
            {
            DoLockIAPTablesL(ETrue);
            iWAPRelock = EFalse;
            }
        }

    if (iapReLock)
        {
        TInt reclockerr = DoProtectIAPRecordL(aId, ETrue);
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting WAPAccessRecord returned code = %d"),
                reclockerr);
        if (reclockerr == KErrNone)
            iapReLock = EFalse;
        }
    if (apEnforce)
        apEnforce = EFalse; // to get rid of warnings

    return ret;
    }

// ---------------------------------------------------------------------------
// Removes iapID and linger interval from CentralRepository
// ---------------------------------------------------------------------------
//
void CNSmlInternetAdapter::DeleteLingerL(const TInt aIapId)
    {
    TInt err(KErrNone);

    // Open repository
    CRepository* repository = CRepository::NewLC(KCRUidPDPContextManager);

    // Find if an entry for "iapId" already exists in CentRep 
    RArray<TUint32> foundKeys;
    foundKeys.Reset();

    err = repository->FindEqL(KIapColumn, // partial key
            KColumnMask, // key mask
            aIapId, // value
            foundKeys); // found keys

    if (err == KErrNone)
        {
        if (foundKeys.Count() > 0)
            {
            TInt arrayCount(0);

            // Get number of iapId&linger entries in Centrep
            err = repository->Get(KPdpContextManagerLingerArrayCount,
                    arrayCount);

            if (err == KErrNone)
                {
                TInt32 row = foundKeys[0] & KRowMask;

                // Shift existing entries one position left
                for (TInt i = row + 1; i <= arrayCount; i++)
                    {
                    TInt iapId(0);
                    TInt linger(0);

                    err = repository->Get((KIapColumn | i), iapId);

                    if (err == KErrNone)
                        {
                        err = repository->Get((KLingerColumn | i), linger);
                        }

                    if (err == KErrNone)
                        {
                        err = repository->Set((KIapColumn | (i - 1)), iapId);
                        }

                    if (err == KErrNone)
                        {
                        err = repository->Set((KLingerColumn | (i - 1)),
                                linger);
                        }
                    }

                // Remove the last entry on row arrayCount
                if (err == KErrNone)
                    {
                    TUint32 errorKey(0);

                    err = repository->Delete((KIapColumn | arrayCount),
                            0xFFFFFFFF, errorKey);

                    if (err == KErrNone)
                        {
                        err = repository->Delete(
                                (KLingerColumn | arrayCount), 0xFFFFFFFF,
                                errorKey);
                        arrayCount--;
                        }
                    }

                // Update number of iapId&linger entries in Centrep
                err = repository->Set(KPdpContextManagerLingerArrayCount,
                        arrayCount);
                }
            }
        }

    foundKeys.Close();
    CleanupStack::PopAndDestroy(repository);

    }
//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::DeleteAPL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::DeleteAPL(TUint32 aId)
    {
    TBool ret = EFalse;

    //Check MIP IAP first then go forward
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        TBool apEnforce = EFalse;
        TBool wlanEnforce = EFalse;
        TInt wError = KErrNone;
        TRAP(wError, wlanEnforce = CheckWLANEnforcementL());TRAPD( eError,apEnforce=CheckEnforcementL() )
        if( wlanEnforce)
            {
            UnprotectWLANAPItemL(aId);
            }
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::DeleteAPL(): EError %d , APEnforcement is %d"),
                eError, apEnforce);
        if (eError == KErrNone && apEnforce)
            {
            DoLockIAPTablesL(EFalse);
            iWAPRelock = ETrue;
            }
        }

    if (iDatabase->InTransaction())
        iDatabase->CommitTransaction();
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    RCmConnectionMethodExt cmConMethodExt;
    TRAPD(err, cmConMethodExt = cmManagerExt.ConnectionMethodL(aId));
    CleanupClosePushL(cmConMethodExt);
    if (err == KErrNone)
        {
        TRAPD(dError, cmConMethodExt.DeleteL());
        if (dError == KErrNone)
            {
            DeleteLingerL(aId);
            ret = ETrue;
            }
        }

    CleanupStack::PopAndDestroy();
    cmManagerExt.Close();
    if (!iDatabase->InTransaction())
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("DeleteAPL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("DeleteAPL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        }

    //The following code is kept under #ifndef , so that it can be removed
    //after RD_DM_APPLICATION_LEVEL_ROAMING_SUPPORT becomes stable.

    /* #ifndef RD_DM_APPLICATION_LEVEL_ROAMING_SUPPORT
     CCommsDbTableView*  checkView;
     CCommsDbTableView*  proxyView;
     
     checkView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP),TPtrC(COMMDB_ID),  aId);
     TInt error = checkView->GotoFirstRecord();
     if (error == KErrNone)
     {
     //
     // Remove possible NAPDef-data first if ONLY IAP
     //
     TBuf<KCommsDbSvrMaxFieldLength> serviceType;
     TUint32                         serviceId;

     checkView->ReadTextL(TPtrC(IAP_SERVICE_TYPE),   serviceType);
     checkView->ReadUintL(TPtrC(IAP_SERVICE),        serviceId);  


     CCommsDbTableView* tableView;
     tableView= iDatabase->OpenViewMatchingUintLC(serviceType, TPtrC(COMMDB_ID), serviceId);
     error = tableView->GotoFirstRecord();
     if (error == KErrNone )
     {
     error = tableView->DeleteRecord();
     }
     CleanupStack::PopAndDestroy();  // tableView
     
     //
     // Delete Proxies for AP also
     //
     proxyView = iDatabase->OpenViewMatchingTextLC(TPtrC(PROXIES),
     TPtrC(PROXY_SERVICE_TYPE),
     serviceType);

     TInt errorProxy = proxyView->GotoFirstRecord();

     while (errorProxy == KErrNone)
     {
     TUint32 lValue;
     proxyView->ReadUintL(TPtrC(PROXY_ISP),lValue);
     if(lValue ==  serviceId)
     {
     proxyView->ReadUintL(TPtrC(COMMDB_ID),iProxyId);
     DBG_ARGS8(_S8("DeleteAPL:Delete ServiceId=%d ProxyId=%d"),lValue, iProxyId);
     error = proxyView->DeleteRecord();
     }
     errorProxy = proxyView->GotoNextRecord();
     }
     
     CleanupStack::PopAndDestroy();  // proxyView

     error = checkView->DeleteRecord();
     if( error == KErrNone )
     {
     ret = ETrue;
     }

     // Delete WAP also
     if(WapAPExistsL(aId))
     {
     DeleteWapAPL(aId);
     }
     }

     CleanupStack::PopAndDestroy();      // checkView
     #endif
     */
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWAPRelock)
            {
            DoLockIAPTablesL(ETrue);
            iWAPRelock = EFalse;
            }
        }
    return ret;
    }

//------------------------------------------------------------------------------
// TPtrC8 CNSmlInternetAdapter::LastURISeg(const TDesC8& aURI)
// Returns only the last uri segment
//------------------------------------------------------------------------------
TPtrC8 CNSmlInternetAdapter::LastURISeg(const TDesC8& aURI)
    {
    TInt i;
    for (i = aURI.Length() - 1; i >= 0; i--)
        {
        if (aURI[i] == '/')
            {
            break;
            }
        }
    if (i == 0)
        {
        return aURI;
        }
    else
        {
        return aURI.Mid(i + 1);
        }
    }

//------------------------------------------------------------------------------
// TPtrC8 CNSmlInternetAdapter::FirstURISeg(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
//------------------------------------------------------------------------------
TPtrC8 CNSmlInternetAdapter::FirstURISeg(const TDesC8& aURI)
    {
    TInt j = 0;
    for (TInt i = 0; i <= aURI.Length() - 1; i++)
        {
        if (aURI[i] == '/')
            {
            for (j = i + 1; j <= aURI.Length() - 1; j++)
                {
                if (aURI[j] == '/') // Second slash => AP/xxxx
                    break;
                }
            break;
            }
        }
    return aURI.Left(j);
    }

//------------------------------------------------------------------------------
// TPtrC8 CNSmlInternetAdapter::GetDynamicAPNodeUri( const TDesC8& aURI )
// returns AP/xxx URI
//------------------------------------------------------------------------------
TPtrC8 CNSmlInternetAdapter::GetDynamicAPNodeUri(const TDesC8& aURI)
    {
    _DBG_FILE("CNSmlInternetAdapter::GetDynamicAPNodeUri(): begin");
    DBG_ARGS8(_S8("CNSmlInternetAdapter::GetDynamicAPNodeUri() - <%S> "),
            &aURI);

    TInt i = 0;
    for (i = aURI.Find(KNSmlDdfAP) + 3; i < aURI.Length(); i++)
        {
        if (aURI[i] == '/')
            {
            break;
            }
        }

    _DBG_FILE("CNSmlInternetAdapter::GetDynamicAPNodeUri(): end");

    return aURI.Left(i);
    }

//------------------------------------------------------------------------------
// CNSmlInternetAdapter::ExecuteBufferL(const TDesC8& aURI)
// Executes commands for AP after Bearer-data is set
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::ExecuteBufferL(const TDesC8& aURI,
        const TBool aCompleting)
    {
    //
    // Choose AP according to aURI
    //
    iExecutingBuffer = ETrue;
    for (TInt i = 0; i < iBuffer->Count(); i++)
        {
        TPtrC8 parentUri = FirstURISeg(aURI);
        if (iBuffer->At(i).iMappingName->Compare(parentUri) == 0)
            {
            iExecutionIndex = i;
            break;
            }
        }

    if ((iExecutionIndex < 0) || (iExecutionIndex >= iBuffer->Count()))
        {
        return;
        }

    iISPId = 0;
    //
    // Execute all commands for One AP NAPDef
    //
    if (iBuffer->At(iExecutionIndex).iDnsUpdateBuf)
        {
        iDnsUpdate = ETrue;
        }

    for (TInt cmd = iBuffer->At(iExecutionIndex).iNodeBuf->Count() - 1; cmd
            >= 0; cmd--)
        {
        TBuf8<16> addLUID;
        _LIT8(KFormat, "%d");

        if (iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iUri->Match(_L8(
                "AP/*/Px*")) != 0)
            {

            if (!iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iLeaf)
                {
                addLUID.Format(KFormat, 0);
                if (iBuffer->At(iExecutionIndex).iLuid > 0)
                    {
                    addLUID.Format(KFormat, -1); // Used for Status return
                    }
                AddNodeObjectL(
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iUri->Des(),
                        addLUID, iBuffer->At(iExecutionIndex).iNodeBuf->At(
                                cmd).iStatusRef);

                iBuffer->At(iExecutionIndex).iExecuted = ETrue;
                }
            else
                {
                if (iBuffer->At(iExecutionIndex).iDnsUpdateBuf)
                    {
                    addLUID.Zero();
                    addLUID.Append(iBuffer->At(iExecutionIndex).iNodeBuf->At(
                            cmd).iLuid->Des());
                    }
                else
                    {
                    addLUID.Format(KFormat,
                            iBuffer->At(iExecutionIndex).iLuid);
                    if (!iBuffer->At(iExecutionIndex).iDirection || // The three values has to be set
                            !iBuffer->At(iExecutionIndex).iBearer
                            || !iBuffer->At(iExecutionIndex).iNameReceived) // before new AP set
                        {
                        addLUID.Format(KFormat, KNSmlIncompleteAP); // (-1) Used for error 
                        } // Status return
                    }
                //iIAPName = iBuffer->At(iExecutionIndex).iName->Des();

                AddLeafObjectL(
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iUri->Des(),
                        addLUID,
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iData->Des(),
                        KNSmlMimeTypeTextPlain,
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iStatusRef);

                if (iBuffer->At(iExecutionIndex).iLuid == 0) // After BearerL
                    iBuffer->At(iExecutionIndex).iLuid = iISPId;
                }

            TInt uidValue = IntLUID(addLUID);
            if ((iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iUri->Des().Match(
                    _L8("AP/*/NAPDef/*/NAPAddr")) != KErrNotFound)
                    && (uidValue == KErrNone))
                {
                iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iDone = EFalse;
                }
            else
                {
                iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd).iDone = ETrue;
                }

            }

        }
    //
    // Execute all commands for One AP Px
    //
    for (TInt cmd2 = 0; cmd2 < iBuffer->At(iExecutionIndex).iNodeBuf->Count(); cmd2++)
        {
        TBuf8<16> addLUID2;
        _LIT8(KFormat, "%d");
        if (!iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iDone)
            {
            if (!iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iLeaf)
                {
                addLUID2.Format(KFormat, 0);
                if (iBuffer->At(iExecutionIndex).iLuid > 0)
                    {
                    addLUID2.Format(KFormat, -1); // Used for Status return
                    }
                AddNodeObjectL(
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iUri->Des(),
                        addLUID2, iBuffer->At(iExecutionIndex).iNodeBuf->At(
                                cmd2).iStatusRef);

                iBuffer->At(iExecutionIndex).iExecuted = ETrue;
                }
            else
                {
                addLUID2.Format(KFormat, iBuffer->At(iExecutionIndex).iLuid);
                iIAPName = iBuffer->At(iExecutionIndex).iName->Des();
                if (!iBuffer->At(iExecutionIndex).iDirection || // Allvalues has to be set
                        !iBuffer->At(iExecutionIndex).iBearer
                        || !iBuffer->At(iExecutionIndex).iName) // before new AP set
                    {
                    addLUID2.Format(KFormat, KNSmlIncompleteAP); // (-1) Used for error 
                    } // Status return

                AddLeafObjectL(
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iUri->Des(),
                        addLUID2,
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iData->Des(),
                        KNSmlMimeTypeTextPlain,
                        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iStatusRef);

                if (iBuffer->At(iExecutionIndex).iLuid == 0) // After BearerL
                    iBuffer->At(iExecutionIndex).iLuid = iISPId;
                }
            } //++

        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iDone = ETrue;

        delete iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iUri;
        iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iUri = NULL;

        if (iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iData)
            {
            delete iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iData;
            iBuffer->At(iExecutionIndex).iNodeBuf->At(cmd2).iData = NULL;
            }
        }

    // Wlan buffer execution only when called from CompleteOutstandingCmdsL (= aCompleting)
    for (TInt i = 0; aCompleting && i
            < iBuffer->At(iExecutionIndex).iWlanNodeBuf->Count(); ++i)
        {
        TBuf8<16> addLUID;
        _LIT8(KFormat, "%d");

        if (!iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iLeaf)
            {
            addLUID.Format(KFormat, 0);

            if (iBuffer->At(iExecutionIndex).iLuid > 0)
                {
                addLUID.Format(KFormat, iBuffer->At(iExecutionIndex).iLuid);
                }

            AddNodeObjectL(
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iUri->Des(),
                    addLUID,
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iStatusRef);

            iBuffer->At(iExecutionIndex).iExecuted = ETrue;
            }
        else
            {
            addLUID.Zero();
            addLUID.Append(
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iLuid->Des());

            iIAPName = iBuffer->At(iExecutionIndex).iName->Des();

            AddLeafObjectL(
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iUri->Des(),
                    addLUID,
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iData->Des(),
                    KNSmlMimeTypeTextPlain,
                    iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iStatusRef);

            if (iBuffer->At(iExecutionIndex).iLuid == 0) // After BearerL            
                {
                iBuffer->At(iExecutionIndex).iLuid = iISPId;
                }

            }

        iBuffer->At(iExecutionIndex).iWlanNodeBuf->At(i).iDone = ETrue;
        }

    //
    //  Remove data from handled AP-commands
    //

    ClearBuffer(iBuffer->At(iExecutionIndex).iNodeBuf);

    if (aCompleting)
        {
        delete iBuffer->At(iExecutionIndex).iNodeBuf;

        ClearBuffer(iBuffer->At(iExecutionIndex).iWlanNodeBuf);
        delete iBuffer->At(iExecutionIndex).iWlanNodeBuf;

        if (iBuffer->At(iExecutionIndex).iMappingName)
            {
            delete iBuffer->At(iExecutionIndex).iMappingName;
            iBuffer->At(iExecutionIndex).iMappingName = NULL;
            }

        if (iBuffer->At(iExecutionIndex).iName)
            {
            delete iBuffer->At(iExecutionIndex).iName;
            iBuffer->At(iExecutionIndex).iName = NULL;
            }

        iBuffer->Delete(iExecutionIndex);
        iBuffer->Compress();
        }

    iExecutingBuffer = EFalse;
    iStaleMapUpdate = EFalse;
    }

//------------------------------------------------------------------------------
// TPtrC16 CNSmlInternetAdapter::GetAddURISeg(const TDesC8& aURI,const TDesC8 aName)
// Returns uri segment until the next segment after given aName
//------------------------------------------------------------------------------
TPtrC8 CNSmlInternetAdapter::GetAddURISeg(const TDesC8& aURI,
        const TDesC8& aName)
    {
    TInt lStrt = aURI.Find(aName);
    if (lStrt == KErrNotFound)
        {
        return aURI;
        }

    lStrt += 1;
    TInt i = 0;
    for (i = lStrt + aName.Length(); i <= aURI.Length() - 1; i++)
        {
        if (aURI[i] == '/')
            {
            break;
            }
        }
    if (i == 0)
        {
        return aURI;
        }
    else
        {
        return aURI.Left(i);
        }
    }

//=============================================
//      CNSmlInternetAdapter::InitializeWAPAIPL()
//      Initialize WAP IAP-data before insert
//      
//=============================================
TInt CNSmlInternetAdapter::InitializeWAPIAPL(CCommsDbTableView* aTable,
        const TDesC8& aName)
    {
    aTable->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(aName)); // Same as IAP


    aTable->WriteTextL(TPtrC(WAP_CURRENT_BEARER), iWapBearer);
    aTable->WriteLongTextL(TPtrC(WAP_START_PAGE), ConvertTo16LC(
            iWAPAccessPoint));
    CleanupStack::PopAndDestroy(2); //ConvertTo16LC
    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::InitializeWAPL()
//      Initialize WAP IP Bearer-data  before insert
//      
//=============================================
TInt CNSmlInternetAdapter::InitializeWAPL(CCommsDbTableView* aTable)
    {
    TUint32 wapPortNumber = 0;

    aTable->WriteUintL(TPtrC(WAP_ACCESS_POINT_ID), iWapId);
    aTable->WriteTextL(TPtrC(WAP_GATEWAY_ADDRESS), KNSmlEmptyIpv4Address);

    aTable->WriteUintL(TPtrC(WAP_WSP_OPTION), EWapWspOptionConnectionOriented); // EWapWspOptionConnectionless
    aTable->WriteBoolL(TPtrC(WAP_SECURITY), EFalse); // if 9200 => false

    aTable->WriteUintL(TPtrC(WAP_IAP), iISPId);
    aTable->WriteUintL(TPtrC(WAP_PROXY_PORT), wapPortNumber);

    aTable->WriteTextL(TPtrC(WAP_PROXY_LOGIN_NAME), ConvertTo16LC(
            iWapUserName));
    aTable->WriteTextL(TPtrC(WAP_PROXY_LOGIN_PASS), ConvertTo16LC(
            iWapPassword));

    CleanupStack::PopAndDestroy(2); //ConvertTo16LC
    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::IsWapPort()
//      Check if portnbr for proxy or wap
//      
//=============================================
void CNSmlInternetAdapter::IsWapPort(const TDesC8& aObject)
    {
    iWapPort = EFalse;
    TInt portNbr = GetIntObject8(aObject);
    if (portNbr >= KNSmlApWapLowerBase && portNbr <= KNSmlApWapUpperBase)
        {
        iWapPort = ETrue;
        }
    }

// -------------------------------------------------------------------------------------
// CNSmlInternetAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CNSmlInternetAdapter::FillNodeInfoL(MSmlDmDDFObject& aNode,
        TSmlDmAccessTypes aAccTypes, MSmlDmDDFObject::TOccurence aOccurrence,
        MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,
        const TDesC8& aDescription)

    {
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);
    if (aFormat != MSmlDmDDFObject::ENode)
        {
        aNode.AddDFTypeMimeTypeL(KNSmlMimeType);
        }
    aNode.SetDescriptionL(aDescription);
    }

//=======================================================
//      CNSmlInternetAdapter::UpdateIAPnameL()
//      Update IAP-table name add Network-table if needed
//      Add/update WAP-table name also
//      
//=======================================================
TInt CNSmlInternetAdapter::UpdateIAPnameL(TUint32 aId, const TDesC8& aObject)
    {

    TUint32 nwId = 0;
    TInt ret = KErrNone;
    TBool iapReLock = EFalse;

    TBool apEnforce = EFalse;
    TRAPD( eError,apEnforce=CheckEnforcementL() )
    DBG_ARGS8(_S8("CNSmlInternetAdapter::UpdateIAPNAme(): EError %d , APEnforcement is %d"),eError,apEnforce);
    if (eError == KErrNone && apEnforce)
        {
        DoLockIAPTablesL(EFalse);
        iWAPRelock = ETrue;
        }

    CCommsDbTableView* iapUpdate = iDatabase->OpenViewMatchingUintLC(TPtrC(
            IAP), TPtrC(COMMDB_ID), aId);
    if (IsIAPRecordLockedL(aId))
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is LOCKED");
        TInt reclockerr =
                ((CCommsDbProtectTableView*) iapUpdate)->UnprotectRecord();
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                reclockerr);
        if (reclockerr == KErrNone)
            iapReLock = ETrue;
        }
    else
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::AddLeafObjectL(): apaccesspoint record is not write protected");
        }
    TInt error = iapUpdate->GotoFirstRecord();
    if (error == KErrNone)
        {
        _DBG_FILE("CNSmlInternetAdapter::UpdateIAPName, IAP_NETWORK set");
        TRAPD(err, iapUpdate->ReadUintL(TPtrC(IAP_NETWORK), nwId));

        if (iapUpdate->UpdateRecord() == KErrNone)
            {
            iapUpdate->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(aObject));

            if (err != KErrNone) // Network entry not yet created
                {
                AddNetworkL(nwId, aObject);
                if (nwId > 0)
                    {
                    _DBG_FILE(
                            "CNSmlInternetAdapter::UpdateIAPName, IAP_NETWORK set");
                    DBG_ARGS8(
                            _S8(
                                    "CNSmlInternetAdapter::UpdateIAPName(): networks is %d"),
                            nwId);
                    iapUpdate->WriteUintL(TPtrC(IAP_NETWORK), nwId);
                    }
                else
                    {
                    ret = KErrGeneral;
                    }
                }
            iapUpdate->PutRecordChanges();
            }
        CleanupStack::PopAndDestroy(); //ConvertTo16LC
        }
    CleanupStack::PopAndDestroy(); // iapUpdate


    if (WapAPExistsL(aId))
        { // Update handled
        CCommsDbTableView* wapView;
        wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
                TPtrC(COMMDB_ID), iWapId);
        TInt errorCode = wapView->GotoFirstRecord();

        // Read all columns needed.
        if (errorCode == KErrNone)
            {
            if (wapView->UpdateRecord() == KErrNone)
                {
                wapView->WriteTextL(TPtrC(COMMDB_NAME),
                        ConvertTo16LC(aObject));
                wapView->PutRecordChanges();
                CleanupStack::PopAndDestroy(); //ConvertTo16LC
                }
            }
        CleanupStack::PopAndDestroy(); // wapView
        }
    else
        { // Insert handled
        CCommsDbTableView* insView;
        TInt wapInitOK = KErrCancel;

        insView = iDatabase->OpenTableLC(TPtrC(WAP_ACCESS_POINT));
        iWapId = 0;
        wapInitOK = insView->InsertRecord(iWapId);
        if (wapInitOK == KErrNone)
            {
            iWAPAccessPoint.Zero();
            iWapUserName.Zero();
            iWapPassword.Zero();
            iWapBearer = TPtrC(WAP_IP_BEARER);

            wapInitOK = InitializeWAPIAPL(insView, aObject);
            wapInitOK = insView->PutRecordChanges();

            if (wapInitOK == KErrNone)
                {
                CCommsDbTableView* updView;
                TUint32 iWapId2 = 0;
                iISPId = aId; // IAP number set
                updView = iDatabase->OpenTableLC(TPtrC(WAP_IP_BEARER));
                if (updView->InsertRecord(iWapId2) == KErrNone)
                    {
                    InitializeWAPL(updView);
                    updView->PutRecordChanges();
                    }
                CleanupStack::PopAndDestroy(); // updView
                }
            }
        else
            {
            ret = KErrGeneral;
            }
        CleanupStack::PopAndDestroy(); // insView
        }
    if (FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
        {
        if (iWAPRelock)
            {
            DoLockIAPTablesL(ETrue);
            iWAPRelock = EFalse;
            }
        }
    if (iapReLock)
        {
        TInt reclockerr = DoProtectIAPRecordL(aId, ETrue);
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::AddLeafObjectL ,UnProtecting IAPAccessRecord returned code = %d"),
                reclockerr);
        if (reclockerr == KErrNone)
            iapReLock = EFalse;
        }
    return ret;
    }

//=============================================
//      CNSmlInternetAdapter::InitializeVPNServiceL()
//      Initialize VPNService-data before insert
//=============================================
TInt CNSmlInternetAdapter::InitializeVPNServiceL(CCommsDbTableView* aTable)
    {
    _LIT(KdefPolicy, "Home");

    aTable->WriteTextL(TPtrC(COMMDB_NAME), iIAPName); // Same as IAP

    aTable->WriteTextL(TPtrC(VPN_SERVICE_POLICY), KdefPolicy);
    aTable->WriteUintL(TPtrC(VPN_SERVICE_IAP), 1);
    aTable->WriteUintL(TPtrC(VPN_SERVICE_NETWORKID), 1);

    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::InitializeLANServiceL()
//      Initialize LANService-data before insert
//=============================================
TInt CNSmlInternetAdapter::InitializeLANServiceL(CCommsDbTableView* aTable)
    {
    if (!iWlanSupported)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::InitializeLANServiceL, WLAN not supported.");
        return KErrNotSupported;
        }
    _LIT(KgifNetworks, "ip,ip6");
    _LIT(Kdns6Address, "0:0:0:0:0:0:0:0");
    _LIT(KdefaultNetmask, "255.255.255.0");

    aTable->WriteTextL(TPtrC(COMMDB_NAME), ConvertTo16LC(iIAPName)); // Same as IAP
    aTable->WriteTextL(TPtrC(LAN_IF_NETWORKS), KgifNetworks);
    aTable->WriteBoolL(TPtrC(SERVICE_IP_ADDR_FROM_SERVER), ETrue); // DHCP by default.
    aTable->WriteTextL(TPtrC(SERVICE_IP_ADDR), KNSmlEmptyIpv4Address);
    aTable->WriteTextL(TPtrC(SERVICE_IP_NETMASK), KdefaultNetmask);
    aTable->WriteBoolL(TPtrC(SERVICE_IP_DNS_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(SERVICE_IP_NAME_SERVER1), KNSmlEmptyIpv4Address);
    aTable->WriteTextL(TPtrC(SERVICE_IP_NAME_SERVER2), KNSmlEmptyIpv4Address);
    aTable->WriteTextL(TPtrC(SERVICE_IP_GATEWAY), KNSmlEmptyIpv4Address);

    aTable->WriteBoolL(TPtrC(SERVICE_IP6_DNS_ADDR_FROM_SERVER), ETrue);
    aTable->WriteTextL(TPtrC(SERVICE_IP6_NAME_SERVER1), Kdns6Address);
    aTable->WriteTextL(TPtrC(SERVICE_IP6_NAME_SERVER2), Kdns6Address);
    aTable->WriteTextL(TPtrC(SERVICE_CONFIG_DAEMON_MANAGER_NAME),
            KDaemonManagerName);
    aTable->WriteTextL(TPtrC(SERVICE_CONFIG_DAEMON_NAME), KConfigDaemonName);

    CleanupStack::PopAndDestroy(); //ConvertTo16LC
    return KErrNone;
    }

//=============================================
//      CNSmlInternetAdapter::IsWLANfield()
//      Check if handled by WLAN-adapter
//=============================================
TBool CNSmlInternetAdapter::IsWLANfield(const TDesC8& aURI)
    {
    TBool ret(EFalse);

    SetField(aURI);

    if (KNSmlDdfNetworkID().Match(*iField) == 0) // ID not valid
        {
        return ret;
        }
    if (KNSmlDdfAP().Match(*iField) == 0) // AP not valid
        {
        return ret;
        }
    if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*")) != KErrNotFound)
        {
        DBG_ARGS8(_S8("AP:IsWLANfield URI - <%S> <%S> FOUND"), &aURI, iField);
        ret = ETrue;
        }

    return ret;
    }

//=============================================
//      CNSmlInternetAdapter::IsWlanL()
//      Check if WLAN accesspoint
//=============================================
TBool CNSmlInternetAdapter::IsWlanL(const TDesC8& aURI)
    {
    TBool ret(EFalse);

    if (!iWlanSupported)
        {
        _DBG_FILE("CNSmlInternetAdapter::IsWlanL, WLAN not supported.");
        return ret;
        }

    TBuf<KCommsDbSvrMaxFieldLength> serviceType;

    // Get IAP nbr for servicetype
    TUint32 apWlan = GetAPIdFromURIL(aURI);

    //  IAP-table search serviceType (=Bearer)
    CCommsDbTableView* iapViews = iDatabase->OpenViewMatchingUintLC(
            TPtrC(IAP), TPtrC(COMMDB_ID), apWlan);

    TInt errorCode = iapViews->GotoFirstRecord();

    // Read iapService from (IAP)
    if (errorCode == KErrNone)
        {
        iapViews->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        if (serviceType == TPtrC(LAN_SERVICE))
            {
            ret = ETrue;
            }
        }
    CleanupStack::PopAndDestroy(); // iapViews

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::GetLanBearerL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::GetLanBearerL(TUint32& aId)
    {
    TBool ret = EFalse;

    if (!iWlanSupported)
        {
        _DBG_FILE("CNSmlInternetAdapter::GetLanBearerL, WLAN not supported.");
        return ret;
        }

    CCommsDbTableView* checkView;

    HBufC* bName = HBufC::NewLC(15);
    TPtr bNamePtr = bName->Des();
    _LIT(KWlanBearer, "WLANBearer");
    bNamePtr.Format(KWlanBearer);

    checkView = iDatabase->OpenViewMatchingTextLC(TPtrC(LAN_BEARER), TPtrC(
            COMMDB_NAME), bNamePtr);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        checkView->ReadUintL(TPtrC(COMMDB_ID), aId);
        ret = ETrue;
        }
    else
        {
        aId = 1; // Default modem 
        }
    DBG_ARGS8(_S8("CNSmlInternetAdapter:GetLanBearerL %d"), aId);

    CleanupStack::PopAndDestroy(2); // checkView bName

    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::GetModemBearerL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::GetModemBearerL(TUint32& aId)
    {
    TBool ret = EFalse;
    CCommsDbTableView* checkView;

    HBufC* bName = HBufC::NewLC(15);
    TPtr bNamePtr = bName->Des();

    if (iBearer == TPtrC(OUTGOING_GPRS) || iBearer == TPtrC(INCOMING_GPRS))
        {
        _LIT(KGprsModem, "GPRS Modem");
        bNamePtr.Format(KGprsModem);
        }

    checkView = iDatabase->OpenViewMatchingTextLC(TPtrC(MODEM_BEARER), TPtrC(
            COMMDB_NAME), bNamePtr);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
        checkView->ReadUintL(TPtrC(COMMDB_ID), aId);
        ret = ETrue;
        }
    else
        {
        aId = 5; // Default modem 1->5 ???
        }
    DBG_ARGS8(_S8("CNSmlInternetAdapter:GetModemBearerL %d"), aId);

    CleanupStack::PopAndDestroy(2); // checkView bName

    return ret;
    }

// ---------------------------------------------------------
//      NextAPNameL()
//      Generates new AP name.
// ---------------------------------------------------------
void CNSmlInternetAdapter::NextAPNameL(const TDesC8& aName)
    {
    // generate name candidate
    HBufC8* nameCandidate = HBufC8::NewLC(aName.Length() + 12);
    TPtr8 candidatePtr = nameCandidate->Des();

    // loop while valid name is found
    TBool validName;

    // loop while valid name is found
    TInt i = 0;
    do
        {
        validName = ETrue;
        candidatePtr.Format(aName);
        if (i > 0)
            {
            _LIT(KLeftBrace, "(");
            _LIT(KRightBrace, ")");
            HBufC* buf = HBufC::NewLC(10); //10 = max length of 32bit integer
            TPtr ptrBuf = buf->Des();
            ptrBuf.Num(i);
            candidatePtr.Append(KLeftBrace);
            candidatePtr.Append(ptrBuf);
            candidatePtr.Append(KRightBrace);
            CleanupStack::PopAndDestroy(); //buf
            }
        i++;
        if (!IsValidNameL(candidatePtr))
            {
            validName = EFalse;
            }
        }
    while (!validName);

    iIAPName = candidatePtr;

    CleanupStack::PopAndDestroy(); //nameCandidate
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::IsValidNameL()
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::IsValidNameL(const TDesC8& aName)
    {
    TBool ret = ETrue;
    CCommsDbTableView* nameView;

    nameView = iDatabase->OpenViewMatchingTextLC(TPtrC(IAP), TPtrC(
            COMMDB_NAME), aName);

    TInt errorCode = nameView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        ret = EFalse; // If found error returned        
        }

    CleanupStack::PopAndDestroy(); // nameView
    return ret;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::InitializeDNSParamsFromBuffL(const TPtrC8 aParentUri,
//                                                          TInt& aDnsPri )
// Finds parameters needed in updating DNSAddrL from buffer
//------------------------------------------------------------------------------
TBool CNSmlInternetAdapter::InitializeDNSParamsFromBuffL(
        const TPtrC8 aParentUri, TInt& aDnsPri)
    {
    TInt cmd(0);
    TBool found(EFalse);

    TNSmlAPBufferElement& thisElem = iBuffer->At(0);

    // Parse URI to match
    CBufBase* buf = CBufFlat::NewL(150);
    CleanupStack::PushL(buf);
    buf->InsertL(0, aParentUri);
    buf->InsertL(buf->Size(), KNSmlDmApValFSlash);
    buf->InsertL(buf->Size(), KNSmlDdfDNSPriority);
    TBuf8<150> tmp;
    buf->Read(0, tmp, buf->Size());

    cmd = thisElem.iNodeBuf->Count() - 1;
    // check if the DNSPriority is sent for DNSAddr/<x>
    while (cmd >= 0)
        {
        if (thisElem.iNodeBuf->At(cmd).iUri->Match(tmp) != KErrNotFound)
            {
            TLex8 lex(thisElem.iNodeBuf->At(cmd).iData->Des());
            lex.Val(aDnsPri);
            thisElem.iNodeBuf->At(cmd).iDone = ETrue;
            found = ETrue;
            }
        cmd--;
        }
    CleanupStack::PopAndDestroy(buf);
    return found;
    }

//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::DnsToLuid(const UriUtils::TUriHostType aType,)
//                                      const TInt aPriority ) const
// Encodes special luid for DNSAddr/<X>
//------------------------------------------------------------------------------
TInt CNSmlInternetAdapter::DnsToLuid(const UriUtils::TUriHostType aType,
        const TInt aPriority) const
    {
    return KNSmlApDnsLuidUpperBase + static_cast<TInt> (aType) * 10
            + aPriority;
    }
//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::LuidToDns(UriUtils::TUriHostType& aType,)
//                                     TInt& aPriority, const TInt aLuid ) const
// Decodes DNAddr/<X> type from LUID
//------------------------------------------------------------------------------
TInt CNSmlInternetAdapter::LuidToDns(UriUtils::TUriHostType& aType,
        TInt& aPriority, const TInt aLuid) const
    {
    TInt err = KErrNone;
    if (aLuid < KNSmlApDnsLuidUpperBase)
        {
        err = KErrGeneral;
        }
    else
        {
        aPriority = (aLuid - KNSmlApDnsLuidUpperBase) % 10;
        if ((aPriority > 0) && (aPriority < 3))
            {
            TInt type = (aLuid - KNSmlApDnsLuidUpperBase - aPriority) / 10;
            if ((type >= 0) && (type < 2))
                {
                aType = static_cast<UriUtils::TUriHostType> (type);
                }
            else
                {
                err = KErrGeneral;
                }
            }
        else
            {
            err = KErrGeneral;
            }
        }
    return err;
    }
//------------------------------------------------------------------------------
// TBool CNSmlInternetAdapter::RemoveLastSeg(const TDesC8& aURI)
// Returns URI without its last segment
//------------------------------------------------------------------------------
TPtrC8 CNSmlInternetAdapter::RemoveLastSeg(const TDesC8& aURI) const
    {
    TInt i;
    for (i = aURI.Length() - 1; i >= 0; i--)
        {
        if (aURI[i] == KNSmlDMUriSeparator)
            {
            break;
            }
        }
    if (i > 0)
        {
        return aURI.Left(i);
        }
    else
        {
        return KNullDesC8();
        }
    }

//------------------------------------------------------------------------------
// void CNSmlInternetAdapter::ClearBuffer()
// Deletes all dynamically allocated data from given array's each element
// and resets the buffer.
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::ClearBuffer(
        CArrayFixFlat<TNSmlAPAddElement>* aBuffer)
    {
    for (TInt i = 0; i < aBuffer->Count(); i++)
        {
        delete aBuffer->At(i).iUri;
        aBuffer->At(i).iUri = NULL;

        delete aBuffer->At(i).iData;
        aBuffer->At(i).iData = NULL;

        delete aBuffer->At(i).iLuid;
        aBuffer->At(i).iLuid = NULL;
        }

    aBuffer->Reset();
    }

//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::IsValidIPv4AddressL(const TDesC8& aNapAddr)
//Checks if the string is of correct IPV4 address format x.x.x.x and 
//numbers are within the range 0-255
//returns true on sucess and false on failure.
//----------------------------------------------------------------------
TBool CNSmlInternetAdapter::IsValidIPv4AddressL(const TDesC8& aNapAddr)
    {
    TInt digitLen(0);
    TBool ret = EFalse;
    _LIT8(KNSmlDMV4Format, "*.*.*.*");

    if (aNapAddr.Match(KNSmlDMV4Format) != KErrNotFound)
        {
        TInt pos = 0;
        TInt numDots = 0;
        TInt maxIP = 255;
        TInt minIP = 0;
        for (TInt i = 0; i < aNapAddr.Length(); i++)
            {
            digitLen++;
            //Check if aNapAddr contain any characters other than 
            // 0 - 9 and ".",  If yes return from the function.
            if (aNapAddr[i] != KNSmlDMDot && (aNapAddr[i]
                    < KNSmlDMAsciiIntStart || aNapAddr[i]
                    > KNSmlDMAsciiIntEnd))
                {
                ret = EFalse;
                break;
                }
            if (aNapAddr[i] == KNSmlDMDot)
                {
                ret = ETrue;
                if (i > 0)
                    {
                    TInt digit;
                    TPtrC8 ptrOneDigit = aNapAddr.Mid(pos, digitLen - 1);
                    TLex8 lexint(ptrOneDigit);
                    if (lexint.Val(digit) != KErrNone || digit >= maxIP
                            || digit < minIP)
                        {
                        ret = EFalse;
                        break;
                        }
                    } // end of if i > 0
                pos = i + 1;
                numDots++;
                digitLen = 0;
                } // end of if
            } //End of for
        if (numDots == 3 && ret)
            {
            ret = ETrue;
            }
        else
            {
            ret = EFalse;
            }
        } // end of outer if, format check
    return ret;
    }

TBool CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL(TUint32 aRecord)
    {
    _DBG_FILE("CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL():  BEGIN");
    TBool ret = EFalse;
    CCommsDbTableView* wapView;
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL record being checked = %d"),
            aRecord);
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
            TPtrC(COMMDB_ID), aRecord);

    TInt aAccess(0);
    if (wapView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL():wap record exists, check for access type");
        TInt err = (((CCommsDbProtectTableView*) wapView)->GetRecordAccess(
                aAccess));
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL , GetrecordAccess code is =%d and acess type of record = %d"),
                err, aAccess);
        CleanupStack::PopAndDestroy(wapView);
        if (aAccess == RDbRowSet::EReadOnly)
            ret = ETrue;
        }

    _DBG_FILE("CNSmlInternetAdapter::IsWAPAccessPointRecordLockedL():  END");
    return ret;

    }
//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::DoProtectWAPAccessRecordL(TUint32 aRecord,TBool aProtect)
// protect or unprotect wapaccesspoint record of access point being altered 
//----------------------------------------------------------------------


TInt CNSmlInternetAdapter::DoProtectWAPAccessRecordL(TUint32 aRecord,
        TBool aProtect)
    {
    _DBG_FILE("CNSmlInternetAdapter::DoProtectWAPAccessRecordL():  BEGIN");
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::DoProtectWAPAccessRecordL , record and protect value  =%d  = %d"),
            aRecord, aProtect);
    TInt err = KErrNone;

    CCommsDbTableView* wapView;
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(WAP_ACCESS_POINT),
            TPtrC(COMMDB_ID), aRecord);
    if (wapView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::DoProtectWAPAccessRecordL():  wap record exists");
        if (!aProtect)
            {
            err = ((CCommsDbProtectTableView*) wapView)->UnprotectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectWAPAccessRecordL ,error code of Unprotectrecord = %d"),
                    err);
            }

        else
            {
            err = ((CCommsDbProtectTableView*) wapView)->ProtectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectWAPAccessRecordL ,error code of ProtectRecord = %d"),
                    err);
            }

        CleanupStack::PopAndDestroy(wapView);
        }

    _DBG_FILE("CNSmlInternetAdapter::DoProtectWAPAccessRecordL():  END");
    return err;
    }

//------------------------------------------------------------------------------
// TUint32 CNSmlInternetAdapter::GetIAPSeamlessnessL(TUint32 aId)
// Gets the seamlessness value from the IAPMetadata table though the cmmanager API
//------------------------------------------------------------------------------

TUint32 CNSmlInternetAdapter::GetIAPSeamlessnessL(TUint32 aId)
    {
    _DBG_FILE("CNSmlInternetAdapter::GetIAPSeamlessnessL(): begin");
    //Deleting iDatabase, Required since CommsDB has one view open for reading and commsdat API would
    //return KErrLocked if this is not done
    TBool reopenTransaction(EFalse);
    if (iDatabase->InTransaction())
        {
        iDatabase->CommitTransaction();
        iTransactionCommitted = ETrue;
        reopenTransaction = ETrue;
        }

    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    RCmConnectionMethodExt cm = cmManagerExt.ConnectionMethodL(aId);
    CleanupClosePushL(cm);
    TUint32 smless = cm.GetIntAttributeL(CMManager::ECmSeamlessnessLevel);
    CleanupStack::PopAndDestroy();
    cmManagerExt.Close();

    //Recreate reference to CommsDB since other tables
    //are manipulated with Commsdb 
    if (reopenTransaction)
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("GetIAPSeamlessnessL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("GetIAPSeamlessnessL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        }

    _DBG_FILE("CNSmlInternetAdapter::GetIAPSeamlessnessL(): End");
    return smless;

    }

//------------------------------------------------------------------------------
// TUint32 CNSmlInternetAdapter::GetIAPMetaDataL(TUint32 aId)
// Gets the metadata value from the IAPMetadata table though the cmmanager API
//--------------------------------------------------------------------------

TUint32 CNSmlInternetAdapter::GetIAPMetaDataL(TUint32 aId)
    {
    _DBG_FILE("CNSmlInternetAdapter::GetIAPMetaDataL(): begin");
    //Deleting iDatabase, Required since CommsDB has one view open for reading and commsdat API would
    //return KErrLocked if this is not done 
    TBool reopenTransaction(EFalse);
    if (iDatabase->InTransaction())
        {
        iDatabase->CommitTransaction();
        iTransactionCommitted = ETrue;
        reopenTransaction = ETrue;
        }

    TUint32 metadata, highlight, hidden;
    RCmManagerExt cmManagerExt;

    cmManagerExt.OpenL();
    RCmConnectionMethodExt cm = cmManagerExt.ConnectionMethodL(aId);
    CleanupClosePushL(cm);

    TBool metaHidden = cm.GetBoolAttributeL(CMManager::ECmMetaHiddenAgent);
    TBool metaHL = cm.GetBoolAttributeL(CMManager::ECmMetaHighlight);
    highlight = EMetaHighlight & metaHL;
    hidden = EMetaHiddenAgent & metaHidden;
    metadata = highlight | hidden;

    CleanupStack::PopAndDestroy();
    cmManagerExt.Close();
    //Recreate reference to CommsDB since other tables
    //are manipulated with Commsdb 
    if (reopenTransaction)
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("GetIAPMetaDataL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("GetIAPMetaDataL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        }

    _DBG_FILE("CNSmlInternetAdapter::GetIAPMetaDataL(): end");
    return metadata;

    }
//------------------------------------------------------------------------------
// void CNSmlInternetAdapter::SetIAPSeamlessnessL(TUint32 apMetaID,TUint32 aSeam,TInt aStatusRef)
// Sets the seamlessness value to the IAPMetadata table though the cmmanager API
//------------------------------------------------------------------------------
void CNSmlInternetAdapter::SetIAPSeamlessnessL(TUint32 apMetaID,
        TUint32 aSeam, TInt aStatusRef)
    {

    _DBG_FILE("CNSmlInternetAdapter::SetIAPSeamlessnessL(): begin");
    TBool reopenTransaction(EFalse);
    if (iDatabase->InTransaction())
        {
        iDatabase->CommitTransaction();
        iTransactionCommitted = ETrue;
        reopenTransaction = ETrue;
        }

    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    RCmConnectionMethodExt cm;
    TInt cmerror = KErrNone;
    TRAP(cmerror, cm = cmManagerExt.ConnectionMethodL(apMetaID));
    CleanupClosePushL(cm);TRAPD(sErr,cm.SetIntAttributeL( CMManager::ECmSeamlessnessLevel,aSeam ))
    if(sErr == KErrNone)
        {
        cm.UpdateL();
        iCallBack->SetStatusL(aStatusRef,CSmlDmAdapter::EOk);
        }
    else
        {
        iCallBack->SetStatusL(aStatusRef,CSmlDmAdapter::EError);
        }

    CleanupStack::PopAndDestroy();

    cmManagerExt.Close();
    //  iDatabase = CCommsDatabase::NewL();
    if (reopenTransaction)
        {
        TInt dberr = iDatabase->BeginTransaction();
        if (dberr == KErrLocked)
            {
            _DBG_FILE("SetIAPSeamlessnessL: CommsDat was locked.");
            TInt retry = KBeginTransRetryCount;
            while (retry > 0 && dberr == KErrLocked)
                {
                User::After(KBeginTransRetryDelay);
                _DBG_FILE("SetIAPSeamlessnessL: Slept 1 second. Try again");
                dberr = iDatabase->BeginTransaction();
                retry--;
                }
            if (dberr != KErrNone)
                {
                _DBG_FILE(
                        "CNSmlInternetAdapter: BeginTransaction failed completely.");
                User::Leave(dberr);
                }
            }
        }

    _DBG_FILE("CNSmlInternetAdapter::SetIAPSeamlessnessL(): end");
    }

//------------------------------------------------------------------------------
// void CNSmlInternetAdapter::SetIAPMetaDataL(TUint32 apMetaID,TUint32 aMeta,TInt aStatusRef)
// Sets the metadata value to the IAPMetadata table though the cmmanager API
//------------------------------------------------------------------------------

void CNSmlInternetAdapter::SetIAPMetaDataL(TUint32 apMetaID, TUint32 aMeta,
        TInt aStatusRef)
    {

    _DBG_FILE("CNSmlInternetAdapter::SetIAPMetaDataL(): begin");
    TInt hMHLErr = KErrNone;
    TInt hMHAErr = KErrNone;
    if (iDatabase)
        {
        iDatabase->CommitTransaction();
        // delete iDatabase;
        }
    RCmManagerExt cmManagerExt;
    cmManagerExt.OpenL();
    RCmConnectionMethodExt cm = cmManagerExt.ConnectionMethodL(apMetaID);
    CleanupClosePushL(cm);

    if (aMeta & EMetaHighlight)
        {
        TRAP(hMHLErr,
                cm.SetBoolAttributeL(CMManager::ECmMetaHighlight, ETrue));
        }
    else
        {
        TRAP(hMHLErr, cm.SetBoolAttributeL(CMManager::ECmMetaHighlight,
                EFalse));
        }

    if (aMeta & EMetaHiddenAgent)
        {
        TRAP(hMHAErr, cm.SetBoolAttributeL(CMManager::ECmMetaHiddenAgent,
                ETrue));
        }
    else
        {
        TRAP(hMHAErr, cm.SetBoolAttributeL(CMManager::ECmMetaHiddenAgent,
                EFalse));
        }
    if ((hMHAErr == KErrNone) && (hMHLErr == KErrNone))
        {
        cm.UpdateL();
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);

        }
    else
        {
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        }

    CleanupStack::PopAndDestroy();

    cmManagerExt.Close();
    TInt dberr = iDatabase->BeginTransaction();
    if (dberr == KErrLocked)
        {
        _DBG_FILE("SetIAPMetaDataL: CommsDat was locked.");
        TInt retry = KBeginTransRetryCount;
        while (retry > 0 && dberr == KErrLocked)
            {
            User::After(KBeginTransRetryDelay);
            _DBG_FILE("SetIAPMetaDataL: Slept 1 second. Try again");
            dberr = iDatabase->BeginTransaction();
            retry--;
            }
        if (dberr != KErrNone)
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter: BeginTransaction failed completely.");
            User::Leave(dberr);
            }
        }

    _DBG_FILE("CNSmlInternetAdapter::SetIAPMetaDataL(): end");
    }

void CNSmlInternetAdapter::AddLingerL(const TInt aIapId,
        const TInt aLingerInterval)
    {
    TInt err(KErrNone);

    // Open repository
    CRepository* repository = CRepository::NewLC(KCRUidPDPContextManager);

    // Find if an entry for "iapId" already exists in CentRep 
    RArray<TUint32> foundKeys;
    foundKeys.Reset();

    err = repository->FindEqL(KIapColumn, // partial key
            KColumnMask, // key mask
            aIapId, // value
            foundKeys); // found keys

    if (err == KErrNone || err == KErrNotFound)
        {
        if (foundKeys.Count() == 0)
            {
            // Add a new entry
            TInt arrayCount(0);

            // Get number of iapId&linger entries in Centrep
            err = repository->Get(KPdpContextManagerLingerArrayCount,
                    arrayCount);

            if (err == KErrNone)
                {
                arrayCount++;

                err = repository->Create((KIapColumn | arrayCount), aIapId);

                if (err == KErrNone)
                    {
                    err = repository->Create((KLingerColumn | arrayCount),
                            aLingerInterval);
                    }

                if (err == KErrNone)
                    {
                    // Update number of iapId&linger entries in Centrep
                    err = repository->Set(KPdpContextManagerLingerArrayCount,
                            arrayCount);
                    }
                }
            }
        else
            {
            // Update existing entry on current row
            TInt32 row = foundKeys[0] & KRowMask;

            err = repository->Set((KLingerColumn | row), aLingerInterval);
            }
        }

    foundKeys.Close();
    CleanupStack::PopAndDestroy(repository);

    User::LeaveIfError(err);
    }

void CNSmlInternetAdapter::UpdateLingerL(const TInt aIapId,
        const TInt aLingerInterval)
    {
    TInt err(KErrNone);

    // Open repository
    CRepository* repository = CRepository::NewLC(KCRUidPDPContextManager);

    // Find if an entry for "iapId" already exists in CentRep 
    RArray<TUint32> foundKeys;
    foundKeys.Reset();

    err = repository->FindEqL(KIapColumn, // partial key
            KColumnMask, // key mask
            aIapId, // value
            foundKeys); // found keys

    if (err == KErrNone || err == KErrNotFound)
        {

        if (foundKeys.Count() == 0)
            {
            // Add a new entry
            TInt arrayCount(0);

            // Get number of iapId&linger entries in Centrep
            err = repository->Get(KPdpContextManagerLingerArrayCount,
                    arrayCount);

            if (err == KErrNone)
                {
                arrayCount++;

                err = repository->Create((KIapColumn | arrayCount), aIapId);

                if (err == KErrNone)
                    {
                    err = repository->Create((KLingerColumn | arrayCount),
                            aLingerInterval);
                    }

                if (err == KErrNone)
                    {
                    // Update number of iapId&linger entries in Centrep
                    err = repository->Set(KPdpContextManagerLingerArrayCount,
                            arrayCount);
                    }
                }
            }
        else
            {
            // Update existing entry on currebt row
            TInt32 row = foundKeys[0] & KRowMask;

            err = repository->Set((KLingerColumn | row), aLingerInterval);
            }
        }

    foundKeys.Close();
    CleanupStack::PopAndDestroy(repository);

    User::LeaveIfError(err);
    }
void CNSmlInternetAdapter::GetLingerL(const TInt aIapId,
        TInt& aLingerInterval)
    {
    TInt err(KErrNone);

    // Open repository
    CRepository* repository = CRepository::NewLC(KCRUidPDPContextManager);

    // Find if an entry for "iapId" already exists in CentRep 
    RArray<TUint32> foundKeys;
    foundKeys.Reset();

    err = repository->FindEqL(KIapColumn, // partial key
            KColumnMask, // key mask
            aIapId, // value
            foundKeys); // found keys

    if (err == KErrNone)
        {
        if (foundKeys.Count() > 0)
            {
            // Find linger interval for the iap id on current row
            TInt32 row = foundKeys[0] & KRowMask;

            err = repository->Get((KLingerColumn | row), aLingerInterval);
            }
        }

    foundKeys.Close();
    CleanupStack::PopAndDestroy(repository);

    User::LeaveIfError(err);
    }

//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::CheckEnforcementL()
// If AP policy is enforced true is returned else false
//----------------------------------------------------------------------
TBool CNSmlInternetAdapter::CheckEnforcementL()
    {
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);
    TBool enforceActive(EFalse);
    User::LeaveIfError(info->EnforcementActive(EAPEnforcement, enforceActive));
    CleanupStack::PopAndDestroy(info);
    return enforceActive;
    }

//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::CheckWLANEnforcementL()
// If WLAN AP policy is enforced true is returned else false
//----------------------------------------------------------------------
TBool CNSmlInternetAdapter::CheckWLANEnforcementL()
    {
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);
    TBool enforceActive(EFalse);
    User::LeaveIfError(info->EnforcementActive(EWLANEnforcement,
            enforceActive));
    CleanupStack::PopAndDestroy(info);
    return enforceActive;
    }

//----------------------------------------------------------------------
//TInt CNSmlInternetAdapter::DoProtectWAPTablesL(TBool aProtect)
// if aProtect is true , tables are locked otherwise tables are unlocked.
//----------------------------------------------------------------------

TInt CNSmlInternetAdapter::DoProtectWAPTablesL(TBool aProtect)
    {
    TInt err = KErrNone;
    CCommsDatabaseProtect* dbprotect = CCommsDatabaseProtect::NewL();
    CleanupStack::PushL(dbprotect);
    for (TInt i(0); i < iLockTableList.Count(); i++)
        {
        if (!aProtect)
            {
            err = dbprotect->UnProtectTable(iLockTableList[i]);
            }
        else
            {
            err = dbprotect->ProtectTable(iLockTableList[i]);
            }

        }
    CleanupStack::PopAndDestroy(dbprotect);
    return err;
    }
//----------------------------------------------------------------------
//void CNSmlInternetAdapter::DoLockIAPTablesL(TBool aProtect)
// if aProtect is true , tables are locked otherwise tables are unlocked.
//----------------------------------------------------------------------

void CNSmlInternetAdapter::DoLockIAPTablesL(TBool aLock)
    {
    TInt protError = KErrNone;
    TInt err = KErrNone;
    iDatabase->CommitTransaction();
    iLockTableList.AppendL(TPtrC(WAP_ACCESS_POINT));
    iLockTableList.AppendL(TPtrC(WAP_IP_BEARER));
    iLockTableList.AppendL(TPtrC(IAP));

    TRAP(err, protError = DoProtectWAPTablesL(aLock));
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::DoLockIAPTablesL(): Table protect error %d , leave error %d "),
            protError, err);

    if (protError != KErrNone) // To remove the warnings
        {
        protError = KErrNone;
        }
    TInt dberr = iDatabase->BeginTransaction();
    if (dberr == KErrLocked)
        {
        _DBG_FILE("DoLockIAPTablesL: CommsDat was locked.");
        TInt retry = KBeginTransRetryCount;
        while (retry > 0 && dberr == KErrLocked)
            {
            User::After(KBeginTransRetryDelay);
            _DBG_FILE("DoLockIAPTablesL: Slept 1 second. Try again");
            dberr = iDatabase->BeginTransaction();
            retry--;
            }
        if (dberr != KErrNone)
            {
            _DBG_FILE(
                    "CNSmlInternetAdapter: BeginTransaction failed completely.");
            User::Leave(dberr);
            }
        }
    }
//--------------------------------------------------------------------
//TInt UnprotectWLANAPItemL(TUint32 aId)
//Unprotect the wlan AP if only wlan enforcement is on
//-------------------------------------------------------------------
TInt CNSmlInternetAdapter::UnprotectWLANAPItemL(TUint32 aId)
    {
    _DBG_FILE("CNSmlInternetAdapter::UnprotectWLANAPItemL():  BEGIN");
    CCommsDbTableView* wapView;

    TInt errorCode = KErrNone;
    TBuf<KCommsDbSvrMaxFieldLength> serviceType;

    //check if its WLAN AP being deleted
    wapView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(COMMDB_ID),
            aId);

    errorCode = wapView->GotoFirstRecord();

    if (errorCode == KErrNone)
        {
        wapView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        if (serviceType != TPtrC(LAN_SERVICE))
            {
            CleanupStack::PopAndDestroy(); // wapView   
            return KErrNone;
            }
        }

    ((CCommsDbProtectTableView*) wapView)->UnprotectRecord();

    CleanupStack::PopAndDestroy(); // wapView   

    _DBG_FILE("CNSmlInternetAdapter::UnprotectWLANAPItemL():  END");
    return errorCode;
    }

//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::IsIAPRecordLockedL(TUint32 aRecord)
//check for wap access point record  lock status 
//----------------------------------------------------------------------

TBool CNSmlInternetAdapter::IsIAPRecordLockedL(TUint32 aRecord)
    {
    _DBG_FILE("CNSmlInternetAdapter::IsIAPRecordLockedL():  BEGIN");
    TBool ret = EFalse;
    CCommsDbTableView* apView;
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::IsIAPRecordLockedL record being checked = %d"),
            aRecord);
    apView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(COMMDB_ID),
            aRecord);

    TInt aAccess(0);
    if (apView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::IsIAPRecordLockedL():wap record exists, check for access type");
        TInt err = (((CCommsDbProtectTableView*) apView)->GetRecordAccess(
                aAccess));
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::IsIAPRecordLockedL , GetrecordAccess code is =%d and acess type of record = %d"),
                err, aAccess);
        CleanupStack::PopAndDestroy(apView);
        if (aAccess == RDbRowSet::EReadOnly)
            ret = ETrue;
        }

    _DBG_FILE("CNSmlInternetAdapter::IsIAPRecordLockedL():  END");
    return ret;

    }
//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::DoProtectIAPRecordL(TUint32 aRecord,TBool aProtect)
// protect or unprotect wapaccesspoint record of access point being altered 
//----------------------------------------------------------------------


TInt CNSmlInternetAdapter::DoProtectIAPRecordL(TUint32 aRecord,
        TBool aProtect)
    {
    _DBG_FILE("CNSmlInternetAdapter::DoProtectIAPRecordL():  BEGIN");
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::DoProtectIAPRecordL , record and protect value  =%d  = %d"),
            aRecord, aProtect);
    TInt err = KErrNone;

    CCommsDbTableView* apView;
    apView = iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(COMMDB_ID),
            aRecord);
    if (apView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::DoProtectIAPRecordL():  wap record exists");
        if (!aProtect)
            {
            err = ((CCommsDbProtectTableView*) apView)->UnprotectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectIAPAccessRecordL ,error code of Unprotectrecord = %d"),
                    err);
            }

        else
            {
            err = ((CCommsDbProtectTableView*) apView)->ProtectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectIAPRecordL ,error code of ProtectRecord = %d"),
                    err);
            }

        CleanupStack::PopAndDestroy(apView);
        }

    _DBG_FILE("CNSmlInternetAdapter::DoProtectIAPRecordL():  END");
    return err;
    }
//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::IsProxyRecordLockedL(TUint32 aRecord)
//check for proxy record  lock status 
//----------------------------------------------------------------------

TBool CNSmlInternetAdapter::IsProxyRecordLockedL(TUint32 aRecord)
    {
    _DBG_FILE("CNSmlInternetAdapter::IsProxyRecordLockedL():  BEGIN");
    TBool ret = EFalse;
    CCommsDbTableView* proxyView;
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::IsProxyRecordLockedL record being checked = %d"),
            aRecord);
    proxyView = iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES), TPtrC(
            COMMDB_ID), aRecord);

    TInt aAccess(0);
    if (proxyView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::IsProxyRecordLockedL():wap record exists, check for access type");
        TInt err = (((CCommsDbProtectTableView*) proxyView)->GetRecordAccess(
                aAccess));
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::IsProxyRecordLockedL , GetrecordAccess code is =%d and acess type of record = %d"),
                err, aAccess);
        CleanupStack::PopAndDestroy(proxyView);
        if (aAccess == RDbRowSet::EReadOnly)
            ret = ETrue;
        }

    _DBG_FILE("CNSmlInternetAdapter::IsProxyRecordLockedL():  END");
    return ret;

    }
//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::DoProtectProxyRecordL(TUint32 aRecord,TBool aProtect)
// protect or unprotect proxy record of access point being altered 
//----------------------------------------------------------------------


TInt CNSmlInternetAdapter::DoProtectProxyRecordL(TUint32 aRecord,
        TBool aProtect)
    {
    _DBG_FILE("CNSmlInternetAdapter::DoProtectProxyRecordL():  BEGIN");
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::DoProtectProxyRecordL , record and protect value  =%d  = %d"),
            aRecord, aProtect);
    TInt err = KErrNone;

    CCommsDbTableView* proxyView;
    proxyView = iDatabase->OpenViewMatchingUintLC(TPtrC(PROXIES), TPtrC(
            COMMDB_ID), aRecord);
    if (proxyView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::DoProtectProxyRecordL():  wap record exists");
        if (!aProtect)
            {
            err = ((CCommsDbProtectTableView*) proxyView)->UnprotectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectProxyRecordL ,error code of Unprotectrecord = %d"),
                    err);
            }

        else
            {
            err = ((CCommsDbProtectTableView*) proxyView)->ProtectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectProxyRecordL ,error code of ProtectRecord = %d"),
                    err);
            }

        CleanupStack::PopAndDestroy(proxyView);
        }

    _DBG_FILE("CNSmlInternetAdapter::DoProtectProxyRecordL():  END");
    return err;
    }

//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::IsServiceRecordLockedL(TUint32 aRecord)
//check for proxy record  lock status 
//----------------------------------------------------------------------

TBool CNSmlInternetAdapter::IsServiceRecordLockedL(TUint32 aRecord)
    {
    _DBG_FILE("CNSmlInternetAdapter::IsProxyRecordLockedL():  BEGIN");
    TBool ret = EFalse;
    CCommsDbTableView* serviceView;
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::IsProxyRecordLockedL record being checked = %d"),
            aRecord);
    serviceView = iDatabase->OpenViewMatchingUintLC(iProxyServiceType, TPtrC(
            COMMDB_ID), aRecord);

    TInt aAccess(0);
    if (serviceView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::IsProxyRecordLockedL():wap record exists, check for access type");
        TInt err =
                (((CCommsDbProtectTableView*) serviceView)->GetRecordAccess(
                        aAccess));
        DBG_ARGS8(
                _S8(
                        "CNSmlInternetAdapter::IsProxyRecordLockedL , GetrecordAccess code is =%d and acess type of record = %d"),
                err, aAccess);
        CleanupStack::PopAndDestroy(serviceView);
        if (aAccess == RDbRowSet::EReadOnly)
            ret = ETrue;
        }

    _DBG_FILE("CNSmlInternetAdapter::IsProxyRecordLockedL():  END");
    return ret;

    }
//----------------------------------------------------------------------
//TBool CNSmlInternetAdapter::DoProtectProxyRecordL(TUint32 aRecord,TBool aProtect)
// protect or unprotect proxy record of access point being altered 
//----------------------------------------------------------------------


TInt CNSmlInternetAdapter::DoProtectServiceRecordL(TUint32 aRecord,
        TBool aProtect)
    {
    _DBG_FILE("CNSmlInternetAdapter::DoProtectProxyRecordL():  BEGIN");
    DBG_ARGS8(
            _S8(
                    "CNSmlInternetAdapter::DoProtectProxyRecordL , record and protect value  =%d  = %d"),
            aRecord, aProtect);
    TInt err = KErrNone;

    CCommsDbTableView* serviceView;
    serviceView = iDatabase->OpenViewMatchingUintLC(iProxyServiceType, TPtrC(
            COMMDB_ID), aRecord);
    if (serviceView != NULL)
        {
        _DBG_FILE(
                "CNSmlInternetAdapter::DoProtectProxyRecordL():  wap record exists");
        if (!aProtect)
            {
            err
                    = ((CCommsDbProtectTableView*) serviceView)->UnprotectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectProxyRecordL ,error code of Unprotectrecord = %d"),
                    err);
            }

        else
            {
            err = ((CCommsDbProtectTableView*) serviceView)->ProtectRecord();
            DBG_ARGS8(
                    _S8(
                            "CNSmlInternetAdapter::DoProtectProxyRecordL ,error code of ProtectRecord = %d"),
                    err);
            }

        CleanupStack::PopAndDestroy(serviceView);
        }

    _DBG_FILE("CNSmlInternetAdapter::DoProtectProxyRecordL():  END");
    return err;
    }
//--------------------------------------------------------------------
//TInt CNSmlInternetAdapter::IsAPUriFormatMatchPredefined(const TDesC8 & aURI)
//
//-------------------------------------------------------------------


TBool CNSmlInternetAdapter::IsAPUriFormatMatchPredefined(const TDesC8 & aURI)
    {
    if (aURI.Match(_L8("AP/APId*")) != KErrNotFound || aURI.Match(_L8(
            "AP/APId*/NAPDef/NAPId*")) != KErrNotFound)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }

    }
//--------------------------------------------------------------------
//TInt CNSmlInternetAdapter::ConstructTreeL(const TDesC8& aURI)
//
//-------------------------------------------------------------------


TUint32 CNSmlInternetAdapter::ConstructTreeL(const TDesC8& aURI)
    {
    _DBG_FILE("CNSmlInternetAdapter::ConstructTreeL(): AP begin");
    TUint32 returnLuid = 0; // IAP doesn't exist

    CCommsDbTableView* apView = iDatabase->OpenTableLC(TPtrC(IAP));

    TInt errorCode = apView->GotoFirstRecord();

    while (errorCode == KErrNone)
        {
        TUint32 lValue;
        TBool validService = EFalse;
        apView->ReadUintL(TPtrC(COMMDB_ID), lValue);

        TBuf<KCommsDbSvrMaxFieldLength> serviceType;
        apView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        TUint32 bearerId = 0;
        apView->ReadUintL(TPtrC(IAP_BEARER), bearerId);
        //
        // Skip other service types than GSM outgoing
        //
        if (serviceType == TPtrC(OUTGOING_GPRS) || serviceType == TPtrC(
                INCOMING_GPRS))
            {
            validService = ETrue;
            }

        else if (serviceType == TPtrC(LAN_SERVICE) && bearerId > 0)
            {
            validService = ETrue;
            }
        else if (serviceType == TPtrC(VPN_SERVICE))
            {
            validService = ETrue;
            }

        if (validService)
            {
            // Skip others
            //
            //  Check if in aPreviousURISegmentList
            // 
            TBool skipToNextIAP = EFalse;
            // KNSmlDMStart includes start text for URISeg
            TBuf8<20> addAPURI; // AP/APIdnnn , nnn = profileid
            addAPURI.Append(_L8("AP/APId"));
            addAPURI.AppendNumFixedWidth(lValue, EDecimal, 3);

            TBuf8<16> addLUID;
            _LIT8(KFormat, "%d");
            addLUID.Format(KFormat, lValue);

            HBufC8* mapInfo = iCallBack->GetLuidAllocL(addAPURI);
            CleanupStack::PushL(mapInfo);
            if (mapInfo->Length() == 0)
                {
                iCallBack->SetMappingL(addAPURI, addLUID);
                if (aURI.Find(addAPURI) >= 0)
                    {
                    returnLuid = lValue;
                    iPrevLUID = lValue;
                    }
                }
            else
                {
                skipToNextIAP = ETrue;
                }
            CleanupStack::PopAndDestroy(mapInfo);

            if (!skipToNextIAP)
                {
                // Add the NAPDef also 

                TBuf<KCommsDbSvrMaxFieldLength> serviceType;
                TUint32 serviceId(NULL);

                CCommsDbTableView* napdefView =
                        iDatabase->OpenViewMatchingUintLC(TPtrC(IAP), TPtrC(
                                COMMDB_ID), lValue);

                errorCode = napdefView->GotoFirstRecord();
                if (errorCode == KErrNone)
                    {
                    napdefView->ReadTextL(TPtrC(IAP_SERVICE_TYPE),
                            serviceType);
                    napdefView->ReadUintL(TPtrC(IAP_SERVICE), serviceId); // ID 
                    }

                CCommsDbTableView* tableView =
                        iDatabase->OpenViewMatchingUintLC(serviceType, TPtrC(
                                COMMDB_ID), serviceId);

                errorCode = tableView->GotoFirstRecord();

                while (errorCode == KErrNone)
                    {
                    TUint32 lValue;
                    tableView->ReadUintL(TPtrC(COMMDB_ID), lValue);
                    //
                    //  Check if in aPreviousURISegmentList
                    // 

                    _LIT8(Kprev, "NAPId");
                    TBuf8<15> addNAME(Kprev); // APIdnnn , nnn = profileid
                    addNAME.AppendNumFixedWidth(lValue, EDecimal, 3);

                    _LIT8(Kprev2, "/NAPDef/NAPId");
                    TBuf8<80> addURI; // AP/xxx/NAPDef/NAPIdnnn , nnn = id nbr
                    addURI.Append(addAPURI);
                    addURI.Append(Kprev2);
                    addURI.AppendNumFixedWidth(lValue, EDecimal, 3);

                    TBuf8<16> addLUID;
                    _LIT8(KFormat, "%d");
                    addLUID.Format(KFormat, lValue);

                    HBufC8* mapInfo = iCallBack->GetLuidAllocL(addURI);
                    CleanupStack::PushL(mapInfo);
                    if (mapInfo->Length() == 0)
                        {
                        // Also added to mapping
                        iCallBack->SetMappingL(addURI, addLUID);
                        if (aURI.Find(addURI) >= 0)
                            returnLuid = lValue;
                        _DBG_FILE(
                                "CNSmlInternetAdapter::ConstructTreeL(): Set NAPDef Mapping ");

                        TBuf<KCommsDbSvrMaxFieldLength> columnValue;
                        TBuf8<16> addDNSLUID;
                        TBuf8<80> addDNSURI; // AP/xxx/NAPDef/DNSAddr/DNSx
                        _LIT8(KFormat, "%d");
                        tableView->ReadTextL(TPtrC(SERVICE_IP_NAME_SERVER1),
                                columnValue);

                        if ((columnValue.Length() > 0))
                            {
                            _LIT8(Kprev2, "/DNSAddr/DNS1");
                            addDNSURI.Append(addURI);
                            addDNSURI.Append(Kprev2);
                            addLUID.Format(KFormat, DnsToLuid(
                                    UriUtils::EIPv4Host, 1));
                            iCallBack->SetMappingL(addDNSURI, addLUID);
                            if (aURI.Find(addDNSURI) >= 0)
                                returnLuid
                                        = DnsToLuid(UriUtils::EIPv4Host, 1);
                            addDNSURI.Zero();
                            }

                        tableView->ReadTextL(TPtrC(SERVICE_IP_NAME_SERVER2),
                                columnValue);
                        if (columnValue.Length() > 0)
                            {
                            _LIT8(Kprev2, "/DNSAddr/DNS2");
                            addDNSURI.Append(addURI);
                            addDNSURI.Append(Kprev2);
                            addLUID.Format(KFormat, DnsToLuid(
                                    UriUtils::EIPv4Host, 2));
                            iCallBack->SetMappingL(addDNSURI, addLUID);
                            if (aURI.Find(addDNSURI) >= 0)
                                returnLuid
                                        = DnsToLuid(UriUtils::EIPv4Host, 2);
                            addDNSURI.Zero();
                            }

                        tableView->ReadTextL(TPtrC(SERVICE_IP6_NAME_SERVER1),
                                columnValue);
                        if (columnValue.Length() > 0)
                            {
                            _LIT8(Kprev2, "/DNSAddr/DNS3");
                            addDNSURI.Append(addURI);
                            addDNSURI.Append(Kprev2);
                            addLUID.Format(KFormat, DnsToLuid(
                                    UriUtils::EIPv6Host, 1));
                            iCallBack->SetMappingL(addDNSURI, addLUID);
                            if (aURI.Find(addDNSURI) >= 0)
                                returnLuid
                                        = DnsToLuid(UriUtils::EIPv6Host, 1);
                            addDNSURI.Zero();
                            }
                        tableView->ReadTextL(TPtrC(SERVICE_IP6_NAME_SERVER2),
                                columnValue);
                        if (columnValue.Length() > 0)
                            {
                            _LIT8(Kprev2, "/DNSAddr/DNS4");
                            addDNSURI.Append(addURI);
                            addDNSURI.Append(Kprev2);
                            addLUID.Format(KFormat, DnsToLuid(
                                    UriUtils::EIPv6Host, 2));
                            iCallBack->SetMappingL(addDNSURI, addLUID);
                            if (aURI.Find(addDNSURI) >= 0)
                                returnLuid
                                        = DnsToLuid(UriUtils::EIPv6Host, 2);
                            addDNSURI.Zero();
                            }
                        _DBG_FILE(
                                "CNSmlInternetAdapter::ConstructTreeL(): Set DNS Mapping ");

                        TUint32 nwID = 0;
                        napdefView->ReadUintL(TPtrC(IAP_NETWORK), nwID); // Networks luid

                        CCommsDbTableView* nwView =
                                iDatabase->OpenViewMatchingUintLC(TPtrC(
                                        NETWORK), TPtrC(COMMDB_ID), nwID);

                        errorCode = nwView->GotoFirstRecord();

                        if (errorCode == KErrNone)
                            {
                            _LIT8(Kprev2, "/Networks/NwId");
                            TBuf8<80> addNwURI; // AP/xxx/NAPDef/xxx/Networks/NwIdnnn , nnn = id nbr
                            addNwURI.Append(addURI);
                            addNwURI.Append(Kprev2);
                            addNwURI.AppendNumFixedWidth(nwID, EDecimal, 3);
                            TBuf8<16> addNwLUID;
                            _LIT8(KFormat, "%d");
                            addNwLUID.Format(KFormat, nwID);
                            HBufC8* mapInfo = iCallBack->GetLuidAllocL(
                                    addNwURI);
                            CleanupStack::PushL(mapInfo);

                            if (mapInfo->Length() == 0)
                                {
                                // Also added to mapping
                                iCallBack->SetMappingL(addNwURI, addNwLUID);
                                if (aURI.Find(addNwURI) >= 0)
                                    returnLuid = nwID;
                                _DBG_FILE(
                                        "CNSmlInternetAdapter::ConstructTreeL(): Set Networks Mapping ");
                                }
                            CleanupStack::PopAndDestroy(mapInfo);
                            }
                        CleanupStack::PopAndDestroy(); // nwView              

                        if (iWlanSupported)
                            {
                            // Check WLAN node
                            if (serviceType == TPtrC(LAN_SERVICE))
                                {
                                TUint32 wlanID = serviceId;
                                _LIT8(Kprev2, "/WLAN/WLId");
                                TBuf8<80> addWlanURI; // AP/xxx/NAPDef/xxx/WLAN/WLIdnnn , nnn = id nbr
                                addWlanURI.Append(addURI);
                                addWlanURI.Append(Kprev2);
                                addWlanURI.AppendNumFixedWidth(wlanID,
                                        EDecimal, 3);
                                TBuf8<16> addWlanLUID;
                                _LIT8(KFormat, "%d");
                                addWlanLUID.Format(KFormat, wlanID);
                                HBufC8* mapInfo = iCallBack->GetLuidAllocL(
                                        addWlanURI);
                                CleanupStack::PushL(mapInfo);

                                if (mapInfo->Length() == 0)
                                    {
                                    // Also added to mapping
                                    iCallBack->SetMappingL(addWlanURI,
                                            addWlanLUID);
                                    if (aURI.Find(addWlanURI) >= 0)
                                        returnLuid = wlanID;
                                    _DBG_FILE(
                                            "CNSmlInternetAdapter::ConstructTreeL(): Set WLAN Mapping ");

                                    TBuf<KCommsDbSvrMaxFieldLength> value;
                                    TBuf8<16> addWkLUID;
                                    TBuf8<80> addWkURI; // AP/xxx/NAPDef/xxx/WLAN/xxx/WEPKey/WKIdn, n = id nbr

                                    for (TInt value = 1; value <= 4; value++)
                                        {
                                        _LIT8(Kprev2, "/WEPKey/WKId");
                                        addWkURI.Append(addWlanURI);
                                        addWkURI.Append(Kprev2);
                                        addWkURI.AppendNumFixedWidth(value,
                                                EDecimal, 1);
                                        _LIT8(KFormat, "%d");
                                        addWkLUID.Format(KFormat, (1000
                                                * value) + value);
                                        iCallBack->SetMappingL(addWkURI,
                                                addWkLUID);
                                        if (aURI.Find(addWkURI) >= 0)
                                            returnLuid = (1000 * value)
                                                    + value;
                                        addWkURI.Zero();
                                        }
                                    }
                                CleanupStack::PopAndDestroy(mapInfo);
                                }
                            }

                        //Check Bearer node 

                        CCommsDbTableView* bView =
                                iDatabase->OpenViewMatchingUintLC(TPtrC(
                                        NETWORK), TPtrC(COMMDB_ID), lValue);
                        errorCode = bView->GotoFirstRecord();

                        if (errorCode == KErrNone)
                            {
                            _LIT8(Kprev2, "/Bearer/BId");
                            TBuf8<80> addbURI; // AP/xxx/NAPDef/xxx/Bearer/BIdnnn , nnn = id nbr
                            addbURI.Append(addURI);
                            addbURI.Append(Kprev2);
                            addbURI.AppendNumFixedWidth(lValue, EDecimal, 3);
                            TBuf8<16> addbLUID;
                            _LIT8(KFormat, "%d");
                            addbLUID.Format(KFormat, lValue);
                            HBufC8* mapInfo = iCallBack->GetLuidAllocL(
                                    addbURI);
                            CleanupStack::PushL(mapInfo);

                            if (mapInfo->Length() == 0)
                                {
                                // Also added to mapping
                                iCallBack->SetMappingL(addbURI, addbLUID);
                                if (aURI.Find(addbURI) >= 0)
                                    returnLuid = lValue;
                                _DBG_FILE(
                                        "CNSmlInternetAdapter::ConstructTreeL(): Set Bearer Mapping ");
                                }
                            CleanupStack::PopAndDestroy(mapInfo);
                            }
                        CleanupStack::PopAndDestroy(); // bView 

                        //Check NAPAuthInf node NAPAuthInf/AUId

                        CCommsDbTableView* authView =
                                iDatabase->OpenViewMatchingUintLC(TPtrC(
                                        NETWORK), TPtrC(COMMDB_ID), lValue);

                        errorCode = authView->GotoFirstRecord();

                        if (errorCode == KErrNone)
                            {
                            _LIT8(Kprev2, "/Bearer/BId");
                            TBuf8<80> addauthURI; // AP/xxx/NAPDef/xxx/Bearer/BIdnnn , nnn = id nbr
                            addauthURI.Append(addURI);
                            addauthURI.Append(Kprev2);
                            addauthURI.AppendNumFixedWidth(lValue, EDecimal,
                                    3);
                            TBuf8<16> addauthLUID;
                            _LIT8(KFormat, "%d");
                            addauthLUID.Format(KFormat, lValue);
                            HBufC8* mapInfo = iCallBack->GetLuidAllocL(
                                    addauthURI);
                            CleanupStack::PushL(mapInfo);

                            if (mapInfo->Length() == 0)
                                {
                                // Also added to mapping
                                iCallBack->SetMappingL(addauthURI,
                                        addauthLUID);
                                if (aURI.Find(addauthURI) >= 0)
                                    returnLuid = lValue;
                                _DBG_FILE(
                                        "CNSmlInternetAdapter::ConstructTreeL(): Set NAPAuthInf Mapping ");
                                }
                            CleanupStack::PopAndDestroy(mapInfo);
                            }
                        CleanupStack::PopAndDestroy(); // authView  

                        //Check Proxy node          

                        CCommsDbTableView* pxView =
                                iDatabase->OpenViewMatchingTextLC(TPtrC(
                                        PROXIES), TPtrC(PROXY_SERVICE_TYPE),
                                        serviceType);

                        errorCode = pxView->GotoFirstRecord();
                        if (errorCode == KErrNone)
                            {
                            while (errorCode == KErrNone)
                                {
                                TUint32 lProxyIspId;
                                TUint32 lProxyId;
                                pxView->ReadUintL(TPtrC(PROXY_ISP),
                                        lProxyIspId);
                                if (lProxyIspId == serviceId) //ServiceIsp) // Right value ISP
                                    {
                                    pxView->ReadUintL(TPtrC(COMMDB_ID),
                                            lProxyId);

                                    _LIT8(Kprev2, "/Px/PXId");
                                    TBuf8<80> addURI; // AP/xxx/Px/PXIdnnn , nnn = id nbr
                                    addURI.Append(addAPURI);
                                    addURI.Append(Kprev2);
                                    addURI.AppendNumFixedWidth(lProxyId,
                                            EDecimal, 3);

                                    TBuf8<16> addLUID;
                                    _LIT8(KFormat, "%d");
                                    addLUID.Format(KFormat,
                                            KNSmlApProxyLowerBase + lProxyId); // 100000 + lProxyId
                                    HBufC8* mapInfo =
                                            iCallBack->GetLuidAllocL(addURI);
                                    CleanupStack::PushL(mapInfo);

                                    if (mapInfo->Length() == 0)
                                        {
                                        // Also added to mapping
                                        iCallBack->SetMappingL(addURI,
                                                addLUID);
                                        if (aURI.Find(addURI) >= 0)
                                            returnLuid
                                                    = KNSmlApProxyLowerBase
                                                            + lProxyId;
                                        _DBG_FILE(
                                                "CNSmlInternetAdapter::ConstructTreeL(): Set Proxy Mapping ");
                                        _LIT8(KProxy, "/Port/PROXY");
                                        TBuf8<80> addPortURI; // AP/xxx/Px/Port/PROXY
                                        addPortURI.Append(addURI);
                                        addPortURI.Append(KProxy);
                                        TBuf8<16> addPortLUID;
                                        _LIT8(KFormat, "%d");
                                        addPortLUID.Format(KFormat, 1);
                                        iCallBack->SetMappingL(addPortURI,
                                                addPortLUID);
                                        if (aURI.Find(addPortURI) >= 0)
                                            returnLuid = 1;
                                        }
                                    CleanupStack::PopAndDestroy(mapInfo);
                                    }
                                errorCode = pxView->GotoNextRecord();
                                }
                            }
                        CleanupStack::PopAndDestroy();//Pxview             
                        }
                    CleanupStack::PopAndDestroy(mapInfo);
                    errorCode = tableView->GotoNextRecord();
                    }
                CleanupStack::PopAndDestroy();//tableview
                CleanupStack::PopAndDestroy();//napdefview
                }
            }
        errorCode = apView->GotoNextRecord();
        }
    CleanupStack::PopAndDestroy(); // apView
    _DBG_FILE("CNSmlInternetAdapter::ConstructTreeL(): AP end");
    return returnLuid;
    }
// End of File
