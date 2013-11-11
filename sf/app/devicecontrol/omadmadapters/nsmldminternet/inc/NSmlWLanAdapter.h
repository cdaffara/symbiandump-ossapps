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
 * Description:   Wlan adapter
 *
 */

#ifndef __NSMLWLANADAPTER_H__
#define __NSMLWLANADAPTER_H__

// -----------------------------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------------------------
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <utf.h>
#include <commdb.h>
#include <EapSettings.h>
#include "smldmadapter.h"
#include "nsmlconstants.h"
#include "NSmlWlanSettings.h"

// -----------------------------------------------------------------------------------------------
// Constants
// -----------------------------------------------------------------------------------------------
const TInt KNSmlWepKeyGranularity = 4;
const TInt KNSmlMaxURISegLen = 32;
const TInt KExpandedEAPIdLength = 8;
const TInt KNsmlSubjectKeyIdLength = 255;
const TInt KIdentifierLength = 20;
const TUint KMaxLengthOfEapList = 50;

_LIT8( KNSmlWLanAdapterDDFversion, "1.0" );
_LIT16( KNSmlWLanServiceTable, "WLANServiceTable" );
_LIT8( KNSmlWLanId, "Id" );
_LIT8( KNSmlWLanName, "Name" );
_LIT8( KNSmlWLanHidden, "Hidden" );
_LIT8( KNSmlWLanReadOnly, "ReadOnly" );
_LIT8( KNSmlWLanServiceId, "ServiceID" );
_LIT8( KNSmlWLanNetworkMode, "NetworkMode" );
_LIT8( KNSmlWLanConnectionMode, "WlanConnectionMode" );
_LIT8( KNSmlWLanSsid, "SSID" );
_LIT8( KNSmlWLanUsedSsid, "UsedSSID" );
_LIT8( KNSmlWLanSsidHidden, "SSIDHidden" );
_LIT8( KNSmlWLanUseWPAPSK, "UseWPAPSK" );
_LIT8( KNSmlWLanEnablePSK, "EnablePSK" );
_LIT8( KNSmlWLanWepKey1, "WEPKey1InHex" );
_LIT8( KNSmlWLanWepKey2, "WEPKey2InHex" );
_LIT8( KNSmlWLanWepKey3, "WEPKey3InHex" );
_LIT8( KNSmlWLanWepKey4, "WEPKey4InHex" );
_LIT8( KNSmlWLanWepIndex, "WEPKeyIndex" );
_LIT8( KNSmlWLanWepKey, "WEPKey" );
_LIT8( KNSmlWLanSecurityMode, "SecurityMode" );
_LIT8( KNSmlWLanWpaAuthMode, "WPAAuthMode" );
_LIT8( KNSmlWLanWpaPreSharedKey, "WPAPreSharedKey" );
_LIT8( KNSmlWLanEapList, "WLANEAPList" );
_LIT8( KNSmlWLanAuthMode, "WEPAuthMode" );
_LIT8( KNSmlWLanWepKeyId, "WEPKeyID" );
_LIT8( KNSmlWLanWepKeyData, "Data" );
_LIT8( KNSmlWLanNodeName, "WLAN" );

_LIT8( KNSmlEmpty, "" );
#define  KNSmlWLanDefaultSSID _S("Net1")

_LIT8( KNSmlWLan, "/WLAN" );
_LIT8( KNSmlNAPDefWlanNode,"Name/NAPID/Bearer/IPAddrFromServer/IfNetworks/NAPAddr/NAPAddrTy/IPDNSAddrFromServer/DNSAddr/DefGW/NetworkMask/Networks/IAPService");

_LIT8( KNSmlWLanListOfLeafs, "SSID/UsedSSID/SSIDHidden/NetworkMode/SecurityMode/WPAPreSharedKey/UseWPAPSK/WEPKey/WEPKeyIndex/WLANEAPList/WEPAuthMode/EAP/SecondarySSID" );

