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
* Description:  Contains the CPrinterElement class definition and the Transport enum.
*
*/


#ifndef CPRINTERELEMENT_H
#define CPRINTERELEMENT_H

#include <e32base.h>

#include "tprinter.h"
#include "cprintercapabilities.h"

/// Possible transports for a print job.
enum Transport
	{
	BPPTrans,
	DPOFTrans
	};

class CPrinterElement : public CBase
	{
	public:
		~CPrinterElement();
		static CPrinterElement* NewLC();

	private:
		CPrinterElement();

	public:
		Transport	iTransport;
		TPrinter	iPrinter;
		TInt		iTransportData;
		TInt		iPrinterClass;
		CPrinterCapabilities* iPrinterCapabilities;
	};

#endif // CPRINTERELEMENT_H

//  End of File
