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
* Description:  Contains the PrintSessionStatus and PrinterClass enumerations,
* 				 as well as some constants.
*
*/


#ifndef IMGPRINTKONST_H
#define IMGPRINTKONST_H

/// Status available for the PrintSession. 
/// @deprecated The PrintSession class was removed completely. If no one else uses this, this will be left out in future versions.
enum PrintSessionStatus
	{
		EStatusNone = 0,
		EPrinterDiscovered,
		ERemovePrinter,
		EDiscoveryDone,
		EPrintJobProgress,
		EPrintError,
		EPrintJobDone,
		EPreviewImage,
		EServerError,
		ECancelJob,
		ECancelDiscovery,

		ELast

	};

/// Printer classes' available.
enum PrinterClass
	{
		EDefault = 0,
		EBigAdaptive = 1,
		ESmallAdaptive = 2,
		EDesk = 3,
		ELinked = 99
	};

#endif // IMGPRINTKONST_H

//  End of File
