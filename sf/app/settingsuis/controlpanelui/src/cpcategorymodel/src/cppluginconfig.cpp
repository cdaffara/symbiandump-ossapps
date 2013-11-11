/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  An entry for a plugin defined in cpcfg file.
*
*/

#include "cppluginconfig.h"
#include <cplogger.h>

CpPluginConfig::CpPluginConfig() : 
    mUid(0),
    mDisplayName(QString()),
    mPluginFile(QString()),
    mDescription(QString())
{

}

void CpPluginConfig::dump()
{   
    CPFW_LOG(QLatin1String("id = ") + QString("0x%1").arg(mUid,0,16));
    CPFW_LOG(QLatin1String("diplayname = ") + mDisplayName);
    CPFW_LOG(QLatin1String("dll = ") + mPluginFile);
    CPFW_LOG(QLatin1String("desc = ") + mDescription);
}
