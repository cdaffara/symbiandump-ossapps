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
 * Description:Implementation of the search google online factory.
 *
 */

#include <QtGui> // For Q_EXPORT_PLUGIN2

#include "t_bingpluginfactory.h"
#include "t_bingplugin.h"


OnlineSearchPlugin* bingpluginFactory::createPlugin() const
{
	return new bingplugin(); 
}

Q_EXPORT_PLUGIN2(Bing,bingpluginFactory)
