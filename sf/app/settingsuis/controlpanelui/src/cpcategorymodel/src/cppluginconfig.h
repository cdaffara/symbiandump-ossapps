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
* Description:  An entry for a controlpanel plugin defined in cpcfg file.
*
*/

#ifndef CPPLUGINCONFIG_H
#define CPPLUGINCONFIG_H

#include <QString>

class CpPluginConfig
{
public:
    CpPluginConfig();
    void dump();
public:
    int mUid; //UID3 for the plugin 
    QString mDisplayName;
    QString mPluginFile; 
    QString mDescription;
};

#endif /* CPPLUGINCONFIG_H */
