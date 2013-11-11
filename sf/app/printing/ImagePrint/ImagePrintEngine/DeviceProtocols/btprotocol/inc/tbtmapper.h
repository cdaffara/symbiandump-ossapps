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
* Description:  Contains the TBtMapper class definition.
*
*/


#ifndef TBTMAPPER_H
#define TBTMAPPER_H

#include <e32base.h>
#include <libc/stdlib.h>

#include "printmessagecodes.h"
#include "printcapabilitycodes.h"

enum TBtPrintState
{
	EBtStateUnknown = 0,	//@brief job state not known
	EBtStateIdle,			//@brief printer state idle
	EBtStateProcessing,		//@brief printer processing
	EBtStateStopped,		//@brief printer or job state stopped
	EBtStatePrinting,		//@brief printing job
	EBtStateWaiting,		//@brief job is waiting
	EBtStateCompleted,		//@brief job ready
	EBtStateAborted,		//@brief job aborted for some error
	EBtStateCancelled,		//@brief job cancelled
	EBtStateSending			//@brief sending job. Internal state to indicate the data is about transmissed.
};

_LIT8(KBtStateUnknown, 		"unknown");
_LIT8(KBtStateIdle, 		"idle");
_LIT8(KBtStateProcessing, 	"processing");
_LIT8(KBtStateStopped, 		"stopped");
_LIT8(KBtStatePrinting, 	"printing");
_LIT8(KBtStateWaiting, 		"waiting");
_LIT8(KBtStateCompleted, 	"completed");
_LIT8(KBtStateAborted, 		"aborted");
_LIT8(KBtStateCancelled, 	"cancelled");

_LIT8(KPrintReasonNone, 				"none");
_LIT8(KPrintReasonAttentionRequired, 	"attention-required");
_LIT8(KPrintReasonMediaJam, 			"media-jam");
_LIT8(KPrintReasonPaused, 				"paused");
_LIT8(KPrintReasonDoorOpen, 			"door-open");
_LIT8(KPrintReasonMediaLow, 			"media-low");
_LIT8(KPrintReasonMediaEmpty, 			"media-empty");
_LIT8(KPrintReasonOutputAreaAlmostFull,	"output-area-almost-full");
_LIT8(KPrintReasonOutputAreaFull, 		"output-area-full");
_LIT8(KPrintReasonMarkerSupplyLow, 		"marker-supply-low");
_LIT8(KPrintReasonMarkerSupplyEmpty, 	"marker-supply-empty");
_LIT8(KPrintReasonMarkerFailure, 		"marker-failure");
_LIT8(KPrintReasonMediaChangeRequest, 	"media-change-request");
_LIT8(KPrintReasonVendorDefined, 		"vendor-defined");
_LIT8(KPrintReasonHwError, 				"hardware-error");
_LIT8(KPrintReasonUriNotFound, 			"external-access-uri-not-found");
_LIT8(KPrintReasonObjectFailure, 		"external-access-object-failure");
_LIT8(KPrintReasonFormatError, 			"external-access-doc-format-error");
_LIT8(KPrintReasonHttpError, 			"external-access-http-error");
_LIT8(KPrintReasonCopies, 				"copies");
_LIT8(KPrintReasonSides, 				"sides");
_LIT8(KPrintReasonNumberUp, 			"number-up");
_LIT8(KPrintReasonOrientationRequested,	"orientation-requested");
_LIT8(KPrintReasonMediaSize, 			"media-size");
_LIT8(KPrintReasonMediaType, 			"media-type");
_LIT8(KPrintReasonPrintQuality, 		"print-quality");
_LIT8(KPrintReasonTextLayout, 			"text-layout");
_LIT8(KPrintReasonImageLayout, 			"image-layout");
_LIT8(KPrintReasonImageOrientation, 	"image-orientation");
_LIT8(KPrintReasonPdlFidelity, 			"pdl-fidelity");
_LIT8(KPrintReasonFontFamily, 			"font-family");
_LIT8(KPrintReasonFontSize, 			"font-size");


