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
 * Description:  Online Handler.
 *
 */
#include "onlinehandler.h"
#include "searchutility.h"
#include "onlinesearchplugin.h"  // after export should be included as systeminc
#include "onlinesearchpluginfactory.h"  // after export should be included as systeminc
#include <QPluginLoader>
#ifdef Q_OS_SYMBIAN
#include <QLibraryInfo>
#else
#include <QCoreApplication>
#endif
#include <QDir>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbinstance.h>
#include <hbicon.h>
#include <serviceprovider.h>
#include <isengine.h>

#ifdef WIN32
#define PLUGINFILE_SUFFIX "dll"
#else
#define PLUGINFILE_SUFFIX "qtplugin"
#endif

// ---------------------------------------------------------------------------
// OnlineHandler::OnlineHandler
// ---------------------------------------------------------------------------
//
OnlineHandler::OnlineHandler(QObject* parent) :
    QObject(parent), mOnlinePlugin(NULL), mEng(NULL), mCurrentProvider(NULL)
    {
    }
// ---------------------------------------------------------------------------
// OnlineHandler::~OnlineHandler
// ---------------------------------------------------------------------------
//
OnlineHandler::~OnlineHandler()
    {
    if (mOnlinePlugin)
        delete mOnlinePlugin;
    if (mEng)
        delete mEng;
    if (mCurrentProvider)
        delete mCurrentProvider;
    }
void OnlineHandler::setDefaultProvider(const int id)
    {
    if (mEng)
        mEng->SetSelectedProvider(id);
    }
// ---------------------------------------------------------------------------
// OnlineHandler::readSettings
// ---------------------------------------------------------------------------
//
void OnlineHandler::readSettings()
    {
    if (!mEng)
        mEng = new IsEngine();
    mAvailableProviders.clear();
    mEng->Providers(mAvailableProviders);
    for (int i = 0; i < mAvailableProviders.count(); i++)
        {
        emit providerDetails(mAvailableProviders.at(i).Name(),
                mAvailableProviders.at(i).Icon(),
                mAvailableProviders.at(i).Id());
        }
    mCurrentProvider = mEng->SelectedProvider(); // default provider
    if (mCurrentProvider)
        {
        mDefaultPluginName = mCurrentProvider->Name();
        emit defaultProvider(mCurrentProvider->Id());
        }
    else
        {
        mDefaultPluginName = QString();
        }
    }
// ---------------------------------------------------------------------------
// OnlineHandler::writeSettings
// ---------------------------------------------------------------------------
//
void OnlineHandler::writeSettings(const int providerId)
    {
    if (mEng && mEng->SetSelectedProvider(providerId) && mCurrentProvider)
        {
        mCurrentProvider = mEng->SelectedProvider(); // default provider
        emit defaultProvider(mCurrentProvider->Id());
        }
    return;
    }
// ---------------------------------------------------------------------------
// OnlineHandler::initializePlugin
// ---------------------------------------------------------------------------
//
void OnlineHandler::initializePlugin()
    {
    /* Below assumes, only default plugins are handled now. If user selects different plugin some more logic needed here. */

    if (mOnlinePlugin)
        {
        delete mOnlinePlugin;
        mOnlinePlugin = NULL;
        }
    mOnlinePlugin = loadSearchPlugin(mDefaultPluginName);
    if (mOnlinePlugin)
        {
        connect(mOnlinePlugin, SIGNAL(pluginInitialized(bool)), this,
                SLOT(isPluginIntialized(bool)));

        connect(mOnlinePlugin, SIGNAL(shutdownCompleted(bool)), this,
                SLOT(isShutdownCompleted(bool)));

        connect(mOnlinePlugin, SIGNAL(handleBackEvent()), this,
                SLOT(backEvent()));

        mOnlinePlugin->initializePlugin();
        }
    }
void OnlineHandler::loadISPlugin(int id, QString query)
    {
    if (!mEng)
        mEng = new IsEngine();
    mLoadSuggestion = true;
    mLoadSuggestionLink = query;
    QList<ServiceProvider> iAvailableProviders;
    mEng->Providers(iAvailableProviders);
    for (int i = 0; i < iAvailableProviders.count(); i++)
        {
        if (iAvailableProviders.at(i).Id() == id)
            {
            mDefaultPluginName = iAvailableProviders.at(i).Name();
            break;
            }
        }
    initializePlugin();
    }
