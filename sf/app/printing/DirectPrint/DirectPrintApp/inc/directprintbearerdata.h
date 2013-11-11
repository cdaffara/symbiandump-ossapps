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
#ifndef __DIRECTPRINTBEARERDATA_H__
#define __DIRECTPRINTBEARERDATA_H__

// System includes
#include <e32base.h>
#include <in_sock.h>

// User includes
#include "DirectPrintDef.h"

// Forward declarations
class RReadStream;
class RWriteStream;

// Class declaration
/**
 *  Bearer data class
 *  more_complete_description
 */
class TDirectPrintBearerData
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
	 * @param aData Bearer data.
	 * @return Bearer data.
	 */
	IMPORT_C TDirectPrintBearerData& operator=(const TDirectPrintBearerData& aData);

public:
	/** The ID of printer data. */
	TInt iPrinterId;
	/** The ID of bearer data. */
	TInt iBearerId;
	/** Name of bearer. */
	TBuf<KDirectPrintNameLength> iBearerName;
	/** AccessPoint. */
	TBuf<KDirectPrintNameLength> iAccessPoint;
	/** HostName. */
	TBuf<KDirectPrintNameLength> iHostName;
	/** IPAddress of host. */
	TInetAddr iHostIp;
	/** Number of port. */
	TInt iPort;
	/** UserName. */
	TBuf<KDirectPrintNameLength> iUserName;
	/** Name of que. */
	TBuf<KDirectPrintNameLength> iQueName;
	};

typedef RArray<TDirectPrintBearerData> RDPBearerDataArray;

#endif // __DIRECTPRINTBEARERDATA_H__
