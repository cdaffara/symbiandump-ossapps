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
#ifndef CPIXNPEXCEPTIONS_H_
#define CPIXNPEXCEPTIONS_H_

#include <e32std.h>

/*
 * CPix NP Plugin provides its own exceptions, that are 
 * based on plain text codes. The reason for using strings to
 * represent exceptions is simply, because NPN_SetException 
 * accepts only strings as exception codes. Also strings are expected
 * to be more informative and convinient for JavaScript programmers.
 * 
 * CPixNPException is used to declare CPix NP Plugin exceptions and 
 * for convert both CPix error codes and S60 system wide error codes
 * into CPix NP exception format. 
 */

_LIT8( KCPixNPCannotOpenDatabaseException, "CannotOpenDatabaseException" );
_LIT8( KCPixNPQueryParsingFailedException, "QueryParsingFailedException" );
_LIT8( KCPixNPDatabaseQueryFailedException, "DatabaseQueryFailedException" );
_LIT8( KCPixNPDocumentAccessFailedException, "DocumentAccessFailedException" );

_LIT8( KCPixNPIllegalArgumentException, "IllegalArgumentException" );
_LIT8( KCPixNPNotEnoughArgumentsException, "NotEnoughArgumentsException" );
_LIT8( KCPixNPInternalError, "InternalError" );

// Translations from S60 System wide error codes
_LIT8( KCPixNPInUseException, "InUseException" );
_LIT8( KCPixNPServerTerminated, "ServerTerminatedException" );
_LIT8( KCPixNPServerBusy, "ServerBusyException" );
_LIT8( KCPixNPCouldNotConnect, "CouldNotConnectException" );

_LIT8( KCPixNPUnknownException, "UnknownException" );

/**
 * Describes the given error code. Returns NULL if given code
 * is KErrNone. 
 * 
 * @return CPix NPlugin error code string or NULL for KErrNone. Does not return ownership. 
 */
const TDesC8* DescribeErrorCode( TInt aErrorCode ); 

#endif /*CPIXNPEXCEPTIONS_H_*/