_LIT8( KNSmlWLanListOfSecondaryLeafs, "SSID/UsedSSID" );
_LIT8( KNSmlWLanListOfWepKeys, "1/2/3/4" );
_LIT8( KNSmlWLanListOfWepKeyLeafs, "WEPKeyID/Data" );
_LIT8( KNSmlWLanAllowUnsecure8, "allow unsecure" );
_LIT8( KNSmlWLanWep8, "WEP" );
_LIT8( KNSmlWLan8021x8, "802.1X" );
_LIT8( KNSmlWLanWpa8, "WPA" );
_LIT8( KNSmlWLanWpa28, "WPA2" );
_LIT8( KNSmlWLanWpaPreSharedKey8, "WPA pre-shared key" );
_LIT8( KNSmlWLanAdHoc8, "ADHOC" );
_LIT8( KNSmlWLanInfrastructure8, "Infrastructure" );
_LIT8( KNSmlMimeType, "text/plain" );

#ifdef FF_WLAN_EXTENSIONS
_LIT8( KNSmlWLanListOfEAPLeafs, "EAPType/Username/Password/Realm/UsePseudonym/VerifyCertRealm/RequireClientAuthentication/SessionValidityTime/CipherSuite/AllowPEAPv0/AllowPEAPv1/AllowPEAPv2/Certificate/AuthProvMode/UnauthProvMode/PACGroupRef/WarnADHPNoPAC/WarnADHPNoMatchPAC/WarnNotDefaultSrv" );
#else
_LIT8( KNSmlWLanListOfEAPLeafs, "EAPType/Username/Password/Realm/UsePseudonym/VerifyCertRealm/RequireClientAuthentication/SessionValidityTime/CipherSuite/AllowPEAPv0/AllowPEAPv1/AllowPEAPv2/Certificate" );
#endif

_LIT8( KNSmlWLanListOfCertificateLeafs, "IssuerName/SubjectName/CertType/SerialNumber/SubjectKeyId/Fingerprint" );

_LIT8( KNSmlWLanSecSsid, "SecondarySSID" );
_LIT8( KNSmlWLanEap, "EAP" );
_LIT8( KNSmlWLanEapType, "EAPType" );
_LIT8( KNSmlWLanUsername, "Username" );
_LIT8( KNSmlWLanPassword, "Password" );
_LIT8( KNSmlWLanRealm, "Realm" );
_LIT8( KNSmlWLanUsePseudonym, "UsePseudonym" );
_LIT8( KNSmlWLanEncapsulation, "Encapsulation" );
_LIT8( KNSmlWLanVerifyCertRealm, "VerifyCertRealm" );
_LIT8( KNSmlWLanRequireClientAuthentication, "RequireClientAuthentication" );
_LIT8( KNSmlWLanSessionValidityTime, "SessionValidityTime" );
_LIT8( KNSmlWLanCipherSuite, "CipherSuite" );
_LIT8( KNSmlWLanAllowPEAPv0, "AllowPEAPv0" );
_LIT8( KNSmlWLanAllowPEAPv1, "AllowPEAPv1" );
_LIT8( KNSmlWLanAllowPEAPv2, "AllowPEAPv2" );
#ifdef FF_WLAN_EXTENSIONS 
_LIT8( KNSmlWLanAuthProvMode, "AuthProvMode" );
_LIT8( KNSmlWLanUnauthProvMode, "UnauthProvMode" );
_LIT8( KNSmlWLanPACGroupRef, "PACGroupRef" );
_LIT8( KNSmlWLanWarnADHPNoPAC, "WarnADHPNoPAC" );
_LIT8( KNSmlWLanWarnADHPNoMatchPAC, "WarnADHPNoMatchPAC" );
_LIT8( KNSmlWLanWarnNotDefaultSrv, "WarnNotDefaultSrv" );
#endif
_LIT8( KNSmlWLanCertificate, "Certificate" );
_LIT8( KNSmlWLanIssuerName, "IssuerName" );
_LIT8( KNSmlWLanSubjectName, "SubjectName" );
_LIT8( KNSmlWLanCertType, "CertType" );
_LIT8( KNSmlWLanSerialNumber, "SerialNumber" );
_LIT8( KNSmlWLanSubjectKeyId, "SubjectKeyId" );
_LIT8( KNSmlWLanFingerprint, "Fingerprint" );

