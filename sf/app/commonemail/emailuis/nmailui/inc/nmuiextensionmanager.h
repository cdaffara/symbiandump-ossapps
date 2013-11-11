/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMUIEXTENSIONMANAGER_H_
#define NMUIEXTENSIONMANAGER_H_

#include <QList>

#include "nmuidef.h"

class NmAction;
class NmActionRequest;
class NmUiExtensionInterface;
class QPluginLoader;


class NMUI_EXPORT NmUiExtensionManager
{
public:

    NmUiExtensionManager();
    virtual ~NmUiExtensionManager();

    void getActions(
        const NmActionRequest &menuRequest,
        QList<NmAction*> &actionList);

private:
    void loadExtensionPlugins();
    QStringList pluginFolders();

    QList<NmUiExtensionInterface*> mExtensions;
    QList<QPluginLoader *> mPluginLoaders;

};


#endif /* NMUIEXTENSIONMANAGER_H_ */
