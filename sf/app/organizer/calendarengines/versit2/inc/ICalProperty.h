#ifndef ICALPROPERTY_H
#define ICALPROPERTY_H/*
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
* Description:   Holds the definition of CICalProperty.
*
*/



// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class CICalPropertyParam;
class CICalValue;
class CICalContentLineWriter;

/**
Class representing a property.
@publishedPartner
*/
class CICalProperty : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CICalProperty* NewL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C static CICalProperty* NewLC(const TDesC& aType, const TDesC& aValue);
	
	IMPORT_C static CICalProperty* NewL(const TDesC& aType, CICalValue* aValue);
	IMPORT_C static CICalProperty* NewLC(const TDesC& aType, CICalValue* aValue);
	
	~CICalProperty();
	
	// Non-exported constructors.
	static CICalProperty* NewL(const TDesC& aType);
	static CICalProperty* NewLC(const TDesC& aType);

public:	// Methods.
	IMPORT_C const TDesC& Type() const;
	IMPORT_C CICalPropertyParam& AddPropertyParamL(const TDesC& aType, CICalValue* aValue);
	IMPORT_C CICalPropertyParam& AddPropertyParamL(const TDesC& aType, const TDesC& aValue);
	IMPORT_C void AddValueL(const TDesC& aValue);
	IMPORT_C void AddValueL(CICalValue* aValue);
	IMPORT_C const RPointerArray<CICalPropertyParam>& Parameters() const;
	IMPORT_C const RPointerArray<CICalValue>& Values() const;
	IMPORT_C const CICalPropertyParam* FindParam(const TDesC& aType) const;
	
	CICalPropertyParam& CICalProperty::AddPropertyParamL();
	void RemovePropertyParamL(const CICalPropertyParam& aParam);
	void ExternalizeL(CICalContentLineWriter& aWriter);
	
private:	// Construction.
	CICalProperty();
	CICalProperty(CICalValue* aValue);
	void ConstructL(const TDesC& aType);
	void ConstructL(const TDesC& aType, const TDesC& aValue);
	void ConstructL(const TDesC& aType, CICalValue* aValue);
	
private:	// Attributes.
	HBufC* iType;
	CICalValue* iValue;
	RPointerArray<CICalPropertyParam> iParams;
	RPointerArray<CICalValue> iValues;
	};
	
#endif	// ICALPROPERTY_H

// End of File
