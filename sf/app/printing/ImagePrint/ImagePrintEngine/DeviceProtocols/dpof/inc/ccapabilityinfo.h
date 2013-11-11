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
* Description:  Contains the CCapabilityInfo class definition.
*
*/


#ifndef CCAPABILITYINFO_H
#define CCAPABILITYINFO_H

#include <e32base.h>
#include <barsread.h> 

#include "tprintcapability.h"

/**
 *	@brief Class that stores a printer's capability information, which is obtained from a resource file.
 *
 *	class CCapabilityInfo - Holds all of the information about a Capability.   Each capability
 *	described in the resource file will have a single instance of the CCapabilityInfo created for it.
 *	This class also includes the current setting, which is only used if the capability is 
 *	actually in use by the current print job. 
 */
class CCapabilityInfo : public CBase
    {
	public:
	
		static CCapabilityInfo* NewLC(TResourceReader& aReader);
		~CCapabilityInfo();

		TPrintCapability& Capability();		
		TInt SetCurrentValue( TInt aValue );
		TInt PrinterClass() const; 		
		TInt LinkID() const;		
		TInt CurrentValue() const;		
		TInt ValueLink( TInt aValue );
		CCapabilityInfo* CopyL() const;

	private:
		void ConstructL(TResourceReader& aReader);

	private:
		TPrintCapability iCapability;
		RArray<TInt>	iLinks;
		TInt			iLinkID;
		TInt			iPrinterClass;
		TInt			iCurrentValue;
    };

#endif // CCAPABILITYINFO_H

//  End of File