_LIT8(KTrue, "True");
_LIT8(KFalse, "False");
_LIT8(KTtrue, "true");
_LIT8(KFfalse, "false");
_LIT8(KSlash, "/");
_LIT8(KDash, "-");
_LIT8(KNSmlWLanEapId, "EAPId" );

_LIT8(KSuitesDefault, "-010,-022,-019,-047,-050,-051,-004,-005");

const TUint RSA_3DES_SHA = 10;
const TUint DHE_RSA_3DES_SHA = 22;
const TUint DHE_DSS_3DES_SHA = 19;
const TUint RSA_AES_SHA = 47; // ?
const TUint DHE_RSA_AES_SHA = 50; // ?
const TUint DHE_DSS_AES_SHA = 51; // ?
const TUint RSA_RC4_MD5 = 4;
const TUint RSA_RC4_SHA = 5;
const TUint NUMBER_OF_CIPHERSUITES = 8;
const TUint KExpandedEapIdLength = 8;
const TUint8 KEapPlus = '+';

_LIT8(KEAPSIM, "EAP-SIM");
_LIT8(KEAPAKA, "EAP-AKA");
_LIT8(KEAPTLS, "EAP-TLS");
_LIT8(KEAPPEAP, "EAP-PEAP");
_LIT8(KEAPTTLS, "EAP-TTLS");
_LIT8(KEAPLEAP, "EAP-LEAP");
_LIT8(KEAPMSCHAPV2, "EAP-MSCHAPV2");
_LIT8(KMSCHAPV2, "MSCHAPV2");
_LIT8(KEAPGTC, "EAP-GTC");
_LIT8(KEAPFAST, "EAP-FAST");
_LIT8(KEAPTTLSPAP, "EAP-TTLSPAP");

_LIT8(KEapAll,"-017,-018,-023,-013,-025,-021,-043" );

const TUint8 KEapNoneId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
const TUint8 KEapTlsTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d
    };
const TUint8 KEapGtcTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06
    };
const TUint8 KEapLeapTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11
    };
const TUint8 KEapSimTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12
    };
const TUint8 KEapTtlsTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15
    };
const TUint8 KEapAkaTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17
    };
const TUint8 KEapPeapTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19
    };
const TUint8 KEapMschapv2TypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A
    };
const TUint8 KEapSecuridTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20
    };
const TUint8 KEapFastTypeId[] =
    {
    0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2B
    };
const TUint8 KMschapv2TypeId[] =
    {
    0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x63
    };
const TUint8 KTtlspapTypeId[] =
    {
    0xfe, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x62
    };
const TUint8 KNotInsidePEAP = 0x04;
const TUint8 KNotOutsidePEAP = 0x08;
const TUint8 KNotInsideTTLS = 0x10;

//Forward Declarations
class TEapExpandedType;

// -----------------------------------------------------------------------------------------------
// Structures
// -----------------------------------------------------------------------------------------------
struct TNSmlWepKeyBufferElement
    {
    HBufC8* iUri;
    HBufC8* iData;
    TInt iWepKeyId;
    TInt iIdStatusRef;
    //TInt iUseWPAPSK;
    TInt iDataStatusRef;
    TInt iNodeStatusRef;
    TInt iLengthStatusRef;
    TBool iExecuted;
    TBool iDataSet;
    TInt iLuid;
    };

struct TNSmlSecondarySSIDBufferElement
    {
    HBufC8* iUri;
    TBuf<KMaxTextLength> ScannedId;
    TBuf<KMaxTextLength> UsedId;
    TInt wlanLuid;
    TBool iExecuted;
    };

struct TNSmlEAPBufferElement
    {
    EAPSettings* iEAPSettings;
    TEapExpandedType iEncapsulatingExpandedEapId;
    RArray<TInt> iStatusRefArray;
    TInt iWLANServiceId;
    TBuf8<KExpandedEapIdLength> iEapTypeString;
    TBuf8<KExpandedEapIdLength> iEncapsulatingString;
    };

// -----------------------------------------------------------------------------------------------
// Enumerations
// -----------------------------------------------------------------------------------------------
enum TConnectionMode
    {
    EAdhoc = 0, EInfrastructure = 1
    };

enum TWLanSecurityMode
    {
    EAllowUnsecure = 1, EWep = 2, EWlan8021x = 4, EWpa = 8, EWpa2 = 16
    };

