// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// timrfc822datefield.h
//

/**
 * @file 
 * @internal
 * @released
 */

#ifndef __TIMRFC822DATEFIELD_H__
#define __TIMRFC822DATEFIELD_H__

#include <miutstd.hrh>			// KUidMsvEmailTextEntryValue

/** 
 * Identifies which version of CImHeader has been stored in stream
 * @internalComponent
 * @released
 */
const TUint16 KImHeaderVersion = 2;

/** 
 * @internalTechnology
 * @released
 */
const TUid KUidMsvEmailTextEntry			= {KUidMsvEmailTextEntryValue};
const TUid KUidMsvEmailHtmlEntry			= {KUidMsvEmailHtmlEntryValue};

/** 
 * @internalComponent
 * @released
 */
const TUid KUidMsvEmailExternalBodyEntry	= {KUidMsvEmailExternalBodyEntryValue};
const TUid KUidMsvEmailRtfEntry				= {KUidMsvEmailRtfEntryValue};

/** 
 * @internalComponent
 * @released
 */
_LIT8(KMiutDateFormat, " %S, %2d %S %04d %02d:%02d:%02d ");
_LIT8(KMiutMonthNames, "JanFebMarAprMayJunJulAugSepOctNovDec");
_LIT8(KMiutDayNames, "MonTueWedThuFriSatSun");
_LIT8(KMiutTimeZoneNeg, "-%02d%02d");
_LIT8(KMiutTimeZonePos, "+%02d%02d");
_LIT8(KMiutBase64String, "Base64");
_LIT8(KMiutQPString, "quoted-printable");
_LIT8(KMiut7BitString, "7bit");
_LIT8(KMiut8BitString, "8bit");
_LIT8(KMiutBinaryString, "binary");
_LIT8(KMiutUUString, "uuencode");
_LIT8(KMiutWildcardBase64, "*base64*");
_LIT8(KMiutWildcardQP, "*quoted-printable*");
_LIT8(KMiutWildcard7Bit, "*7bit*");
_LIT8(KMiutWildcard8Bit, "*8bit*");
_LIT8(KMiutWildcardBinary, "*binary*");
_LIT8(KMiutWildcardUU, "*uu*");
_LIT(KMiutFormatString, "%S");

/** 
 * @internalTechnology
 * @released
 */
_LIT(KImEngineResourceFile, "\\resource\\messaging\\imcm.rsc");

/** 
 * @internalComponent
 * @released
 */
const TInt KMiutDateStringLength	= 32;
const TInt KCenturyThreshold		= 100;
const TInt KCenturyOffset			= 70;
const TInt KThisCentury				= 1900;	// as defined in 1998
const TInt KNextCentury				= 2000;

/** 
 * @internalComponent
 * @released
 */
const TInt KMaxImMimeFieldLength	= 1000;
const TInt KImMailMaxBufferSize		= 1000;	// 1000 is defined by the SMTP spec as max space
const TInt KSmtpMaxBufferExcludingCRLF = 979; // Line length limit is 1000 chars per line including CRLF (RFC2822, Section 2.1.1)
                                              // 1000 chars including "Field name: "+"Field body"+CRLF (here "Resent-Message-ID: " is largest field)
/** 
 * Maximum size for descriptors stored in HBufC members of CImHeader = 1000 chars
 * @internalTechnology
 * @released
 */
const TInt KMaxImHeaderStringLength = 1000;

/** 
 * @internalComponent
 * @released
 */
void CopyArrayL(const CDesCArray& aSource, CDesCArray& aDestination);

class TImRfc822DateField
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C TInt ParseDateField(const TDesC8& aRfc822DateField, TTime& rTime);
	IMPORT_C void SetDate(const TTime& aTimeDate, TDes8& rOutputLine);	// NB assumes that "Date: " string has already been inserted into  rOutputLine

private:
	TBool GetMonth( const TDesC8& name, TMonth& month );
	TBool GetTimezone( const TDesC8& name, TInt& minsOffset );
	};

#endif //__TIMRFC822DATEFIELD_H__
