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
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTPRINTERDATA_H__
#define __DIRECTPRINTPRINTERDATA_H__

// System includes
#include <e32cmn.h>

// User includes
#include "DirectPrintDef.h"

// Forward declarations
class RReadStream;
class RWriteStream;

// Class declaration
/**
 *  Printer data class
 *  more_complete_description
 */
class TDirectPrintPrinterData
	{
public:
	/**
	 * Internalize operation.
	 *
	 * @param aStream ReadStream.
	 */
	IMPORT_C void InternalizeL(RReadStream& aStream);

	/**
	 * Externalize operation.
	 *
	 * @param aStream WriteStream.
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream)const;

	/**
	 * Substitution operator.
	 *
	 * @param aData Printer data.
	 * @return Printer data.
	 */
	IMPORT_C TDirectPrintPrinterData& operator=(const TDirectPrintPrinterData& aData);

public:
	/** The ID of printer data. */
	TInt iPrinterId;
	/** Name of printer. */
	TBuf<KDirectPrintNameLength> iPrinterName;
	/** The UID of printer driver. */
	TUid iPrinterDriver;
	/** Name of printer driver. */
	TBuf<KDirectPrintNameLength> iDriverName;
	/** The ID of printer type. */
	TInt iPrinterType;
	/** Name of printer type. */
	TBuf<KDirectPrintNameLength> iTypeName;
	};

typedef RArray<TDirectPrintPrinterData> RDPPrinterDataArray;

#endif // __DIRECTPRINTPRINTERDATA_H__
