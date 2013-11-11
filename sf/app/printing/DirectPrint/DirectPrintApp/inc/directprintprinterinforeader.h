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
* Description:
* Printer information reader class
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTPRINTERINFOREADER_H__
#define __DIRECTPRINTPRINTERINFOREADER_H__

// System includes
#include <e32base.h>
#include <xmlengdocument.h>				// RXmlEngDocument
#include <xmlengdomparser.h>			// RXmlEngDOMParser
#include <xmlengdomimplementation.h>	// RXmlEngDOMImplementation

// User includes

// Forward declarations
class CDirectPrintModel;

// Class declaration
/**
 *  Printer information reader class
 *  more_complete_description
 */
class CDirectPrintPrinterInfoReader : public CBase
	{
public:
	/** Constructors */
	static CDirectPrintPrinterInfoReader* NewL(CDirectPrintModel& aModel);
	static CDirectPrintPrinterInfoReader* NewLC(CDirectPrintModel& aModel);
	/** Destructor */
	~CDirectPrintPrinterInfoReader();

	void ReadPrinterInfoL();

	void GetManufactureL(TDes& aManufacture);
	TUid GetDriverL(TDesC& aId);
	TUid GetSettingUIPluginL(TDesC& aId);

protected:
	CDirectPrintPrinterInfoReader(CDirectPrintModel& aModel);
	void ConstructL();

private:
	void ReadXmlDataL(const TDesC& aParam);
	TBool CheckManufactureL(const TDesC& aParam);

private:
	// Model object
	CDirectPrintModel& iModel;

	RXmlEngDocument iXmlEngDoc;
	RXmlEngDOMImplementation iXmlDomImpl;
	RXmlEngDOMParser iXmlParser;
	};

#endif // __DIRECTPRINTPRINTERINFOREADER_H__
