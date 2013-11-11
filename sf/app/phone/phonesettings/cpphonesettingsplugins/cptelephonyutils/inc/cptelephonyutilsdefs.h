/*
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
 */

#ifndef CPTELEPHONYUTILSDEFS_H_
#define CPTELEPHONYUTILSDEFS_H_


#ifdef BUILD_TELEPHONYUTILS
#define TELEPHONYUTILS_EXPORT Q_DECL_EXPORT
#else
#define TELEPHONYUTILS_EXPORT Q_DECL_IMPORT
#endif

namespace CpTelephonyUtils {

/*! Type of a call forwarding request. */
enum CallDivertType
    {
    /*! Not a call forwarding request.                      */
    NoCallDivert                    = 1,
    /*! Call forwarding with number information.            */
    CallDivertWithNumber,
    /*! Call forwarding with number and timeout information.*/
    CallDivertWithNumberAndTimeout,
    };

enum BasicServiceGroups
{
    Unknown = -1, //this is not part of GSM standard
    // Phone settings updates this automatically to ETelephony or to EAltTele,
    // depending on if ALS is supported, and which line is active.
    AllTeleAndBearer = 0,
    AllTele = 10,
    Telephony = 11,
    AllDataTele = 12,
    Fax = 13,
    Sms = 16,
    AllDataExSms = 18, //voice broadcast in gsm 02.30 v. 7.1.0
    AllTeleExcSms = 19,

    AllPlmnTele = 50,
    PlmnTele1 = 51,
    PlmnTele2 = 52,
    PlmnTele3 = 53,
    PlmnTele4 = 54,
    PlmnTele5 = 55,
    PlmnTele6 = 56,
    PlmnTele7 = 57,
    PlmnTele8 = 58,
    PlmnTele9 = 59,
    PlmnTeleA = 60,
    PlmnTeleB = 61,
    PlmnTeleC = 62,
    PlmnTeleD = 63,
    PlmnTeleE = 64,
    PlmnTeleF = 65,

    AllBearer = 20,
    AllAsync = 21,
    AllSync = 22,
    SyncData = 24,
    AsyncData = 25,
    PacketData = 26,
    PadAccess = 27,
    AllPlmnBearer = 70,
    PlmnBearerServ1 = 71,
    PlmnBearerServ2 = 72,
    PlmnBearerServ3 = 73,
    PlmnBearerServ4 = 74,
    PlmnBearerServ5 = 75,
    PlmnBearerServ6 = 76,
    PlmnBearerServ7 = 77,
    PlmnBearerServ8 = 78,
    PlmnBearerServ9 = 79,
    PlmnBearerServA = 80,
    PlmnBearerServB = 81,
    PlmnBearerServC = 82,
    PlmnBearerServD = 83,
    PlmnBearerServE = 84,
    PlmnBearerServF = 85,

    AltTele = 89
// no ui note support
};

} // namespace 

#endif /* CPTELEPHONYUTILSDEFS_H_ */
