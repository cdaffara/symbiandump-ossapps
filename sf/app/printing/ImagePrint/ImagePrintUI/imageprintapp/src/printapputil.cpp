/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <ErrorUI.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikenv.h>
#include <obex.h>
#include <bluetooth/hci/hcierrors.h>
#include <imageprintapp.rsg>

#include "printapputil.h"
#include "printmessagecodes.h"
#include "cimageprintappui.h"
#include "clog.h"

// Displays the error note
void PrintAppUtil::ShowErrorNoteL(
    TInt aResourceId )
    {
    HBufC* str = StringLoader::LoadLC( aResourceId );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy( str );  // str
    }

// Displays the information note
void PrintAppUtil::ShowInfoNoteL(
    TInt aResourceId )
    {
    HBufC* str = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy( str);  // str
    }

// Displays error message based on the error code
void PrintAppUtil::ShowErrorMsgL(
    TInt aErrCode )
    {
    if ( aErrCode != KErrNone )
        {
        HBufC* str = PrintAppUtil::PrintErrorMsgLC( aErrCode );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
        note->ExecuteLD( *str );
        CleanupStack::PopAndDestroy( str );  // str
        }
    }

//  Loads printer application specific error message
HBufC* PrintAppUtil::PrintErrorMsgLC(
    TInt aErrCode )
    {
    HBufC* errStr = 0;
    TInt resourceId( 0 );

    switch ( aErrCode )
        { 
        case ( KHCIErrorBase - EPageTimedOut ):
            resourceId = R_NOTE_CONNECT_PRINT_ERROR;
            break;
        case KErrIrObexRespServiceUnavail:
            resourceId = R_NOTE_SEND_PRINT_ERROR;
            break;   
        case KErrDisconnected:
            resourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
            break;
        case EObexGeneralError:
            resourceId = R_NOTE_GENERAL_PRINT_ERROR;
            break;
        case EPbStatusErrorReasonInk:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_INK;
        	break;
        case EPbStatusErrorReasonHardwareCoverOpen:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_COVER_OPEN;
        	break;
        case EPbStatusErrorReasonHardwarePrinterBusy:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_PRINTER_BUSY;
        	break;
        case EPbStatusErrorReasonHardwareNoInkCartridge:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_NO_INK_CARTRIDGE;
        	break;
        case EPbStatusErrorReasonNoReason:
            resourceId  = R_NOTE_PRINT_STATUS_ERROR;
            break;
        case EPbOutOfPaper:
            resourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_OUT;
            break;    
        case EPbStatusErrorReasonInkEmpty:
            resourceId = R_NOTE_IMAGEPRINT_ERROR_INK_OUT;
            break;        
        case EPrintReasonPaused:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_PRINTER_PAUSED;
        	break;
        case EPrintReasonMarkerFailure:
        	resourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_FAILURE;
        	break;
        	
        case KErrHCILinkDisconnection:
        	resourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
        	break;
         
        default:
            resourceId = R_NOTE_GENERAL_PRINT_ERROR;
            break;
        }
	errStr = StringLoader::LoadLC( resourceId );
    
// Add error code to message only when debug build is created.
#ifdef _DEBUG	
    if ( aErrCode != 0 )
    	{
    	// append error code to message
	    _LIT( KErrTmp, ": %d" );
	    TBuf<32> errCodeMsg;
	    errCodeMsg.Format( KErrTmp, aErrCode );
	    errStr = errStr->ReAllocL( errStr->Length() + 32 );
	    CleanupStack::Pop(); // errStr before realloc
	    CleanupStack::PushL( errStr );
	    TPtr p( errStr->Des() );
	    p += errCodeMsg;
    	}
#endif // _DEBUG
    	
    return errStr;
    }

//  Adds application path
TFileName PrintAppUtil::AddApplicationPath(
    const TDesC& aFileName )
    {
    CEikAppUi* appUi = static_cast<CEikAppUi*>( CEikonEnv::Static()->AppUi() );
    TFileName fullFilePath = appUi->Application()->AppFullName();

    TParse parse;
    parse.Set( fullFilePath, NULL, NULL );
    fullFilePath = parse.DriveAndPath();
    fullFilePath.Append( aFileName );
    return fullFilePath;
    }

//  End of File
