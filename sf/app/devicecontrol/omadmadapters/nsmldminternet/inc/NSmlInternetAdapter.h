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

#ifndef __NSMLINTERNETADAPTER_H__
#define __NSMLINTERNETADAPTER_H__

// SyncML device management LoginScript
//#define __SYNCML_DM_LSCRIPT 1
//#define __SYNCML_DM_WLAN 1
// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <utf.h>
#include <bldvariant.hrh>
#include <commdb.h>
#include <metadatabase.h>
#include "smldmadapter.h"
#include "nsmlconstants.h"

#include "NSmlWLanAdapter.h"
#include "NSmlWlanSettings.h"

const TInt KNSmlInternetAdapterFatMinSize = 512;
const TInt KNSmlIncompleteAP = -1;
// Buffering Stuff
const TInt KNSmlAPGranularity = 4;
const TUint8 KNSmlDMUriSeparator = 0x2f;

const TUint8 KNSmlDMDot = 0x2e;
const TUint8 KNSmlDMAsciiIntStart = 0x30;
const TUint8 KNSmlDMAsciiIntEnd = 0x39;
// Other constants
const TInt KNSmlApDnsLuidUpperBase = 300000;
const TInt KNSmlAp3gpppsLowerBase = 50000;
const TInt KNSmlApProxyLowerBase = 100000;
const TInt KNSmlApProxyUpperBase = 200000;
const TInt KNSmlApWapLowerBase = 9200;
const TInt KNSmlApWapUpperBase = 9203;

//DDF version number
_LIT8( KNSmlInternetAdapterDDFversion, "1.1");

//DDF accepted AP-fieldnames
_LIT8( KNSmlDdfAP, "AP" );

_LIT8( KNSmlDdfPx, "Px" );
_LIT8( KNSmlDdfPxName, "Name" );
_LIT8( KNSmlDdfPxId, "PxID" );
_LIT8( KNSmlDdfPxPW, "PxPW" );
_LIT8( KNSmlDdfPPGAuthTy, "PPGAuthTy" );
_LIT8( KNSmlDdfDomain, "Domain" );
_LIT8( KNSmlDdfDomainL, "DomainL" );
_LIT8( KNSmlDdfPxAddr, "PxAddr" );
_LIT8( KNSmlDdfToNAPID, "ToNAPID" );
_LIT8( KNSmlDdfToNAPIDL, "ToNAPIDL" );
_LIT8( KNSmlDdfStartpg, "Startpg" );
_LIT8( KNSmlDdfBasAuthId, "BasAuthId" );
_LIT8( KNSmlDdfBasAuthPW, "BasAuthPW" );
_LIT8( KNSmlDdfPort, "Port" );
_LIT8( KNSmlDdfPortNbr, "PortNbr" );
_LIT8( KNSmlDdfPxAuthTy, "PxAuthTy" );
_LIT8( KNSmlDdfService, "Service" );
_LIT8( KNSmlDdfServiceL, "ServiceL" );
_LIT8( KNSmlDdfUsePTxtLog, "UsePTxtLog" );

_LIT8( KNSmlDdfGPRSPDP, "GPRSPDP" );
_LIT8( KNSmlDdfDefGW, "DefGW" );
_LIT8( KNSmlDdfNetworkMask, "NetworkMask" );

_LIT8( KNSmlDdfPPPComp, "PPPComp" );

_LIT8( KNSmlDdfNAPDef, "NAPDef" );
_LIT8( KNSmlDdfNAPName, "Name" );
_LIT8( KNSmlDdfNAPLinger, "Linger" );
_LIT8( KNSmlDdfNAPID, "NAPID" );
_LIT8( KNSmlDdfBearer, "Bearer" );
_LIT8( KNSmlDdfBearerL, "BearerL" );
_LIT8( KNSmlDdfDirection, "Direction" );
_LIT8( KNSmlDdfInternet, "Internet" );
_LIT8( KNSmlDdfNAPAddr, "NAPAddr" );
_LIT8( KNSmlDdfNAPAddrTy, "NAPAddrTy" );
_LIT8( KNSmlDdfDNSAddr, "DNSAddr" );
_LIT8( KNSmlDdfDNSAddrL, "DNSAddrL" );
_LIT8( KNSmlDdfDNSAddrTy, "DNSAddrTy" );
_LIT8( KNSmlDdfDNSPriority, "DNSPriority" );
_LIT8( KNSmlDdfNAPAuthInf, "NAPAuthInf" );
_LIT8( KNSmlDdfAuthName, "AuthName" );
_LIT8( KNSmlDdfAuthSecr, "AuthSecr" );
_LIT8( KNSmlDdfPxAuthInf, "PxAuthInf" );
_LIT8( KNSmlDdfPxAuthId, "PxAuthID" );
_LIT8( KNSmlDdfPxAuthPW, "PxAuthPW" );

