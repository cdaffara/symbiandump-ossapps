/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:     Definitions to WPAPAdapter
*
*/





#ifndef WPAPDEFS_H
#define WPAPDEFS_H

#include <e32base.h>

// CONSTANTS

const TUint32 KMaxWEPKeyLength = 29;
// The maximum WPA Pre-Shared Key length.
const TUint KMaxWpaPskLength = 63;
// same as connection name max length in Prov req.
const TInt KNameMaxLength = 30;
const TInt KTwoNameServersPerNetworkType = 2;
// The maximum length of key data
const TUint KMaxLengthOfKeyData = 58;
const TUint KMaxLengthOfEapList = 50;
const TUint KExpandedEapIdLength = 8;
// Number of keys
const TUint KMaxNumberofKeys = 4;

const TInt KInitialArraySize = 3;

const TUint KMaxSubKeyLenght = 255;

_LIT(KIPV4, "IPV4");
_LIT(KIPV6, "IPV6");
_LIT(KCLWSP, "CL-WSP");
_LIT(KCOWSP, "CO-WSP");
_LIT(KCLSECWSP, "CL-SEC-WSP");
_LIT(KCOSECWSP, "CO-SEC-WSP");
_LIT(K9200, "9200");
_LIT(K9201, "9201");
_LIT(K9202, "9202");
_LIT(K9203, "9203");
_LIT(K80, "80");
_LIT(K8080, "8080");
_LIT(K443, "443");

_LIT(KGSMGPRS, "GSM-GPRS");

//Characteristic WLAN
_LIT(KWLAN, "WLAN");
_LIT(PRISSID, "PRI-SSID");
_LIT(PRIUSSID, "PRI-U-SSID");
_LIT(PRIHSSID, "PRI-H-SSID");

//Characteristic WLAN/SEC-SSID
_LIT(SECSSID, "SEC-SSID");
_LIT(SSSID, "S-SSID");
_LIT(SUSSID, "S-U-SSID");

_LIT(NETMODE, "NETMODE");
_LIT(SECMODE, "SECMODE");

//Characteristic WLAN/EAP
_LIT(KEAP, "EAP");
_LIT(EAPTYPE, "EAPTYPE");
_LIT(EAPUSERNAME, "USERNAME");
_LIT(EAPPASSWORD, "PASSWORD");
_LIT(EAPREALM, "REALM");
_LIT(EAPUSEPSEUD, "USE-PSEUD");
_LIT(EAPMAXAUTHS, "MAXAUTHS");
_LIT(EAPENCAPS, "ENCAPS");
_LIT(EAPVERSERREALM, "VER-SER-REALM");
_LIT(EAPCLIENTHAUTH, "CLIENTH-AUTH");
_LIT(EAPSESVALTIME, "SES-VAL-TIME");
_LIT(EAPCIPSUIT, "CIP-SUIT");
_LIT(EAPPEAPV0, "PEAP-V0");
_LIT(EAPPEAPV1, "PEAP-V1");
_LIT(EAPPEAPV2, "PEAP-V2");

//Characteristic WLAN/EAP/CERT
_LIT(KCERT, "CERT");
_LIT(EAPISSNAME, "ISS-NAME");
_LIT(EAPSUBNAME, "SUB-NAME");
_LIT(EAPCERTTYPE, "CERT-TYPE");
_LIT(EAPSERNUM, "SER-NUM");
_LIT(EAPSUBKEYID, "SUB-KEY-ID");
_LIT(EAPTHUMBPRINT, "THUMBPRINT");

_LIT(EAPCERTUSER, "USER");
_LIT(EAPCERTCA, "CA");

_LIT(WPAPRESKEYASC, "WPA-PRES-KEY-ASC");
_LIT(WPAPRESKEYHEX, "WPA-PRES-KEY-HEX");
_LIT(DEFAULTWEPIND, "WEPKEYIND");
_LIT(WEPAUTHMODE, "WEPAUTHMODE");

