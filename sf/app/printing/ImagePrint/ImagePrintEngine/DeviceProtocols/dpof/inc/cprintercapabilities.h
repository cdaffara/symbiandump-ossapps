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
* Description:  Contains the CPrinterCapabilities class definition.
*
*/


#ifndef CPRINTERCAPABILITIES_H
#define CPRINTERCAPABILITIES_H

#include <e32base.h>
#include <barsread.h>

#include "ccapabilityarray.h"

/**
 *	@brief Class that stores a printer class capabilities in full detail.
 *
 *	class PrinterCapabilities - An array which holds pointers to capabilities that 
 *	exist in the CapabilitiesArray(s).  Each printer that is found will have it's own
 *	PrinterCapabilities object, and when the PrinterCapability functions are called from
 *	the ClientAPI, the data is obtained directly from this PrinterCapabilities class.
 *	Different printers may refer back to the same object from the capabilities Array.  
 *	When the PrinterCapabilities object is created, it will contain the default 
 *	capabilities and current settings.  If the printer is selected for the print job,
 *	the settings and configuration may change to reflect the current print job settings.
 *	
 *	BUILDING PRINTER CAPABILITIES
 *	Each resource contains a printerclass field, which identifies which class of printer this 
 *	capability is defined for.  When a printer is discovered and a new PrinterCapabilities 
 *	object is created,  the CapabilitiyArray is searched for capabilites with that class to 
 *	add.  This occurs in the BuildCCapabilityArray function. 
 *
 *	While a resource file can have the same capabilitiy defined multiple times for multiple 
 *	PrinterClasses, A PrinterClass can have a capability defined only once for the class.
 *	One of the classes is "ELinked".  Capabilities of this class are dependent upon the current
 *	value of other capabilities and can be linked into one or more other PrinterCapability arrays.
 *	Each enumerated value in the resource is a value pair, with the second number being a link code.
 *	If non-zero, then the resource with right LinkID is also added to the PrinterCapabilites.
 *	When SetCurrentCapability is called and a current value is changed, the linked capability must
 *	also be checked to see if it is changed.  
 *
 *	Note that setting the current value can only occur for the current print job, but when it does
 *	the capabilities for a particular printer can actually change during the life of the job,
 *	requiring the application to get the printer capabilities multiple times.  
 *	Notes on implementation Limitations:  
 *
 *	The current implementation uses one and only one CapabilitiesInfo object for
 *	each capabiltiy described in the resource file.  All references to the capability
 *	refer back to this object.  This is fine as long as there is only one print job.
 *	When we have multiple print jobs, this will not work for two reasons.  
 *
 *	1 - There could be multiple current settings fora particular capability. 
 *
 *	2 - Different jobs could configure linked capabilities for a printer. 
 */
class CPrinterCapabilities : public CBase
	{
	public:
		static CPrinterCapabilities* NewL(CCapabilityArray* aSourceArray, TInt aPrinterClass);
		~CPrinterCapabilities();
		TInt Count() const;			
		void GetIDs(RArray<TInt>& aCapabilityIDs) const;
		TInt GetCapability(TInt aCapabilityID, TPrintCapability& aCapability) const;
		TInt SetCurrentValueL(TInt aCapabilityID, TInt aValue);
		TBool GetCurrentValue(TInt aCapabilityID, TInt &aCapabilityValue) const;

	private:
		CPrinterCapabilities(CCapabilityArray* aSourceArray, TInt aPrinterClass);
		void ConstructL();
		void BuildCapabilityArrayL();

	private:
		TInt iPrinterClass;
		RPointerArray<CCapabilityInfo> iPrinterDataArray;
		CCapabilityArray* iSourceArray;
	};

#endif // CPRINTERCAPABILITIES_H

//  End of File