_LIT8( KNSmlDdfIPAddrFromServer,"IPAddrFromServer" );
_LIT8( KNSmlDdfIPAddr, "IPAddr" );
_LIT8( KNSmlDdfDNSAddrFromServer,"IPDNSAddrFromServer" );
_LIT8( KNSmlDdfIPv6DNSAddrFromServer,"IPv6DNSAddrFromServer" );
_LIT8( KNSmlDdfIfNetworks, "IfNetworks" );
_LIT8( KNSmlDdfIAPService, "IAPService" );
_LIT8( KNSmlDdfLANService, "LANService" );

_LIT8( KNSmlDdfIAPSeamlessness, "Seamlessness" );
_LIT8( KNSmlDdfIAPMetaData, "MetaData" );

_LIT8( KNSmlDdfNoPxFor, "NoPxFor" );
_LIT8( KNSmlDdfNoPxForL, "NoPxForL" );

_LIT8( KNSmlDdfNetworks, "Networks" );
_LIT8( KNSmlDdfNetworkName, "Name" );
_LIT8( KNSmlDdfNetworkID, "ID" );

//DDF AP-field descriptions
_LIT8( KNSmlDdfAPDesc, "AP (Access Point) node" );

_LIT8( KNSmlDdfPxDesc, "Px (Proxy) node" );
_LIT8( KNSmlDdfPxNameDesc, "Px name description" );
_LIT8( KNSmlDdfNameDesc, "Displayable name for the Proxy" );
_LIT8( KNSmlDdfPxIdDesc, "Define one Proxy entity" );
_LIT8( KNSmlDdfPxPWDesc, "Password for one Proxy entity" );
_LIT8( KNSmlDdfPPGAuthTyDesc, "Link PxID and PxPW to an authentication method" );
_LIT8( KNSmlDdfPxAddrDesc, "Proxy-Address" );
_LIT8( KNSmlDdfToNAPIDDesc, "Parent to all Network Access Point link objects" );
_LIT8( KNSmlDdfToNAPIDLDesc, "NAPID end" );
_LIT8( KNSmlDdfStartpgDesc, "The WAP homepage or start page" );
_LIT8( KNSmlDdfBasAuthIdDesc, "Basic authentication identifier" );
_LIT8( KNSmlDdfBasAuthPWDesc, "Basic authentication password" );
_LIT8( KNSmlDdfPortDesc, "Port node" );
_LIT8( KNSmlDdfPortNbrDesc, "Value of the port number" );
_LIT8( KNSmlDdfServiceDesc, "Service node" );
_LIT8( KNSmlDdfServiceLDesc, "ServiceL" );
_LIT8( KNSmlDdfUsePTxtLogDesc, "Plain Text Login is used or not" );

_LIT8( KNSmlDdfGPRSPDPDesc, "GPRS PDP-type" );
_LIT8( KNSmlDdfDefGWDesc, "Default Gateway" );
_LIT8( KNSmlDdfNetworkMaskDesc, "Network/Subnet mask" );

_LIT8( KNSmlDdfPPPCompDesc, "PPP Compression" );

