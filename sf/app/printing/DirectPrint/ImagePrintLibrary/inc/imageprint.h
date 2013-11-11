/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Client include file for the ImagePrint server. Contains the 
* 				 CImagePrint class definition, and the following interfaces:
* 				 MIdleObserver, MPrinterDiscoveryObserver and MPrintEventObserver
*
*/


// Protection against nested includes
#ifndef IMAGEPRINT_H
#define IMAGEPRINT_H

// System includes
#include <e32base.h>

class TPrinter;
class TEvent;

_LIT( KTabChar, "\t" );
_LIT( KParamFile, "\\system\\data\\T_AIW_TEMP.DAT");

/// Error constant defined to indicate Invalid Sequence.
const TInt KErrInvalidSequence = -50;
/// Error constant defined to indicate Invalid Data.
const TInt KErrInvalidData = -51;
/// Error constant defined to indicate that a printer has been connected
const TInt KErrConnected = -500;

/// BPP Protocol Identifier.
const TUint KImagePrint_PrinterProtocol_BPP = 1;
/// BIP Protocol Identifier.
const TUint KImagePrint_PrinterProtocol_BIP = 2;
/// OPP Protocol Identifier (Printer).
const TUint KImagePrint_PrinterProtocol_OPP_Printer = 4;
/// DPOF Protocol Identifier.
const TUint KImagePrint_PrinterProtocol_DPOF = 8;
/// PictBridge Protocol Identifier.
const TUint KImagePrint_PrinterProtocol_PictBridge = 16;
/// OPP Protocol Identifier (PC).
const TUint KImagePrint_PrinterProtocol_OPP_PC = 32;
/// UPnP Protocol Identifier
const TUint KImagePrint_PrinterProtocol_UPnP = 64;
/// LPR Protocol Identifier
const TUint KImagePrint_PrinterProtocol_LPR = 128;

/// Enums used to specify the current state in the DiscoveryStatusL callback.
enum TDiscoveryStatus
	{
	EDiscoverying = 0,
	EDiscoveryFinished = 1,
	EDiscoveryCancelling = 2
	};

/// Enums used to specify the current state in the PrintJobProgressEvent callback.
enum TPrintJobProgressEventStatus
	{
	EActive = 0,
	EDone = 1
	};

/**
 *	@brief Interface to be implemented by the class using CImagePrint. It is used by CImagePrint to report anything related
 *	to the discovery process.
 */
class MPrinterDiscoveryObserver
	{
	public:
		/**
		 * Called by Image Printer Server to notify the client application that a printer was discovered.
		 *  @param aPrinterInfo Contains printer information of the discovered device.
		 */
		virtual void FoundPrinterL( const TPrinter& aPrinterInfo ) = 0;
		/**
		 * Called by Image Printer Server to notify the client application that an operation 
		 * has completed, either successfully or with an error.
		 * @param aStatus Indicate the status of the discovery process. One of the values of \c ::TDiscoveryStatus
		 * @param aErrorCode \c KErrNone if there was no error, otherwise another of the system-wide error codes.
		 * @param aErrorStringCode StringCode - Descriptive text that can be used to further explain error.
		 */
		virtual void DiscoveryStatusL( TInt aStatus, 
										TInt aErrorCode, 
										TInt aErrorStringCode ) = 0;
		/**
		 * Called by Image Printer Server to notify the client application that a printer 
		 * returned from the cache was not discovered during the discovery phase, 
		 * and should be removed from the UI. If this call is needed, it will be made 
		 * after all BPP printers have been returned, but before the discovery completes.
		 *  @param aPrinterInfo Contains printer information of the printer that was not found during discovery
		 */
		virtual void RemovePrinterL( const TPrinter& aPrinterInfo ) = 0;
	};

/**
 *	@brief Interface to be implemented by the class using CImagePrint. It is used by CImagePrint to report anything related
 *	to the printing process, once it is started.
 */
class MPrintEventObserver
	{
	public:
		virtual void PrintJobProgressEvent( TInt aStatus, 
										TInt aPercentCompletion, 
										TInt aJobStateCode ) = 0;
										
		virtual void PrintJobErrorEvent( TInt aError, 
										TInt aErrorStringCode ) = 0;
										
		virtual void PrinterStatusEvent( TInt aError, 
										TInt aErrorStringCode ) = 0;
										
		virtual void PreviewImageEvent( TInt aFsBitmapHandle ) = 0;
		
		virtual void ShowMessageL( TInt aMsgLine1Code, 
										TInt aMsgLine2Code ) = 0;
										
		virtual TBool AskYesNoQuestionL( TInt aMsgLine1Code, 
										TInt aMsgLine2Code ) = 0;
										
		virtual const TDesC& AskForInputL( TInt aMsgLine1Code, 
										TInt aMsgLine2Code ) = 0;
	};

/**
 *  @brief Interface to be implemented by the class using CImagePrint. It is used by CImagePrint to report
 *  anything related to status changes in the handling protocol.
 *  @sa CImagePrint::RegisterIdleObserver
 */
class MIdleObserver
	{
	public:
		/** Called by CImagePrint to notify of events. The meaning of the event and associated error and mesage codes
		 *  is protocol dependant.
		 */
		virtual void StatusEvent( const TEvent &aEvent, 
										TInt aError, 
										TInt aMsgCode ) = 0;
	};


#endif // IMAGEPRINT_H

//  End of File
