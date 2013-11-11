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

#include "clog.h"
#include "imgpprintapputil.h"
#include "printmessagecodes.h"
#include "cimgpprintutils.h"

const TInt KClientErrorForbidden = 1025;

// Displays the error note
void IMGPPrintAppUtil::ShowErrorNoteL(
    TInt aResourceId )
    {
    HBufC* str = StringLoader::LoadLC( aResourceId );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy();  // str
    }

// Displays the information note
void IMGPPrintAppUtil::ShowInfoNoteL(
    TInt aResourceId )
    {
    HBufC* str = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote;
    note->ExecuteLD( *str );
    CleanupStack::PopAndDestroy();  // str
    }

// Displays error message based on the error code
void IMGPPrintAppUtil::ShowErrorMsgL(
    TInt aErrCode, TInt aErrorStringCode )
    {
    if ( aErrCode == KErrNoMemory )
        {
        CCoeEnv::Static()->HandleError( aErrCode );
        }
    else if ( aErrCode != KErrNone )
        {
        HBufC* str = IMGPPrintAppUtil::PrintErrorMsgLC( aErrCode, aErrorStringCode );
        CAknErrorNote* note = new ( ELeave ) CAknErrorNote;
        note->ExecuteLD( *str );
        CleanupStack::PopAndDestroy();  // str
        }
    }

//  Loads printer application specific error message
HBufC* IMGPPrintAppUtil::PrintErrorMsgLC(
    TInt aErrCode, TInt aErrorStringCode )
    {
    HBufC* errStr = 0;
    TInt resourceId( 0 );
    TInt finalErrCode( 0 );

    if ( aErrorStringCode )
        {        
        HandleByStringCodeL( aErrCode, aErrorStringCode, resourceId );
        }

    if ( resourceId == 0 )
        {
        HandleByErrorCodeL( resourceId, finalErrCode, aErrCode );
        }
    else
      	{
      	finalErrCode = aErrorStringCode;
       	}

    errStr = StringLoader::LoadLC( resourceId );
    
// Add error code to message only when debug build is created.
#ifdef _DEBUG	    
    if ( finalErrCode != 0 )
    	{
    	// append error code to message
	    _LIT( KErrTmp, ": %d" );
	    TBuf<32> errCodeMsg;
	    errCodeMsg.Format( KErrTmp, finalErrCode );
	    errStr = errStr->ReAllocL( errStr->Length() + 32 );
	    CleanupStack::Pop(); // errStr before realloc
	    CleanupStack::PushL( errStr );
	    TPtr p( errStr->Des() );
	    p += errCodeMsg;
    	}
#endif // _DEBUG   
    return errStr;
    }

