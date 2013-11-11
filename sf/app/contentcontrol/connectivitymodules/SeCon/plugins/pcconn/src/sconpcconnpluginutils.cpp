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
* Description:  PC Connectivity Plug-in Utils implementation
*
*/


// INCLUDE FILES
#include <obexconstants.h>
#include "sconpcconnpluginutils.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TPCConnpluginUtils::ConvertFTPResponseCode( TInt aError )
// Converts Symbian error code to OBEX error code
// -----------------------------------------------------------------------------
//
TInt TPCConnpluginUtils::ConvertFTPResponseCode( TInt aError )
	{
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertFTPResponseCode( TInt aError )\
	 : begin, aError: %d", aError );
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
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertFTPResponseCode( \
	TInt aError ) returned : %d", obexErr );
	return obexErr;
};

// -----------------------------------------------------------------------------
// TPCConnpluginUtils::ConvertPCDResponseCode( TInt aError )
// Converts Symbian error code to OBEX error code
// -----------------------------------------------------------------------------
//
TInt TPCConnpluginUtils::ConvertPCDResponseCode( TInt aError )
	{
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertPCDResponseCode( TInt \
	aError ) : begin, aError: %d", aError );
	
	TInt obexErr ( KErrIrObexRespInternalError );
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
			obexErr = KErrIrObexRespInternalError;
			break;
		case KErrNoMemory:
			obexErr = KErrIrObexRespDatabaseFull;
			break;
		case KErrNotSupported:
			obexErr = KErrIrObexRespUnsupMediaType;
			break;
		case KErrArgument:
		case KErrTotalLossOfPrecision:
		case KErrBadHandle:
		case KErrOverflow:
		case KErrUnderflow:
			obexErr = KErrIrObexRespPreCondFailed;
			break;
		case KErrAlreadyExists:
			obexErr = KErrIrObexRespConflict;
			break;
		case KErrPathNotFound:
		case KErrDied:
		case KErrInUse:
			obexErr = KErrIrObexRespTimedOut;
			break;
		case KErrServerTerminated:
			obexErr = KErrIrObexRespPreCondFailed;
			break;
		case KErrServerBusy:
			obexErr = KErrIrObexRespConflict;
			break;
		case KErrCompletion:
		case KErrNotReady:
		case KErrUnknown:
		case KErrCorrupt:
			obexErr = KErrIrObexRespInternalError;
			break;
		case KErrAccessDenied:
		case KErrLocked:
			obexErr = KErrIrObexRespUnauthorized;
			break;
		case KErrWrite:
		case KErrDisMounted:
		case KErrEof:
			obexErr = KErrIrObexRespInternalError;
			break;
		case KErrDiskFull:
			obexErr = KErrIrObexRespDatabaseFull;
			break;
		case KErrBadDriver:
		case KErrBadName:
			obexErr = KErrIrObexRespNotFound;
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
		case KErrTooBig:
		case KErrDivideByZero:
		case KErrBadPower:
		case KErrDirFull:
		case KErrHardwareNotAvailable:
			obexErr = KErrIrObexRespInternalError;
			break;
		default:
			obexErr = KErrIrObexRespInternalError;
			break;
		};
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertPCDResponseCode(\
	 TInt aError ) returned : %d", obexErr );
	return obexErr;
};

// -----------------------------------------------------------------------------
// TPCConnpluginUtils::ConvertObexMoveResponseCode( TInt aError )
// Converts Symbian error code to OBEX error code
// -----------------------------------------------------------------------------
//
TInt TPCConnpluginUtils::ConvertObexMoveResponseCode( TInt aError )
	{
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertObexMoveResponseCode( TInt \
	aError ) : begin, aError: %d", aError );
	
	TInt obexErr ( KErrIrObexRespInternalError );
	switch( aError )
		{
		case KErrNone:
			obexErr = KErrIrObexRespSuccess;
			break;
		case KErrNotFound:
		case KErrPathNotFound:
			obexErr = KErrIrObexRespNotFound;
			break;
        case KErrAccessDenied:
            obexErr = KErrIrObexRespForbidden;
            break;
        case KErrNoMemory:
        case KErrDiskFull:
            obexErr = KErrIrObexRespDatabaseFull;
            break;
        case KErrInUse:
        case KErrLocked:
            obexErr = KErrIrObexRespConflict;
            break;
        case KErrNotSupported:
            obexErr = KErrIrObexRespNotImplemented;
            break;
        case KErrAlreadyExists:
            obexErr = KErrIrObexRespNotModified;
            break;
		
		default:
			obexErr = KErrIrObexRespInternalError;
			break;
		};
	LOGGER_WRITE_1( "TPCConnpluginUtils::ConvertObexMoveResponseCode(\
	 TInt aError ) returned : %d", obexErr  );
	return obexErr;
};

//End of file

