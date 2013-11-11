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
* Description:  Contains the TPrintCapabilityCodes and TPrintCapabilityOptions enumerations.
*
*/


#ifndef PRINTCAPABILITYCODES_H
#define PRINTCAPABILITYCODES_H

/// All the printer capabilities
enum TPrintCapabilityCodes
    {
		EPrintCapabLayout = 500,
		EPrintCapabPaperSize = 501,
		EPrintCapabQuality = 502
    };

/// Print capability generic options
enum TPrintCapabilityOptions
	{
		// Generic
		EPrintCapabYes = 1000,
		EPrintCapabNo = 1001,
		EPrintCapabOn = 1002,
		EPrintCapabOff = 1003,

		// Quality
		EPrintCapabQualityDefault = 1100,
		EPrintCapabQualityNormal = 1101,
		EPrintCapabQualityDraft = 1102,
		EPrintCapabQualityFine = 1103,   

		// Paper size 
		EPrintCapabPaperSizeAuto = 1200,
		EPrintCapabPaperSizeLetter = 1201,
		EPrintCapabPaperSizeA4 = 1221,
		EPrintCapabPaperSizeA6 = 1222,
		EPrintCapabPaperSize4x6 = 1250,
		EPrintCapabPaperSize5x7 = 1251,
		EPrintCapabPaperSizeSticker = 1270,

		// Layout
		EPrintCapabLayout1Up = 1501,
		EPrintCapabLayout2Up = 1502,
		//EPrintCapabLayout3Up = 1503,
		EPrintCapabLayout4Up = 1504,
		//EPrintCapabLayout5Up = 1505,
		EPrintCapabLayout6Up = 1506,
		EPrintCapabLayout9Up = 1509,
		EPrintCapabLayout12Up = 1512,
		EPrintCapabLayout16Up = 1513,
		EPrintCapabLayout16UpStick = 1516,
		EPrintCapabLayout1UpSmall = 1551,
		EPrintCapabLayout1UpMedium = 1552,
		EPrintCapabLayout1UpLarge = 1553,
		EPrintCapabLayout1UpBorderless = 1554,
		EPrintCapabLayout1UpBorder = 1555,
		//EPrintCapabLayout1UpPassport = 1556,    
		//EPrintCapabLayout1UpPassportSF = 1557,  // SmallFormat paper

		EPrintCapabLayout2UpPassport508x508Lg = 1558,
		EPrintCapabLayout2UpPassport254x364Lg = 1559,
		EPrintCapabLayout2UpPassport350x450Lg = 1560,
		EPrintCapabLayout2UpPassport364x508Lg = 1561,
		EPrintCapabLayout2UpPassport450x550Lg = 1562,

		EPrintCapabLayout2UpPassport508x508Sm = 1563,

		EPrintCapabLayout4UpStick = 1564,
		EPrintCapabLayout1UpStick = 1565,

		EPrintCapabLayout2UpPassport254x364Sm = 1566,
		EPrintCapabLayout2UpPassport350x450Sm = 1567,
		EPrintCapabLayout2UpPassport364x508Sm = 1568,
		EPrintCapabLayout2UpPassport450x550Sm = 1569,

		EPrintCapabLayoutLast = 3000
	};

#endif  //  PRINTCAPABILITYCODES_H

//  End of File
