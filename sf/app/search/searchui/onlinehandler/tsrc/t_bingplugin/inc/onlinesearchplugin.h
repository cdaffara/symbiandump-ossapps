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
 * Description:  OnlineSearchPlugin class definition
 *
 */

#ifndef ONLINESEARCHPLUGIN_H
#define ONLINESEARCHPLUGIN_H

#include <QObject>

/**
 * @ingroup group_onlinesearchpluginmodel
 * @brief Represents a plugin plugin in the framework.
 *
 * OnlineSearchPlugin is an interface through which setting plugin plugin can be 
 * executed in the search handlers.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */

class OnlineSearchPlugin : public QObject
    {
Q_OBJECT
public:

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~OnlineSearchPlugin()
        {
        }

    /**
     * Called by the search handlers during initialize cycle of the plugin, it is during
     * this phase the plugin widget
     * usually creates its initial view.
     * Once the initialization is done, pluginInitialized signal must be 
     * emitted.
     * @since S60 ?S60_version.
     */
    virtual void initializePlugin() = 0;

    /**
     * Called by the search handlers when the plugin becomes the current plugin.  
     * @since S60 ?S60_version.
     */
    virtual void activatePlugin() = 0;
    /**
     * Called by the search handlers when the plugin becomes the current plugin.  
     * @since S60 ?S60_version.
     */
    virtual void activatePlugin(const QString &searchQuery) = 0;

    /**
     * Called by the search handlers when the plugin is no longer being displayed.  
     */
    virtual void deactivatePlugin() = 0;

    /**
     * Called by the search handlers before the plugin destructor is called.
     * @param reason The reason why the search handlers is being killed. 
     */
    virtual void shutdownPlugin() = 0;    

protected:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     */
    OnlineSearchPlugin()
        {
        }

signals:

    /**
     * Emit this signal when changing the view inside the plugin.
     * @param caller The calling plugin plugin instance.
     * @param viewWidget The new plugin view widget to be shown.
     */
    void viewChanged();

    /**
     * Emit this signal to indicate initialization is done. 
     * @param caller The calling plugin plugin instance.
     * @param success True if succeeded, false otherwise.
     */
    void pluginInitialized(bool success);

    /**
     * Emit this signal to indicate that the plugin plugin can be deleted. 
     * Note that this signal does not need to be emitted if true is returned
     * from shutdownPlugin.
     * @param caller The calling plugin plugin instance.
     */
    void shutdownCompleted(bool success);

    /**
     * Called by search handlers when back button is clicked in the plugin view.
     * In this method the plugin plugin can implement internally showing and
     * hiding GUI elements if the plugin has multiple 'views'.
     */
    void handleBackEvent();
    };

#endif // ONLINESEARCHPLUGIN_H
