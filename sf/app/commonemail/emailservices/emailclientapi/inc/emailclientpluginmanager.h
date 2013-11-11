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
* Contributors: Definition for plugin manager interface
*
* Description: 
*
*/

#ifndef MEMAILPLUGINMANAGER_H_
#define MEMAILPLUGINMANAGER_H_

#include "CFSMailCommon.h"

class MEmailClientPluginManager
    {
public:
    virtual CFSMailPlugin* GetPluginByUid(TUid aUid) = 0; 
    };

#endif // MEMAILPLUGINMANAGER_H_

// End of file