void IMGPPrintAppUtil::HandleByStringCodeL( TInt aErrCode, TInt aErrorStringCode, TInt& aResourceId  )
	{    
	TBool matchOne = ETrue;	
	TBool matchTwo = ETrue;	
	TBool matchThree = ETrue;	
    StringCodeInkL( aErrorStringCode, aResourceId, matchOne );    
    StringCodeHwL( aErrorStringCode, aResourceId, matchTwo );        
    StringCodePaperL( aErrorStringCode, aResourceId, matchThree );
	
	if ( !matchOne && !matchTwo && !matchThree)
		{	
		switch ( aErrorStringCode )
	        {
	        case EObexConnectError: //-3000
	            {
	            if ( aErrCode == KHCIErrorBase-EPageTimedOut ) //-6004
	                {
	                aResourceId = R_NOTE_CONNECT_PRINT_ERROR;
	                }
	            else if ( aErrCode == KHCIErrorBase-EHostResourceRejection ) //-6013
	                {
	                aResourceId = R_NOTE_SEND_PRINT_ERROR;
	                }
	            else
	                {
	                aResourceId = R_NOTE_CONNECT_PRINT_ERROR;
	                }           
	            }
                break;
	            
	        case KErrL2CAPRequestTimeout:
	            aResourceId = R_NOTE_SEND_PRINT_ERROR;
	            break;
	      
	        case KErrDisconnected:
	            // If aError check is needed, it is in one case -6305.
	            aResourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
	            break;
	
	        case EObexGeneralError:
	            aResourceId = R_NOTE_GENERAL_PRINT_ERROR;
	            break;
	        
	       	case EPbStatusErrorReasonFileFileDecode:
	       		aResourceId = R_NOTE_IMAGEPRINT_ERROR_FILE_DECODE;
	       		break;
	        	        	
	        case EPbStatusErrorReasonFile:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_FILE;
	        	break; 
	        	
	        case EPbStatusErrorReasonWarning:
	        	aResourceId = R_NOTE_PRINT_STATUS_ERROR;
	        	break;
	        
	        case EPbStatusErrorReasonNoReason:
	            aResourceId  = R_NOTE_PRINT_STATUS_ERROR;
	            break;
	
	        case EPbOutOfPaper:
	            aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_OUT;
	            break;
	            
	        case EPrintReasonPaused:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PRINTER_PAUSED;
	        	break;
	        
	        case EPrintReasonOutputAreaAlmostFull:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_OUTPUT_AREA_ALMOST_FULL;
	        	break;
	
	        case EPrintReasonOutputAreaFull:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_OUTPUT_AREA_FULL;
	        	break;
	
	        case EPrintReasonMarkerFailure:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_FAILURE;
	        	break;
	        	
	        default:           
	            break;
	        } 
		}
	}
void IMGPPrintAppUtil::StringCodeInkL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  )
	{
	 switch ( aErrorStringCode )
	        {            
	        case EPbStatusErrorReasonInkLow:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_LOW;
	        	break;
	         	        	        	        
	        case EPbStatusErrorReasonInkWaste:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_WASTE;
	        	break;
	        	
	        case EPbStatusErrorReasonInk:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK;
	        	break; 
	        	
	        case EPbStatusErrorReasonInkEmpty:
	        	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_OUT;
	        	break;
	        	
	        case EPrintReasonMarkerSupplyLow:
               	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_SUPPLY_LOW;
               	break;

            case EPrintReasonMarkerSupplyEmpty:
               	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_SUPPLY_EMPTY;
               	break;

       	    default:
           	    aMatch = EFalse;
           	    break;
	        }
	}

void IMGPPrintAppUtil::StringCodeHwL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  )
	{
	 switch ( aErrorStringCode )
	        {
		 	case EPbStatusErrorReasonHardwareCoverOpen:
	         	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_COVER_OPEN;
	         	break;
	         	
	         case EPbStatusErrorReasonHardwareFatal:
	         	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_FATAL;
	         	break;
	         	
	         case EPbStatusErrorReasonHardwareServiceCall:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_SERVICECALL;
            	break;
                	
            case EPbStatusErrorReasonHardwarePrinterUnavailable:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_PRINTER_UNAVAILABLE;
            	break;
            
            case EPbStatusErrorReasonHardwarePrinterBusy:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_PRINTER_BUSY;
            	break;
            	
            case EPbStatusErrorReasonHardwareLever:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_LEVER;
            	break;
            	
            case EPbStatusErrorReasonHardwareNoMarkingAgent:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_NO_MARKING_AGENT;
            	break;
            
            case EPbStatusErrorReasonHardwareInkCoverOpen:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_INK_COVER_OPEN;
            	break;
            
            case EPbStatusErrorReasonHardwareNoInkCartridge:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_NO_INK_CARTRIDGE;
            	break;
            	
            case EPbStatusErrorReasonHardware:
            	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE;
            	break; 

    	    default:
        	    aMatch = EFalse;
        	    break;
	        }
	}

