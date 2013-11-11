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

#ifndef PBAPXML_H
#define PBAPXML_H

#include <e32base.h>

// forward declarations
class RWriteStream;
class RBuf8;

/**
simple utility class used for writing parts of vCard listing DTD to stream
*/
class PbapDTD
	{
public:
	static void WriteBeginL(RWriteStream& aWriteStream);
	static void WriteListingEntryL(RWriteStream& aWriteStream, TInt aHandle, const TDesC& aName);
	static void WriteEndL(RWriteStream& aWriteStream);

private:
	static void EscapeAttributeValueL(RBuf8& aValue);	
	};

#endif //PBAPXML_H
