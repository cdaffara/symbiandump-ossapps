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
#ifndef __DIRECTPRINTPROTOCOLINFO_H__
#define __DIRECTPRINTPROTOCOLINFO_H__

// System includes
#include <e32base.h>
#include <ecom/ImplementationInformation.h>

// Class declaration
/**
 *  Listbox class
 *  more_complete_description
 */
class CDirectPrintProtocolInfo : public CBase
	{
public:
	/** Constructors */
	static CDirectPrintProtocolInfo* NewLC(const CImplementationInformation& aImplInfo);
	/** Destructor */
	~CDirectPrintProtocolInfo();

	TUid ImplementationUid() const;

	const TDesC& DisplayName() const;

private:
	CDirectPrintProtocolInfo();

private:
	/** The implementation UID */
	TUid iImplementationUid;
	/** Display name */
	HBufC* iDisplayName;
	};

#endif // __DIRECTPRINTPROTOCOLINFO_H__
