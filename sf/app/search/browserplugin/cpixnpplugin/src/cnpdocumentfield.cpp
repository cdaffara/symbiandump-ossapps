/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CNPDocumentField.h"
#include "CNPSearchDocument.h"
#include "CDocumentField.h"

CNPDocumentField *CNPDocumentField::NewL( CNPSearchDocument& aOwner, const CDocumentField& aField )
	{
	CNPDocumentField* self = new (ELeave)CNPDocumentField( aOwner, aField );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
	}

CNPDocumentField::~CNPDocumentField()
	{
	iOwner.Release(); // After this moment, the owner may have become released
	}
	
CNPDocumentField::CNPDocumentField( CNPSearchDocument& aOwner, const CDocumentField& aField  )
: 	iOwner( aOwner ), 
	iField( aField )
	{
	iOwner.Retain(); 
	}

void CNPDocumentField::ConstructL()
	{
	InitInterfaceL();
	}

const TDesC& CNPDocumentField::Name() const
	{
	return iField.Name(); 
	}

const TDesC& CNPDocumentField::Value() const
	{
	return iField.Value(); 
	}