// ---------------------------------------------------------------------------
// OnlineHandler::isPluginIntialized
// ---------------------------------------------------------------------------
//
void OnlineHandler::isPluginIntialized(bool success)
    {
    if (mLoadSuggestion)
        {
        activatePlugin(mLoadSuggestionLink);
        mLoadSuggestion = false;
        }
    else
        {
        emit pluginIntialized(success);
        }
    }
// ---------------------------------------------------------------------------
// OnlineHandler::isShutdownCompleted
// ---------------------------------------------------------------------------
//
void OnlineHandler::isShutdownCompleted(bool success)
    {
    mOnlinePlugin->deleteLater();
    emit pluginShutdown(success);
    }
// ---------------------------------------------------------------------------
// OnlineHandler::backEvent
// ---------------------------------------------------------------------------
//
void OnlineHandler::backEvent()
    {
    emit backEventTriggered();
    }
// ---------------------------------------------------------------------------
// OnlineHandler::activatePlugin
// ---------------------------------------------------------------------------
//
bool OnlineHandler::activatePlugin()
    {
    if (mOnlinePlugin)
        mOnlinePlugin->activatePlugin();
    return true;
    }
// ---------------------------------------------------------------------------
// OnlineHandler::activatePlugin
// ---------------------------------------------------------------------------
//
bool OnlineHandler::activatePlugin(const QString &str)
    {
    if (mOnlinePlugin)
        mOnlinePlugin->activatePlugin(str);
    return true;
    }
// ---------------------------------------------------------------------------
// OnlineHandler::unloadPlugin
// ---------------------------------------------------------------------------
//
void OnlineHandler::unloadPlugin()
    {
    if (mOnlinePlugin)
        {
        disconnect(mOnlinePlugin, SIGNAL(pluginInitialized(bool)), this,
                SLOT(isPluginIntialized(bool)));

        disconnect(mOnlinePlugin, SIGNAL(shutdownCompleted(bool)), this,
                SLOT(isShutdownCompleted(bool)));

        disconnect(mOnlinePlugin, SIGNAL(handleBackEvent()), this,
                SLOT(backEvent()));

        mOnlinePlugin->deactivatePlugin();
        }
    }
// ---------------------------------------------------------------------------
// OnlineHandler::unloadPluginandExit
// ---------------------------------------------------------------------------
//
void OnlineHandler::unloadPluginandExit()
    {
    if (mOnlinePlugin)
        {
        disconnect(mOnlinePlugin, SIGNAL(pluginInitialized(bool)), this,
                SLOT(isPluginIntialized(bool)));

        disconnect(mOnlinePlugin, SIGNAL(shutdownCompleted(bool)), this,
                SLOT(isShutdownCompleted(bool)));

        disconnect(mOnlinePlugin, SIGNAL(handleBackEvent()), this,
                SLOT(backEvent()));

        mOnlinePlugin->deactivatePlugin();
        mOnlinePlugin->shutdownPlugin();
        }
    }
// ---------------------------------------------------------------------------
// OnlineHandler::loadSearchPlugin
// ---------------------------------------------------------------------------
//
OnlineSearchPlugin *OnlineHandler::loadSearchPlugin(const QString &pluginFile)
    {
    //   CpLogger logger;
    //   logger << "loading plugin:" << pluginFile << "\r\n";

    QFileInfo fileInfo(pluginFile);

    if (!fileInfo.isAbsolute())
        {
        QString fileName = fileInfo.fileName();
        if (fileInfo.suffix().compare(PLUGINFILE_SUFFIX, Qt::CaseInsensitive))
            {
            fileName = fileInfo.baseName() + '.' + PLUGINFILE_SUFFIX;
            }

        QStringList pluginDirs = SearchUtility::pluginDirs();
        foreach(const QString &pluginDir,pluginDirs)
                {
                fileInfo.setFile(pluginDir + fileName);
                if (fileInfo.exists() && QLibrary::isLibrary(
                        fileInfo.absoluteFilePath()))
                    {
                    break;
                    }
                }
        }

    QPluginLoader loader(fileInfo.absoluteFilePath());
    OnlineSearchPluginFactory *pluginFactory = qobject_cast<
            OnlineSearchPluginFactory*> (loader.instance());
    OnlineSearchPlugin *plugin = pluginFactory->createPlugin();
    /*
     * why unload?
     if (!plugin) {
     loader.unload();
     }
     */
    //   logger << (plugin ? "load plugin succeed." : "load plugin failed.") << "\r\n";    
    return plugin;
    }