//Characteristic WLAN/WEPKEY
_LIT(KWEP, "WEPKEY");
_LIT(WEPKEYLENGTH, "LENGTH");
_LIT(WEPKEYINDEX, "INDEX");
_LIT(WEPKEYDATA, "DATA");

_LIT(KPAP, "PAP");
_LIT(KCHAP, "CHAP");
_LIT(KMD5, "MD5");
_LIT(KANALOGMODEM, "ANALOG-MODEM");
_LIT(KV110, "V.110");
_LIT(KV120, "V.120");
_LIT(KAUTOBAUDING, "AUTOBAUDING");
_LIT(K9600, "9600");
_LIT(K14400, "14400");
_LIT(K19200, "19200");
_LIT(K28800, "28800");
_LIT(K38400, "38400");
_LIT(K43200, "43200");
_LIT(KIPv6, "IPv6");
_LIT(KIPv4, "IPv4");
_LIT(KAddrTypeE164, "E164");
_LIT(KAddrTypeAPN, "APN");

_LIT(KADHOC, "ADHOC");

_LIT(KWEPSEC, "WEP");
_LIT(KWPASEC, "WPA");
_LIT(KWPA2SEC, "WPA2");
_LIT(K8021XSEC, "8021X");
_LIT(KWPAPRESSEC, "WPA-PRESHARED-KEY");
_LIT(KWPA2PRESSEC, "WPA2-PRESHARED-KEY");


_LIT(KEAPSIM, "EAP-SIM");
_LIT(KEAPAKA, "EAP-AKA");
_LIT(KEAPTLS, "EAP-TLS");
_LIT(KEAPPEAP, "EAP-PEAP");
_LIT(KEAPTTLS, "EAP-TTLS");
_LIT(KEAPLEAP, "EAP-LEAP");
_LIT(KEAPMSCHAPV2, "EAP-MSCHAPV2");
_LIT(KEAPGTC, "EAP-GTC");
_LIT(KEAPFAST, "EAP-FAST");

/*
_LIT8(KEapNone,"0");
_LIT8(KEapGtc,"6");
_LIT8(KEapTls, "13"); 
_LIT8(KEapLeap,"17");
_LIT8(KEapSim,"18");
_LIT8(KEapTtls,"21");
_LIT8(KEapAka,"23");
_LIT8(KEapPeap,"25");
_LIT8(KEapMschapv2,"26");
_LIT8(KEapSecurid,"32");
_LIT8(KEapFast,"43");
*/
_LIT8(KEapAll,"-017,-018,-023,-013,-025,-021,-043" );

const TUint8 KEapNoneId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const TUint8 KEapTlsTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d};
const TUint8 KEapGtcTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06};
const TUint8 KEapLeapTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
const TUint8 KEapSimTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12};
const TUint8 KEapTtlsTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15};
const TUint8 KEapAkaTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17};
const TUint8 KEapPeapTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19};
const TUint8 KEapMschapv2TypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A};
const TUint8 KEapSecuridTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20};
const TUint8 KEapFastTypeId[] = {0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2B};


const TUint8 KEapPlus = '+'; 

_LIT(KWEPAUTHMODEOPEN, "OPEN");
_LIT(KWEPAUTHMODESHARED, "SHARED");

enum TWEPAuthentication
    {
    EAuthOpen,              // Open authentication
    EAuthShared             // Shared authentication
    };
    
// DATA TYPES

/**
* Data structure for storing a WEP key.
*/
struct TWep
    {
    /** Specifies which key to add or remove. Range: 0-3 */
    TUint32 KeyIndex;
    /** The length of KeyMaterial in bytes. Range: 0-29 */
    TUint32 KeyLength;
    /** Array that stores the WEP key. */
    TUint8 KeyMaterial[KMaxWEPKeyLength];
    };

/**
* Data structure for storing a WPA Pre-Shared Key.
*/
struct TWpaPsk
	{
	/** The length of KeyMaterial in bytes. Range: 0-63 */
	TUint KeyLength;
	/** Array of that stores the WPA Pre-Shared Key. */
	TUint8 KeyMaterial[KMaxWpaPskLength];
	};



#endif // WPAPDEFS_H

// End of File.
