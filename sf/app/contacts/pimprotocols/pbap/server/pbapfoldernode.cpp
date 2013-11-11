// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pbapfolderclient.h"
#include "pbapappheader.h"
#include "pbapfoldernode.h"
#include "pbapexporter.h"
#include "pbaperrorreporter.h"
#include "pbapserver.h"

#include "btaccesshostlog.h"



CFolderNode::CFolderNode(MVirtualFolderClient& aClient, const TDesC& aFolderName)
: CFolderBase(aClient, aFolderName)
	{
	LOG_FUNC
	}
	
	
CFolderNode::~CFolderNode()
	{
	LOG_FUNC
	delete iAppHeader;
	}


/**
 Implements base class Get function, calls inteface to handle each type of
 supported get request. The concrete phonebook folder subclasses classes 
 implement this interface.
 
 Takes ownership of aGetData.
*/	
TInt CFolderNode::Get(const TDesC& aObjectName, CPbapAppHeader* aGetData)
	{
	LOG_FUNC
	if(!aGetData)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderNullAppHeader));
		return KErrBadHandle;
		}
	if(iAppHeader)
		{
		__ASSERT_DEBUG(EFalse, Panic(EVirtualFolderAppHeaderExists));
		return KErrAlreadyExists;
		}
	
	
	// take ownership of the app header to allow get requests handlers to be asynchronous
	iAppHeader = aGetData;

	TInt error = KErrNone;
	
	switch (iAppHeader->Operation())
		{
		case EPullVCard:
			{
			// convert hexadecimal string representation of handle to integer
			TLex lex(aObjectName);
			TUint handle;
			error = lex.Val(handle, EHex);
			if (error == KErrNone)
				{
				error = DoGetItem(handle);
				}	
			break;
			}
		case EPullPhoneBook:
			{
			error = DoGetFolder();
			break;
			}
		case EPullVCardListing:
			{
			// if trying to pull a listing from non-existing folder, return KErrNotFound
			if(aObjectName.Size() != NULL)
				{
				error = KErrNotFound;
				}
			else
				{
				error = DoGetListing();
				}
			break;
			}
		case ERequestPhoneBookSize:
			{
			error = DoGetCount();
			break;
			}
		default:
			{
			error = KErrNotSupported;
			break;
			}
		}		

	if (error != KErrNone)
		{
		// if an error occured then delete the app header as export is not in progress
		delete iAppHeader;
		iAppHeader = NULL;		
		}
		
	return error;
	}
