/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include "cpreviewinfo.h"

// Default constructor
CPreviewInfo::CPreviewInfo(
    TInt aCapabilityUid,
    TBool aPaperLandscape )
    {
    iUid = aCapabilityUid;
    iPaperLandscape = aPaperLandscape;

    // No, sticker templates are not used by default
    iOneImagePerPage = EFalse;
    }

// Default destructor
CPreviewInfo::~CPreviewInfo()
    {
    iImageInfo.ResetAndDestroy();
    }

//  End of File
