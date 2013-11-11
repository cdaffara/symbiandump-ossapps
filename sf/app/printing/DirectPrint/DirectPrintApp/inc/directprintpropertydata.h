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
#ifndef __DIRECTPRINTPROPERTYDATA_H__
#define __DIRECTPRINTPROPERTYDATA_H__

// System includes
#include <e32base.h>
#include <e32cmn.h>

// User includes
#include "DirectPrintDef.h"

// Class declaration
/**
 *  Printer property data class
 *  more_complete_description
 */
class TDirectPrintPropertyData
	{
public:
	/**
	 * Internalize operation.
	 *
	 * @param aStream ReadStream.
	 */
	inline void InternalizeL(RReadStream& aStream);

	/**
	 * Externalize operation.
	 *
	 * @param aStream WriteStream.
	 */
	inline void ExternalizeL(RWriteStream& aStream)const;

	/**
	 * Substitution operator.
	 *
	 * @param aData Printer property data.
	 * @return Printer property data.
	 */
	inline TDirectPrintPropertyData& operator=(const TDirectPrintPropertyData& aData);

public:
	/** The ID of printer data. */
	TInt iPrinterId;
	/** The ID of application. */
	TInt iApplicationId;
	/** The ID of item type. */
	TInt iItemType;
	/** The ID of item. */
	TInt iItemId;
	/** Name of item. */
	TBuf<KDirectPrintNameLength> iItemName;
	/** Numerical value. */
	TInt iItemValue;
	/** Text value. */
	TBuf<KDirectPrintNameLength> iItemTextValue;
	};

typedef RArray<TDirectPrintPropertyData> RDPPropertyDataArray;



inline void TDirectPrintPropertyData::InternalizeL(RReadStream& aStream)
	{
	iItemType = aStream.ReadInt32L();
	iItemId = aStream.ReadInt32L();
	TInt length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iItemName, length);
		}
	iItemValue = aStream.ReadInt32L();
	length = aStream.ReadInt32L();
	if(length > 0)
		{
		aStream.ReadL(iItemTextValue, length);
		}
	}

inline void TDirectPrintPropertyData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iItemType);
	aStream.WriteInt32L(iItemId);
	aStream.WriteInt32L(iItemName.Length());
	if (iItemName.Length() > 0)
		{
		aStream.WriteL(iItemName);
		}
	aStream.WriteInt32L(iItemValue);
	aStream.WriteInt32L(iItemTextValue.Length());
	if (iItemTextValue.Length() > 0)
		{
		aStream.WriteL(iItemTextValue);
		}
	}

inline TDirectPrintPropertyData& TDirectPrintPropertyData::operator=(const TDirectPrintPropertyData& aData)
	{
	iPrinterId = aData.iPrinterId;
	iApplicationId = aData.iApplicationId;
	iItemType = aData.iItemType;
	iItemId = aData.iItemId;
	iItemName.Copy(aData.iItemName);
	iItemValue = aData.iItemValue;
	iItemTextValue.Copy(aData.iItemTextValue);
	return *this;
	}

#endif // __DIRECTPRINTPROPERTYDATA_H__