void IMGPPrintAppUtil::StringCodePaperL( TInt aErrorStringCode, TInt& aResourceId, TBool& aMatch  )
	{
	 switch ( aErrorStringCode )
	        {	        
			case EPbStatusErrorReasonPaperLoad:
		       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_LOAD;
		       	break;
	       	
	       	case EPbStatusErrorReasonPaperEmpty:
		       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_OUT;
		       	break;        
	
	       	case EPbStatusErrorReasonPaperEject:
		       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_EJECT;
		       	break;
	
	       	case EPbStatusErrorReasonPaperJam:
		       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_JAM;
		       	break;
	
	       	case EPbStatusErrorReasonPaperMedia:
		       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_MEDIA;
		       	break;
	
	       	case EPbStatusErrorReasonPaperNearlyEmpty:
	       		aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_LOW;
	       		break;
	       	
	       	case EPbStatusErrorReasonPaperCombination:
	       		aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_COMBINATION;
	       		break;
	       
	       	case EPbStatusErrorReasonPaper:
	       		aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER;
	       		break;  

		    default:
	    	    aMatch = EFalse;
	    	    break;
	        }
	}

void IMGPPrintAppUtil::HandleByErrorCodeL( TInt& aResourceId, TInt& aFinalErrCode, TInt aErrCode  )
	{	
	aFinalErrCode = aErrCode;
	
	TBool matchOne   = ETrue;  
	TBool matchTwo   = ETrue;
	TBool matchThree = ETrue;	
	ErrorCodeInkL( aResourceId, aErrCode, matchOne );    
	ErrorCodeHwL( aResourceId, aErrCode, matchTwo );
	ErrorCodePaperL( aResourceId, aErrCode,matchThree );
	
	if ( !matchOne && !matchTwo && !matchThree)
		{
		switch ( aErrCode )
	    {
		    case KClientErrorForbidden:
		    	aResourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
		        break;	
		   
		    case KErrDisconnected:
		    	aResourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
		        break;
		
		    case EObexGeneralError:
		    	aResourceId = R_NOTE_GENERAL_PRINT_ERROR;
		        break;
		  
		   	case EPbStatusErrorReasonFileFileDecode:
		   		aResourceId = R_NOTE_IMAGEPRINT_ERROR_FILE_DECODE;
		   		break;
		    	        	
		    case EPbStatusErrorReasonFile:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_FILE;
		    	break;        
		          
		    case EPbStatusErrorReasonNoReason:
		    	aResourceId  = R_NOTE_PRINT_STATUS_ERROR;
		        break;
			
		    case KErrCorrupt: // Only corrupted images selected:
		    	aResourceId =  R_QTN_PRINT_SELECTNEW_NOTE;
		        break;
		        
		    case EPrintReasonPaused:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PRINTER_PAUSED;
		    	break;
		    
		    case EPrintReasonOutputAreaAlmostFull:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_OUTPUT_AREA_ALMOST_FULL;
		    	break;
		
		    case EPrintReasonOutputAreaFull:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_OUTPUT_AREA_FULL;
		    	break;
		 
		    case EPrintReasonMarkerFailure:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_FAILURE;
		    	break;
		    	
		    case KErrHostUnreach:
		    	aResourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
		    	break;	
		    	
		    case ( KHCIErrorBase-EHostResourceRejection ):
		    	aResourceId = R_NOTE_SEND_PRINT_ERROR;
				break;
			
		    case ( KHCIErrorBase-ERemoteHostTimeout ):
		    	aResourceId = R_NOTE_SEND_PRINT_ERROR;
		    	break;	  
		    	
		    case ( KHCIErrorBase-EPageTimedOut ):
		    	aResourceId = R_NOTE_CONNECT_PRINT_ERROR;
		        break;
				
			case KErrHCILinkDisconnection:
				aResourceId = R_NOTE_DISCONNECT_PRINT_ERROR;
				break;
		        
		    default:	  
				aResourceId = R_NOTE_GENERAL_PRINT_ERROR;
				break;
		    }
		}
	}

