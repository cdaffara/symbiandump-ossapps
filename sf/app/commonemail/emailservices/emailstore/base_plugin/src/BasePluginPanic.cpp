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
* Description:   Module panics implementation.
*
*/


#include <e32std.h>
#include "BasePluginPanic.h"

_LIT( KBasePluginCategory, "BasePlugin" );

/**
 *
 */
GLDEF_C void BasePluginPanic( TBasePluginPanic aPanic )
    {
    User::Panic( KBasePluginCategory, aPanic );
    }
