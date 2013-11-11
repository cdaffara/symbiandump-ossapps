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
#ifndef __DIRECTPRINTSELECTITEM_H__
#define __DIRECTPRINTSELECTITEM_H__

// System includes
#include <e32cmn.h>

// Class declaration
/**
 *  Select item information class
 *  more_complete_description
 */
class TDirectPrintSelectItem
	{
public:
	/** Id. */
	TInt iId;
	/** Name. */
	TBuf<50> iName;
	};

typedef RArray<TDirectPrintSelectItem> RSelectItemArray;

#endif // __DIRECTPRINTSELECTITEM_H__
