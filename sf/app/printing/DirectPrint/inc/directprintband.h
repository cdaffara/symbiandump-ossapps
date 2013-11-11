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
* Test print band ECOM plugin
*
*/

// Protection against nested includes
#ifndef DIRECTPRINTBAND_H
#define DIRECTPRINTBAND_H

// System includes
#include <e32std.h>
#include <e32base.h>
#include <gdi.h>
#include <ecom/ecom.h>

// Forward declarations
class CDirectPrintBand;
class CPrintSetup;
class RXmlEngDocument;
class MPageRegionPrinter;

// CLASS DECLARATION

/**
 *  CDirectPrintBand
 * 
 */
class CDirectPrintBand : public CBase
	{
public:
	inline static CDirectPrintBand* NewL(TUid aImplementationUid);
	virtual ~CDirectPrintBand();

	/**
	 * Initialize the print band 
	 *
	 * @param aPrintSetup Pointer to the CPrintSetup
	 * @param aParam Xml parameter object
	 * @param aFileName Sets the file name if define the file name to xml,
	 *					Not set if not define the file name to xml
	 */
	virtual void InitPrintBandL(CPrintSetup* aPrintSetup,
								RXmlEngDocument* aParam,
								const TDesC& aFileName) = 0;

	/**
	 * Return the print band object
	 *
	 * @return MPageRegionPrinter* Pointer to the print band object
	 */
	virtual MPageRegionPrinter* RegionPrinter() = 0;

	/**
	 * Return the total page number
	 *
	 * @return TInt the total page number
	 */
	virtual TInt PageNum() = 0;

public:
	TUid iDtorIdKey;
	};

inline CDirectPrintBand* CDirectPrintBand::NewL(TUid aImplementationUid)
	{ // static
	TAny* any = REComSession::CreateImplementationL(aImplementationUid,
									 _FOFF(CDirectPrintBand, iDtorIdKey));
	CDirectPrintBand* plugin = reinterpret_cast<CDirectPrintBand*>(any);
	return plugin;
	}

inline CDirectPrintBand::~CDirectPrintBand()
	{
	REComSession::DestroyedImplementation(iDtorIdKey);
	}

#endif // DIRECTPRINTBAND_H
// End of file
