/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of ShareUI STUB
*
*/

// Version : %version:  %

#include <qobject>

#include "shareui.h"
#include "mpxvideo_debug.h"


// ----------------------------------------------------------------------------
// ShareUi()
// ----------------------------------------------------------------------------
//
ShareUi::ShareUi()
{
    MPX_DEBUG(_L("ShareUi::ShareUi()"));
}

// ----------------------------------------------------------------------------
// ~ShareUi()
// ----------------------------------------------------------------------------
//
ShareUi::~ShareUi()
{
    MPX_DEBUG(_L("ShareUi::~ShareUi()"));
}

// ----------------------------------------------------------------------------
// send()
// ----------------------------------------------------------------------------
//
bool ShareUi::send( QStringList& fileList, bool embedded )
{
    MPX_DEBUG(_L("ShareUi::send()"));

    Q_UNUSED( fileList );
    Q_UNUSED( embedded );

    return true;
}
