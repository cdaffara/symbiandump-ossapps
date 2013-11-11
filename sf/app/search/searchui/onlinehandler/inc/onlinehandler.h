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
 * Description:  Onlinehandler.
 *
 */

#ifndef ONLINEHANDLER_H
#define ONLINEHANDLER_H

#include <QObject>
#include <QVector>
#include <QDir>
#include <hbicon.h>
#include "search_global.h"
class HbMainWindow;
class HbView;
class OnlineSearchPlugin;
class IsEngine;
class ServiceProvider;
class HbIcon;

#ifdef SEARCHONLINEHANDLER_LIB
#define ONLINEHANDLER_EXPORT Q_DECL_EXPORT
#else
#define ONLINEHANDLER_EXPORT Q_DECL_IMPORT
#endif

SEARCH_CLASS(SearchOnlineHandlerTest)
/**
 * @ingroup group_OnlineHandler
 * @brief Loads and connects onlinesearchprovider plugins
 *
 * This service is responsible for loading onlinesearchprovider plugins.
 *
 * @since S60 ?S60_version.
 */
class ONLINEHANDLER_EXPORT OnlineHandler : public QObject
    {
Q_OBJECT

public:
    /**
     * Default constructor.
     *
     * @since S60 ?S60_version.
     */
    OnlineHandler(QObject* = 0);

    /**
     * Destructor.
     *
     * @since S60 ?S60_version.
     */
    virtual ~OnlineHandler();
    void setDefaultProvider(const int);

    void isProvidersAvailable();
    void initializePlugin();    
    bool activatePlugin();
    bool activatePlugin(const QString &str);

    void readSettings();
    void writeSettings(const int);
    
    void loadISPlugin(int id, QString query);

    void unloadPlugin();
    void unloadPluginandExit();
public slots:
    void isPluginIntialized(bool);
    void isShutdownCompleted(bool);
    void backEvent();

signals:
    void providerDetails(QString, HbIcon, int);
    void defaultProvider(const int);
    void pluginIntialized(bool);
    void pluginShutdown(bool);
    void backEventTriggered();
private:
    OnlineSearchPlugin *loadSearchPlugin(const QString &pluginFile);

private:
    QString mDefaultPluginName;
    OnlineSearchPlugin *mOnlinePlugin;
    IsEngine* mEng;
    ServiceProvider* mCurrentProvider;
    QList<ServiceProvider> mAvailableProviders;
    bool mLoadSuggestion;
    QString mLoadSuggestionLink;
SEARCH_FRIEND_CLASS    (SearchOnlineHandlerTest)
    };
#endif