// Paper size
// According to the "ftp://ftp.pwg.org/pub/pwg/standards/pwg5101.1.pdf",
// only the size of paper is defined
_LIT8(KPaperSizeMax,	"custom_max_");	//custom_max_[nn]x[nn][mm|in]
_LIT8(KPaperSizeMin,	"custom_min_");	//custom_min_[nn]x[nn][mm|in]
_LIT8(KPaperSizeA6, 	"_105x148mm");	//iso_a6_105x148mm
_LIT8(KPaperSizeA4, 	"_210x297mm");	//iso_a4_210x297mm
_LIT8(KPaperSize5x7, 	"_5x7in");		//na_5x7_5x7in
_LIT8(KPaperSize4x6, 	"_4x6in");		//na_index-4x6_4x6in
_LIT8(KPaperSizeLetter,	"_8.5x11in");	//na_letter_8.5x11in

// Quality
_LIT8(KQualityDraft, 	"draft");
_LIT8(KQualityNormal, 	"normal");
_LIT8(KQualityHigh, 	"high");

_LIT8(KHexPrefix, "0x");

/**
 *	@brief A printer abstraction.
 *
 *	Class representing the basic properties of a printing device.
 *	Published as part of the ImagePrint Server API.
 */
class TBtMapper
	{
	public:

		inline static TInt32 HexToInt(const TDesC8& aString)
		{
			TInt len(0);
			if( KErrNotFound <  aString.Find(KHexPrefix()))
				len = KHexPrefix().Length();

			TPtrC8 str = aString.Right(aString.Length()-len);

			TLex8 lex(str);
			TUint32 val = KErrNotFound;
			lex.Val(val, EHex);
			LOG82("[TBtMapper::OperationStatus]\t %S: %d", &aString, val);
			return val;
		}

		inline static TInt32 HexToInt(const TDesC& aString)
		{
			TBuf8<0x100> str;
			str.Copy(aString);
			return HexToInt(str);
		}

		inline static TInt32 Int(const TDesC8& aString)
		{
			TLex8 lex(aString);
			TInt32 value = KErrNotFound;
			lex.Val(value);
			return value;
		}

		inline static TInt32 Int(const TDesC& aString)
		{
			TLex lex(aString);
			TInt32 value = KErrNotFound;
			lex.Val(value);
			return value;
		}

		inline static TUint32 Uint(const TDesC8& aString)
		{
			TUint32 value = Int(aString);
			return value;
		}

		inline static TUint32 Uint(const TDesC& aString)
		{
			TUint32 value = Int(aString);
			return value;
		}

		inline static TInt State(const TDesC8& aStrState)
		{
			if(0 == aStrState.Compare(KBtStateUnknown)) //job
				return EBtStateUnknown;
			if(0 == aStrState.Compare(KBtStateIdle)) // printer
				return EBtStateIdle;
			if(0 == aStrState.Compare(KBtStateProcessing))  // printer
				return EBtStateProcessing;
			if(0 == aStrState.Compare(KBtStateStopped)) //job, printer
				return EBtStateStopped;
			if(0 == aStrState.Compare(KBtStatePrinting)) //job
				return EBtStatePrinting;
			if(0 == aStrState.Compare(KBtStateWaiting)) //job
				return EBtStateWaiting;
			if(0 == aStrState.Compare(KBtStateCompleted)) //job
				return EBtStateCompleted;
			if(0 == aStrState.Compare(KBtStateAborted)) //job
				return EBtStateAborted;
			if(0 == aStrState.Compare(KBtStateCancelled)) //job
				return EBtStateCancelled;

			return EBtStateUnknown;
		}

		inline static TInt PrintError(const TDesC8& aCode)
		{
			if(0 == aCode.Compare(KPrintReasonNone))
				return ENoMessage;
			if(0 == aCode.Compare(KPrintReasonAttentionRequired))
				return EPrintReasonAttentionRequired;
			if(0 == aCode.Compare(KPrintReasonMediaJam))
				return EPbStatusErrorReasonPaperJam;
			if(0 == aCode.Compare(KPrintReasonPaused))
				return EPrintReasonPaused;
			if(0 == aCode.Compare(KPrintReasonDoorOpen))
				return EPbStatusErrorReasonHardwareCoverOpen;
			if(0 == aCode.Compare(KPrintReasonMediaLow))
				return EPbStatusErrorReasonPaperNearlyEmpty;
			if(0 == aCode.Compare(KPrintReasonMediaEmpty))
				return EPbStatusErrorReasonPaperEmpty;
			if(0 == aCode.Compare(KPrintReasonOutputAreaAlmostFull))
				return EPrintReasonOutputAreaAlmostFull;
			if(0 == aCode.Compare(KPrintReasonOutputAreaFull))
				return EPrintReasonOutputAreaFull;
			if(0 == aCode.Compare(KPrintReasonMarkerSupplyLow))
				return EPrintReasonMarkerSupplyLow;
			if(0 == aCode.Compare(KPrintReasonMarkerSupplyEmpty))
				return EPrintReasonMarkerSupplyEmpty;
			if(0 == aCode.Compare(KPrintReasonMarkerFailure))
				return EPrintReasonMarkerFailure;
			if(0 == aCode.Compare(KPrintReasonMediaChangeRequest))
				return EPbStatusErrorReasonPaperMedia;
			if(0 == aCode.Compare(KPrintReasonVendorDefined))
				return EPbCheckPrinter;

			return EPbCheckPrinter;
		};

		inline static TInt HttpError(TInt /*aCode*/)
		{
			return EPbCheckPrinter;
		};

		inline static TInt OperationStatus(const TDesC8& aCode)
		{
		    return HexToInt(aCode);
		}

		inline static void Layout(const TDesC8& aCode, RArray<TInt>& aArray)
		{
			TInt layout;
			switch(Int(aCode))
			{
				case 16:
					layout = EPrintCapabLayout16Up;
					aArray.Append(layout);
				case 12:
					layout = EPrintCapabLayout12Up;
					aArray.Append(layout);
				case 9:
					layout = EPrintCapabLayout9Up;
					aArray.Append(layout);
				case 6:
					layout = EPrintCapabLayout6Up;
					aArray.Append(layout);
				case 4:
					layout = EPrintCapabLayout4Up;
					aArray.Append(layout);
				case 2:
					layout = EPrintCapabLayout2Up;
					aArray.Append(layout);
				case 1:
					layout = EPrintCapabLayout1UpBorder;
					aArray.Append(layout);
				default:
					layout = EPrintCapabLayout1UpBorderless;
					aArray.Append(layout);
				break;
				
			}
		};

		inline static TInt Size(const TDesC8& aCode)
		{
			if(0 <= aCode.Find(KPaperSize4x6))
				return EPrintCapabPaperSize4x6;//default
			if(0 <= aCode.Find(KPaperSizeA4))
				return EPrintCapabPaperSizeA4;
			if(0 <= aCode.Find(KPaperSizeA6))
				return EPrintCapabPaperSizeA6;
			if(0 <= aCode.Find(KPaperSize5x7))
				return EPrintCapabPaperSize5x7;
			if(0 <= aCode.Find(KPaperSizeLetter))
				return EPrintCapabPaperSizeLetter;		
			return KErrNotSupported;
		};

		inline static TInt Quality(const TDesC8& aCode)
		{
			if(0 <= aCode.Find(KQualityDraft))
				return EPrintCapabQualityDraft;
			if(0 <= aCode.Find(KQualityNormal))
				return EPrintCapabQualityNormal;
			if(0 <= aCode.Find(KQualityHigh))
				return EPrintCapabQualityFine;			
			return EPrintCapabQualityDefault;
		};

		inline static TInt TripletValue(const TDesC8& aParams, TInt& aPos, TUint& aTag)
		{
			// get tag
			aTag = aParams[aPos];
			++aPos;

			//get len
			TInt len = aParams[aPos];
			++aPos;

			TInt value = 0;
			TInt shift = (len-1)*8;
			while(0 <= shift)
			{
				value |= (aParams[aPos] << shift);
				++aPos;
				shift -= 8;
			}

			return value;
		};

	};

#endif // TBTMAPPER_H

//  End of File
