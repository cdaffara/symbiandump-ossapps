// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// pbapvcardexporterimpl.h
// 
//

#ifndef PBAPVCARDEXPORTERUTIL_H
#define PBAPVCARDEXPORTERUTIL_H

#include <cntdb.h>
#include "pbaprequest.h"
#include <logwrap.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntconvertercallback.h"
#endif
#include <tz.h>

class RWriteStream;
class CPbapLogWrapper;
class CParserVCard;

class CPbapVCardExporterUtil : public CBase, public MConverterCallBack
/**
Converts contact and call history items to vCard 2.1 or vCard 3.0 objects
@internalComponent
@released
*/
	{
public:
	/**
	*/
	static CPbapVCardExporterUtil* NewL(CContactDatabase& aDatabase, CPbapLogWrapper* aLogWrapper);
	~CPbapVCardExporterUtil();
	/**
	*/
	void ExportContactL(TContactItemId aContactId, RWriteStream& aWriteStream,
						TVCardVersion aFormat, TUint64 aFilter);
	/**
	*/
	void ExportCallHistoryL(const CLogEvent& aEvent, RWriteStream& aWriteStream,
						TVCardVersion aFormat, TUint64 aFilter);
	void ExportEmptyVCardL(RWriteStream& aWriteStream, TVCardVersion aFormat);

	TContactItemId FindContactIdFromNumberL(const TDesC& aNumber);
public:
	/**
	PBAP clients are supposed to provide any information regarding the contact item represented by aContactId.
	This information should be in form of standard vCard property, all such properties should be appended to the array aPropertyList.
	*/
	void AddIntraContactPropertiesL(const TContactItemId& aContactId, CArrayPtr<CParserProperty>* aPropertyList);
	
private:
	CPbapVCardExporterUtil(CContactDatabase& aDatabase, CPbapLogWrapper* aLogWrapper);
	void ConstructL();

	TBool ContactExistsL(TContactItemId aContactId);
	CParserVCard* CreateVCardParserLC(TVCardVersion aFormat);
	CParserProperty* EmptyNamePropertyL();
	CParserProperty* CallDateTimePropertyL(const CLogEvent& aLogEvent, TVCardVersion aFormat);
	CParserProperty* DesPropertyL(const TDesC8& aToken, const TDesC& aDes);
	TPtrC8 ParameterValueFromEventL(const CLogEvent& aLogEvent);
	
private:
 	CContactDatabase& 	iDatabase;
	CPbapLogWrapper*	iLogWrapper;
	const CLogEvent*	iLogEvent;
	TContactItemId		iLogEventContactId;
	TVCardVersion 		iFormat;
	TUint64			iFilter;	
	RTz                 	iTzClient;
	TInt iMatchDigitCount;
	};

#endif // PBAPVCARDEXPORTERUTIL_H
