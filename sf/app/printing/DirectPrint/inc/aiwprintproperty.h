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
*
*/

// Protection against nested includes
#ifndef __AIWPRINTPROPERTY_H__
#define __AIWPRINTPROPERTY_H__

const TUid KPrintPropertyCat = {0x10012345};
enum TPrintPropertyKeys
    { 
    EPrintProperty,
    };

enum 
	{
	ENotAiwCall,
	EAiwPrintCall,
	EAiwQuickPrintCall
	};

#endif  // __AIWPRINTPROPERTY_H__
