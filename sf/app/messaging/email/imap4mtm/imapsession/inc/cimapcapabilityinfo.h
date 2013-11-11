// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CIMAPCAPABILITYINFO_H__
#define __CIMAPCAPABILITYINFO_H__

#include <e32base.h>

/**
Stores the capability information provided by the server.
@internalTechnology
@prototype
*/
class CImapCapabilityInfo : public CBase
	{
public:
	enum TImapCapabilityFlags {	

				EVersion4Rev1	= 0x1,
				EStartTls		= 0x2,	
				ELoginDisabled	= 0x4,
				EIdle			= 0x8,
				EAuthPlain		= 0x10,
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
				EBinaryCap		= 0x20,
				EAuthCramMd5	= 0x40,
				EAuthLogin		= 0x80,
#endif
				};

	IMPORT_C static CImapCapabilityInfo* NewL();
	~CImapCapabilityInfo();
   
	IMPORT_C void SetFlag(TImapCapabilityFlags aFlagId, TBool aValue);
	IMPORT_C TBool QueryFlag(const TImapCapabilityFlags aFlagId) const;

	// Not exported.
	void Reset();

private:
	CImapCapabilityInfo();

private:
	/**
	Bitwise combinaton of TImapCapabilityFlags.
	*/
	TUint8	iFlags;
	};
	
#endif // __CIMAPCAPABILITYINFO_H__