enum TWepAuthenticationMode
    {
    EOpen = 0, EShared = 1
    };

enum TNsmlWLANAdapterEapType
    {
    EEapNone = 0,
    EEapGtc = 6,
    EEapTls = 13,
    EEapLeap = 17,
    EEapSim = 18,
    EEapTtls = 21,
    EEapAka = 23,
    EEapPeap = 25,
    EEapMschapv2 = 26,
    EEapSecurid = 32,
    EEapFast = 43,
    ETtlsPlainPap = 98,
    EPlainMschapv2 = 99
    };
// ------------------------------------------------------------------------------------------------
// CNSmlWLanAdapter 
// ------------------------------------------------------------------------------------------------
class CNSmlWLanAdapter : public CSmlDmAdapter
    {
public:
    static CNSmlWLanAdapter* NewL(MSmlDmCallback* aDmCallback,
            CCommsDatabase& aDatabase);
    static CNSmlWLanAdapter* NewLC(MSmlDmCallback* aDmCallback,
            CCommsDatabase& aDatabase);
    void ConstructL(CCommsDatabase& aDatabase);
    virtual ~CNSmlWLanAdapter();

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
    CSmlDmAdapter::TError FetchLeafObjectL(const TDesC8& aURI,
            const TDesC8& aLUID, const TDesC8& aType, CBufBase& aObject);

private:
    CNSmlWLanAdapter(TAny* aEcomArguments, CCommsDatabase& aDatabase);
    void FillNodeInfoL(MSmlDmDDFObject& aNode, TSmlDmAccessTypes aAccTypes,
            MSmlDmDDFObject::TOccurence aOccurrence,
            MSmlDmDDFObject::TScope aScope,
            MSmlDmDDFObject::TDFFormat aFormat, const TDesC8& aDescription);
    HBufC* IntToDesLC(const TInt aLuid);
    HBufC8* IntToDes8LC(const TInt aLuid);
    TInt DesToInt(const TDesC8& aLuid);
    TInt GetIdFromLuid(const TDesC8& aLuid);
    TUint32 GetSecSSIDFromUriL(const TDesC8& aUri);
    TInt GetEAPIdFromUri(const TDesC8& aUri);
    TInt GetCertificateIdFromUri(const TDesC8& aUri);
    TInt GetWepKeyIdFromLuid(const TDesC8& aLuid);
    TInt GetWepKeyIdFromUri(const TDesC8& aUri);
    TInt NumOfUriSegs(const TDesC8& aUri);
    TPtrC8 GetLastUriSeg(const TDesC8& aURI);
    TPtrC8 RemoveLastUriSeg(const TDesC8& aURI);
    void ExecuteBufferL(TBool aFinal = EFalse);
    TDesC8& ConvertTo8LC(const TDesC& aSource);
    TDesC16& ConvertTo16LC(const TDesC8& aSource);
    void DesToBufferL(CBufBase& aTo, const TDesC& aFrom);
    void FillCipherSuiteBufferL(CBufBase& aBuffer);
    void FillCipherSuitesL(const TDesC8& aObject, const TInt aLuid);
    void ConvertEAPStringToIds(TDes8& aEAPString, TEapExpandedType& aId);
    TInt GetServiceIdFromUriL(const TDesC8& aURI);
    TInt GetEncapsEAPIdFromUri(const TDesC8& aUri);
    void ResetEAPStructL();
    void DeleteEAPStructL();
    void GetExpandedType(const TInt aEapType, const TInt aTunnelingType,
            TEapExpandedType& aExpanedId, TEapExpandedType& aExpanedEncapsId);

    MSmlDmCallback* iCallBack;
    CWlanSettings* iWlanSettings;
    CArrayFixFlat<TNSmlWepKeyBufferElement> *iBuffer;
    TWlanSettings *iSettings;
    EAPSettings* iEAPSettings;
    RPointerArray<TNSmlSecondarySSIDBufferElement> iSecondaryBuffer;
    RPointerArray<TNSmlEAPBufferElement> iEAPBuffer;
    CCommsDatabase& iDatabase;

    };

#endif __NSMLWLANADAPTER_H__
