// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSVER_H__
#define __AGSVER_H__

#include <e32cmn.h>

class RReadStream;
class RWriteStream;

class TAgnVersion : public TVersion
/** The version number of the agenda model, consisting of major, minor and build 
numbers.

An object of this class is returned by the CalFileVersionUtils function CurrentFileVersion(). 
This returns the version of model that was used to create the file. 
@internalAll
@released
*/
	{
public:
	inline TAgnVersion(TVersion aVersion) : TVersion(aVersion) {};
	inline TAgnVersion() {};
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	TBool operator==(const TAgnVersion) const;
	};
#endif
