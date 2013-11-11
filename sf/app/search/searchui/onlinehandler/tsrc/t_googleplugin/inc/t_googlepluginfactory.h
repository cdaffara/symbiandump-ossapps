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
 * Description:    Declaration of the search google online factory.
 *
 */

#ifndef GOOGLEPLUGINFACTORY_H
#define GOOGLEPLUGINFACTORY_H

#include <QObject>

#include <OnlineSearchPluginfactory.h>

class OnlineSearchPlugin;


class googlepluginFactory : public QObject, public OnlineSearchPluginFactory
{
	Q_OBJECT
	Q_INTERFACES(OnlineSearchPluginFactory)

public:
    
     // Wizard factory
	OnlineSearchPlugin* createPlugin() const;
};

#endif // GOOGLEPLUGINFACTORY_H
