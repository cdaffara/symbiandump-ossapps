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
* Contributors:
*
* Description:  Simple Video Playback MXP view plugin
*
*/

#include <xqplugin.h>

#include "svpbdocumentloader.h"
#include "svpbengine.h"
#include "svpbplugin.h"
#include "svpbview.h"
#include "mpxhbvideocommondefs.h"
#include "trace.h"

static const QString NAME_DOCML = ":/resources/simplevideoplayback.docml";
static const QString NAME_VIEW = "view";

SvpbPlugin::SvpbPlugin() : mView(0), mEngine(0)
{
    FUNC_LOG;
}

SvpbPlugin::~SvpbPlugin()
{
    FUNC_LOG;
    destroyView();
}

void SvpbPlugin::createView()
{
    FUNC_LOG;

    if (!mView) {
        SvpbDocumentLoader loader;
        bool ok;
        loader.load(NAME_DOCML, &ok);

        if (ok) {
            mView = qobject_cast<SvpbView *>(loader.findWidget(NAME_VIEW));
            HANDLE_ERROR_NULL(mView);
            connect(mView, SIGNAL(finished()), SLOT(back()), Qt::QueuedConnection);

            mEngine = new SvpbEngine;
            HANDLE_ERROR_NULL(mEngine);
            connect(mEngine, SIGNAL(finished()), SLOT(back()), Qt::QueuedConnection);

            connect(mView, SIGNAL(tapped()), mEngine, SLOT(togglePause()));
            connect(mView, SIGNAL(longTapped()), mEngine, SLOT(stop()));
        }
        else {
            ERROR(QString("Unable to read ").append(NAME_DOCML));
        }
    }
}

void SvpbPlugin::destroyView()
{
    FUNC_LOG;

    deactivateView();

    delete mEngine; // disconnects any signals
    mEngine = 0;

    delete mView; // disconnects any signals
    mView = 0;
}

void SvpbPlugin::activateView()
{
    FUNC_LOG;

    if (mEngine && mView) {
        mEngine->connectMPX();
        mView->activate();
        mEngine->setSurfaceContainer(mView->surfaceContainer());
    }
}

void SvpbPlugin::deactivateView()
{
    FUNC_LOG;

    if (mEngine && mView) {
        mView->deactivate();
        mEngine->disconnectMPX();
    }
}

QGraphicsWidget* SvpbPlugin::getView()
{
    FUNC_LOG;

    return mView;
}

void SvpbPlugin::orientationChange(Qt::Orientation orientation)
{
    FUNC_LOG;
    Q_UNUSED( orientation );
}

void SvpbPlugin::back()
{
    FUNC_LOG;

    emit command(MpxHbVideoCommon::CollectionView);
}

XQ_EXPORT_PLUGIN2(simplevideoplayback, SvpbPlugin);
