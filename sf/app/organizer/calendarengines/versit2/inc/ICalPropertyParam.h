#ifndef ICALPROPERTYPARAM_H
#define ICALPROPERTYPARAM_H/*
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
* Description:   Holds the definition of CICalPropertyParam.
*
*/



// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class CICalValue;
class CICalContentLineWriter;

/**
Class representing a property parameter.
@publishedPartner
*/
class CICalPropertyParam : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CICalPropertyParam* NewL(const TDesC& aType, CICalValue* aValue);
	IMPORT_C static CICalPropertyParam* NewLC(const TDesC& aType, CICalValue* aValue);
	
	IMPORT_C static CICalPropertyParam* NewL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C static CICalPropertyParam* NewLC(const TDesC& aType, const TDesC& aValue);
	
	~CICalPropertyParam();
	
	// Non-exported constructor.	
	static CICalPropertyParam* NewLC();

public:	// Methods
	IMPORT_C void AddValueL(const TDesC& aValue);
	IMPORT_C void AddValueL(CICalValue* aValue);
	IMPORT_C const TDesC& Type() const;
	IMPORT_C const RPointerArray<CICalValue>& Values() const;

	void SetTypeL(const TDesC& aType);
	void ExternalizeL(CICalContentLineWriter& aWriter);
	static TBool EqualsL(const CICalPropertyParam& aParam1, const CICalPropertyParam& aParam2);
	
private:	// Construction.
	CICalPropertyParam();
	CICalPropertyParam(CICalValue* aValue);
	void ConstructL(const TDesC& aType);
	void ConstructL(const TDesC& aType, const TDesC& aValue);
	void ConstructL(const TDesC& aType, CICalValue* aValue);
	
private:	// Attributes.
	HBufC* iType;
	CICalValue* iValue;
	RPointerArray<CICalValue> iValues;
	};
	
#endif	// ICALPROPERTYPARAM_H

// End of File
