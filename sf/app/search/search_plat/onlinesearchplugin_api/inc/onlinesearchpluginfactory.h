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
 * Description:  OnlineSearchPluginFactory class definition
 *
 */

#ifndef ONLINESEARCHPLUGINFACTORY_H
#define ONLINESEARCHPLUGINFACTORY_H

/**
 * @file
 * @ingroup Common
 * @brief Provides online search plugin factory method interface for loading the actual plugin.
 */

class OnlineSearchPlugin;

/**
 * @ingroup group_onlinesearchpluginmodel
 * @brief A factory class through which the wizard plugin can be instantiated.
 *
 * OnlineSearchPluginFactory contains a factory method to create a wizard plugin instance.
 * 
 * @lib ?library
 * @since S60 ?S60_version
 */

class OnlineSearchPluginFactory
{
public:
    /**
     * Instantiates a wizard plugin object.
     * @return The constructed wizard plugin object.
     * @since S60 ?S60_version.
     */    
	   virtual OnlineSearchPlugin* createPlugin() const = 0;
};

Q_DECLARE_INTERFACE(OnlineSearchPluginFactory, "com.nokia.search.OnlineSearchPluginFactory/1.0");

#endif // ONLINESEARCHPLUGIN_H