_LIT8( KNSmlDdfNAPDefDesc, "Network Access Point Definition objects parent" );
_LIT8( KNSmlDdfNAPNameDesc, "Displayable name for the NAP" );
_LIT8( KNSmlDdfNAPIDDesc, "Used by TO-NAPID link of the Proxy" );
_LIT8( KNSmlDdfNAPLingerDesc, "Linger value" );
_LIT8( KNSmlDdfBearerDesc, "Bearer node" );
_LIT8( KNSmlDdfBearerLDesc, "Bearer valid" );
_LIT8( KNSmlDdfDirectionDesc, "Direction for network type" );
_LIT8( KNSmlDdfInternetDesc, "IP-routable network segment" );
_LIT8( KNSmlDdfNAPAddrDesc, "NAP-Address" );
_LIT8( KNSmlDdfNAPAddrTyDesc, "NAP-Address-Type" );
_LIT8( KNSmlDdfDNSAddrDesc, "DNSAddr node" );
_LIT8( KNSmlDdfDNSAddrLDesc, "Address of a DNS server" );
_LIT8( KNSmlDdfDNSAddrTyDesc, "Type of DNS address" );
_LIT8( KNSmlDdfDNSPriorityDesc, "Rank of DNS server" );
_LIT8( KNSmlDdfNAPAuthInfDesc, "Authentication Info" );
_LIT8( KNSmlDdfAuthNameDesc, "Loginname" );
_LIT8( KNSmlDdfAuthSecrDesc, "Loginpassword" );
_LIT8( KNSmlDdfPxAuthInfDesc, "Proxy Authentication Info" );
_LIT8( KNSmlDdfPxAuthIdDesc, "Proxy Loginname" );
_LIT8( KNSmlDdfPxAuthPWDesc, "Proxy Loginpassword" );

_LIT8( KNSmlDdfIPAddrFromServerDesc,"Get IP addresses from server?" );
_LIT8( KNSmlDdfIPAddrDesc, "IP-address" );
_LIT8( KNSmlDdfDNSAddrFromServerDesc,"Get DNS addresses from server?" );
_LIT8( KNSmlDdfIPv6DNSAddrFromServerDesc,"Get IPv6 addresses from server?" );
_LIT8( KNSmlDdfIfNetworksDesc, "Comma separated list of network protocols, e.g. PPP" );
_LIT8( KNSmlDdfIAPServiceDesc, "IAP service" );

_LIT8( KNSmlDdfIAPSeamlessnessDesc, "IAP seamlessness value" );
_LIT8( KNSmlDdfIAPMetaDataDesc, "IAP meta data value" );

_LIT8( KNSmlDdfDomainDesc, "Domain desc" );
_LIT8( KNSmlDdfDomainLDesc, "Domain, for which the Proxy is responsible" );

_LIT8( KNSmlDdfNoPxForDesc, "No proxy for listed AP:s" );
_LIT8( KNSmlDdfNoPxForLDesc, "No proxy for this AP" );

_LIT8( KNSmlDdfNetworksDesc, "Networks" );
_LIT8( KNSmlDdfNetworkNameDesc, "Network Name" );
_LIT8( KNSmlDdfNetworkIDDesc, "Network ID" );

//Other 
_LIT8( KNSmlDNS1, "/DNS1" );
_LIT8( KNSmlDNS2, "/DNS2" );
_LIT8( KNSmlDNS3, "/DNS3" );
_LIT8( KNSmlDNS4, "/DNS4" );

_LIT8(KNSmlAPnode, "NAPDef/Px");

_LIT8(KNSmlNAPDefGPRSnode, "Name/NAPID/Startpg/Bearer/IPAddr/IPAddrFromServer/IfNetworks/NAPAddr/NAPAddrTy/IPDNSAddrFromServer/IPv6DNSAddrFromServer/DNSAddr/NAPAuthInf/DefGW/NetworkMask/PPPComp/UsePTxtLog/GPRSPDP/Networks/IAPService/Seamlessness/MetaData/Linger");

//_LIT8(KNSmlNAPDefCSDnode, "Name/NAPID/Bearer/IPAddr/IPAddrFromServer/IfNetworks/NAPAddr/NAPAddrTy/CallTy/IPDNSAddrFromServer/IPv6DNSAddrFromServer/DNSAddr/NAPAuthInf/LnkSpeed/DefGW/NetworkMask/PPPComp/UseCB/CBTy/CBNbr/UsePTxtLog/ModemInit/Networks/IAPService");

