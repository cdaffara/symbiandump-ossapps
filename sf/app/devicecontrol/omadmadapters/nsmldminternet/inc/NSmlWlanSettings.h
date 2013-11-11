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
 * Description:   Wlan Adapter commsDB handler
 *
 */

#ifndef __NSMLWLANSETTINGS_H__
#define __NSMLWLANSETTINGS_H__

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_partner.h>
#endif
#include <commdb.h>
#include <commsdattypesv1_1.h>
#include <EapType.h>

const TInt KMaxPSKLength = 63;
const TInt KMinPSKLength = 8;

// ==========================================================================
// Class forwards
// ==========================================================================
//
class CWlanSettings;

class TWlanSettings
    {
public:
    TBuf<KMaxTextLength> Name;
    TUint32 Id;
    TUint32 ServiceID;
    TUint32 ConnectionMode; // Infrastructure / adhoc
    TBuf<KMaxTextLength> SSID;
    TBuf8<KMaxTextLength> WepKey1;
    TBuf8<KMaxTextLength> WepKey2;
    TBuf8<KMaxTextLength> WepKey3;
    TBuf8<KMaxTextLength> WepKey4;
    TUint32 WepIndex;
    TUint32 SecurityMode; // SecurityMode (Allow unsecure, wep, 802.1xs, wpa) enum EWlanSecurityMode
    TUint32 WPAMode; // enum EWpaMode
    TUint32 UseWPAPSK; // CR ID: TMVI-5Y49AS
    TBuf8<KMaxPSKLength> WPAPreSharedKey;
    TUint32 AuthMode;
    TBuf<KMaxTextLength> UsedSSID;
    TUint32 ScanSSID;
    TBuf<KMaxLongTextLength> EapList;
    TBool SecondarySSIDsExisting;
    //    TBuf<KMaxTextLength>        SecondarySSID;
    //    TBuf<KMaxTextLength>        UsedSecondarySSID;

    };

class TSecondarySSID
    {
public:
    TUint32 Id;
    TBuf<KMaxTextLength> ScannedId;
    TBuf<KMaxTextLength> UsedId;
    };

// -----------------------------------------------------------------------------------------------
// Defines
// -----------------------------------------------------------------------------------------------

class CWlanSettings : public CBase
    {
public:
    static CWlanSettings* NewL(CCommsDatabase& aDatabase);
    void ConstructL();
    ~CWlanSettings();

    TInt GetWlanSettings(TUint32 aLuid, TWlanSettings& aWlanSettings);
    TInt DeleteWlanSettings(TUint32 aLuid);
    TInt WriteWlanSettings(TWlanSettings& aWlanSettings);
    void ConnectToDatabaseL();
    TInt RecordExists(TUint32 aLuid);
    TInt PerformLockWLANTablesL(TBool aProtect);
    TBool CheckEnforcementL();
    TBool CheckAPEnforcementL();
    TBool iWLANRelock;

    TInt GetEAPSettings(const TInt aId, TEapExpandedType& aExpandedId,
            TEapExpandedType& aEncapsId, EAPSettings& aEapSettings);
    TInt DeleteEAPSettings(TInt aId);
    void DeleteWlanEapSettingsL(TInt aWlanId);
    TInt DeleteSecondarySSIDsL(TUint32 aWLANId);
    TInt DeleteOneSecondarySSIDL(TUint32 aWLANId, TUint32 asecId);
    TInt WriteEAPSettings(EAPSettings& aEapSettings);
    TInt WriteSecondarySSIDL(TUint32 aWlanID, TSecondarySSID& aSettings,
            TBool aNew);
    void GetSecondarySSIDListL(TUint32 aLuid,
            RArray<TSecondarySSID>& aSecondarySSIDs);
    void GetEAPInterfaceL(const TInt aId, TEapExpandedType& aEncapsId,
            TEapExpandedType& aExpandedId);
    TInt InstalledEAPsL(CBufBase& aEAPList);

private:

    CWlanSettings(CCommsDatabase& aDatabase);
    TInt GoToRecord(TUint32 aId);
    void GetDataFromRecordL(TWlanSettings* aWlanSettings);
    void WriteDataToRecordL(TWlanSettings* aWlanSettings);
    void InitialiseRecordL();
    void SetSecondaryViewToRecordL(TUint32 aId);

    TDesC8& ConvertTo8LC(const TDesC& aSource);
    TDesC16& ConvertTo16LC(const TDesC8& aSource);
    TBool IsDisallowedInsideTTLS(const CImplementationInformation& aImplInfo);
    TBool IsDisallowedInsidePEAP(const CImplementationInformation& aImplInfo);
    TBool
            IsDisallowedOutsidePEAP(
                    const CImplementationInformation& aImplInfo);

    CCommsDatabase& iDatabase;
    CCommsDbTableView* iTableView;
    CCommsDbTableView* iSecondaryView;
    CEapType* iEapType;
    TUint32 iServiceID;
    TBool iExpandedEAPTypeFieldsUsed;
    };

#endif __NSMLWLANSETTINGS_H__
