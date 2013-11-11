/*
 * Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Wlan Adapter
 *
 */

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include "NSmlWLanAdapter.h"
#include "NSmlWlanSettings.h"
#include "nsmldebug.h"
#include <commdb.h>
#include <utf.h>
#include <cdbstore.h>
#include <cdbcols.h>
#include <EapType.h>
#include <nsmldmuri.h>
#include <EapExpandedType.h>
#include <EapGeneralSettings.h>

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TInt KMaxBuf = 255;
//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::NewL( )
//-----------------------------------------------------------------------------
CNSmlWLanAdapter* CNSmlWLanAdapter::NewL(MSmlDmCallback* aDmCallback,
        CCommsDatabase& aDatabase)
    {
    _DBG_FILE("CNSmlWLanAdapter::NewL(): begin");

    CNSmlWLanAdapter* self = NewLC(aDmCallback, aDatabase);
    CleanupStack::Pop(self);

    _DBG_FILE("CNSmlWLanAdapter::NewL(): end");
    return self;
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::NewLC( )
//-----------------------------------------------------------------------------
CNSmlWLanAdapter* CNSmlWLanAdapter::NewLC(MSmlDmCallback* aDmCallback,
        CCommsDatabase& aDatabase)
    {
    _DBG_FILE("CNSmlWLanAdapter::NewLC(): begin");

    CNSmlWLanAdapter* self = new (ELeave) CNSmlWLanAdapter(aDmCallback,
            aDatabase);
    CleanupStack::PushL(self);

    self->iCallBack = aDmCallback;
    self->ConstructL(aDatabase);

    _DBG_FILE("CNSmlWLanAdapter::NewLC(): end");
    return self;
    }

//-----------------------------------------------------------------------------
// void CNSmlWLanAdapter::ConstructL( )
// Second phase constructor
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::ConstructL(CCommsDatabase& aDatabase)
    {
    _DBG_FILE("CNSmlWLanAdapter::ConstructL(): begin");

    iWlanSettings = CWlanSettings::NewL(aDatabase);
    iBuffer = new (ELeave) CArrayFixFlat<TNSmlWepKeyBufferElement> (
            KNSmlWepKeyGranularity);
    iSettings = new (ELeave) TWlanSettings;
    iEAPSettings = new (ELeave) EAPSettings;

    _DBG_FILE("CNSmlWLanAdapter::ConstructL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter::CNSmlWLanAdapter( )
// Constructor
//-----------------------------------------------------------------------------
CNSmlWLanAdapter::CNSmlWLanAdapter(TAny* aEcomArguments,
        CCommsDatabase& aDatabase) :
    CSmlDmAdapter(aEcomArguments), iDatabase(aDatabase)
    {
    _DBG_FILE("CNSmlWLanAdapter::CNSmlWLanAdapter(): begin");
    _DBG_FILE("CNSmlWLanAdapter::CNSmlWLanAdapter(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter::~CNSmlWLanAdapter( )
// Destructor
//-----------------------------------------------------------------------------
CNSmlWLanAdapter::~CNSmlWLanAdapter()
    {
    _DBG_FILE("CNSmlWLanAdapter::~CNSmlWLanAdapter(): begin");

    if (iBuffer)
        {

        for (TInt index = 0; index < iBuffer->Count(); index++)
            {
            delete iBuffer->At(index).iUri;
            delete iBuffer->At(index).iData;

            iBuffer->Delete(index);
            }

        iBuffer->Reset();
        delete iBuffer;

        }
    delete iWlanSettings;
    delete iSettings;

    TRAP_IGNORE(DeleteEAPStructL());

    for (TInt i = 0; i < iEAPBuffer.Count(); i++)
        {
        iEAPBuffer[i]->iStatusRefArray.Close();
        iEAPBuffer[i]->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypes.Close();
        iEAPBuffer[i]->iEAPSettings->iDisabledEncapsulatedEAPExpandedTypes.Close();
        iEAPBuffer[i]->iEAPSettings->iCipherSuites.Close();
        delete iEAPBuffer[i]->iEAPSettings;
        iEAPBuffer[i]->iEAPSettings = NULL;
        }
    iEAPBuffer.ResetAndDestroy();

    TInt count = iSecondaryBuffer.Count();
    for (TInt index = 0; index < count; index++)
        {
        delete iSecondaryBuffer[index];
        }

    iSecondaryBuffer.Reset();

    _DBG_FILE("CNSmlWLanAdapter::~CNSmlWLanAdapter(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::DDFVersionL( CBufBase& aDDFVersion )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    _DBG_FILE("CNSmlWLanAdapter::DDFVersionL(): begin");

    aDDFVersion.InsertL(0, KNSmlWLanAdapterDDFversion);

    _DBG_FILE("CNSmlWLanAdapter::DDFVersionL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    _DBG_FILE("CNSmlWLanAdapter::DDFStructureL(): begin");

    TSmlDmAccessTypes *aclTypes = new (ELeave) TSmlDmAccessTypes();
    CleanupStack::PushL(aclTypes);
    //
    // Set Get as acceptable operations
    //
    aclTypes->SetGet();
    aclTypes->SetAdd(); // 041208 Added
    aclTypes->SetDelete();
    aclTypes->SetReplace();

    TSmlDmAccessTypes accessTypesAll;
    accessTypesAll.SetGet();
    accessTypesAll.SetDelete();
    accessTypesAll.SetAdd();
    accessTypesAll.SetReplace();

    TSmlDmAccessTypes accessTypesNoDelete;
    accessTypesNoDelete.SetGet();
    accessTypesNoDelete.SetAdd();
    accessTypesNoDelete.SetReplace();

    TSmlDmAccessTypes accessTypesGetAdd;
    accessTypesGetAdd.SetGet();
    accessTypesGetAdd.SetAdd();

    TSmlDmAccessTypes accessTypesGetAddDel;
    accessTypesGetAddDel.SetGet();
    accessTypesGetAddDel.SetAdd();
    accessTypesGetAddDel.SetDelete();
    //WLAN
    MSmlDmDDFObject& rootNode = aDDF.AddChildObjectL(KNSmlWLanNodeName);
    FillNodeInfoL(rootNode, accessTypesGetAdd, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>
    MSmlDmDDFObject& wlanNode = rootNode.AddChildObjectGroupL();
    FillNodeInfoL(wlanNode, accessTypesGetAddDel, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/SSID
    MSmlDmDDFObject& ssid = wlanNode.AddChildObjectL(KNSmlWLanSsid);
    FillNodeInfoL(ssid, accessTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/UsedSSID
    MSmlDmDDFObject& usedSsid = wlanNode.AddChildObjectL(KNSmlWLanUsedSsid);
    FillNodeInfoL(usedSsid, accessTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/SSIDHidden
    MSmlDmDDFObject& ssidHidden = wlanNode.AddChildObjectL(
            KNSmlWLanSsidHidden);
    FillNodeInfoL(ssidHidden, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/NetworkMode
    MSmlDmDDFObject& networkMode = wlanNode.AddChildObjectL(
            KNSmlWLanNetworkMode);
    FillNodeInfoL(networkMode, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/SecurityMode
    MSmlDmDDFObject& securityMode = wlanNode.AddChildObjectL(
            KNSmlWLanSecurityMode);
    FillNodeInfoL(securityMode, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/UseWPAPSK
    MSmlDmDDFObject& useWPAPSK = wlanNode.AddChildObjectL(KNSmlWLanUseWPAPSK);
    FillNodeInfoL(useWPAPSK, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/WPAPreSharedKey
    MSmlDmDDFObject& wpaPreSharedKey = wlanNode.AddChildObjectL(
            KNSmlWLanWpaPreSharedKey);
    FillNodeInfoL(wpaPreSharedKey, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBin, KNSmlEmpty);

    //WLAN/<x>/WLANEAPList
    MSmlDmDDFObject& wlanEapList = wlanNode.AddChildObjectL(KNSmlWLanEapList);
    FillNodeInfoL(wlanEapList, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/WEPKeyIndex
    MSmlDmDDFObject& wepKeyIndex =
            wlanNode.AddChildObjectL(KNSmlWLanWepIndex);
    FillNodeInfoL(wepKeyIndex, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EInt, KNSmlEmpty);

    //WLAN/<x>/WEPAuthMode
    MSmlDmDDFObject& wlanWepAuthMode = wlanNode.AddChildObjectL(
            KNSmlWLanAuthMode);
    FillNodeInfoL(wlanWepAuthMode, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EInt, KNSmlEmpty);

    //WLAN/<x>/WEPKey/
    MSmlDmDDFObject& wepKeyRoot = wlanNode.AddChildObjectL(KNSmlWLanWepKey);
    FillNodeInfoL(wepKeyRoot, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/WEPKey/<x>
    MSmlDmDDFObject& wepKey = wepKeyRoot.AddChildObjectGroupL();
    FillNodeInfoL(wepKey, accessTypesGetAdd, MSmlDmDDFObject::EOneOrN,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/WEPKey/<x>/WEPKeyID
    MSmlDmDDFObject& wepKeyId = wepKey.AddChildObjectL(KNSmlWLanWepKeyId);
    FillNodeInfoL(wepKeyId, accessTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EInt, KNSmlEmpty);

    //WLAN/<x>/WEPKey/<x>/Data
    MSmlDmDDFObject& data = wepKey.AddChildObjectL(KNSmlWLanWepKeyData);
    FillNodeInfoL(data, accessTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EBin, KNSmlEmpty);

    //WLAN/<x>/SecondarySSID/
    MSmlDmDDFObject& secondarySSidRoot = wlanNode.AddChildObjectL(
            KNSmlWLanSecSsid);
    FillNodeInfoL(secondarySSidRoot, accessTypesGetAdd,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/SecondarySSID/<x>
    MSmlDmDDFObject& secondarySSid = secondarySSidRoot.AddChildObjectGroupL();
    FillNodeInfoL(secondarySSid, accessTypesGetAddDel,
            MSmlDmDDFObject::EOneOrN, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/SecondarySSID/<x>/SSID
    MSmlDmDDFObject& secSsidId = secondarySSid.AddChildObjectL(KNSmlWLanSsid);
    FillNodeInfoL(secSsidId, accessTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/SecondarySSID/<x>/UsedSSID
    MSmlDmDDFObject& secUsedSsidId = secondarySSid.AddChildObjectL(
            KNSmlWLanUsedSsid);
    FillNodeInfoL(secUsedSsidId, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/
    MSmlDmDDFObject& eapRoot = wlanNode.AddChildObjectL(KNSmlWLanEap);
    FillNodeInfoL(eapRoot, accessTypesGetAdd, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>
    MSmlDmDDFObject& eap = eapRoot.AddChildObjectGroupL();
    FillNodeInfoL(eap, accessTypesGetAdd, MSmlDmDDFObject::EOneOrN,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/EAPType
    MSmlDmDDFObject& eapType = eap.AddChildObjectL(KNSmlWLanEapType);
    FillNodeInfoL(eapType, accessTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Username
    MSmlDmDDFObject& eapUsername = eap.AddChildObjectL(KNSmlWLanUsername);
    FillNodeInfoL(eapUsername, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Password
    MSmlDmDDFObject& eapPassword = eap.AddChildObjectL(KNSmlWLanPassword);
    FillNodeInfoL(eapPassword, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Realm
    MSmlDmDDFObject& eapRealm = eap.AddChildObjectL(KNSmlWLanRealm);
    FillNodeInfoL(eapRealm, accessTypesNoDelete, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/UsePseudonym
    MSmlDmDDFObject& eapPseudo = eap.AddChildObjectL(KNSmlWLanUsePseudonym);
    FillNodeInfoL(eapPseudo, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Encapsulation
    // Encapsulation is not supported using this parameter. It is done
    // with */EAP/EAPIdxxx-yyy/* where xxx is the encapsulated and yyy the encapsulating method.
    /*MSmlDmDDFObject& eapEncapsulation = eap.AddChildObjectL( KNSmlWLanEncapsulation );
     FillNodeInfoL( eapEncapsulation,
     accessTypesNoDelete,
     MSmlDmDDFObject::EZeroOrOne,
     MSmlDmDDFObject::EDynamic,
     MSmlDmDDFObject::EChr,
     KNSmlEmpty );                           */

    //WLAN/<x>/EAP/<x>/VerifyCertRealm
    MSmlDmDDFObject& eapVerifyCertRealm = eap.AddChildObjectL(
            KNSmlWLanVerifyCertRealm);
    FillNodeInfoL(eapVerifyCertRealm, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/RequireClientAuthentication
    MSmlDmDDFObject& eapRequireClientAuthentication = eap.AddChildObjectL(
            KNSmlWLanRequireClientAuthentication);
    FillNodeInfoL(eapRequireClientAuthentication, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/SessionValidityTime
    MSmlDmDDFObject& eapSessionValidityTime = eap.AddChildObjectL(
            KNSmlWLanSessionValidityTime);
    FillNodeInfoL(eapSessionValidityTime, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/CipherSuite
    MSmlDmDDFObject& eapCipherSuite = eap.AddChildObjectL(
            KNSmlWLanCipherSuite);
    FillNodeInfoL(eapCipherSuite, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/AllowPEAPv0
    MSmlDmDDFObject& eapAllowPEAPv0 = eap.AddChildObjectL(
            KNSmlWLanAllowPEAPv0);
    FillNodeInfoL(eapAllowPEAPv0, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/AllowPEAPv1
    MSmlDmDDFObject& eapAllowPEAPv1 = eap.AddChildObjectL(
            KNSmlWLanAllowPEAPv1);
    FillNodeInfoL(eapAllowPEAPv1, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/AllowPEAPv2
    MSmlDmDDFObject& eapAllowPEAPv2 = eap.AddChildObjectL(
            KNSmlWLanAllowPEAPv2);
    FillNodeInfoL(eapAllowPEAPv2, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool, KNSmlEmpty);

#ifdef FF_WLAN_EXTENSIONS 

    //WLAN/<x>/EAP/<x>/AuthProvModeAllowed
    MSmlDmDDFObject& authProvMode = eap.AddChildObjectL( KNSmlWLanAuthProvMode );
    FillNodeInfoL( authProvMode,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNSmlEmpty );

    //WLAN/<x>/EAP/<x>/UnauthProvModeAllowed
    MSmlDmDDFObject& unauthProvMode = eap.AddChildObjectL( KNSmlWLanUnauthProvMode );
    FillNodeInfoL( unauthProvMode,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNSmlEmpty );

    //WLAN/<x>/EAP/<x>/PACGroupReference
    MSmlDmDDFObject& pacGroupRef = eap.AddChildObjectL( KNSmlWLanPACGroupRef );
    FillNodeInfoL( pacGroupRef,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr,
            KNSmlEmpty );

    //WLAN/<x>/EAP/<x>/WarnADHPNoPAC
    MSmlDmDDFObject& warnADHPNoPAC = eap.AddChildObjectL( KNSmlWLanWarnADHPNoPAC );
    FillNodeInfoL( warnADHPNoPAC,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNSmlEmpty );

    //WLAN/<x>/EAP/<x>/WarnADHPNoMatchingPAC
    MSmlDmDDFObject& warnADHPNoMatchingPAC = eap.AddChildObjectL( KNSmlWLanWarnADHPNoMatchPAC );
    FillNodeInfoL( warnADHPNoMatchingPAC,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNSmlEmpty );

    //WLAN/<x>/EAP/<x>/WarnNotDefaultServer
    MSmlDmDDFObject& warnNoDefServ = eap.AddChildObjectL( KNSmlWLanWarnNotDefaultSrv );
    FillNodeInfoL( warnNoDefServ,
            accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EBool,
            KNSmlEmpty );

#endif           

    //WLAN/<x>/EAP/<x>/Certificate
    MSmlDmDDFObject& eapCertificateRoot = eap.AddChildObjectL(
            KNSmlWLanCertificate);
    FillNodeInfoL(eapCertificateRoot, accessTypesGetAdd,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>
    MSmlDmDDFObject& eapCertificate =
            eapCertificateRoot.AddChildObjectGroupL();
    FillNodeInfoL(eapCertificate, accessTypesGetAdd,
            MSmlDmDDFObject::EOneOrMore, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/IssuerName
    MSmlDmDDFObject& eapIssuerName = eapCertificate.AddChildObjectL(
            KNSmlWLanIssuerName);
    FillNodeInfoL(eapIssuerName, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/SubjectName
    MSmlDmDDFObject& eapSubjectName = eapCertificate.AddChildObjectL(
            KNSmlWLanSubjectName);
    FillNodeInfoL(eapSubjectName, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/CertType
    MSmlDmDDFObject& eapCertType = eapCertificate.AddChildObjectL(
            KNSmlWLanCertType);
    FillNodeInfoL(eapCertType, accessTypesNoDelete, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EDynamic, MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/SerialNumber
    MSmlDmDDFObject& eapSerialNumber = eapCertificate.AddChildObjectL(
            KNSmlWLanSerialNumber);
    FillNodeInfoL(eapSerialNumber, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/SubjectKeyId
    MSmlDmDDFObject& eapSubjectKeyId = eapCertificate.AddChildObjectL(
            KNSmlWLanSubjectKeyId);
    FillNodeInfoL(eapSubjectKeyId, accessTypesNoDelete,
            MSmlDmDDFObject::EOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    //WLAN/<x>/EAP/<x>/Certificate/<x>/Fingerprint
    MSmlDmDDFObject& eapFingerprint = eapCertificate.AddChildObjectL(
            KNSmlWLanFingerprint);
    FillNodeInfoL(eapFingerprint, accessTypesNoDelete,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::EChr, KNSmlEmpty);

    CleanupStack::PopAndDestroy(); //aclTypes

    _DBG_FILE("CNSmlWLanAdapter::DDFStructureL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::AddLeafObjectL( const TDesC& aURI,const 
// TDesC& aParentLUID, const TDesC8& aObject, const TDesC& aType, 
// const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::AddLeafObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, const TDesC8& aObject,
        const TDesC8& /*aType*/, const TInt aStatusRef)
    {
    DBG_ARGS8(_S8("CNSmlWLanAdapter::AddLeafObjectL - <%S> <%S> <%S>"),
            &aURI, &aParentLUID, &aObject);

    TInt uriSegs = NumOfUriSegs(aURI);
    TInt luid = GetIdFromLuid(aParentLUID);
    TInt err(KErrNone);
    TPtrC8 lastUriSeg = GetLastUriSeg(aURI);

    TInt eapIndex(0);

    TInt eapId = GetEAPIdFromUri(aURI);
    if (eapId == KErrNotFound)
        {
        _DBG_FILE("CNSmlWLanAdapter::AddLeafObjectL(): Non-EAP setting.");
        if (luid == KErrNotFound)
            {
            return;
            }
        err = iWlanSettings->GetWlanSettings(luid, *iSettings);
        }
    else
        {
        _DBG_FILE("CNSmlWLanAdapter::AddLeafObjectL(): EAP setting.");
        TInt encapsEapId = GetEncapsEAPIdFromUri(aURI);
        if (encapsEapId == KErrNotFound)
            {
            _DBG_FILE(
                    "CNSmlWLanAdapter::AddLeafObjectL(): Does not have encapsulation id xx-xx.");
            encapsEapId = EEapNone;
            }
        // append it to the appropriate list ('+' enabled, '-' disabled)
        _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
        _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
        const TInt KPlainMsChapV2ImplUid = 99;

        TBuf8<KExpandedEapIdLength> cmpbuf;
        if (eapId == KPlainMsChapV2ImplUid)
            {
            cmpbuf.Append(KMsChapV2Padding);
            }
        else
            {
            cmpbuf.Append(KPadding);
            }
        cmpbuf.Append(eapId);
        TEapExpandedType expandedTypecmbuf(cmpbuf);

        TBuf8<KExpandedEapIdLength> encapscmpbuf;
        if (encapsEapId == KPlainMsChapV2ImplUid)
            {
            encapscmpbuf.Append(KMsChapV2Padding);
            }
        else
            {
            encapscmpbuf.Append(KPadding);
            }
        encapscmpbuf.Append(encapsEapId);
        TEapExpandedType expandedTypeencapscmbuf(encapscmpbuf);

        TInt i(0);
        // This is EAP addition. Check if the buffer for this EAP already exists
        for (i = 0; i < iEAPBuffer.Count(); i++)
            {
            if ((expandedTypecmbuf.Compare(
                    iEAPBuffer[i]->iEAPSettings->iEAPExpandedType) == 0)
                    && (expandedTypeencapscmbuf.Compare(
                            iEAPBuffer[i]->iEncapsulatingExpandedEapId) == 0))
                {
                // The correct buffer was found.     			
                _DBG_FILE(
                        "CNSmlWLanAdapter::AddLeafObjectL(): Found existing buffer.");
                eapIndex = i;
                break;
                }
            }
        if (i == iEAPBuffer.Count())
            {
            DBG_ARGS8(
                    _S8(
                            "CNSmlWLanAdapter::AddLeafObjectL(): No buffer found. Create new: <%d>-<%d>"),
                    eapId, encapsEapId);
            // Buffer was not found. Create new.
            TInt WLANServiceId = GetServiceIdFromUriL(aURI);

            if (WLANServiceId == KErrNotFound)
                {
                _DBG_FILE(
                        "CNSmlWLanAdapter::AddLeafObjectL(): Could not find WLAN service ID from URI.");
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                return;
                }

            TNSmlEAPBufferElement* buffer =
                    new (ELeave) TNSmlEAPBufferElement;
            CleanupStack::PushL(buffer);

            buffer->iEAPSettings = new (ELeave) EAPSettings;
            CleanupStack::PushL(buffer->iEAPSettings);

            // append it to the appropriate list ('+' enabled, '-' disabled)
            _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
            _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
            const TInt KPlainMsChapV2ImplUid = 99;

            TBuf8<KExpandedEapIdLength> tempbuf;
            if (eapId == KPlainMsChapV2ImplUid)
                {
                tempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                tempbuf.Append(KPadding);
                }
            tempbuf.Append(eapId);

            buffer->iEAPSettings->iEAPExpandedType = tempbuf;
            buffer->iWLANServiceId = WLANServiceId;
            TBuf8<KExpandedEapIdLength> encapsTempbuf;
            if (encapsEapId == KPlainMsChapV2ImplUid)
                {
                encapsTempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                encapsTempbuf.Append(KPadding);
                }
            encapsTempbuf.Append(encapsEapId);

            buffer->iEncapsulatingExpandedEapId = encapsTempbuf;
            iEAPBuffer.AppendL(buffer);

            CleanupStack::Pop(buffer->iEAPSettings);
            CleanupStack::Pop(buffer);

            eapIndex = iEAPBuffer.Count() - 1;
            }
        }
    if (err != KErrNone)
        {
        DBG_ARGS(_S16("CNSmlWLanAdapter::AddLeafObjectL - ErrorCode <%D>"),
                err);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        return;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*/*"))
            != KErrNotFound && uriSegs == 9)
        {
        TInt wepKeyId = GetWepKeyIdFromLuid(aParentLUID);

        if (wepKeyId != KErrNotFound)
            {
            if (lastUriSeg.Match(KNSmlWLanWepKeyData) != KErrNotFound)
                {
                switch (wepKeyId)
                    {
                    case 1:
                        iSettings->WepKey1 = aObject;
                        break;
                    case 2:
                        iSettings->WepKey2 = aObject;
                        break;
                    case 3:
                        iSettings->WepKey3 = aObject;
                        break;
                    case 4:
                        iSettings->WepKey4 = aObject;
                        break;
                    default:
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::ENotFound);
                        return;
                    }
                }
            else if (lastUriSeg.Match(KNSmlWLanWepKeyId) != KErrNotFound)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                return;
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                return;
                }
            }

        else
            {
            TInt index = -1;

            for (TInt i = 0; i < iBuffer->Count(); i++)
                {
                if (aURI.Find(iBuffer->At(i).iUri->Des()) != KErrNotFound)
                    {
                    index = i;
                    break;
                    }
                }
            if (index < 0)
                {
                wepKeyId = GetWepKeyIdFromUri(aURI);

                TBuf8<80> addLUID; // AP/<x>/NapDef/<x>/WLAN/<x>/WEPKey/WKIdnnn
                addLUID.AppendNum(wepKeyId);
                addLUID.AppendNumFixedWidth(luid, EDecimal, 3);

                DBG_ARGS8(
                        _S8(
                                "WLanAdapter::AddleafObject - mapping uri: <%S> to luid: <%S>"),
                        &aURI, &addLUID);
                DBG_ARGS8(_S8("Parametrit - <%S> <%S>"), &aURI, &addLUID);
                iCallBack->SetMappingL(aURI, addLUID);
                if (lastUriSeg.Match(KNSmlWLanWepKeyData) != KErrNotFound)
                    {
                    switch (wepKeyId)
                        {
                        case 1:
                            iSettings->WepKey1 = aObject;
                            break;
                        case 2:
                            iSettings->WepKey2 = aObject;
                            break;
                        case 3:
                            iSettings->WepKey3 = aObject;
                            break;
                        case 4:
                            iSettings->WepKey4 = aObject;
                            break;
                        default:
                            iCallBack->SetStatusL(aStatusRef,
                                    CSmlDmAdapter::ENotFound);
                            return;
                        }
                    }
                else if (lastUriSeg.Match(KNSmlWLanWepKeyId) != KErrNotFound)
                    {
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                    return;
                    }
                else
                    {
                    iCallBack->SetStatusL(aStatusRef,
                            CSmlDmAdapter::ENotFound);
                    return;
                    }
                }
            else
                {
                //Add WepKeyId and execute buffer
                if (lastUriSeg.Match(KNSmlWLanWepKeyId) != KErrNotFound)
                    {
                    iBuffer->At(index).iWepKeyId = DesToInt(aObject);
                    iBuffer->At(index).iIdStatusRef = aStatusRef;

                    //                  ExecuteBufferL( ); (JPLA-6M29NR caused error in WEPkey Data if buffer is executed here)
                    return;
                    }

                else if (lastUriSeg.Match(KNSmlWLanWepKeyData)
                        != KErrNotFound)
                    {
                    iBuffer->At(index).iData = aObject.AllocLC();
                    iBuffer->At(index).iDataSet = ETrue;
                    CleanupStack::Pop(); //iBuffer->At(index).iData
                    iBuffer->At(index).iDataStatusRef = aStatusRef;
                    return;
                    }
                else
                    {
                    iCallBack->SetStatusL(aStatusRef,
                            CSmlDmAdapter::ENotFound);
                    return;
                    }
                }
            }
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/*")) != KErrNotFound
            && uriSegs == 7)
        {
        if (lastUriSeg.Compare(KNSmlWLanSsid) == 0)
            {
            TInt returnValue = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iSettings->SSID, aObject);
            DBG_ARGS(_S16("WLanAdapter::ssid convert: <%D>"), returnValue);
            if (returnValue != KErrNone)
                {
                // setting the default value to the network name
                iSettings->SSID = TPtrC(KNSmlWLanDefaultSSID);
                DBG_ARGS(
                        _S16(
                                "CNSmlWLanAdapter::AddLeafObjectL SSID - ErrorCode <%D>"),
                        returnValue);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanUsedSsid) == 0)
            {
            TInt returnValue = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iSettings->UsedSSID, aObject);
            DBG_ARGS(_S16("WLanAdapter::usedSsid convert: <%D>"), returnValue);
            }

        else if (lastUriSeg.Compare(KNSmlWLanSsidHidden) == 0)
            {
            if (aObject.MatchF(_L8("True")) != KErrNotFound)
                {
                iSettings->ScanSSID = 1;
                }
            else
                {
                iSettings->ScanSSID = 0;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanNetworkMode) == 0)
            {
            if (aObject.MatchF(_L8("ADHOC")) != KErrNotFound)
                {
                iSettings->ConnectionMode = EAdhoc;
                }

            else if (aObject.MatchF(_L8("Infrastructure")) != KErrNotFound)
                {
                iSettings->ConnectionMode = EInfrastructure;
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::EInvalidObject);
                return;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanSecurityMode) == 0)
            {
            if (aObject.MatchF(_L8("NONE")) != KErrNotFound)
                {
                iSettings->SecurityMode = EAllowUnsecure;
                }
            else if (aObject.MatchF(_L8("WEP")) != KErrNotFound)
                {
                iSettings->SecurityMode = EWep;
                }
            else if (aObject.MatchF(_L8("8021X")) != KErrNotFound)
                {
                iSettings->SecurityMode = EWlan8021x;
                }
            else if (aObject.MatchF(_L8("WPA")) != KErrNotFound)
                {
                iSettings->SecurityMode = EWpa;
                }
            else if (aObject.MatchF(_L8("WPA2")) != KErrNotFound)
                {
                iSettings->SecurityMode = EWpa2;
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::EInvalidObject);
                return;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanUseWPAPSK) == 0)
            {
            if (aObject.MatchF(_L8("True")) != KErrNotFound)
                {
                iSettings->UseWPAPSK = 1;
                }
            else
                {
                iSettings->UseWPAPSK = 0;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanWpaPreSharedKey) == 0)
            {
            if (aObject.Length() > KMaxPSKLength)
                {
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::ETooLargeObject);
                return;
                }
            /*else if(aObject.Length() < KMinPSKLength)
             {
             iCallBack->SetStatusL( aStatusRef, CSmlDmAdapter::EInvalidObject );
             return;
             }*/
            else
                {
                iSettings->WPAPreSharedKey = aObject;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanEapList) == 0)
            {
            TInt returnValue = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iSettings->EapList, aObject);
            DBG_ARGS(_S16("WLanAdapter::EapList convert: <%D>"), returnValue);
            }

        else if (lastUriSeg.Compare(KNSmlWLanAuthMode) == 0)
            {
            if (aObject.MatchF(_L8("1")) != KErrNotFound)
                {
                iSettings->AuthMode = EShared;
                }
            else
                {
                iSettings->AuthMode = EOpen;
                }
            }

        else if (lastUriSeg.Compare(KNSmlWLanWepIndex) == 0)
            {
            TInt32 actualIndex = DesToInt(aObject);
            if (actualIndex < 4 && actualIndex >= 0)
                {
                iSettings->WepIndex = actualIndex;
                }
            else
                {
                if (actualIndex > 3)
                    {
                    iCallBack->SetStatusL(aStatusRef,
                            CSmlDmAdapter::ETooLargeObject);
                    }
                else
                    {
                    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                    }
                return;
                }
            }

        else
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            return;
            }
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*/*"))
            != KErrNotFound && uriSegs == 9)
        {
        TSecondarySSID ssid;
        ssid.Id = 0;
        TUint32 id = GetSecSSIDFromUriL(aURI);
        RArray<TSecondarySSID> secondaryList;
        CleanupClosePushL(secondaryList);
        iWlanSettings->GetSecondarySSIDListL(luid, secondaryList);
        TInt count = secondaryList.Count();
        for (TInt i = 0; i < count; i++)
            {
            if (id == secondaryList[i].Id)
                {
                ssid = secondaryList[i];
                break;
                }
            }
        CleanupStack::PopAndDestroy(&secondaryList);
        if (ssid.Id == 0)
            {

            _DBG_FILE("CNSmlWLanAdapter::Coming to check buffer ");

            //Check if its buffered here - Divya
            TInt index = -1;

            for (TInt i = 0; i < iSecondaryBuffer.Count(); i++)
                {
                if (aURI.Find(iSecondaryBuffer[i]->iUri->Des())
                        != KErrNotFound)
                    {
                    index = i;
                    _DBG_FILE("CNSmlWLanAdapter::one indexis matched :-)");
                    break;
                    }
                }
            if (index < 0)
                {
                _DBG_FILE("CNSmlWLanAdapter::one indexis matched :-)");
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                return;
                }
            else
                {
                DBG_ARGS8(
                        _S8(
                                "CWlanadaptes::Execute Buffer - copying in buffer instead of giving 404 uri: object id  : <%S>"),
                        &aObject);

                if (lastUriSeg.Compare(KNSmlWLanSsid) == 0)
                    {
                    if (aObject.Length() <= KMaxTextLength)
                        {
                        iSecondaryBuffer[index]->ScannedId.Copy(aObject);
                        }
                    else
                        {
                        _DBG_FILE(
                                "CNSmlWLanAdapter::AddLeafObject - SecondarySSID too long");
                        iCallBack->SetStatusL(aStatusRef,
                                CSmlDmAdapter::EError);
                        return;
                        }

                    }
                else if (lastUriSeg.Compare(KNSmlWLanUsedSsid) == 0)
                    {
                    iSecondaryBuffer[index]->UsedId.Copy(aObject);
                    }

                }

            }

        else
            {

            if (lastUriSeg.Compare(KNSmlWLanSsid) == 0)
                {
                ssid.ScannedId.Copy(aObject);
                }
            else if (lastUriSeg.Compare(KNSmlWLanUsedSsid) == 0)
                {
                ssid.UsedId.Copy(aObject);
                }
            TInt retval = iWlanSettings->WriteSecondarySSIDL(luid, ssid,
                    EFalse);
            if (retval == KErrNone)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                }
            else
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                }
            }
        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/*")) != KErrNotFound
            && uriSegs == 9)
        {
        DBG_ARGS8(_S8("CNSmlWLanAdapter::AddLeafObjectL - EAP index: <%d>"),
                eapIndex);

        if (lastUriSeg.Compare(KNSmlWLanEapType) == 0)
            {
            TInt eapid = GetEAPIdFromUri(aURI);
            // append it to the appropriate list ('+' enabled, '-' disabled)
            _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
            _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
            const TInt KPlainMsChapV2ImplUid = 99;
            TBuf8<KExpandedEapIdLength> tempbuf;
            if (eapid == KPlainMsChapV2ImplUid)
                {
                tempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                tempbuf.Append(KPadding);
                }
            tempbuf.Append(eapid);
            // Get EAP ID from URI instead of the value because the server gives the value
            // incorrectly as the textual representation of the EAP method instead of the IANA number.            
            iEAPBuffer[eapIndex]->iEAPSettings->iEAPExpandedType = tempbuf;
            }
        else if (lastUriSeg.Compare(KNSmlWLanUsername) == 0)
            {
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iEAPBuffer[eapIndex]->iEAPSettings->iUsername, aObject);
            if (retval == KErrNone)
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iUsernamePresent = ETrue;
                }
            else
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iUsername.Zero();
                DBG_ARGS(
                        _S16(
                                "CNSmlWLanAdapter::AddLeafObjectL EAP username - ErrorCode <%D>"),
                        retval);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }

            }
        else if (lastUriSeg.Compare(KNSmlWLanPassword) == 0)
            {
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iEAPBuffer[eapIndex]->iEAPSettings->iPassword, aObject);
            if (retval == KErrNone)
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iPasswordPresent = ETrue;
                }
            else
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iPassword.Zero();
                DBG_ARGS(
                        _S16(
                                "CNSmlWLanAdapter::AddLeafObjectL EAP password - ErrorCode <%D>"),
                        retval);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanRealm) == 0)
            {
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    iEAPBuffer[eapIndex]->iEAPSettings->iRealm, aObject);
            if (retval == KErrNone)
                iEAPBuffer[eapIndex]->iEAPSettings->iRealmPresent = ETrue;

            }
        else if (lastUriSeg.Compare(KNSmlWLanUsePseudonym) == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iUsePseudonyms = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iUsePseudonyms = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iUsePseudonymsPresent = ETrue;
            }
        else if (lastUriSeg.Compare(KNSmlWLanEncapsulation) == 0)
            {
            TBuf8<KExpandedEAPIdLength> dummy;
            //ConvertEAPStringToIds( aObject, dummy, iEAPBuffer[eapIndex]->iEncapsulatingExpandedEapId );
            }
        else if (lastUriSeg.Compare(KNSmlWLanVerifyCertRealm) == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iVerifyServerRealm
                        = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iVerifyServerRealm
                        = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iVerifyServerRealmPresent
                    = ETrue;

            }
        else if (lastUriSeg.Compare(KNSmlWLanRequireClientAuthentication)
                == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iRequireClientAuthentication
                        = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iRequireClientAuthentication
                        = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iRequireClientAuthenticationPresent
                    = ETrue;

            }
        else if (lastUriSeg.Compare(KNSmlWLanSessionValidityTime) == 0)
            {
            TLex8 lex(aObject);
            TUint newVal;
            TInt sesValErr = lex.Val(newVal);
            // Check whether the conversion went ok
            if (sesValErr == KErrNone)
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iSessionValidityTime
                        = newVal;
                iEAPBuffer[eapIndex]->iEAPSettings->iSessionValidityTimePresent
                        = ETrue;
                }
            else
                {
                // there were errors, let's leave the function and update status
                DBG_ARGS(
                        _S16(
                                "CNSmlWLanAdapter::AddLeafObjectL SessionValidityTime - ErrorCode <%D>"),
                        sesValErr);
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::EInvalidObject);
                return;
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanCipherSuite) == 0)
            {
            FillCipherSuitesL(aObject, eapIndex);
            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv0) == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv0Allowed = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv0Allowed = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iPEAPVersionsPresent = ETrue;
            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv1) == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv1Allowed = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv1Allowed = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iPEAPVersionsPresent = ETrue;

            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv2) == 0)
            {
            if ((aObject == KTrue) || (aObject == KTtrue))
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv2Allowed = ETrue;
            else
                iEAPBuffer[eapIndex]->iEAPSettings->iPEAPv2Allowed = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iPEAPVersionsPresent = ETrue;
            }

#ifdef FF_WLAN_EXTENSIONS 
        else if (lastUriSeg.Compare(KNSmlWLanAuthProvMode) == 0)
            {
            if ( (aObject == KTrue) || (aObject == KTtrue) )
            iEAPBuffer[eapIndex]->iEAPSettings->iAuthProvModeAllowed = ETrue;
            else
            iEAPBuffer[eapIndex]->iEAPSettings->iAuthProvModeAllowed = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iAuthProvModeAllowedPresent = ETrue;
            }

        else if (lastUriSeg.Compare(KNSmlWLanUnauthProvMode) == 0)
            {
            if ( (aObject == KTrue) || (aObject == KTtrue) )
            iEAPBuffer[eapIndex]->iEAPSettings->iUnauthProvModeAllowed = ETrue;
            else
            iEAPBuffer[eapIndex]->iEAPSettings->iUnauthProvModeAllowed = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iUnauthProvModeAllowedPresent = ETrue;
            }

        else if (lastUriSeg.Compare(KNSmlWLanPACGroupRef) == 0)
            {
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8( iEAPBuffer[eapIndex]->iEAPSettings->iPACGroupReference, aObject );
            if (retval == KErrNone)
            iEAPBuffer[eapIndex]->iEAPSettings->iPACGroupReferencePresent = ETrue;

            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnADHPNoPAC) == 0)
            {
            if ( (aObject == KTrue) || (aObject == KTtrue) )
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoPAC = ETrue;
            else
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoPAC = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoPACPresent = ETrue;
            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnADHPNoMatchPAC) == 0)
            {
            if ( (aObject == KTrue) || (aObject == KTtrue) )
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoMatchingPAC = ETrue;
            else
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoMatchingPAC = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iWarnADHPNoMatchingPACPresent = ETrue;
            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnNotDefaultSrv) == 0)
            {
            if ( (aObject == KTrue) || (aObject == KTtrue) )
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnNotDefaultServer = ETrue;
            else
            iEAPBuffer[eapIndex]->iEAPSettings->iWarnNotDefaultServer = EFalse;

            iEAPBuffer[eapIndex]->iEAPSettings->iWarnNotDefaultServerPresent = ETrue;
            }

#endif            

        // Save the status reference
        iEAPBuffer[eapIndex]->iStatusRefArray.AppendL(aStatusRef);
        return;
        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*/*"))
            != KErrNotFound && uriSegs == 11)
        {
        TPtrC8 ptr = NSmlDmURI::ParentURI(aURI);

        TBuf<KGeneralStringMaxLength> certBuffer;

        // Get the LUID from ptr2
        HBufC8* certLuid = iCallBack->GetLuidAllocL(ptr);

        TInt certIndex = GetIdFromLuid(*certLuid);

        delete certLuid;

        DBG_ARGS8(
                _S8(
                        "CNSmlWLanAdapter::AddLeafObjectL - cert index: <%d>, cert count: <%d>"),
                certIndex,
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates.Count());

        if (iEAPBuffer[eapIndex]->iEAPSettings->iCertificates.Count() - 1
                < certIndex)
            {
            _DBG_FILE(
                    "CNSmlWLanAdapter::AddLeafObject - Certificate buffer does not exist. Create it.");
            // The certificate buffer does not exist. Create a new one
            EapCertificateEntry *entry = new (ELeave) EapCertificateEntry;
            TRAPD(err, iEAPBuffer[eapIndex]->iEAPSettings->iCertificates.AppendL(entry));
            if(err != KErrNone)
                {
                delete entry;
                DBG_ARGS(_S16(
                              "CNSmlWLanAdapter::AddLeafObjectL - ErrorCode <%D>"),
                              err);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificatesPresent = ETrue;

            certIndex
                    = iEAPBuffer[eapIndex]->iEAPSettings->iCertificates.Count()
                            - 1;

            TBuf8<2> newLuid;
            newLuid.Num(certIndex);

            DBG_ARGS8(
                    _S8(
                            "WLanAdapter::AddLeafObjectL - mapping uri: <%S> to luid: <%S>"),
                    &aURI, &newLuid);
            iCallBack->SetMappingL(aURI, newLuid);
            }

        if (lastUriSeg.Compare(KNSmlWLanIssuerName) == 0)
            {
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    certBuffer, aObject);
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetIssuerName(
                    certBuffer);
            if (retval == KErrNone)
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetIssuerNamePresent();
            }
        else if (lastUriSeg.Compare(KNSmlWLanSubjectName) == 0)
            {
            certBuffer.FillZ();
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    certBuffer, aObject);
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSubjectName(
                    certBuffer);

            if (retval == KErrNone)
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSubjectNamePresent();
            }
        else if (lastUriSeg.Compare(KNSmlWLanCertType) == 0)
            {
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetCertType(
                    (EapCertificateEntry::TCertType) DesToInt(aObject));
            }
        else if (lastUriSeg.Compare(KNSmlWLanSerialNumber) == 0)
            {
            certBuffer.FillZ();
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    certBuffer, aObject);
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSerialNumber(
                    certBuffer);
            if (retval == KErrNone)
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSerialNumberPresent();

            }
        else if (lastUriSeg.Compare(KNSmlWLanSubjectKeyId) == 0)
            {

            // Define literals to search the subject key for possible 0X/0x beginning
            _LIT(KHexIdLC, "0x");
            _LIT(KHexIdUC, "0X");
            TBuf<2> hexIdBuf;
            // The lenght of the key given as input
            TInt keyLen = aObject.Length();

            // setting the given key to the key buffer
            TBuf8<KNsmlSubjectKeyIdLength> origKey = aObject;
            origKey.SetLength(keyLen);
            TBuf8<KIdentifierLength> key;
            TLex8 tmpByte;
            TUint16 byte;

            // Remove possible spaces from the beginning
            origKey.TrimLeft();

            // the key should be atleast two chars long
            if (origKey.Length() >= 2)
                {
                // Copy the two left most characters in to the buffer
                hexIdBuf.Copy(origKey.Left(2));

                // If the first characters are 0x or 0X, then thet should be ignored
                if (hexIdBuf.Compare(KHexIdLC) == 0 || hexIdBuf.Compare(
                        KHexIdUC) == 0)
                    {
                    // delete two characters
                    origKey.Delete(0, 2);
                    }
                }

            // looping the subject key through, removing whitespaces
            for (TInt i = 0; i < keyLen; i++)
                {
                // removing white spaces from the left side of the key
                origKey.TrimLeft();
                // check that there are characters left
                if (origKey.Length() >= 2)
                    {
                    // pick the two left most bytes from the key
                    tmpByte = origKey.Left(2);
                    // convert byte into binary format
                    err = tmpByte.Val(byte, EHex);

                    // check whether conversion to decimal went ok
                    if (err != KErrNone)
                        {
                        // if there are problems, then leave the loop
                        break;
                        }

                    // store the appended byte into the key variable
                    key.Append(byte);
                    // delete two characters from the left side of the character array in the buffer
                    origKey.Delete(0, 2);
                    }
                else if (origKey.Length() == 1)
                    {
                    // pick the left most byte from the key
                    tmpByte = origKey.Left(1);
                    // convert byte into binary format
                    err = tmpByte.Val(byte, EHex);

                    // check whether conversion to decimal went ok
                    if (err != KErrNone)
                        {
                        // if there are problems, then leave the loop
                        break;
                        }

                    // store the appended byte into the key variable
                    key.Append(byte);
                    // delete two characters from the left side of the character array in the buffer
                    origKey.Delete(0, 1);
                    }
                else
                    {
                    // leave the loop, no characters are left
                    break;
                    }
                }
            // store key value only if no errors occurred
            if (err == KErrNone)
                {
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSubjectKeyId(
                        key);
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetSubjectKeyIdPresent();
                }
            else
                {
                // there were errors, let's leave the function and update status
                DBG_ARGS(_S16(
                        "CNSmlWLanAdapter::AddLeafObjectL - ErrorCode <%D>"),
                        err);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
                return;
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanFingerprint) == 0)
            {
            certBuffer.FillZ();
            TInt retval = CnvUtfConverter::ConvertToUnicodeFromUtf8(
                    certBuffer, aObject);
            iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetThumbprint(
                    certBuffer);
            if (retval == KErrNone)
                iEAPBuffer[eapIndex]->iEAPSettings->iCertificates[certIndex]->SetThumbprintPresent();
            }
        // Save the status reference
        iEAPBuffer[eapIndex]->iStatusRefArray.AppendL(aStatusRef);

        return;
        }

    if (eapId == KErrNotFound)
        {
        err = iWlanSettings->WriteWlanSettings(*iSettings);
        }

    if (err != KErrNone)
        {
        DBG_ARGS(_S16("CNSmlWLanAdapter::AddLeafObjectL - ErrorCode <%D>"),
                err);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
        return;
        }

    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);

    _DBG_FILE("CNSmlWLanAdapter::AddLeafObjectL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::UpdateLeafObjectL( const TDesC& aURI, 
//    const TDesC& aLUID, const TDesC8& aObject, const TDesC& aType, 
//    const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::UpdateLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType,
        const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::UpdateLeafObjectL(): begin");
    DBG_ARGS8(_S8("WLanAdapter::UpdateLeafObjectL - <%S> <%S> <%S>"), &aURI,
            &aLUID, &aObject);

    AddLeafObjectL(aURI, aLUID, aObject, aType, aStatusRef);

    _DBG_FILE("CNSmlWLanAdapter::UpdateLeafObjectL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::UpdateLeafObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType )
//------------------------------------------------------------------------------

void CNSmlWLanAdapter::UpdateLeafObjectL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/,
        const TDesC8& /*aType*/, TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::UpdateLeafObjectL(): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlWLanAdapter::UpdateLeafObjectL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::DeleteObjectL( const TDesC8& aURI, 
// const TDesC8& aLUID, const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::DeleteObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::DeleteObjectL(): begin");
    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;

    if (aLUID.Length() == 0)
        {
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);

        _DBG_FILE("CNSmlWLanAdapter::DeleteObjectL(): end");
        return;
        }

    TInt luid = GetIdFromLuid(aLUID);

    if ((NumOfUriSegs(aURI) < 6) || (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 6))
        {
        if (luid == KErrNotFound)
            {
            return;
            }
        TInt ret = iWlanSettings->DeleteWlanSettings(luid);

        if (ret == KErrNone)
            {
            status = CSmlDmAdapter::EOk;
            }
        else
            {
            status = CSmlDmAdapter::EError;
            }

        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey")) != KErrNotFound
            && NumOfUriSegs(aURI) == 7)
        {
        // Individual WLAN settings cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 8)
        {
        _DBG_FILE(
                "CNSmlWLanAdapter::DeleteObjectL(): matching SecondarySSID/*");
        //Check if the secondary SSID is present , if so delete that 
        TUint32 secid = GetSecSSIDFromUriL(aURI);
        TInt retvalue(KErrNone);
        _DBG_FILE(
                "CNSmlWLanAdapter::DeleteObjectL() before call to DeleteOneSecondarySSIDL ");
        TRAPD(ssidError, retvalue = iWlanSettings->DeleteOneSecondarySSIDL(
                luid, secid));
        _DBG_FILE(
                "CNSmlWLanAdapter::DeleteObjectL() After call to DeleteOneSecondarySSIDL ");
        DBG_ARGS8(_S8(
                "TRAP ERROR  ssidError : Id = %d and retvalue eror = %d"),
                ssidError, retvalue);

        if (ssidError == KErrNone)
            {
            _DBG_FILE("CNSmlWLanAdapter::DeleteObjectL() SSID trap KERRONONE");
            if (retvalue == KErrNone)
                {
                _DBG_FILE(
                        "CNSmlWLanAdapter::DeleteObjectL() retvalue also  KERRONONE, so its deleted");
                status = CSmlDmAdapter::EOk;
                }

            else if (retvalue == KErrNotFound)
                {
                status = CSmlDmAdapter::ENotFound;
                _DBG_FILE(
                        "CNSmlWLanAdapter::DeleteObjectL() retvalue KErrNotFound :-(");
                }

            else
                {
                _DBG_FILE(
                        "CNSmlWLanAdapter::DeleteObjectL() retvalue some error :-(");
                status = CSmlDmAdapter::EError;
                }

            }
        else
            {
            _DBG_FILE("CNSmlWLanAdapter::DeleteObjectL() trap error :-( ");
            status = CSmlDmAdapter::EError;
            }

        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 7)
        {
        // Individual WLAN settings cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 8)
        {
        // Individual WEP keys cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*/Data"))
            != KErrNotFound && NumOfUriSegs(aURI) == 9)
        {
        // Individual WEP keys cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.	
        status = CSmlDmAdapter::EError;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 8)
        {
        // Individual EAP settings cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.	
        status = CSmlDmAdapter::EError;
        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 9)
        {
        // Individual EAP settings cannot be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;
        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 10)
        {
        // Individual certificates are not allowed to be deleted and this case should never happen
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;

        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 11)
        {
        // Individual certificate fields are not allowed to be deleted and this case should never happen.
        // These are deleted through WLAN parent nodes.
        status = CSmlDmAdapter::EError;
        }

    iCallBack->SetStatusL(aStatusRef, status);

    _DBG_FILE("CNSmlWLanAdapter::DeleteObjectL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::FetchLeafObjectL( const TDesC8& aURI, 
//                     const TDesC8& aLUID, const TDesC8& aType, 
//                     const TInt aResultsRef, const TInt aStatusRef )
// not used.
//-----------------------------------------------------------------------------

void CNSmlWLanAdapter::FetchLeafObjectSizeL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, const TDesC8& /*aType*/,
        TInt /*aResultsRef*/, TInt /*aStatusRef*/)
    {
    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectSizeL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectSizeL(): end");
    return;
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::FetchLeafObjectL( const TDesC8& aURI, 
//                     const TDesC8& aLUID, const TDesC8& aType, 
//                     const TInt aResultsRef, const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::FetchLeafObjectL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, const TDesC8& /*aType*/,
        const TInt /*aResultsRef*/, const TInt /*aStatusRef*/)
    {
    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): end");

    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::FetchLeafObjectL( const TDesC& aURI, 
//                     const TDesC& aLUID, const TDesC& aType, 
//                     const TInt aResultsRef, const TInt aStatusRef )
//-----------------------------------------------------------------------------
CSmlDmAdapter::TError CNSmlWLanAdapter::FetchLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aType, CBufBase& aObject)
    {

    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): begin");
    DBG_ARGS8(_S8("WLanAdapter::FetchLeafObjectL - <%S> <%S> <%S>"), &aURI,
            &aLUID, &aType);

    if (aLUID.Length() == 0)
        {

        _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): ENotFound");
        return CSmlDmAdapter::ENotFound;
        }

    CSmlDmAdapter::TError status = CSmlDmAdapter::ENotFound;

    // This luid is the service table id (Found from EAP node and parents)
    TInt luid;
    if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) != KErrNotFound)
        {
        luid = GetServiceIdFromUriL(aURI);
        }
    else
        {
        // The luid supplied is ok
        luid = GetIdFromLuid(aLUID);
        }

    TPtrC8 lastUriSeg = GetLastUriSeg(aURI);

    if (luid == KErrNotFound)
        {
        return CSmlDmAdapter::ENotFound;;
        }

    TInt err = iWlanSettings->GetWlanSettings(luid, *iSettings);

    if (err == KErrNotFound)
        {

        _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): ENotFound");
        return CSmlDmAdapter::ENotFound;
        }

    if (lastUriSeg.Compare(KNSmlWLanSsid) == 0)
        {

        if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
                != KErrNotFound)
            {
            RArray<TSecondarySSID> secondarySSIDs;
            CleanupClosePushL(secondarySSIDs);
            iWlanSettings->GetSecondarySSIDListL(luid, secondarySSIDs);
            TSecondarySSID ssid;
            ssid.Id = 0;
            TUint32 id = GetSecSSIDFromUriL(aURI);
            TInt count = secondarySSIDs.Count();
            for (TInt i = 0; i < count; i++)
                {
                if (id == secondarySSIDs[i].Id)
                    {
                    ssid = secondarySSIDs[i];
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(&secondarySSIDs);
            if (ssid.Id == 0)
                {
                status = CSmlDmAdapter::ENotFound;
                }
            else
                {
                HBufC* data = ssid.ScannedId.AllocLC();
                HBufC8* data8 = HBufC8::NewLC(data->Size());
                TPtr8 dataPtr8 = data8->Des();
                //FFS
                CnvUtfConverter::ConvertFromUnicodeToUtf8(dataPtr8,
                        data->Des());

                aObject.InsertL(0, dataPtr8);
                status = CSmlDmAdapter::EOk;

                CleanupStack::PopAndDestroy(2); //data, data8
                }
            }
        else
            {

            HBufC* data = iSettings->SSID.AllocLC();
            HBufC8* data8 = HBufC8::NewLC(data->Size());
            TPtr8 dataPtr8 = data8->Des();
            //FFS
            CnvUtfConverter::ConvertFromUnicodeToUtf8(dataPtr8, data->Des());

            aObject.InsertL(0, dataPtr8);
            status = CSmlDmAdapter::EOk;

            CleanupStack::PopAndDestroy(2); //data, data8

            }

        }

    else if (lastUriSeg.Compare(KNSmlWLanUsedSsid) == 0)
        {

        if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
                != KErrNotFound)
            {
            RArray<TSecondarySSID> secondarySSIDs;
            CleanupClosePushL(secondarySSIDs);
            iWlanSettings->GetSecondarySSIDListL(luid, secondarySSIDs);
            TSecondarySSID ssid;
            ssid.Id = 0;
            TUint32 id = GetSecSSIDFromUriL(aURI);
            TInt count = secondarySSIDs.Count();
            for (TInt i = 0; i < count; i++)
                {
                if (id == secondarySSIDs[i].Id)
                    {
                    ssid = secondarySSIDs[i];
                    break;
                    }
                }
            CleanupStack::PopAndDestroy(&secondarySSIDs);
            if (ssid.Id == 0)
                {
                status = CSmlDmAdapter::ENotFound;
                }
            else
                {
                HBufC* data = ssid.UsedId.AllocLC();
                HBufC8* data8 = HBufC8::NewLC(data->Size());
                TPtr8 dataPtr8 = data8->Des();
                //FFS
                CnvUtfConverter::ConvertFromUnicodeToUtf8(dataPtr8,
                        data->Des());

                aObject.InsertL(0, dataPtr8);
                status = CSmlDmAdapter::EOk;

                CleanupStack::PopAndDestroy(2); //data, data8
                }
            }
        else
            {

            HBufC* data = iSettings->UsedSSID.AllocLC();
            HBufC8* data8 = HBufC8::NewLC(data->Size());
            TPtr8 dataPtr8 = data8->Des();
            //FFS
            CnvUtfConverter::ConvertFromUnicodeToUtf8(dataPtr8, data->Des());

            aObject.InsertL(0, dataPtr8);
            status = CSmlDmAdapter::EOk;

            CleanupStack::PopAndDestroy(2); //data, data8

            }

        }

    else if (lastUriSeg.Compare(KNSmlWLanSsidHidden) == 0)
        {
        if (iSettings->ScanSSID == 1)
            {
            aObject.InsertL(0, _L8("True"));
            }
        else
            {
            aObject.InsertL(0, _L8("False"));
            }
        status = CSmlDmAdapter::EOk;
        }

    else if (lastUriSeg.Compare(KNSmlWLanNetworkMode) == 0)
        {
        status = CSmlDmAdapter::EOk;

        switch (iSettings->ConnectionMode)
            {
            case 0:
                aObject.InsertL(0, KNSmlWLanAdHoc8);
                break;
            case 1:
                aObject.InsertL(0, KNSmlWLanInfrastructure8);
                break;
            default:
                status = CSmlDmAdapter::ENotFound;
                break;
            }
        }
    else if (lastUriSeg.Compare(KNSmlWLanSecurityMode) == 0)
        {
        status = CSmlDmAdapter::EOk;

        switch (iSettings->SecurityMode)
            {
            case EAllowUnsecure:
                aObject.InsertL(0, KNSmlWLanAllowUnsecure8);
                break;
            case EWep:
                aObject.InsertL(0, KNSmlWLanWep8);
                break;
            case EWlan8021x:
                aObject.InsertL(0, KNSmlWLan8021x8);
                break;
            case EWpa:
                aObject.InsertL(0, KNSmlWLanWpa8);
                break;
            case EWpa2:
                aObject.InsertL(0, KNSmlWLanWpa28);
                break;
            default:
                status = CSmlDmAdapter::ENotFound;
                break;
            }
        }

    else if (lastUriSeg.Compare(KNSmlWLanWpaPreSharedKey) == 0)
        {
        aObject.InsertL(0, iSettings->WPAPreSharedKey);
        status = CSmlDmAdapter::EOk;
        }

    else if (lastUriSeg.Compare(KNSmlWLanUseWPAPSK) == 0)
        {
        if (iSettings->UseWPAPSK == 1)
            {
            aObject.InsertL(0, _L8("True"));
            }
        else
            {
            aObject.InsertL(0, _L8("False"));
            }
        status = CSmlDmAdapter::EOk;
        }

    else if (lastUriSeg.Compare(KNSmlWLanWepIndex) == 0)
        {
        HBufC8* data = IntToDes8LC(iSettings->WepIndex);
        aObject.InsertL(0, data->Des());

        CleanupStack::PopAndDestroy(data); //data
        status = CSmlDmAdapter::EOk;
        }

    else if (lastUriSeg.Compare(KNSmlWLanEapList) == 0)
        {
        HBufC* data = iSettings->EapList.AllocLC();
        HBufC8* data8 = HBufC8::NewLC(data->Size());
        TPtr8 dataPtr8 = data8->Des();
        //FFS
        CnvUtfConverter::ConvertFromUnicodeToUtf8(dataPtr8, data->Des());

        aObject.InsertL(0, dataPtr8);
        status = CSmlDmAdapter::EOk;

        CleanupStack::PopAndDestroy(2); //data, data8

        }

    else if (lastUriSeg.Compare(KNSmlWLanAuthMode) == 0)
        {
        HBufC8* data = IntToDes8LC(iSettings->AuthMode);
        aObject.InsertL(0, data->Des());

        CleanupStack::PopAndDestroy(data); //data
        status = CSmlDmAdapter::EOk;
        }

    else if (lastUriSeg.Compare(KNSmlWLanWepKeyData) == 0)
        {
        TInt wepKeyIndex = GetWepKeyIdFromLuid(aLUID);
        if (wepKeyIndex == KErrNotFound)
            {
            wepKeyIndex = GetWepKeyIdFromUri(aURI);
            TBuf8<80> addLUID; // AP/<x>/NapDef/<x>/WLAN/<x>/WEPKey/WKIdnnn
            addLUID.AppendNum(wepKeyIndex);
            addLUID.AppendNumFixedWidth(luid, EDecimal, 3);
            DBG_ARGS8(
                    _S8(
                            "WLanAdapter::Fetchleaf object, set mapping uri: <%S> to luid: <%S>"),
                    &aURI, &addLUID);
            DBG_ARGS8(_S8("Parametrit - <%S> <%S>"), &aURI, &addLUID);
            iCallBack->SetMappingL(aURI, addLUID);

            }
        status = CSmlDmAdapter::EOk;
        switch (wepKeyIndex)
            {
            case 1:
                aObject.InsertL(0, iSettings->WepKey1);
                break;
            case 2:
                aObject.InsertL(0, iSettings->WepKey2);
                break;
            case 3:
                aObject.InsertL(0, iSettings->WepKey3);
                break;
            case 4:
                aObject.InsertL(0, iSettings->WepKey4);
                break;
            default:
                status = CSmlDmAdapter::EError;
            }
        }
    else if (lastUriSeg.Compare(KNSmlWLanWepKeyId) == 0)
        {
        TInt wepKeyIndex = GetWepKeyIdFromLuid(aLUID);
        if (wepKeyIndex == KErrNotFound)
            {
            wepKeyIndex = GetWepKeyIdFromUri(aURI);
            TBuf8<80> addLUID; // AP/<x>/NapDef/<x>/WLAN/<x>/WEPKey/WKIdnnn
            addLUID.AppendNum(wepKeyIndex);
            addLUID.AppendNumFixedWidth(luid, EDecimal, 3);
            DBG_ARGS8(
                    _S8(
                            "WLanAdapter::Fetchleaf object, set mapping uri: <%S> to luid: <%S>"),
                    &aURI, &addLUID);
            DBG_ARGS8(_S8("Parametrit - <%S> <%S>"), &aURI, &addLUID);
            iCallBack->SetMappingL(aURI, addLUID);
            }
        if (wepKeyIndex > 0)
            {
            HBufC8* data = IntToDes8LC(wepKeyIndex);
            aObject.InsertL(0, data->Des());
            CleanupStack::PopAndDestroy(data); //data
            status = CSmlDmAdapter::EOk;
            }
        else
            {
            status = CSmlDmAdapter::EError;
            }
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) != KErrNotFound)
        {
        TInt eapId = GetEAPIdFromUri(aURI);

        // Check if the id contains encapsulating type as well
        TInt encapsEapId = GetEncapsEAPIdFromUri(aURI);
        if (encapsEapId == KErrNotFound)
            {
            encapsEapId = EEapNone;
            }

        ResetEAPStructL();
        TEapExpandedType expandedTypecmbuf;
        TEapExpandedType expandedTypeencapscmbuf;

        GetExpandedType(eapId, encapsEapId, expandedTypecmbuf,
                expandedTypeencapscmbuf);
        TInt err = iWlanSettings->GetEAPSettings(luid, expandedTypecmbuf,
                expandedTypeencapscmbuf, *iEAPSettings);

        if (err != KErrNone || iEAPSettings == NULL)
            {
            status = CSmlDmAdapter::EError;
            }
        else if (lastUriSeg.Compare(KNSmlWLanEapType) == 0)
            {
            aObject.InsertL(0, *IntToDes8LC(eapId));
            CleanupStack::PopAndDestroy(); // IntToDes8LC
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanUsername) == 0)
            {
            if (iEAPSettings->iUsernamePresent)
                {
                DesToBufferL(aObject, iEAPSettings->iUsername);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanPassword) == 0)
            {
            if (iEAPSettings->iPasswordPresent)
                {
                DesToBufferL(aObject, iEAPSettings->iPassword);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanRealm) == 0)
            {
            if (iEAPSettings->iRealmPresent)
                {
                DesToBufferL(aObject, iEAPSettings->iRealm);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanUsePseudonym) == 0)
            {
            if (iEAPSettings->iUsePseudonymsPresent
                    && iEAPSettings->iUsePseudonyms)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanEncapsulation) == 0)
            {
            if (encapsEapId == EEapPeap)
                {
                aObject.InsertL(0, KEAPPEAP);
                }
            else if (encapsEapId == EEapTtls)
                {
                aObject.InsertL(0, KEAPTTLS);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanVerifyCertRealm) == 0)
            {
            if (iEAPSettings->iVerifyServerRealmPresent
                    && iEAPSettings->iVerifyServerRealm)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanRequireClientAuthentication)
                == 0)
            {
            if (iEAPSettings->iRequireClientAuthenticationPresent
                    && iEAPSettings->iRequireClientAuthentication)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanSessionValidityTime) == 0)
            {
            if (iEAPSettings->iSessionValidityTimePresent)
                {
                aObject.InsertL(0, *IntToDes8LC(
                        (TInt) iEAPSettings->iSessionValidityTime));
                CleanupStack::PopAndDestroy(); // IntToDes8LC
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanCipherSuite) == 0)
            {
            FillCipherSuiteBufferL(aObject);
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv0) == 0)
            {
            if (iEAPSettings->iPEAPVersionsPresent
                    && iEAPSettings->iPEAPv0Allowed)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv1) == 0)
            {
            if (iEAPSettings->iPEAPVersionsPresent
                    && iEAPSettings->iPEAPv1Allowed)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }
        else if (lastUriSeg.Compare(KNSmlWLanAllowPEAPv2) == 0)
            {
            if (iEAPSettings->iPEAPVersionsPresent
                    && iEAPSettings->iPEAPv2Allowed)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

#ifdef FF_WLAN_EXTENSIONS 
        else if (lastUriSeg.Compare(KNSmlWLanAuthProvMode) == 0)
            {
            if (iEAPSettings->iAuthProvModeAllowedPresent && iEAPSettings->iAuthProvModeAllowed)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

        else if (lastUriSeg.Compare(KNSmlWLanUnauthProvMode) == 0)
            {
            if (iEAPSettings->iUnauthProvModeAllowedPresent && iEAPSettings->iUnauthProvModeAllowed)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

        else if (lastUriSeg.Compare(KNSmlWLanPACGroupRef) == 0)
            {
            if (iEAPSettings->iPACGroupReferencePresent)
                {
                DesToBufferL(aObject, iEAPSettings->iPACGroupReference);
                }
            status = CSmlDmAdapter::EOk;
            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnADHPNoPAC) == 0)
            {
            if (iEAPSettings->iWarnADHPNoPACPresent && iEAPSettings->iWarnADHPNoPAC)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnADHPNoMatchPAC) == 0)
            {
            if (iEAPSettings->iWarnADHPNoMatchingPACPresent && iEAPSettings->iWarnADHPNoMatchingPAC)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

        else if (lastUriSeg.Compare(KNSmlWLanWarnNotDefaultSrv) == 0)
            {
            if (iEAPSettings->iWarnNotDefaultServerPresent && iEAPSettings->iWarnNotDefaultServer)
                {
                aObject.InsertL(0, KTrue);
                }
            else
                {
                aObject.InsertL(0, KFalse);
                }
            status = CSmlDmAdapter::EOk;
            }

#endif  

        else if (lastUriSeg.Compare(KNSmlWLanIssuerName) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetIssuerNamePresent())
                    {
                    aObject.InsertL(0,
                            ConvertTo8LC(*(iEAPSettings->iCertificates[certId
                                    - 1]->GetIssuerName())));
                    CleanupStack::PopAndDestroy(); // ConvertTo8LC
                    status = CSmlDmAdapter::EOk;
                    }
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanSubjectName) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetSubjectNamePresent())
                    {
                    aObject.InsertL(0,
                            ConvertTo8LC(*(iEAPSettings->iCertificates[certId
                                    - 1]->GetSubjectName())));
                    CleanupStack::PopAndDestroy(); // ConvertTo8LC
                    status = CSmlDmAdapter::EOk;
                    }
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanCertType) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                aObject.InsertL(
                        0,
                        *IntToDes8LC(
                                iEAPSettings->iCertificates[certId - 1]->GetCertType()));
                CleanupStack::PopAndDestroy(); // IntToDes8LC
                status = CSmlDmAdapter::EOk;
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanSerialNumber) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetSerialNumberPresent())
                    {
                    aObject.InsertL(0,
                            ConvertTo8LC(*(iEAPSettings->iCertificates[certId
                                    - 1]->GetSerialNumber())));
                    CleanupStack::PopAndDestroy(); // ConvertTo8LC
                    status = CSmlDmAdapter::EOk;
                    }
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanSubjectKeyId) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetSubjectKeyIdPresent())
                    {
                    _DBG_FILE(
                            "CNSmlWLanAdapter::FetchLeafObject - Fetch subject key.");
                    // check the lenght of the given subjectKeyId, if it is longer that 20, then we
                    // update the status with error and will not go through the key
                    if ((iEAPSettings->iCertificates[certId - 1]->GetSubjectKeyId().Length())
                            > KKeyIdentifierLength)
                        {
                        status = CSmlDmAdapter::EError;
                        }
                    else
                        {
                        _DBG_FILE(
                                "CNSmlWLanAdapter::FetchLeafObject - key is under max lenght");
                        TBuf8<KKeyIdentifierLength>
                                binKey = iEAPSettings->iCertificates[certId
                                        - 1]->GetSubjectKeyId();
                        TBuf8<2> idBuf;
                        TBuf8<KKeyIdentifierLength * 2> key;
                        TUint8 byte;

                        // hexadecimal representation
                        _LIT8(KFormat, "%02x");

                        // looping the subject key through
                        for (TInt i = 0; i < binKey.Length(); i++)
                            {
                            _DBG_FILE(
                                    "CNSmlWLanAdapter::FetchLeafObject - Loop the key through.");
                            // pick the next value from the buffer
                            byte = binKey[i];
                            // convert the value into hexadecimal format
                            idBuf.Format(KFormat, byte);
                            // store the hexa value into the key variable
                            key.Append(idBuf);
                            }
                        // Copy the key to the aObject variable
                        aObject.InsertL(0, key);
                        status = CSmlDmAdapter::EOk;
                        }
                    }
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanFingerprint) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetThumbprintPresent())
                    {
                    aObject.InsertL(0,
                            ConvertTo8LC(*(iEAPSettings->iCertificates[certId
                                    - 1]->GetThumbprint())));
                    CleanupStack::PopAndDestroy(); // ConvertTo8LC
                    status = CSmlDmAdapter::EOk;
                    }
                }
            }
        else if (lastUriSeg.Compare(KNSmlWLanSerialNumber) == 0)
            {
            status = CSmlDmAdapter::ENotFound;

            TInt certId = GetCertificateIdFromUri(aURI);

            if (iEAPSettings->iCertificatesPresent
                    && iEAPSettings->iCertificates.Count() >= certId
                    && certId != KErrNotFound)
                {
                if (iEAPSettings->iCertificates[certId - 1]->GetSerialNumberPresent())
                    {
                    aObject.InsertL(0,
                            ConvertTo8LC(*(iEAPSettings->iCertificates[certId
                                    - 1]->GetSerialNumber())));
                    CleanupStack::PopAndDestroy(); // ConvertTo8LC
                    status = CSmlDmAdapter::EOk;
                    }
                }
            }
        }

    DBG_ARGS(_S16("CNSmlWLanAdapter::FetchLeafObjectL - Status <%D>"), status);
    _DBG_FILE("CNSmlWLanAdapter::FetchLeafObjectL(): end");
    return status;
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::ChildURIListL( const TDesC& aURI, 
// const TDesC& aLUID, const CArrayFix<TNSmlDmMappingInfo>& aPreviousURISegmentList, 
// const TInt aResultsRef, const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::ChildURIListL(const TDesC8& aURI, const TDesC8& aLUID,
        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
        const TInt aResultsRef, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): begin");
    DBG_ARGS8(_S8("WLanAdapter::ChildURIListL - <%S> <%S>"), &aURI, &aLUID);
    CBufBase* currentUriSegmentList = CBufFlat::NewL(128);
    CleanupStack::PushL(currentUriSegmentList);

    TInt uriSegs = NumOfUriSegs(aURI);
    TInt luid = GetIdFromLuid(aLUID);

    //AP/<x>/NAPDef/<x>/
    if (aURI.Match(_L8("AP/*/NAPDef/*")) != KErrNotFound && uriSegs == 4)
        {
        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->NAPDef - <%S> <%S>"),
                &aURI, &aLUID);
        if (luid == KErrNotFound)
            {
            return;
            }
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            currentUriSegmentList->InsertL(0, KNSmlNAPDefWlanNode);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        else
            {
            currentUriSegmentList->InsertL(0, KNSmlNAPDefWlanNode);
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLan);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }

    //AP/<x>/NAPDef/<x>/WLAN
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN")) != KErrNotFound && uriSegs
            == 5)
        {
        if (aPreviousURISegmentList.Count() > 0)
            {
            HBufC8 *uriSeg = HBufC8::NewLC(KNSmlMaxURISegLen);
            TPtr8 uriSegPtr = uriSeg->Des();
            uriSegPtr = aPreviousURISegmentList.At(0).iURISeg;
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    uriSegPtr);
            CleanupStack::PopAndDestroy(uriSeg); //uriSeg
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        else
            {
            if (luid == KErrNotFound)
                {
                return;
                }
            if (iWlanSettings->RecordExists(luid) < 0)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlWLanAdapter::ChildURIListL(): status: Not found");
                }
            else
                {
                _LIT8(Kprev, "WLId");
                TBuf8<9> addNAME(Kprev); // WLId
                addNAME.AppendNumFixedWidth(luid, EDecimal, 3);

                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        addNAME);
                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        _L8("/"));

                TBuf8<80> addURI; // AP/<x>/NapDef/<x>/WLAN/WLIdnnn , n=aLUID
                addURI.Append(aURI);
                addURI.Append(_L8("/WLId"));
                addURI.AppendNumFixedWidth(luid, EDecimal, 3);

                TBuf8<16> addLUID;
                _LIT8(KFormat, "%d");
                addLUID.Format(KFormat, luid);

                // Also added to mapping
                DBG_ARGS8(
                        _S8(
                                "WLanAdapter::ChildURIListL - mapping uri: <%S> to luid: <%S>"),
                        &addURI, &addLUID);
                DBG_ARGS8(_S8("Parameters - <%S> <%S>"), &aURI, &aLUID);
                iCallBack->SetMappingL(addURI, addLUID);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                        KNullDesC8);
                _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
                }
            }
        }

    //AP/<x>/NAPDef/<x>/WLAN/<x>
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*")) != KErrNotFound
            && uriSegs == 6)
        {
        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->NAPDef - <%S> <%S>"),
                &aURI, &aLUID);
        if (luid == KErrNotFound)
            {
            return;
            }
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLanListOfLeafs);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }

    //AP/<x>/NAPDef/<x>/WLAN/<x>/WEPKey
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey")) != KErrNotFound
            && uriSegs == 7)
        {
        DBG_ARGS8(_S8(
                "WLanAdapter::ChildURIListL->WEPKey - <%S> <%S> <%D> <%D>"),
                &aURI, &aLUID, aResultsRef, aStatusRef);

        for (TInt i = 0; i < 4; i++)
            {
            if (aPreviousURISegmentList.Count() > i)
                {
                HBufC8 *uriSeg = HBufC8::NewLC(KNSmlMaxURISegLen);
                TPtr8 uriSegPtr = uriSeg->Des();
                uriSegPtr = aPreviousURISegmentList.At(i).iURISeg;

                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        uriSegPtr);
                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        _L8("/"));
                CleanupStack::PopAndDestroy(); //uriSeg
                }
            else
                {
                TBuf8<25> addNAME;
                addNAME.Append(_L8("WKId"));
                addNAME.AppendNum(i + 1);
                addNAME.Append(_L8("/"));

                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        addNAME);
                }
            }

        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                KNullDesC8);
        _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
        }

    //AP/<x>/NAPDef/<x>/WEPKey/<x>
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*")) != KErrNotFound
            && uriSegs == 8)
        {
        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->WEPKey/* - <%S> <%D>"),
                &aURI, luid);
        if (luid == KErrNotFound)
            {
            return;
            }
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            //wlan settings not found for aLUID
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            TInt wepKeyId = GetWepKeyIdFromLuid(aLUID);
            if (wepKeyId == KErrNotFound)
                {
                wepKeyId = GetWepKeyIdFromUri(aURI);

                TBuf8<80> addLUID; // AP/<x>/NapDef/<x>/WLAN/<x>/WEPKey/WKIdnnn
                addLUID.AppendNum(wepKeyId);
                addLUID.AppendNumFixedWidth(luid, EDecimal, 3);

                DBG_ARGS8(
                        _S8(
                                "WLanAdapter::ChildURIListL - mapping uri: <%S> to luid: <%S>"),
                        &aURI, &addLUID);
                DBG_ARGS8(_S8("Parametrit - <%S> <%S>"), &aURI, &addLUID);
                iCallBack->SetMappingL(aURI, addLUID);
                }

            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLanListOfWepKeyLeafs);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }

    //AP/<x>/NAPDef/<x>/WLAN/<x>/SecondarySSID/
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID"))
            != KErrNotFound && uriSegs == 7)
        {
        DBG_ARGS8(
                _S8("WLanAdapter::ChildURIListL->EAP - <%S> <%S> <%D> <%D>"),
                &aURI, &aLUID, aResultsRef, aStatusRef);
        if (aPreviousURISegmentList.Count() > 0)
            {
            HBufC8 *uriSeg = HBufC8::NewLC(KNSmlMaxURISegLen);
            TPtr8 uriSegPtr = uriSeg->Des();
            uriSegPtr = aPreviousURISegmentList.At(0).iURISeg;
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    uriSegPtr);
            CleanupStack::PopAndDestroy(uriSeg); //uriSeg
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        else
            {
            if (luid == KErrNotFound)
                {
                return;
                }
            if (iWlanSettings->RecordExists(luid) < 0)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE(
                        "CNSmlWLanAdapter::ChildURIListL(): status: Not found");
                }
            else
                {
                RArray<TSecondarySSID> secondarySSIDs;
                CleanupClosePushL(secondarySSIDs);
                iWlanSettings->GetSecondarySSIDListL(luid, secondarySSIDs);
                TInt count = secondarySSIDs.Count();
                _LIT8(KPrefixSSID, "SecSSID");
                TBuf8<5> addNAME; // number
                for (TInt i = 0; i < count; i++)
                    {
                    if (i != 0) // Don't do it in first iteration, but with all the following
                        {
                        currentUriSegmentList->InsertL(
                                currentUriSegmentList->Size(), _L8("/"));
                        }
                    currentUriSegmentList->InsertL(
                            currentUriSegmentList->Size(), KPrefixSSID);
                    addNAME.Zero();
                    addNAME.AppendNumFixedWidth(secondarySSIDs[i].Id,
                            EDecimal, 3);
                    currentUriSegmentList->InsertL(
                            currentUriSegmentList->Size(), addNAME);
                    }
                CleanupStack::PopAndDestroy(&secondarySSIDs);
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                        KNullDesC8);
                _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
                }
            }
        }
    //AP/<x>/NAPDef/<x>/WLAN/<x>/SecondarySSID/*
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
            != KErrNotFound && uriSegs == 8)
        {
        DBG_ARGS8(_S8(
                "WLanAdapter::ChildURIListL->SecondarySSID/* - <%S> <%D>"),
                &aURI, luid);
        if (luid == KErrNotFound)
            {
            return;
            }
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            //wlan settings not found for aLUID
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLanListOfSecondaryLeafs);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }
    //AP/<x>/NAPDef/<x>/WLAN/<x>/EAP
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP")) != KErrNotFound
            && uriSegs == 7)
        {
        DBG_ARGS8(
                _S8("WLanAdapter::ChildURIListL->EAP - <%S> <%S> <%D> <%D>"),
                &aURI, &aLUID, aResultsRef, aStatusRef);

        // Need to get the service id from parent node
        luid = GetServiceIdFromUriL(aURI);

        if (luid == KErrNotFound)
            {
            return;
            }
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            //wlan settings not found for aLUID
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            iWlanSettings->InstalledEAPsL(*currentUriSegmentList);

            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }
    //AP/<x>/NAPDef/<x>/WLAN/<x>/EAP/<x>
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) != KErrNotFound
            && uriSegs == 8)
        {
        // Need to get the service id from parent node
        luid = GetServiceIdFromUriL(aURI);

        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->EAP/* - <%S> <%D>"),
                &aURI, luid);
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            //wlan settings not found for aLUID
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLanListOfEAPLeafs);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }
    //AP/<x>/NAPDef/<x>/WLAN/<x>/EAP/<x>/Certificate
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate"))
            != KErrNotFound && uriSegs == 9)
        {
        luid = GetServiceIdFromUriL(aURI);

        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->Certificate - <%S> <%D>"),
                &aURI, luid);
        if (iWlanSettings->RecordExists(luid) < 0)
            {
            //wlan settings not found for aLUID
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            TInt eapId = GetEAPIdFromUri(aURI);

            // Check if the id contains encapsulating type as well
            TInt encapsEapId = GetEncapsEAPIdFromUri(aURI);
            if (encapsEapId == KErrNotFound)
                {
                encapsEapId = EEapNone;
                }

            ResetEAPStructL();
            TEapExpandedType expandedTypecmbuf;
            TEapExpandedType expandedTypeencapscmbuf;

            GetExpandedType(eapId, encapsEapId, expandedTypecmbuf,
                    expandedTypeencapscmbuf);

            TInt err = iWlanSettings->GetEAPSettings(luid, expandedTypecmbuf,
                    expandedTypeencapscmbuf, *iEAPSettings);
            if (err != KErrNone || !iEAPSettings->iCertificatesPresent)
                {
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
                _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
                }
            else
                {
                TInt certCount = iEAPSettings->iCertificates.Count();
                HBufC8* pBuf = HBufC8::NewLC(KNSmlMaxURISegLen);
                TPtr8 ptrCerts = pBuf->Des();
                for (TInt i = 0; i < certCount; i++)
                    {
                    ptrCerts.Append(_L8("CertId"));
                    ptrCerts.AppendNumFixedWidth(i + 1, EDecimal, 3);
                    ptrCerts.Append(_L8("/"));
                    }

                currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                        ptrCerts);
                CleanupStack::PopAndDestroy(pBuf); // pBuf
                iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
                iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                        KNullDesC8);
                _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
                }
            }
        }
    //AP/<x>/NAPDef/<x>/WLAN/<x>/EAP/<x>/Certificate/<x>
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*"))
            != KErrNotFound && uriSegs == 10)
        {
        DBG_ARGS8(_S8("WLanAdapter::ChildURIListL->EAP/* - <%S> <%D>"),
                &aURI, luid);
        TInt eapId = GetEAPIdFromUri(aURI);
        // Check if the id contains encapsulating type as well
        TInt encapsEapId = GetEncapsEAPIdFromUri(aURI);
        if (encapsEapId == KErrNotFound)
            {
            encapsEapId = EEapNone;
            }

        ResetEAPStructL();
        TEapExpandedType expandedTypecmbuf;
        TEapExpandedType expandedTypeencapscmbuf;

        GetExpandedType(eapId, encapsEapId, expandedTypecmbuf,
                expandedTypeencapscmbuf);

        TInt err = iWlanSettings->GetEAPSettings(luid, expandedTypecmbuf,
                expandedTypeencapscmbuf, *iEAPSettings);
        if (err != KErrNone || !iEAPSettings->iCertificatesPresent)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): ENotFound end");
            }
        else
            {
            currentUriSegmentList->InsertL(currentUriSegmentList->Size(),
                    KNSmlWLanListOfCertificateLeafs);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
            iCallBack->SetResultsL(aResultsRef, *currentUriSegmentList,
                    KNullDesC8);
            _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): status ok");
            }
        }

    CleanupStack::PopAndDestroy(currentUriSegmentList);

    _DBG_FILE("CNSmlWLanAdapter::ChildURIListL(): end");
    }

//-----------------------------------------------------------------------------
// void CNSmlWLanAdapter::AddNodeObjectL( const TDesC& aURI, const TDesC& aParentLUID, 
// const TInt aStatusRef )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::AddNodeObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, const TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::AddNodeObjectL(): begin");
    DBG_ARGS8(_S8(
            "WLanAdapter::AddNodeObjectL - uri: <%S> to aParentLuid: <%S>"),
            &aURI, &aParentLUID);

    TInt uriSegs = NumOfUriSegs(aURI);
    if ((aURI.Match(_L8("AP/*/NAPDef/*/WLAN")) != KErrNotFound && uriSegs
            == 5) || (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP"))
            != KErrNotFound && uriSegs == 7) || (aURI.Match(_L8(
            "AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate")) != KErrNotFound
            && uriSegs == 9))
        {
        iCallBack->SetMappingL(aURI, aParentLUID);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        return;
        }

    //Add new wlan settings
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 6)
        {
        TBuf<KCommsDbSvrMaxFieldLength> serviceType;
        TUint32 serviceId(NULL);
        TUint32 lValue;
        CCommsDbTableView* napdefView = iDatabase.OpenViewMatchingUintLC(
                TPtrC(IAP), TPtrC(COMMDB_ID), DesToInt(aParentLUID));
        TInt errorCode = napdefView->GotoFirstRecord();
        if (errorCode == KErrNone)
            {
            napdefView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
            napdefView->ReadUintL(TPtrC(IAP_SERVICE), serviceId); // ID 
            }
        CCommsDbTableView* tableView = iDatabase.OpenViewMatchingUintLC(
                serviceType, TPtrC(COMMDB_ID), serviceId);
        errorCode = tableView->GotoFirstRecord();
        if (errorCode == KErrNone)
            {
            tableView->ReadUintL(TPtrC(COMMDB_ID), lValue);
            }
        CleanupStack::PopAndDestroy();//tableview
        CleanupStack::PopAndDestroy();//napdefview

        iSettings->Id = 0;
        iSettings->ServiceID = lValue;
        iSettings->ConnectionMode = 0;
        iSettings->SSID = TPtrC(KNSmlWLanDefaultSSID);
        iSettings->UsedSSID = _L("");
        iSettings->ScanSSID = 0;
        iSettings->WepKey1 = _L8("");
        iSettings->WepKey2 = _L8("");
        iSettings->WepKey3 = _L8("");
        iSettings->WepKey4 = _L8("");
        iSettings->WepIndex = 0;
        iSettings->SecurityMode = EAllowUnsecure;
        iSettings->WPAPreSharedKey = _L8("");
        iSettings->UseWPAPSK = 0;
        iSettings->EapList = _L("");
        iSettings->AuthMode = 0;

        TInt err = iWlanSettings->WriteWlanSettings(*iSettings);
        if (err != KErrNone)
            {
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            return;
            }

        DBG_ARGS8(
                _S8(
                        "WLanAdapter::AddNodeObjectL - mapping uri: <%S> to luid: <%S>"),
                &aURI, &aParentLUID);
        iCallBack->SetMappingL(aURI, aParentLUID);
        iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey")) != KErrNotFound
            && uriSegs == 7)
        {
        if (iWlanSettings->RecordExists(DesToInt(aParentLUID)))
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EOk);
        else
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::ENotFound);
        return;
        }

    //Add wep key for existing record
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*")) != KErrNotFound
            && uriSegs == 8)
        {
        for (TInt i = 0; i < iBuffer->Count(); i++)
            {
            if (iBuffer->At(i).iUri->Match(aURI) != KErrNotFound)
                {
                iCallBack->SetStatusL(aStatusRef,
                        CSmlDmAdapter::EAlreadyExists);

                _DBG_FILE(
                        "CNSmlWLanAdapter::AddNodeObjectL(): EAlreadyExists");
                return;
                }
            }
        DBG_ARGS8(_S8(
                "WLanAdapter::Bufferiin - uri: <%S> to aParentLuid: <%S>"),
                &aURI, &aParentLUID);

        TNSmlWepKeyBufferElement newNode;
        newNode.iUri = aURI.AllocLC();
        newNode.iData = 0;
        newNode.iWepKeyId = 0;
        //newNode.iUseWPAPSK = 0;
        newNode.iIdStatusRef = 0;
        newNode.iDataStatusRef = 0;
        newNode.iLengthStatusRef = 0;
        newNode.iNodeStatusRef = aStatusRef;
        newNode.iExecuted = EFalse;
        newNode.iDataSet = EFalse;
        newNode.iLuid = DesToInt(aParentLUID);
        iBuffer->AppendL(newNode);

        CleanupStack::Pop(); //newNode.iUri

        _DBG_FILE("CNSmlWLanAdapter::AddNodeObjectL(): end");
        return;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 8)
        {
        /*
         TSecondarySSID ssid;
         ssid.ScannedId = KNullDesC;
         ssid.UsedId = KNullDesC;
         TInt luid = GetIdFromLuid( aParentLUID );
         TInt retval = iWlanSettings->WriteSecondarySSIDL(luid, ssid, ETrue);*/

        TNSmlSecondarySSIDBufferElement *newSecNode =
                new (ELeave) TNSmlSecondarySSIDBufferElement;
        ;
        newSecNode->iUri = aURI.AllocLC();
        newSecNode->ScannedId = KNullDesC;
        newSecNode->UsedId = KNullDesC;
        newSecNode->iExecuted = EFalse;
        newSecNode->wlanLuid = GetIdFromLuid(aParentLUID);
        iSecondaryBuffer.AppendL(newSecNode);
        CleanupStack::Pop(); //newNode.iUri
        return;
        }

    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) != KErrNotFound
            && NumOfUriSegs(aURI) == 8)
        {
        TInt eapTypeId = GetEAPIdFromUri(aURI);
        TInt encapsEapId = GetEncapsEAPIdFromUri(aURI);
        if (encapsEapId == KErrNotFound)
            {
            encapsEapId = EEapNone;
            }

        TInt WLANServiceId = GetServiceIdFromUriL(aURI);

        if (WLANServiceId == KErrNotFound)
            {
            _DBG_FILE(
                    "CNSmlWLanAdapter::AddNodeObjectL(): Could not find WLAN service ID from URI.");
            User::Leave(KErrNotFound);
            }
        // append it to the appropriate list ('+' enabled, '-' disabled)
        _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
        _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
        const TInt KPlainMsChapV2ImplUid = 99;

        TBuf8<KExpandedEapIdLength> cmpbuf;
        if (eapTypeId == KPlainMsChapV2ImplUid)
            {
            cmpbuf.Append(KMsChapV2Padding);
            }
        else
            {
            cmpbuf.Append(KPadding);
            }
        cmpbuf.Append(eapTypeId);
        TEapExpandedType expandedTypecmbuf(cmpbuf);

        TBuf8<KExpandedEapIdLength> encapscmpbuf;
        if (encapsEapId == KPlainMsChapV2ImplUid)
            {
            encapscmpbuf.Append(KMsChapV2Padding);
            }
        else
            {
            encapscmpbuf.Append(KPadding);
            }
        encapscmpbuf.Append(encapsEapId);
        TEapExpandedType expandedTypeencapscmbuf(encapscmpbuf);

        TInt i(0);
        // Check if the buffer for this EAP already exists
        for (i = 0; i < iEAPBuffer.Count(); i++)
            {
            if ((expandedTypecmbuf.Compare(
                    iEAPBuffer[i]->iEAPSettings->iEAPExpandedType) == 0)
                    && (expandedTypeencapscmbuf.Compare(
                            iEAPBuffer[i]->iEncapsulatingExpandedEapId) == 0))
                {
                // The correct buffer was found.                
                _DBG_FILE(
                        "CNSmlWLanAdapter::AddNodeObjectL(): Found existing buffer.");
                break;
                }
            }
        // buffer did not exist. Update the existing buffer
        if (i == iEAPBuffer.Count())
            {
            TNSmlEAPBufferElement* buffer =
                    new (ELeave) TNSmlEAPBufferElement;
            CleanupStack::PushL(buffer);

            buffer->iEAPSettings = new (ELeave) EAPSettings;
            CleanupStack::PushL(buffer->iEAPSettings);

            TBuf8<KExpandedEapIdLength> tempbuf;
            if (eapTypeId == KPlainMsChapV2ImplUid)
                {
                tempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                tempbuf.Append(KPadding);
                }
            tempbuf.Append(eapTypeId);

            buffer->iEAPSettings->iEAPExpandedType = tempbuf;
            buffer->iStatusRefArray.AppendL(aStatusRef);
            buffer->iWLANServiceId = WLANServiceId;
            ConvertEAPStringToIds(buffer->iEapTypeString,
                    buffer->iEAPSettings->iEAPExpandedType);
            TBuf8<KExpandedEapIdLength> encapsTempbuf;
            if (encapsEapId == KPlainMsChapV2ImplUid)
                {
                encapsTempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                encapsTempbuf.Append(KPadding);
                }
            encapsTempbuf.Append(encapsEapId);
            buffer->iEncapsulatingExpandedEapId = encapsTempbuf;
            if (buffer->iEncapsulatingExpandedEapId
                    != (*EapExpandedTypeNone.GetType()))
                {
                ConvertEAPStringToIds(buffer->iEncapsulatingString,
                        buffer->iEncapsulatingExpandedEapId);
                }
            iEAPBuffer.AppendL(buffer);

            CleanupStack::Pop(buffer->iEAPSettings);
            CleanupStack::Pop(buffer);
            }

        // Set the mapping luid to be the index to the iEAPBuffer
        TBuf8<2> luid;
        luid.Num(iEAPBuffer.Count() - 1);
        DBG_ARGS8(
                _S8(
                        "WLanAdapter::AddNodeObjectL - mapping uri: <%S> to luid: <%S>"),
                &aURI, &luid);
        iCallBack->SetMappingL(aURI, luid);

        }
    else if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*"))
            != KErrNotFound && NumOfUriSegs(aURI) == 10)
        {
        TPtrC8 ptr = NSmlDmURI::ParentURI(aURI);
        TPtrC8 ptr2 = NSmlDmURI::ParentURI(ptr);

        HBufC8* eapLuidBuf = iCallBack->GetLuidAllocL(ptr2);

        TInt index = GetIdFromLuid(*eapLuidBuf);

        delete eapLuidBuf;

        EapCertificateEntry *entry = new (ELeave) EapCertificateEntry;
        TRAPD(err, iEAPBuffer[index]->iEAPSettings->iCertificates.AppendL(entry));
        if(err != KErrNone)
            {
            delete entry;
            DBG_ARGS(_S16(
                         "CNSmlWLanAdapter::AddLeafObjectL - ErrorCode <%D>"),
                         err);
            iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
            return;
            }
                    
        iEAPBuffer[index]->iEAPSettings->iCertificatesPresent = ETrue;
        iEAPBuffer[index]->iStatusRefArray.AppendL(aStatusRef);

        TBuf8<2> luid;
        luid.Num(iEAPBuffer[index]->iEAPSettings->iCertificates.Count() - 1);

        DBG_ARGS8(
                _S8(
                        "WLanAdapter::AddNodeObjectL - mapping uri: <%S> to luid: <%S>"),
                &aURI, &luid);
        iCallBack->SetMappingL(aURI, luid);
        }
    _DBG_FILE("CNSmlWLanAdapter::AddNodeObjectL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::ExecuteCommandL
// not supported
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::ExecuteCommandL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, const TDesC8& /*aArgument*/,
        const TDesC8& /*aType*/, TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::ExecuteCommandL(): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlWLanAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::ExecuteCommandL ( .. RWriteStream ..)
// not supported
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::ExecuteCommandL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/,
        const TDesC8& /*aType*/, TInt aStatusref)
    {
    _DBG_FILE("CNSmlWLanAdapter::ExecuteCommandL(): begin");
    iCallBack->SetStatusL(aStatusref, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlWLanAdapter::ExecuteCommandL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::CopyCommandL
// not supported
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::CopyCommandL(const TDesC8& /*aTargetURI*/,
        const TDesC8& /*aTargetLUID*/, const TDesC8& /* aSourceURI*/,
        const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/,
        TInt aStatusRef)
    {
    _DBG_FILE("CNSmlWLanAdapter::CopyCommandL(): begin");
    iCallBack->SetStatusL(aStatusRef, CSmlDmAdapter::EError);
    _DBG_FILE("CNSmlWLanAdapter::CopyCommandL(): end");
    }
//------------------------------------------------------------------------------
// CNSmlWLanAdapter::StartAtomicL
// not supported
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::StartAtomicL()
    {
    _DBG_FILE("CNSmlWLanAdapter::StartAtomicL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::StartAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::CommitAtomicL
// not supported
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::CommitAtomicL()
    {
    _DBG_FILE("CNSmlWLanAdapter::CommitAtomicL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::CommitAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::RollbackAtomicL
// returns EError
//------------------------------------------------------------------------------
void CNSmlWLanAdapter::RollbackAtomicL()
    {
    _DBG_FILE("CNSmlWLanAdapter::RollbackAtomicL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::RollbackAtomicL(): end");
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::StreamingSupport
// returns EFalse
//------------------------------------------------------------------------------

TBool CNSmlWLanAdapter::StreamingSupport(TInt& /*aItemSize*/)
    {
    _DBG_FILE("CNSmlWLanAdapter::StreamingSupport(): begin");
    _DBG_FILE("CNSmlWLanAdapter::StreamingSupport(): end");
    return EFalse;
    }

//------------------------------------------------------------------------------
// CNSmlWLanAdapter::StreamCommittedL
// not used in this adapter
//------------------------------------------------------------------------------

void CNSmlWLanAdapter::StreamCommittedL()
    {
    _DBG_FILE("CNSmlWLanAdapter::StreamCommittedL(): begin");
    _DBG_FILE("CNSmlWLanAdapter::StreamCommittedL(): end");
    }

//-----------------------------------------------------------------------------
// CNSmlWLanAdapter* CNSmlWLanAdapter::CompleteOutstandingCmdsL( )
//-----------------------------------------------------------------------------
void CNSmlWLanAdapter::CompleteOutstandingCmdsL()
    {
    _DBG_FILE("CNSmlWLanAdapter::CompleteOutStandingCmdsL(): begin");

    ExecuteBufferL(ETrue);

    _DBG_FILE("CNSmlWLanAdapter::CompleteOutStandingCmdsL(): end");
    }

// ------------------------------------------------------------------------------------------------
// TPtrC8 CNSmlWLanAdapter::GetLastUriSeg(const TDesC8& aURI)
// Returns only the last uri segemnt
// ------------------------------------------------------------------------------------------------
TPtrC8 CNSmlWLanAdapter::GetLastUriSeg(const TDesC8& aURI)
    {
    _DBG_FILE("CNSmlWLanAdapter::GetLastUriSeg(): begin");

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
        _DBG_FILE("CNSmlWLanAdapter::GetLastUriSeg(): end");
        return aURI;
        }
    else
        {
        _DBG_FILE("CNSmlWLanAdapter::GetLastUriSeg(): end");
        return aURI.Mid(i + 1);
        }
    }

// ------------------------------------------------------------------------------------------------
// TPtrC16 CNSmlWLanAdapter::RemoveLastURISeg(const TDesC8& aURI)
// returns parent uri, i.e. removes last uri segment
// ------------------------------------------------------------------------------------------------
TPtrC8 CNSmlWLanAdapter::RemoveLastUriSeg(const TDesC8& aURI)
    {
    _DBG_FILE("CNSmlWLanAdapter::RemoveLastUriSeg(): begin");

    TInt i;
    for (i = aURI.Length() - 1; i >= 0; i--)
        {
        if (aURI[i] == '/')
            {
            break;
            }
        }

    _DBG_FILE("CNSmlWLanAdapter::RemoveLastUriSeg(): end");
    return aURI.Left(i);
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::DesToInt( const TDesC8& aLuid )
// Returns aLuid as integer value
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::DesToInt(const TDesC8& aLuid)
    {
    _DBG_FILE("CNSmlWLanAdapter::DesToInt(): begin");

    TLex8 lex(aLuid);
    TInt value = 0;
    lex.Val(value);

    DBG_ARGS8(_S8("WLanAdapter::DesToInt() - Des: <%S> Int: <%D>"), &aLuid,
            value);
    _DBG_FILE("CNSmlWLanAdapter::DesToInt(): end");
    return value;
    }

// ------------------------------------------------------------------------------------------------
// HBufC* CNSmlWLanAdapter::IntToDesLC(const TInt aLuid)
// ------------------------------------------------------------------------------------------------
HBufC* CNSmlWLanAdapter::IntToDesLC(const TInt aLuid)
    {
    _DBG_FILE("CNSmlWLanAdapter::IntToDesLC(): begin");

    HBufC* buf = HBufC::NewLC(10);
    TPtr ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);

    _DBG_FILE("CNSmlWLanAdapter::IntToDesLC(): end");
    return buf;
    }

// ------------------------------------------------------------------------------------------------
// HBufC8* CNSmlWLanAdapter::IntToDes8LC(const TInt aLuid)
// ------------------------------------------------------------------------------------------------
HBufC8* CNSmlWLanAdapter::IntToDes8LC(const TInt aLuid)
    {
    _DBG_FILE("CNSmlWLanAdapter::IntToDes8LC(): begin");

    HBufC8* buf = HBufC8::NewLC(10);
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);

    _DBG_FILE("CNSmlWLanAdapter::IntToDes8LC(): end");
    return buf;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::NumOfUriSegs( const TDesC8& aUri )
// Return count of URI segments of aUri
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::NumOfUriSegs(const TDesC8& aUri)
    {
    _DBG_FILE("CNSmlWLanAdapter::NumOfUriSegs(): begin");

    TInt count = 1;
    for (TInt i = 0; i < aUri.Length(); i++)
        {
        if (aUri[i] == '/')
            count++;
        }

    _DBG_FILE("CNSmlWLanAdapter::NumOfUriSegs(): end");
    return count;
    }

// -------------------------------------------------------------------------------------
// CNSmlWLanAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CNSmlWLanAdapter::FillNodeInfoL(MSmlDmDDFObject& aNode,
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

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetEAPIdFromUri( const TDesC8& aUri )
// Return WLan ID number from URI
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetEAPIdFromUri(const TDesC8& aUri)
    {
    DBG_ARGS8(_S8("WLanAdapter::GetEAPIdFromUri - <%S>"), &aUri);

    if (aUri.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*")) == KErrNotFound)
        {
        return KErrNotFound;
        }

    TInt begin = aUri.Find(_L8("/EAPId")) + 6;
    if (begin == KErrNotFound)
        {
        return KErrNotFound;
        }

    TInt eapId = DesToInt(aUri.Mid(begin, 3));

    return eapId;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetEncapsEAPIdFromUri( const TDesC8& aUri )
// Return WLan ID number from URI
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetEncapsEAPIdFromUri(const TDesC8& aUri)
    {
    DBG_ARGS8(_S8("WLanAdapter::GetEncapsEAPIdFromUri - <%S>"), &aUri);

    if (aUri.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/EAPId*-*")) == KErrNotFound)
        {
        return KErrNotFound;
        }

    TInt begin = aUri.Find(_L8("/EAPId")) + 9;
    if (aUri[begin] != '-')
        {
        return KErrNotFound;
        }
    begin++;
    TInt end = -1;
    TInt count;

    for (count = 0; count + begin < aUri.Length(); count++)
        {
        if (aUri[begin + count] == '/')
            {
            end = begin + count;
            break;
            }
        else if (aUri.Length() == begin + count + 1)
            {
            count++;
            end = begin + count;
            break;
            }
        }

    if (end > 0)
        {
        TInt eapId = DesToInt(aUri.Mid(begin, count));
        return eapId;
        }
    return KErrNotFound;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetEAPIdFromUriL( const TDesC8& aUri )
// Return WLan ID number from URI
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetServiceIdFromUriL(const TDesC8& aURI)
    {
    DBG_ARGS8(_S8("WLanAdapter::GetServiceIdFromUriL - <%S>"), &aURI);

    if (aURI.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP*")) == KErrNotFound)
        {
        return KErrNotFound;
        }

    TInt end = aURI.Find(_L8("/EAP")) + 4;

    if (end > 0)
        {

        HBufC8* luidBuf = iCallBack->GetLuidAllocL(aURI.Left(end));

        TInt serviceId = GetIdFromLuid(*luidBuf);

        delete luidBuf;

        return serviceId;
        }
    return KErrNotFound;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetSecSSIDFromUriL( const TDesC8& aUri )
// Return WLan ID number from URI
// ------------------------------------------------------------------------------------------------
TUint32 CNSmlWLanAdapter::GetSecSSIDFromUriL(const TDesC8& aUri)
    {
    DBG_ARGS8(_S8("WLanAdapter::GetSecSSIDFromUriL - <%S>"), &aUri);

    if (aUri.Match(_L8("AP/*/NAPDef/*/WLAN/*/SecondarySSID/*"))
            == KErrNotFound)
        {
        User::Leave(KErrNotFound);
        }
    _LIT8(KSSIDString, "/SecondarySSID/SecSSID");
    TInt begin = aUri.Find(KSSIDString) + KSSIDString().Length();
    TInt end = -1;
    TInt count;

    for (count = 0; count + begin < aUri.Length(); count++)
        {
        if (aUri[begin + count] == '/')
            {
            end = begin + count;
            break;
            }
        else if (aUri.Length() == begin + count + 1)
            {
            count++;
            end = begin + count;
            break;
            }
        }

    if (end > 0)
        {
        TLex8 lex(aUri.Mid(begin, count));
        TUint32 value = 0;
        lex.Val(value, EDecimal);
        return value;
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    return 0;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetEAPIdFromUri( const TDesC8& aUri )
// Return WLan ID number from URI
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetCertificateIdFromUri(const TDesC8& aUri)
    {
    DBG_ARGS8(_S8("WLanAdapter::GetCertificateIdFromUri - <%S>"), &aUri);

    if (aUri.Match(_L8("AP/*/NAPDef/*/WLAN/*/EAP/*/Certificate/*"))
            == KErrNotFound)
        {
        return KErrNotFound;
        }
    _LIT8(KCertString, "/Certificate/CertId");

    TInt begin = aUri.Find(KCertString);
    if (begin == KErrNotFound)
        {
        return KErrNotFound;
        }

    begin += KCertString().Length();

    TInt end = -1;
    TInt count;

    for (count = 0; count + begin < aUri.Length(); count++)
        {
        if (aUri[begin + count] == '/')
            {
            end = begin + count;
            break;
            }
        else if (aUri.Length() == begin + count + 1)
            {
            count++;
            end = begin + count;
            break;
            }
        }

    if (end > 0)
        {
        TInt certId = DesToInt(aUri.Mid(begin, count));
        return certId;
        }
    return KErrNotFound;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetWepKeyIdFromUri( const TDesC8& aUri )
// Returns WepKey ID number from URI
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetWepKeyIdFromUri(const TDesC8& aUri)
    {
    if (aUri.Match(_L8("AP/*/NAPDef/*/WLAN/*/WEPKey/*")) == KErrNotFound)
        {
        return KErrNotFound;
        }

    TInt begin = aUri.Find(_L8("WEPKey/WKId")) + 11;
    TInt end = -1;
    TInt count;

    for (count = 0; count < aUri.Length() - 11; count++)
        {
        if (aUri[begin + count] == '/')
            {
            end = begin + count;
            break;
            }

        else if (aUri.Length() == begin + count + 1)
            {
            count++;
            end = begin + count;
            break;
            }
        }

    if (end > 0)
        {
        TInt wlanId = DesToInt(aUri.Mid(begin, count));
        return wlanId;
        }
    return KErrNotFound;
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetWepKeyIdFromLuid( const TDesC8& aLuid )
// Return id number of wepkey
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetWepKeyIdFromLuid(const TDesC8& aLuid)
    {
    TLex8 iLex(aLuid);

    TInt luid;
    if (iLex.Val(luid) == KErrNone)
        {
        if (luid > 1000)
            {
            return luid / 1000;
            }
        else
            {
            return KErrNotFound;
            }
        }
    else
        {
        return KErrNotFound;
        }
    }

// ------------------------------------------------------------------------------------------------
// TInt CNSmlWLanAdapter::GetIdFromLuid( const TDesC& aLuid )
// Return luid of wlan node
// ------------------------------------------------------------------------------------------------
TInt CNSmlWLanAdapter::GetIdFromLuid(const TDesC8& aLuid)
    {
    TLex8 iLex(aLuid);

    TInt luid;
    if (iLex.Val(luid) == KErrNone)
        {
        if (luid < 1000)
            {
            return luid;
            }
        else
            {
            TInt tmp = luid / 1000 * 1000; //First number of luid > 1000
            return luid - tmp;
            }
        }
    else
        {
        return KErrNotFound;
        }
    }

// ------------------------------------------------------------------------------------------------
// void CNSmlWLanAdapter::ExecuteBufferL( TBool aFinal )
// Executes buffered commands from iBuffer
// ------------------------------------------------------------------------------------------------
void CNSmlWLanAdapter::ExecuteBufferL(TBool aFinal)
    {
    for (TInt index = 0; index < iBuffer->Count(); index++)
        {
        DBG_ARGS8(_S8(
                "WLanAdapter::ExecuteBuffer(): Buf count: <%D> index: <%D>"),
                iBuffer->Count(), index);

        if (iBuffer->At(index).iExecuted)
            {
            continue;
            }
        TInt err = 0;

        TInt luid = iBuffer->At(index).iLuid;
        TInt wepKeyId = iBuffer->At(index).iWepKeyId;
        if (!wepKeyId && !aFinal)
            {
            continue;
            }

        if (wepKeyId > 4 || !wepKeyId && aFinal)
            {
            if (iBuffer->At(index).iDataStatusRef)
                iCallBack->SetStatusL(iBuffer->At(index).iDataStatusRef,
                        CSmlDmAdapter::EError);
            if (iBuffer->At(index).iNodeStatusRef)
                iCallBack->SetStatusL(iBuffer->At(index).iNodeStatusRef,
                        CSmlDmAdapter::EError);
            if (iBuffer->At(index).iLengthStatusRef)
                iCallBack->SetStatusL(iBuffer->At(index).iLengthStatusRef,
                        CSmlDmAdapter::EError);

            iBuffer->At(index).iIdStatusRef = 0;
            iBuffer->At(index).iNodeStatusRef = 0;
            iBuffer->At(index).iDataStatusRef = 0;
            iBuffer->At(index).iLengthStatusRef = 0;
            iBuffer->At(index).iExecuted = ETrue;

            continue;
            }

        if (iBuffer->At(index).iDataSet && iBuffer->At(index).iWepKeyId > 0)
            {
            iWlanSettings->GetWlanSettings(luid, *iSettings);

            switch (wepKeyId)
                {
                case 1:
                    iSettings->WepKey1 = iBuffer->At(index).iData->Des();
                    break;
                case 2:
                    iSettings->WepKey2 = iBuffer->At(index).iData->Des();
                    break;
                case 3:
                    iSettings->WepKey3 = iBuffer->At(index).iData->Des();
                    break;
                case 4:
                    iSettings->WepKey4 = iBuffer->At(index).iData->Des();
                    break;
                default:
                    continue;
                }

            err = iWlanSettings->WriteWlanSettings(*iSettings);
            if (err == KErrNone)
                {
                iCallBack->SetStatusL(iBuffer->At(index).iDataStatusRef,
                        CSmlDmAdapter::EOk);
                }
            else
                {
                iCallBack->SetStatusL(iBuffer->At(index).iDataStatusRef,
                        CSmlDmAdapter::EError);
                }

            iBuffer->At(index).iDataStatusRef = 0;
            }

        TBuf8<5> addLUID;
        addLUID.AppendNum(wepKeyId);
        addLUID.AppendNumFixedWidth(luid, EDecimal, 3);

        iCallBack->SetMappingL(iBuffer->At(index).iUri->Des(), addLUID);
        if (iBuffer->At(index).iIdStatusRef)
            iCallBack->SetStatusL(iBuffer->At(index).iIdStatusRef,
                    CSmlDmAdapter::EOk);
        if (iBuffer->At(index).iNodeStatusRef)
            iCallBack->SetStatusL(iBuffer->At(index).iNodeStatusRef,
                    CSmlDmAdapter::EOk);
        if (iBuffer->At(index).iLengthStatusRef)
            iCallBack->SetStatusL(iBuffer->At(index).iLengthStatusRef,
                    CSmlDmAdapter::EOk);

        iBuffer->At(index).iIdStatusRef = 0;
        iBuffer->At(index).iNodeStatusRef = 0;
        iBuffer->At(index).iLengthStatusRef = 0;
        iBuffer->At(index).iExecuted = ETrue;
        }

    //Clean executed command from buffer
    for (TInt i = 0; i < iBuffer->Count(); i++)
        {
        if (iBuffer->At(i).iExecuted)
            {
            if (iBuffer->At(i).iUri)
                {
                delete iBuffer->At(i).iUri;
                iBuffer->At(i).iUri = 0;
                }
            if (iBuffer->At(i).iData)
                {
                delete iBuffer->At(i).iData;
                iBuffer->At(i).iData = 0;
                }
            iBuffer->Delete(i);
            iBuffer->Compress();
            }
        }
    // Secondary SSID Buffer 
    for (TInt index = 0; index < iSecondaryBuffer.Count(); index++)
        {
        DBG_ARGS8(_S8(
                "WLanAdapter::ExecuteBuffer(): Buf count: <%D> index: <%D>"),
                iBuffer->Count(), index);

        if (iSecondaryBuffer[index]->iExecuted)
            {
            continue;
            }

        TSecondarySSID ssid;
        ssid.ScannedId = iSecondaryBuffer[index]->ScannedId;
        ssid.UsedId = iSecondaryBuffer[index]->UsedId;

        DBG_ARGS8(
                _S8(
                        "CWlanadaptes::Execute Buffer - WLAN_SEC_SSID_SCANNED_SSID uri: <%S> ad  WLAN_SEC_SSID_USED_SSID : <%S>"),
                &iSecondaryBuffer[index]->ScannedId,
                &iSecondaryBuffer[index]->UsedId);

        TInt retval = iWlanSettings->WriteSecondarySSIDL(
                iSecondaryBuffer[index]->wlanLuid, ssid, ETrue);
        iSecondaryBuffer[index]->iExecuted = ETrue;

        }
    TBuf<KMaxLengthOfEapList> eapList;
    eapList.Copy(KEapAll);
    // EAP settings
    for (TInt i = 0; i < iEAPBuffer.Count(); i++)
        {

        // Save EAP list
        if (iEAPBuffer[i]->iEapTypeString.Length() > 0
                && iEAPBuffer[i]->iEncapsulatingString.Length() == 0) // AND method is not encapsulated
            {

            TBuf8<3> buf;
            _LIT8(KFormat, "%d");
            // pick the last byte from the array
            buf.Format(KFormat,
                    iEAPBuffer[i]->iEapTypeString[KExpandedEapIdLength - 1]);
            // search for the correct offset for the eap type from the KEapAll literal
            TInt index = KEapAll().Find(buf);

            const TUint8 offset = 2; //length = f "-0" string
            // Add '+' to the correct offset, so that the eap is repsesented activated in the list
            if (index >= offset)
                {
                eapList[index - offset] = KEapPlus;
                }

            }

        // Check if this method has encapsulated methods
        for (TInt j = 0; j < iEAPBuffer.Count(); j++)
            {
            if (iEAPBuffer[i]->iEAPSettings->iEAPExpandedType
                    == iEAPBuffer[j]->iEncapsulatingExpandedEapId)
                {
                // j is encapsulated inside i
                iEAPBuffer[i]->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypes.Append(
                        iEAPBuffer[j]->iEAPSettings->iEAPExpandedType);
                iEAPBuffer[i]->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypesPresent
                        = ETrue;
                }
            }

        TRAPD(err, iWlanSettings->GetEAPInterfaceL(
                iEAPBuffer[i]->iWLANServiceId,
                iEAPBuffer[i]->iEncapsulatingExpandedEapId,
                iEAPBuffer[i]->iEAPSettings->iEAPExpandedType));
        if (err == KErrNone)
            {
            err = iWlanSettings->WriteEAPSettings(
                    *iEAPBuffer[i]->iEAPSettings);
            }

        MSmlDmAdapter::TError status;
        switch (err)
            {
            case KErrNotFound:
                {
                status = CSmlDmAdapter::ENotFound;
                break;
                }
            case KErrNone:
                {
                status = CSmlDmAdapter::EOk;
                break;
                }
            default:
                {
                status = CSmlDmAdapter::EError;
                }
            }

        for (TInt j = 0; j < iEAPBuffer[i]->iStatusRefArray.Count(); j++)
            {
            iCallBack->SetStatusL(iEAPBuffer[i]->iStatusRefArray[j], status);
            }
        }

    // Expanded EAP types in use. Write the eap list to the new columns.

    // generate appropriate entries in the new enabled and disabled list,
    // overwriting those values 

    // count the + and - signs to determine the size of enabled and 
    // disabled descriptors

    TLex lex(eapList);
    TInt numPlus = 0;
    TInt numMinus = 0;
    TChar ch;
    while (!lex.Eos())
        {
        ch = lex.Get();
        if (ch == '+')
            ++numPlus;
        else if (ch == '-')
            ++numMinus;
        }
    // load general EAP settings If
    CEapGeneralSettings* genSettings;
    genSettings = CEapGeneralSettings::NewL(ELan, iSettings->ServiceID);
    CleanupStack::PushL(genSettings);

    // get lists of enabled/disabled EAPs for the IAP

    RArray<TEapExpandedType> enabledEapMethods;
    RArray<TEapExpandedType> disabledEapMethods;

    enabledEapMethods.Reset();
    disabledEapMethods.Reset();

    lex.Assign(eapList);

    while (!lex.Eos())
        {
        // beginning of implementation UID
        TInt16 implUid = 0;

        if (lex.Val(implUid) != KErrNone || !implUid)
            {
            break;
            }

        // append it to the appropriate list ('+' enabled, '-' disabled)
        _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
        _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
        const TInt KPlainMsChapV2ImplUid = 99;

        if (implUid > 0)
            {
            TBuf8<KExpandedEapIdLength> tempbuf;
            if (Abs(implUid) == KPlainMsChapV2ImplUid)
                {
                tempbuf.Append(KMsChapV2Padding);
                }
            else
                {
                tempbuf.Append(KPadding);
                }
            tempbuf.Append(Abs(implUid));
            TEapExpandedType expandedTypebuf(tempbuf);
            enabledEapMethods.Append(expandedTypebuf);
            }

        // swallow the delimiter (',')
        lex.Get();
        }

    if (numPlus != NULL)
        {
        genSettings->SetEapMethods(enabledEapMethods, disabledEapMethods);
        }

    CleanupStack::PopAndDestroy(genSettings);

    for (TInt i = 0; i < iEAPBuffer.Count(); i++)
        {
        iEAPBuffer[i]->iStatusRefArray.Close();
        iEAPBuffer[i]->iEAPSettings->iEnabledEncapsulatedEAPExpandedTypes.Close();
        iEAPBuffer[i]->iEAPSettings->iDisabledEncapsulatedEAPExpandedTypes.Close();
        iEAPBuffer[i]->iEAPSettings->iCipherSuites.Close();
        delete iEAPBuffer[i]->iEAPSettings;
        iEAPBuffer[i]->iEAPSettings = NULL;
        }
    iEAPBuffer.ResetAndDestroy();
    }

//=============================================
//      CNSmlWLanAdapter::ConvertTo8LC()
//      Converts string value to 8-bit
//      
//=============================================
TDesC8& CNSmlWLanAdapter::ConvertTo8LC(const TDesC& aSource)
    {
    HBufC8* buf = HBufC8::NewLC(aSource.Length() * 2);
    TPtr8 bufPtr = buf->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(bufPtr, aSource);

    return *buf;
    }

//=============================================
//      CNSmlWLanAdapter::ConvertTo16LC()
//      Converts string value to 16-bit
//      
//=============================================
TDesC16& CNSmlWLanAdapter::ConvertTo16LC(const TDesC8& aSource)
    {
    HBufC16* buf16 = HBufC16::NewLC(aSource.Length());
    TPtr bufPtr16 = buf16->Des();

    CnvUtfConverter::ConvertToUnicodeFromUtf8(bufPtr16, aSource);

    return *buf16;
    }

void CNSmlWLanAdapter::ResetEAPStructL()
    {
    DeleteEAPStructL();
    iEAPSettings = new (ELeave) EAPSettings;
    }

void CNSmlWLanAdapter::DeleteEAPStructL()
    {
    if (iEAPSettings != NULL)
        {
        if (iEAPSettings->iCertificatesPresent)
            {
            iEAPSettings->iCertificates.Reset();
            }
        iEAPSettings->iEnabledEncapsulatedEAPExpandedTypes.Close();
        iEAPSettings->iDisabledEncapsulatedEAPExpandedTypes.Close();
        iEAPSettings->iCipherSuites.Close();

        delete iEAPSettings;
        iEAPSettings = NULL;
        }
    }

void CNSmlWLanAdapter::DesToBufferL(CBufBase& aTo, const TDesC& aFrom)
    {
    aTo.InsertL(0, ConvertTo8LC(aFrom));
    CleanupStack::PopAndDestroy(); // ConvertTo8LC
    }

void CNSmlWLanAdapter::FillCipherSuiteBufferL(CBufBase& aBuffer)
    {
    if (iEAPSettings == NULL || !iEAPSettings->iCipherSuitesPresent)
        {
        return;
        }
    // 0    5    10   15   20   25   30   35
    _LIT8(KPlus, "+");
    aBuffer.InsertL(0, KSuitesDefault);
    TInt count = iEAPSettings->iCipherSuites.Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        switch (iEAPSettings->iCipherSuites[i])
            {
            case RSA_3DES_SHA:
                {
                aBuffer.Write(0, KPlus);
                break;
                }
            case DHE_RSA_3DES_SHA:
                {
                aBuffer.Write(5, KPlus);
                break;
                }
            case DHE_DSS_3DES_SHA:
                {
                aBuffer.Write(10, KPlus);
                break;
                }
            case RSA_AES_SHA:
                {
                aBuffer.Write(15, KPlus);
                break;
                }
            case DHE_RSA_AES_SHA:
                {
                aBuffer.Write(20, KPlus);
                break;
                }
            case DHE_DSS_AES_SHA:
                {
                aBuffer.Write(25, KPlus);
                break;
                }
            case RSA_RC4_MD5:
                {
                aBuffer.Write(30, KPlus);
                break;
                }
            case RSA_RC4_SHA:
                {
                aBuffer.Write(35, KPlus);
                break;
                }
            default:
                {
                break;
                }

            }
        }

    }

void CNSmlWLanAdapter::FillCipherSuitesL(const TDesC8& aObject,
        const TInt aLuid)
    {
    if (iEAPBuffer[aLuid] == NULL)
        {
        return;
        }
    iEAPBuffer[aLuid]->iEAPSettings->iCipherSuites.Reset();
    iEAPBuffer[aLuid]->iEAPSettings->iCipherSuitesPresent = EFalse; // init to EFalse

    TChar plus('+');

    TInt length = aObject.Length();
    for (TInt i = 0; i + 3 < length; i += 5)
        {
        if (aObject[i] == plus)
            {
            TLex8 lex(aObject.Mid(i + 1, 3));
            TUint suite;
            lex.Val(suite);
            iEAPBuffer[aLuid]->iEAPSettings->iCipherSuites.Append(suite);
            iEAPBuffer[aLuid]->iEAPSettings->iCipherSuitesPresent = ETrue;
            }
        }
    }

void CNSmlWLanAdapter::ConvertEAPStringToIds(TDes8& aEAPString,
        TEapExpandedType& aId)
    {
    if (aId.Compare(*EapExpandedTypeSim.GetType()) == 0)
        {
        aEAPString.Copy(KEapSimTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeAka.GetType()) == 0)
        {
        aEAPString.Copy(KEapAkaTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeTtls.GetType()) == 0)
        {
        aEAPString.Copy(KEapTlsTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypePeap.GetType()) == 0)
        {
        aEAPString.Copy(KEapPeapTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeTtls.GetType()) == 0)
        {
        aEAPString.Copy(KEapTtlsTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeLeap.GetType()) == 0)
        {
        aEAPString.Copy(KEapLeapTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeMsChapv2.GetType()) == 0)
        {
        aEAPString.Copy(KEapMschapv2TypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedPlainMsChapv2.GetType()) == 0)
        {
        aEAPString.Copy(KMschapv2TypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeGtc.GetType()) == 0)
        {
        aEAPString.Copy(KEapGtcTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeFast.GetType()) == 0)
        {
        aEAPString.Copy(KEapFastTypeId, KExpandedEAPIdLength);
        }
    else if (aId.Compare(*EapExpandedTypeTtlsPap.GetType()) == 0)
        {
        aEAPString.Copy(KTtlspapTypeId, KExpandedEAPIdLength);
        }
    else
        {
        aEAPString.Copy(KEapNoneId, KExpandedEAPIdLength);
        }

    }

void CNSmlWLanAdapter::GetExpandedType(const TInt aEapType,
        const TInt aTunnelingType, TEapExpandedType& aExpanedId,
        TEapExpandedType& aExpanedEncapsId)
    {

    // append it to the appropriate list ('+' enabled, '-' disabled)
    _LIT8(KPadding, "\xFE\0\0\0\0\0\0");
    _LIT8(KMsChapV2Padding, "\xFE\xFF\xFF\xFF\0\0\0");
    const TInt KPlainMsChapV2ImplUid = 99;

    TBuf8<KExpandedEapIdLength> cmpbuf;
    if (aEapType == KPlainMsChapV2ImplUid)
        {
        cmpbuf.Append(KMsChapV2Padding);
        }
    else
        {
        cmpbuf.Append(KPadding);
        }
    cmpbuf.Append(aEapType);
    aExpanedId = cmpbuf;

    TBuf8<KExpandedEapIdLength> encapscmpbuf;
    if (aTunnelingType == KPlainMsChapV2ImplUid)
        {
        encapscmpbuf.Append(KMsChapV2Padding);
        }
    else
        {
        encapscmpbuf.Append(KPadding);
        }
    encapscmpbuf.Append(aTunnelingType);
    aExpanedEncapsId = encapscmpbuf;

    }

// end of file
