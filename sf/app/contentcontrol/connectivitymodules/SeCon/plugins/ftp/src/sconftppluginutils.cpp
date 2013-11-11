/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  File Transfer Profile Plug-in utilities
*
*/


// INCLUDE FILES
#include <obexconstants.h>
#include "sconftppluginutils.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TFTPpluginUtils::ConvertFTPResponseCode( TInt aError )
// Converts Symbian error code to OBEX error code
// -----------------------------------------------------------------------------
//
TInt TFTPpluginUtils::ConvertFTPResponseCode( TInt aError )
	{
	LOGGER_WRITE_1( "TFTPpluginUtils::ConvertFTPResponseCode : begin Code  %d", aError );
	TInt obexErr ( KErrIrObexRespBadRequest );
	switch( aError )
		{
		case KErrNone:
			obexErr = KErrNone;
			break;
		case KErrNotFound:
			obexErr = KErrIrObexRespNotFound;
			break;
		case KErrGeneral:
		case KErrCancel:
			obexErr = KErrIrObexRespBadRequest;
			break;
		case KErrNoMemory:
			obexErr = KErrIrObexRespDatabaseFull;
			break;
		case KErrNotSupported:
			obexErr = KErrIrObexRespNotImplemented;
			break;
		case KErrArgument:
		case KErrTotalLossOfPrecision:
		case KErrBadHandle:
		case KErrOverflow:
		case KErrUnderflow:
		case KErrAlreadyExists:
			obexErr = KErrIrObexRespConflict;
			break;
		case KErrPathNotFound:
		case KErrDied:
			obexErr = KErrIrObexRespNotFound;
			break;
		case KErrInUse:
			obexErr = KErrIrObexRespTimedOut;
			break;
		case KErrServerTerminated:
			obexErr = KErrIrObexRespNotFound;
			break;
		case KErrServerBusy:
		case KErrCompletion:
		case KErrNotReady:
		case KErrUnknown:
		case KErrCorrupt:
			obexErr = KErrIrObexRespForbidden;
			break;
		case KErrAccessDenied:
		case KErrLocked:
			obexErr = KErrIrObexRespUnauthorized;
			break;
		case KErrWrite:
		case KErrDisMounted:
		case KErrEof:
			obexErr = KErrIrObexRespForbidden;
			break;
		case KErrDiskFull:
			obexErr = KErrIrObexRespDatabaseFull;
			break;
		case KErrBadDriver:
		case KErrBadName:
			obexErr = KErrIrObexRespPreCondFailed;
			break;
		case KErrCommsLineFail:
		case KErrCommsFrame:
		case KErrCommsOverrun:
		case KErrCommsParity:
		case KErrTimedOut:
		case KErrCouldNotConnect:
		case KErrCouldNotDisconnect:
		case KErrBadLibraryEntryPoint:
		case KErrBadDescriptor:
		case KErrAbort:
			obexErr = KErrIrObexRespForbidden;
			break;
		case KErrTooBig:
			obexErr = KErrIrObexRespReqEntityTooLarge;
			break;
		case KErrDivideByZero:
		case KErrBadPower:
		case KErrDirFull:
		case KErrHardwareNotAvailable:
			obexErr = KErrIrObexRespForbidden;
			break;
		case KErrDisconnected: // System is shutting down
		    obexErr = KErrIrObexRespMethodNotAllowed;
		    break;
		default:
			obexErr = KErrIrObexRespBadRequest;
			break;
		};
	LOGGER_WRITE_1( "TFTPpluginUtils::ConvertFTPResponseCode : returned %d", obexErr );
	return obexErr;
};

//End of file



