// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// IMPSMAIN.CPP 
//

#include <e32std.h>
#include <barsc.h>		// RResourceFile
#include <bautils.h>	// Bafl::
#include <msvutils.h>
#include <miuthdr.h>
#include "impspan.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"
#endif

GLDEF_C void gPanic(TImpsPanic aPanic)
	{
	User::Panic(_L("IMPS"),aPanic);
	}

GLDEF_C void OpenResourceFileL(RResourceFile& rFile, RFs& anFs)
	{
//	NB need to check on all drives - won't necessarily be on
	//	the same one as IMPS.DLL
	//
	TFileName fileName(KImEngineResourceFile);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
	BaflUtils::NearestLanguageFile( anFs, fileName );
	if (BaflUtils::FileExists(anFs,fileName))
		{
		rFile.OpenL(anFs,fileName);		// Qualified: no leavescan error
		return;
		}
	User::Leave(KErrNotFound);
	}

GLDEF_C void CloseResourceFile( TAny* file )
	{
	((RResourceFile*)file)->Close();
	}
