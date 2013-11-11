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
* Description: 
*		This class encapsulates a name and value pair
*
*/


// INCLUDE FILES

#include "CWPNameValue.h"

//  MEMBER FUNCTIONS

EXPORT_C CWPNameValue* CWPNameValue::NewL( HBufC* aName, HBufC* aValue)
    {
    CWPNameValue* self = new (ELeave) CWPNameValue(aName, aValue);
    return self;
    }

EXPORT_C CWPNameValue* CWPNameValue::NewL(const TDesC& aName, const TDesC& aValue)
    {
    CWPNameValue* self = CWPNameValue::NewLC( aName, aValue );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CWPNameValue* CWPNameValue::NewLC(const TDesC& aName, const TDesC& aValue)
    {
    CWPNameValue* self = new (ELeave) CWPNameValue;
    CleanupStack::PushL(self);
    self->ConstructL(aName, aValue);
    return self;
    }

CWPNameValue::~CWPNameValue()
    {
    delete iName;
    delete iValue;
    }

EXPORT_C const TDesC& CWPNameValue::Name() const
    {
    if (iName)
        {
        return *iName;
        }
    return KNullDesC;
    }

EXPORT_C const TDesC& CWPNameValue::Value() const
    {
    if (iValue)
        {
        return *iValue;
        }
    return KNullDesC;
    }

void CWPNameValue::ConstructL(const TDesC& aName, const TDesC& aValue)
    {
	aName.Length()==0 ? iName=KNullDesC().AllocL() : iName = aName.AllocL();
	aValue.Length()==0 ? iValue=KNullDesC().AllocL() : iValue = aValue.AllocL();
    }

CWPNameValue::CWPNameValue()
    {
    }

CWPNameValue::CWPNameValue( HBufC* aName, HBufC* aValue ) :
    iName(aName),
    iValue(aValue)
    {
    }

// end of file