//_LIT8(KNSmlNAPDefGPRSnode, "Name/NAPID/Bearer/IPAddr/IPAddrFromServer/IfNetworks/NAPAddr/NAPAddrTy/CallTy/IPDNSAddrFromServer/IPv6DNSAddrFromServer/DNSAddr/NAPAuthInf/LnkSpeed/DefGW/NetworkMask/PPPComp/UsePTxtLog/GPRSPDP/Networks/IAPService");

_LIT8(KNSmlPxnode, "Name/PxID/PxAddr/Port/ToNAPID/Startpg/Domain/PxAuthInf/NoPxFor");

#ifdef __SYNCML_DM_LSCRIPT
_LIT8(KNSmlLoginscriptnode, "/LoginTW/LoginScriptData");
#endif

_LIT8(KNSmlDNSAddrNode, "DNSAddrL/DNSAddrTy/DNSPriority" );

_LIT8(KNSmlWLannode, "/WLAN/SSID/UsedSSID/SSIDHidden/UseWPAPSK/NetworkMode/SecurityMode/WEPAuthMode/WPAPreSharedKey/WLANEAPList/WEPKeyIndex/WEPKey/Data/Length/WEPKeyID/Username/Password/EAPType/Realm/UsePseudonym/Encapsulation/VerifyCertRealm/RequireClientAuthentication/SessionValidityTime/CipherSuite/AllowPEAPv0/AllowPEAPv1/AllowPEAPv2/IssuerName/SubjectName/CertType/SerialNumber/SubjectKeyId/Fingerprint");

//Default data values
_LIT( KNSmlDefName, "APAdapterIns" );
_LIT( KNSmlDefAP, "AP" );
_LIT8( KNSmlFirstDNSPri, "1" );
_LIT8( KNSmlSecondDNSPri, "2" );
_LIT8( KNSmlDNSIp, "IP" );
_LIT8( KNSmlDNSIp6, "IP6" );
_LIT8( KNSmlEmptyIpv4Address8, "0.0.0.0");
_LIT( KNSmlEmptyIpv4Address, "0.0.0.0");
_LIT8( KNSmlMimeTypeTextPlain, "text/plain" );

_LIT8( KNSmlDmApValOutgoing, "Outgoing" );
_LIT8( KNSmlDmApValIncoming, "Incoming" );
_LIT8( KNSmlDmApVal3Gppps, "3GPPPS" );
_LIT8( KNSmlDmApValGsmCsd, "GSM-CSD" );
_LIT8( KNSmlDmApValGsmGprs, "GSM-GPRS" );
_LIT8( KNSmlDmApValGsmHscsd, "GSM-HSCSD" );
_LIT8( KNSmlDmApValVpn, "VPN" );
_LIT8( KNSmlDmApValWlan, "WLAN" );
_LIT8( KNSmlDmApValFSlash, "/" );
_LIT8( KNSmlDmApValAnalog, "ANALOG-MODEM" );
_LIT8( KNSmlDmApValV110, "V.110" );
_LIT8( KNSmlDmApValV120, "V.120" );

_LIT8( KNSmlDmApValAuto, "Auto" );
_LIT8( KNSmlDmApVal2400, "2400" );
_LIT8( KNSmlDmApVal4800, "4800" );
_LIT8( KNSmlDmApVal9600, "9600" );
_LIT8( KNSmlDmApVal14400, "14400" );
_LIT8( KNSmlDmApVal19200, "19200" );
_LIT8( KNSmlDmApVal28800, "28800" );
_LIT8( KNSmlDmApVal32000, "32000" );
_LIT8( KNSmlDmApVal33600, "33600" );
_LIT8( KNSmlDmApVal38400, "38400" );
_LIT8( KNSmlDmApVal43200, "43200" );
_LIT8( KNSmlDmApVal48000, "48000" );
_LIT8( KNSmlDmApVal56000, "56000" );
_LIT8( KNSmlDmApVal57600, "57600" );
_LIT8( KNSmlDmApVal64000, "64000" );
_LIT8( KNSmlDmApValExtended, "Extended" );
_LIT8( KNSmlDmApValIpv4, "IPv4" );
_LIT8( KNSmlDmApValIpv6, "IPv6" );
_LIT8( KNSmlDmApValTrue, "True" );
_LIT8( KNSmlDmApValFalse, "False" );

