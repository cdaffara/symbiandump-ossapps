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
// provides entry point for IMCM.DLL
// 
//

#include <e32std.h>
#include <bautils.h>	// Bafl::
#include <msvutils.h>
#include "MIUT_ERR.H"
#include "IMCMMAIN.H"
#include "MIUTHDR.H"	// resource file name
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "miut_errconsts.h"
#include "timrfc822datefield.h"
#endif

_LIT(KImcmPanic, "IMCM");


GLDEF_C void gPanic(TImcmPanic aPanic)
	{
	User::Panic(KImcmPanic,aPanic);
	}

GLDEF_C void OpenResourceFileL(RResourceFile& rFile, RFs& anFs)
	{
	// open resource file
	TFileName fileName(KImEngineResourceFile);
	MsvUtils::AddPathAndExtensionToResFileL(fileName);
	BaflUtils::NearestLanguageFile( anFs, fileName );	
	if (BaflUtils::FileExists(anFs,fileName))
 		{
		rFile.OpenL( anFs, fileName );		// Qualified: No leavescan error
 		return;
 		}
 	User::Leave(KErrNotFound);
	}

GLDEF_C void CloseResourceFile( TAny* file )
	{
	((RResourceFile*)file)->Close();
	}


/**
Place holder for removed export CImEmailSettings::SetVersion().

@internalComponent
@released
*/
EXPORT_C void DummyExport01() {}
