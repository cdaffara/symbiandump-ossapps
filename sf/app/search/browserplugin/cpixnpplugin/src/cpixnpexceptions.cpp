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
#include "CPixNPExceptions.h"
#include <Common.h>

#include <e32err.h>

const TDesC8* DescribeErrorCode( TInt aErrorCode )
	{
	switch ( aErrorCode ) 
		{
		// CPix error codes
		case KErrCannotOpenDatabase:
			return &KCPixNPCannotOpenDatabaseException(); 
		case KErrQueryParsingFailed:
			return &KCPixNPQueryParsingFailedException();
		case KErrDatabaseQueryFailed:
			return &KCPixNPDatabaseQueryFailedException();
		case KErrDocumentAccessFailed:
			return &KCPixNPDocumentAccessFailedException();

		// Indexing errors (should not be possible to occur)
		case KErrCannotAddDocument:
		case KErrCannotUpdateDocument:
		case KErrDatabaseFlushFailed:
		case KErrCannotCreateDocument:
		case KErrCannotCreateDocumentField:
		case KErrCannotCreateQueryParser:
		case KErrCannotCreateAnalyzer:
			return &KCPixNPInternalError();
			
		// System wide error codes
		case KErrNone:
			return NULL; 
		case KErrInUse:
			return &KCPixNPInUseException();
		case KErrServerTerminated:
			return &KCPixNPServerTerminated();
		case KErrServerBusy:
			return &KCPixNPServerBusy();
		case KErrCouldNotConnect:
			return &KCPixNPCouldNotConnect();

		// At least this is our fault
		case KErrArgument:
			return &KCPixNPInternalError();

		// Fallback	
		default: 
			return &KCPixNPUnknownException(); 

		}
	}