//class forwards
class CNSmlWLanAdapter;

struct TNSmlAPAddElement
    {
    HBufC8 *iUri;
    HBufC8 *iData;
    TInt iStatusRef;
    HBufC8* iLuid;
    TBool iLeaf;
    TBool iDone;
    };

enum TNSmlAPLeafType
    {
    EDMUpdate, EDMAdd, EDMDelete, EDMUnset
    };

struct TNSmlAPBufferElement
    {
    CArrayFixFlat<TNSmlAPAddElement> *iNodeBuf;
    CArrayFixFlat<TNSmlAPAddElement> *iWlanNodeBuf;
    HBufC8* iMappingName;
    HBufC8* iName;
    TBool iExecuted;
    TInt iLuid;
    TBool iBearer;
    TBool iDirection;
    TBool iNameReceived;
    TBool iDnsUpdateBuf;
    };
//
// ------------------------------------------------------------------------------------------------
// CNSmlInternetAdapter 
// ------------------------------------------------------------------------------------------------
class CNSmlInternetAdapter : public CSmlDmAdapter
    {
public:

    enum TNSmlAPFieldType
        {
        EStr, EInt, EBin, EBool, ESpec, EWap, EWrong
        };
    static CNSmlInternetAdapter* NewL(MSmlDmCallback* aDmCallback);
    static CNSmlInternetAdapter* NewLC(MSmlDmCallback* aDmCallback);

    virtual ~CNSmlInternetAdapter();

    // Adapter interface
    void DDFVersionL(CBufBase& aDDFVersion);
    void DDFStructureL(MSmlDmDDFObject& aDDF);
    void
            AddLeafObjectL(const TDesC8& aURI, const TDesC8& aParentLUID,
                    const TDesC8& aObject, const TDesC8& aType,
                    const TInt aStatusRef);
    void
            UpdateLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
                    const TDesC8& aObject, const TDesC8& aType,
                    const TInt aStatusRef);
    void UpdateLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
            RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef);
    void DeleteObjectL(const TDesC8& aURI, const TDesC8& aLUID,
            const TInt aStatusRef);
    void FetchLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
            const TDesC8& aType, const TInt aResultsRef,
            const TInt aStatusRef);
    void FetchLeafObjectSizeL(const TDesC8& aURI, const TDesC8& aLUID,
            const TDesC8& aType, TInt aResultsRef, TInt aStatusRef);
    void ChildURIListL(const TDesC8& aURI, const TDesC8& aLUID,
            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
            const TInt aResultsRef, const TInt aStatusRef);
    void AddNodeObjectL(const TDesC8& aURI, const TDesC8& aParentLUID,
            const TInt aStatusRef);
    void ExecuteCommandL(const TDesC8& aURI, const TDesC8& aLUID,
            const TDesC8& aArgument, const TDesC8& aType, TInt aStatusRef);
    void ExecuteCommandL(const TDesC8& aURI, const TDesC8& aLUID,
            RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef);
    void CopyCommandL(const TDesC8& aTargetURI, const TDesC8& aTargetLUID,
            const TDesC8& aSourceURI, const TDesC8& aSourceLUID,
            const TDesC8& aType, TInt aStatusRef);
    void StartAtomicL();
    void CommitAtomicL();
    void RollbackAtomicL();
    TBool StreamingSupport(TInt& aItemSize);
    void StreamCommittedL();
    void CompleteOutstandingCmdsL();

