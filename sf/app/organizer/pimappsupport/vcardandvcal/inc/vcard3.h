// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
#ifndef __VCARD3_H__
#define __VCARD3_H__
#include <vcard.h>

NONSHARABLE_CLASS(CParserVCard3) : public CParserVCard
/** A vCard 3.0 parser. 

Overrides CParserVCard::InternalizeL(). Internalizing of vCard 3.0
objects is not supported

@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CParserVCard3* NewL();
	~CParserVCard3();
	
public: //from CParserVCard
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
private:
	CParserVCard3();
	void ConstructL();
	void CreateParserPlugInL();

private:
	CVCard3ParserPlugIn* iPlugInImpl;
	};
#endif //__VCARD3_H__