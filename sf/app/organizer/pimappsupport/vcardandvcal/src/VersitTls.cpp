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

#include <versittls.h>

// User includes
#include <vutil.h>
#include "VersitAdditionalStorage.h"

/*CVersitTlsData*/

/*
 * Note: If you are going to create a parser and delete it again soon afterwards and repeat this many times it would be advantagous
 * to get your own handle to the Versit TLS data as this will stop the TLS data being repeatedly created and destroyed.
 *
 * Returns a reference to the TLS and increments the count of accesser 
 */
EXPORT_C CVersitTlsData& CVersitTlsData::VersitTlsDataL()
/** Returns a reference to the current Thread Local Storage (TLS) object or, if 
there isn't one, it creates one.

If a new TLS object is being created, the object also creates (and owns) a 
Unicode utilities object, and creates a Unicode converter using this object.

If creating a new object, it calls the CVersitUnicodeUtils constructor followed 
by CVersitUnicodeUtils::CreateConverterL().

@return The current Thread Local Storage object. */
	{
	CVersitTlsData* self = static_cast<CVersitTlsData*>(Dll::Tls());
	if	(!self)
		self = NewL();
	++self->iRefCount;
	return *self;
	}

/*
 * Close the Thread Local Storage
 *
 * Deletes the data from TLS if the number of user is none.
 *
 * Note: Only use this static version if you don't have a handle to a <code>CVersitTlsData<\code> class.
 */
EXPORT_C void CVersitTlsData::CloseVersitTlsData()
	{
	CVersitTlsData* self = static_cast<CVersitTlsData*>(Dll::Tls());
	if (self)
		self->VersitTlsDataClose();
	}

EXPORT_C void CVersitTlsData::VersitTlsDataClose()
/** Frees all resources owned by the object if there are no parsers left (i.e. 
created and not destroyed), prior to its destruction. */
	{
	if (--iRefCount<=0)
		{
		delete this;
		}
	}

CVersitTlsData* CVersitTlsData::NewL()
	{
	CVersitTlsData* self = new(ELeave) CVersitTlsData();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	Dll::SetTls(self);
	self->iRefCount=0;		//Defensive code
	return self;
	}


CVersitTlsData::~CVersitTlsData()
	{
	Dll::SetTls(NULL);
	delete iUnicodeUtils;
    delete iAdditionalStorage;
	}


void CVersitTlsData::ConstructL()
	{
	iUnicodeUtils = new(ELeave) CVersitUnicodeUtils();
	iUnicodeUtils->CreateConverterL();
    iAdditionalStorage = CVersitAdditionalStorage::NewL();
	}