private:
    void ConstructL();
    CNSmlInternetAdapter(TAny* aEcomArguments);
    TDesC8& SetIntObjectLC(const TInt& aObject);
    TInt SetField(const TDesC8& aSource);
    TInt GetIntObject8(const TDesC8& aObject);
    TInt IntLUID(const TDesC8& aLUID);

    void AddNodeBufferL(const TDesC8& aURI, const TDesC8& aParentLUID,
            const TInt aStatusRef, const TBool aDontAppend,
            const TBool aWlan = EFalse);

    void AddLeafBufferL(const TDesC8& aURI, const TDesC8& aParentLUID,
            const TDesC8& aObject, const TDesC8& aType,
            const TInt aStatusRef, const TBool aWlan = EFalse);

    CSmlDmAdapter::TError FetchLeafObjectL(const TDesC8& aURI,
            const TDesC8& aLUID, const TDesC8& aType, CBufBase& aObject);
    TPtrC8 LastURISeg(const TDesC8& aURI);
    TPtrC8 FirstURISeg(const TDesC8& aURI);
    TPtrC8 GetDynamicAPNodeUri(const TDesC8& aURI);
    void ExecuteBufferL(const TDesC8& aURI, const TBool aCompleting = EFalse);
    TPtrC8 GetAddURISeg(const TDesC8& aURI, const TDesC8& aName);

    CNSmlInternetAdapter::TNSmlAPFieldType
            GetAPFieldType(const TDesC8& aURI) const;
    TBool GetAPField(TPtrC& aTableName, TPtrC& aColumn) const;
    TBool NotValidStrLenght(const TDesC& aSource);

    TDesC8& ConvertTo8LC(const TDesC& aSource);
    TDesC16& ConvertTo16LC(const TDesC8& aSource);

    TInt InitializeGPRSL(CCommsDbTableView* aTable);
    TInt InitializeProxyL(CCommsDbTableView* aTable);

    TInt InitializeWAPIAPL(CCommsDbTableView* aTable, const TDesC8& aName);
    TInt InitializeWAPL(CCommsDbTableView* aTable);
    TInt InitializeLANServiceL(CCommsDbTableView* aTable);
    TInt InitializeVPNServiceL(CCommsDbTableView* aTable);
    TBool IsValidNameL(const TDesC8& aName);
    TBool IsValidIPv4AddressL(const TDesC8& aNapAddr);
    void NextAPNameL(const TDesC8& aName);

    TUint32 GetIAPSeamlessnessL(TUint32 aId);
    TUint32 GetIAPMetaDataL(TUint32 aId);
    void
            SetIAPSeamlessnessL(TUint32 apMetaID, TUint32 aSeam,
                    TInt aStatusRef);
    void SetIAPMetaDataL(TUint32 apMetaID, TUint32 aMeta, TInt aStatusRef);

    TBool CheckEnforcementL();
    TBool CheckWLANEnforcementL();
    TInt UnprotectWLANAPItemL(TUint32 aId);
    TInt DoProtectWAPTablesL(TBool aProtect);
    void DoLockIAPTablesL(TBool aLock);

    TBool IsIAPRecordLockedL(TUint32 aRecord);
    TInt DoProtectIAPRecordL(TUint32 aRecord, TBool aProtect);
    TBool IsProxyRecordLockedL(TUint32 aRecord);
    TInt DoProtectProxyRecordL(TUint32 aRecord, TBool aProtect);
    TBool IsServiceRecordLockedL(TUint32 aRecord);
    TInt DoProtectServiceRecordL(TUint32 aRecord, TBool aProtect);
    TBool IsWAPAccessPointRecordLockedL(TUint32 aRecord);
    TInt DoProtectWAPAccessRecordL(TUint32 aRecord, TBool aProtect);

    TBool IsWLANfield(const TDesC8& aURI);
    TBool IsWlanL(const TDesC8& aURI);
    TBool GetLanBearerL(TUint32& aId);

    void SaveIAPDataL(const TDesC8& aURI);
    TBool GetModemBearerL(TUint32& aId);
    TBool IsInsertAllowedL();
    TUint32 GetMaxIdL();
    TBool GetProxyIdL(const TDesC8& aURI);

    TBool APExistsL(TUint32 aId);
    TBool NAPDefExistsL(TUint32 aId);
    TBool PxExistsL(TUint32 aId);
    TBool WapAPExistsL(TUint32 aId);
    TBool NetworkExistsL(TUint32 aId);
    TBool NetworkNameExistsL(TUint32& aId, const TDesC8& aObject);
    TBool AddNetworkL(TUint32& aId, const TDesC8& aObject);

    TBool DeleteAPL(TUint32 aId);
    TBool DeleteNAPDefL(TUint32 aId);
    TBool DeleteProxyL(TUint32 aId);
    TBool DeleteWapAPL(TUint32 aId);

    TUint32 GetAPIdFromURIL(const TDesC8& aURI);
    void IsWapPort(const TDesC8& aObject);
    void FillNodeInfoL(MSmlDmDDFObject& aNode, TSmlDmAccessTypes aAccTypes,
            MSmlDmDDFObject::TOccurence aOccurrence,
            MSmlDmDDFObject::TScope aScope,
            MSmlDmDDFObject::TDFFormat aFormat, const TDesC8& aDescription);

    TInt UpdateIAPnameL(TUint32 aId, const TDesC8& aObject);
    TUint32 GetLocationL();

    TBool
            InitializeDNSParamsFromBuffL(const TPtrC8 aParentUri,
                    TInt& aDnsPri);
    TInt
            DnsToLuid(const UriUtils::TUriHostType aType,
                    const TInt aPriority) const;
    TInt LuidToDns(UriUtils::TUriHostType& aType, TInt& aPriority,
            const TInt aLuid) const;
    TPtrC8 RemoveLastSeg(const TDesC8& aURI) const;

    void ClearBuffer(CArrayFixFlat<TNSmlAPAddElement>* aBuffer);
    void LingerValueL(const TDesC8& aObject);
    void AddLingerL(const TInt aIapId, const TInt aLingerInterval);
    void UpdateLingerL(const TInt aIapId, const TInt aLingerInterval);
    void GetLingerL(const TInt aIapId, TInt& aLingerInterval);
    void DeleteLingerL(const TInt aIapId);
    TBool Notalpha(const TDesC& aValue);
    TUint32 ConstructTreeL(const TDesC8& aURI);
    TBool IsAPUriFormatMatchPredefined(const TDesC8 & aURI);

