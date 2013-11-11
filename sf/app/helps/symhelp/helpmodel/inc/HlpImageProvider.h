// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HLPIMAGEPROVIDER_H__
#define __HLPIMAGEPROVIDER_H__

// System includes
#include <e32std.h>

// User includes
#include "hlpzoom.h"

// Classes referenced
class CFbsBitmap;


//
// ----> MHlpImageProvider (header)
//
class MHlpImageProvider
	{
public: // From MHlpImageProvider
	virtual CFbsBitmap* ImageForIdLC(TInt aImageId, MHlpZoomStateManager::THlpZoomState aZoomState) = 0;
	};


#endif
