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
#include "CNPSearchDocument.h"

#include "CNPDocumentFieldInterface.h"
#include "CNPDocumentField.h"
#include <CSearchDocument.h>

CNPSearchDocument *CNPSearchDocument::NewL( CSearchDocument* aDocument )
	{
	CNPSearchDocument* self = new (ELeave)CNPSearchDocument( aDocument );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
	}

CNPSearchDocument::~CNPSearchDocument()
	{
	delete iDocument; 
	}
    
CNPSearchDocument::CNPSearchDocument( CSearchDocument* aDocument )
:	iDocument( aDocument )
	{
	}

void CNPSearchDocument::Deallocate() 
	{
	// NP Environment no more refers us directly. Still, there might
	// references to fields left
	Release();  
	}

void CNPSearchDocument::ConstructL()
	{
	InitInterfaceL();
	Retain(); // Add one reference for the references held by NP enviroment
	}

void CNPSearchDocument::Retain()
	{
	iReferences++; 
	}

void CNPSearchDocument::Release()
	{
	if ( --iReferences == 0 ) 
		{
		delete this; 
		}
	}

NPDocumentFieldObject* CNPSearchDocument::FieldL(const TDesC& aName)
	{
	const CDocumentField* field = iDocument->Field( aName );
	
	if ( field ) 
		{
	    NPDocumentFieldObject *object = 
	    	( NPDocumentFieldObject * ) NPN_CreateObject( iInstanceHandle, NPDocumentFieldClass );
	    object->plugin = CNPDocumentField::NewL( *this, *field );
	    return object; 
		}
    
	return NULL;
	}

const TDesC& CNPSearchDocument::Id() const
	{
	return iDocument->Id(); 
	}
	
const TDesC& CNPSearchDocument::AppClass() const
	{
	return iDocument->AppClass(); 
	}

const TDesC& CNPSearchDocument::Excerpt() const
	{
	return iDocument->Excerpt(); 
	}

TInt CNPSearchDocument::FieldCount() const
	{
	return iDocument->FieldCount(); 
	}

NPDocumentFieldObject* CNPSearchDocument::FieldL( const TInt aIndex )
	{
	if ( aIndex >= 0 && aIndex < iDocument->FieldCount() ) 
		{
		const CDocumentField& field = iDocument->Field( aIndex );
		
	    NPDocumentFieldObject *object = 
	    	( NPDocumentFieldObject * ) NPN_CreateObject( iInstanceHandle, NPDocumentFieldClass );
	    object->plugin = CNPDocumentField::NewL( *this, field );
	    return object; 
		}
	return NULL; 
	}