void IMGPPrintAppUtil::ErrorCodeInkL( TInt& aResourceId, TInt aErrCode, TBool& aMatch )
	{
	switch ( aErrCode )
	    {	  	
	    case EPbStatusErrorReasonInkLow:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_LOW;
	    	break;
	      	        	        	        	        
	    case EPbStatusErrorReasonInkWaste:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_WASTE;
	    	break;
	    	
	    case EPbStatusErrorReasonInk:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK;
	    	break; 
	    	
	    case EPrintReasonMarkerSupplyLow:
 	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_SUPPLY_LOW;
 	    	break;
 	
    	case EPrintReasonMarkerSupplyEmpty:
 	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_MARKER_SUPPLY_EMPTY;
 	    	break; 	    
 	    	
    	case EPbStatusErrorReasonInkEmpty:
 	       	aResourceId = R_NOTE_IMAGEPRINT_ERROR_INK_OUT;
 	        break;

 		default:
 	    	aMatch = EFalse;
 	    	break;
	    }
	}

void IMGPPrintAppUtil::ErrorCodeHwL( TInt& aResourceId, TInt aErrCode, TBool& aMatch )
	{
	switch ( aErrCode )
	    {
		case EPbStatusErrorReasonHardwareCoverOpen:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_COVER_OPEN;
	    	break;
		    	
		    case EPbStatusErrorReasonHardwareFatal:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_FATAL;
		    	break;
		    	
		    case EPbStatusErrorReasonHardwareServiceCall:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_SERVICECALL;
		    	break;
		    	
		    case EPbStatusErrorReasonHardwarePrinterUnavailable:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_PRINTER_UNAVAILABLE;
		    	break;
		    
		    case EPbStatusErrorReasonHardwarePrinterBusy:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_PRINTER_BUSY;
		    	break;
		    	
		    case EPbStatusErrorReasonHardwareLever:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_LEVER;
		    	break;
		    	
		    case EPbStatusErrorReasonHardwareNoMarkingAgent:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_NO_MARKING_AGENT;
		    	break;
		    
		    case EPbStatusErrorReasonHardwareInkCoverOpen:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_INK_COVER_OPEN;
		    	break;
		    
		    case EPbStatusErrorReasonHardwareNoInkCartridge:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE_NO_INK_CARTRIDGE;
		    	break;
		    	
		    case EPbStatusErrorReasonHardware:
		    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_HARDWARE;
		    	break; 
		    	
		    default:
	    	    aMatch = EFalse;
	    	    break;
	    }
	}

void IMGPPrintAppUtil::ErrorCodePaperL( TInt& aResourceId, TInt aErrCode, TBool& aMatch )
	{
	switch ( aErrCode )
	    {
		case EPbStatusErrorReasonPaperLoad:
   	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_LOAD;
   	    	break; 
	   	    	
	    case EPbStatusErrorReasonPaperEmpty:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_OUT;
	    	break;        
	
	    case EPbStatusErrorReasonPaperEject:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_EJECT;
	    	break;
	
	    case EPbStatusErrorReasonPaperJam:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_JAM;
	    	break;
	
	    case EPbStatusErrorReasonPaperMedia:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_MEDIA;
	    	break;
	
	    case EPbStatusErrorReasonPaperNearlyEmpty:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_LOW;
	    	break;
	    	
	    case EPbStatusErrorReasonPaperCombination:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_COMBINATION;
	    	break;
	    
	    case EPbStatusErrorReasonPaper:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER;
	    	break;
	    	
	    case EPbOutOfPaper:
	    	aResourceId = R_NOTE_IMAGEPRINT_ERROR_PAPER_OUT;
	        break;
	    	
	    default:
    	    aMatch = EFalse;
    	    break;
	    }	
	}

//  Adds application path
TFileName IMGPPrintAppUtil::AddApplicationPath(
    const TDesC& aFileName )
    {
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    CEikAppUi* appUi = static_cast<CEikAppUi*>( eikonEnv->AppUi() );
    TFileName fullFilePath = appUi->Application()->AppFullName();
    delete eikonEnv; eikonEnv = NULL;
    
    TParse parse;
    parse.Set( fullFilePath, NULL, NULL );
    fullFilePath = parse.DriveAndPath();
    fullFilePath.Append( aFileName );
    return fullFilePath;
    }

//  End of File