private:
    MSmlDmCallback* iCallBack;

    CNSmlWLanAdapter* iWlanAdapter;

    TInt iLUID;
    TInt iParentLUID;
    TInt iObject;
    TBool iWapPort;
    TInt iDirection;
    TBool iStaleMapUpdate;
    HBufC8* iField;
    TNSmlAPLeafType iLeafType;

    TBuf8<KCommsDbSvrMaxFieldLength> iWAPAccessPoint;
    TUint32 iWapId;
    TBuf<KCommsDbSvrMaxFieldLength> iWapBearer;
    TBuf8<KCommsDbSvrMaxFieldLength> iWapName;
    TBuf8<KCommsDbSvrMaxFieldLength> iWapUserName;
    TBuf8<KCommsDbSvrMaxFieldLength> iWapPassword;

    CArrayFixFlat<TNSmlAPBufferElement> *iBuffer;
    TBool i3GPPPS;

private:
    // data
    CCommsDatabase* iDatabase;

    TBool iIAPExists;

    TInt iDnsIpv4Pri; // 1 or 2 
    TInt iDnsIpv6Pri;
    TInt iExecutionIndex;
    TBool iExecutingBuffer;
    TBool iDnsUpdate;

    TBuf8<KCommsDbSvrMaxFieldLength> iIAPName;

    TUint32 iIAPId;

    TUint32 iISPId;

    TUint32 iProxyId;
    TUint32 iProxyISP;

    TBuf<KCommsDbSvrMaxFieldLength> iProxyServiceType;
    TBuf8<KCommsDbSvrMaxFieldLength> iProxyServerName;

    HBufC8* iPrevURI;
    TInt iPrevLUID;

    TBuf<KCommsDbSvrMaxFieldLength> iBearer;

    TUint32 iBearerType;
    TInt iNetworkId;
    TInt iLingerValue;
    TBool iLingerFlag;
    TBool iTransactionCommitted;
    TBool iWAPRelock;
    RArray<TPtrC> iLockTableList;

    TBool iWlanSupported;
    TBool isAdd;
    };

#endif // __NSMLINTERNETADAPTER_H__
// End of File
