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

#include <s32strm.h>
#include "agsver.h"

void TAgnVersion::InternalizeL(RReadStream& aStream)
/** Internalises the agenda model version object from a read stream. 

 The presence of this function means that the standard templated operator>>() is 
 available to internalise objects of this class.

 @internalAll
 @param aStream Stream from which the object should be internalised. */
	{

	aStream >> iMajor;
	aStream >> iMinor;
	aStream >> iBuild;
	}

void TAgnVersion::ExternalizeL(RWriteStream& aStream) const
/** Externalises the agenda model version object to a write stream. 

 The presence of this function means that the standard templated operator<<() 
 is available to externalise objects of this class.

 @internalAll
 @param aStream Stream to which the object should be externalised. */
	{

	aStream << iMajor;
	aStream << iMinor;
	aStream << iBuild;
	}

/** Compares two agenda versions for equality.
 @internalAll
 @param aVersion The version to compare with this one.
 @return True if aVersion is the same as this. False if they are different. */
TBool TAgnVersion::operator==(TAgnVersion aVersion) const
	{
	return (iMajor == aVersion.iMajor && iMinor == aVersion.iMinor && iBuild == aVersion.iBuild);
	}
