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
#ifndef __DIRECTPRINTPROPERTY_H__
#define __DIRECTPRINTPROPERTY_H__

// System includes
#include <e32base.h>
#include <e32cmn.h>

// User includes
#include "DirectPrintDef.h"
#include "directprintpropertydata.h"

// Class declaration
/**
 *  Printer property class
 *  more_complete_description
 */
class CPrinterProperty : public CBase
	{
public:
	/** Constructors */
	IMPORT_C CPrinterProperty();
	/** Destructor */
	IMPORT_C ~CPrinterProperty();

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
	 * @param aProperty Printer property.
	 * @return Printer property.
	 */
	IMPORT_C CPrinterProperty& operator=(const CPrinterProperty& aProperty);

public:
	/** The ID of printer data. */
	TInt iPrinterId;
	/** The ID of application. */
	TInt iApplicationId;
	/** Printer property data array. */
	RDPPropertyDataArray iPropertyItems;
	};

typedef RPointerArray<CPrinterProperty> RPrinterPropertyArray;

#endif // __DIRECTPRINTPROPERTY_H__
