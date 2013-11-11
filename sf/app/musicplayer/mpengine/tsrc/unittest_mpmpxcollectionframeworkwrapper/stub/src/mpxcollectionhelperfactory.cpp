/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: mpxcollectionuihelper stub for testing mpmpxframeworkwrapper
*
*/




#include <e32debug.h>

#include "stub/inc/mpxcollectionhelperfactory.h"
#include "stub/inc/mpxcollectionuihelper.h"


/*!
 Stub function.
 */
MMPXCollectionUiHelper* CMPXCollectionHelperFactory::NewCollectionUiHelperL(const TUid& aModeId)
{
    Q_UNUSED(aModeId);
    return new MMPXCollectionUiHelper();
}

//end of file
