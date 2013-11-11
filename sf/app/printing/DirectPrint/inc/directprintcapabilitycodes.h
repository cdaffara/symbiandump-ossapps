/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:  Contains the TPrintCapabilityCodes and TPrintCapabilityOptions enumerations.
*
*/


// Protection against nested includes
#ifndef PRINTCAPABILITYCODES_H
#define PRINTCAPABILITYCODES_H

/// All the printer capabilities
enum TPrintCapabilityCodes
    {
		EDirectPrintCapabLayout = 500,
		EDirectPrintCapabPaperSize = 501,
		EDirectPrintCapabQuality = 502,
		EDirectPrintCapabAP = 600,
		EDirectPrintCapabHost = 601,
		EDirectPrintCapabPort = 602,
		EDirectPrintCapabUser = 603,
		EDirectPrintCapabQue = 604,
		EDirectPrintCapabExtent = 901,
		EDirectPrintCapabExtent2 = 902
    };

/// Print capability generic options
enum TPrintCapabilityOptions
	{
		// Generic
		EDirectPrintCapabYes = 1000,
		EDirectPrintCapabNo = 1001,
		EDirectPrintCapabOn = 1002,
		EDirectPrintCapabOff = 1003,

		// Quality
		EDirectPrintCapabQualityDefault = 1100,
		EDirectPrintCapabQualityNormal = 1101,
		EDirectPrintCapabQualityDraft = 1102,
		EDirectPrintCapabQualityFine = 1103,   

		// Paper size 
		EDirectPrintCapabPaperSizeAuto = 1200,
		EDirectPrintCapabPaperSizeLetter = 1201,
		EDirectPrintCapabPaperSizeA4 = 1221,
		EDirectPrintCapabPaperSizeA6 = 1222,
		EDirectPrintCapabPaperSize4x6 = 1250,
		EDirectPrintCapabPaperSize5x7 = 1251,
		EDirectPrintCapabPaperSizeSticker = 1270,

		// Extent
		EDirectPrintCapabExtentSelect1 = 9001,
		EDirectPrintCapabExtentSelect2 = 9002,
		EDirectPrintCapabExtentSelect3 = 9003,
		EDirectPrintCapabExtentSelect4 = 9004
	};

#endif  //  PRINTCAPABILITYCODES_H

//  End of File
