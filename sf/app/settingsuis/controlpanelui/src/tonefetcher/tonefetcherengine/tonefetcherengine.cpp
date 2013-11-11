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
 *     The source file for tone fetcher engine.
 *     
 */
#include "tonefetcherengine.h"
#ifdef Q_OS_SYMBIAN
#include "tonefetcherengine_symbian.h"
#else
#include "tonefetcherengine_stub.h"
#endif

ToneFetcherEngine::ToneFetcherEngine(QObject* parent) : QObject(parent)
{
    d = new ToneFetcherEnginePrivate();
    Q_ASSERT(d);
    
    connect(d, SIGNAL(mdeSessionOpened()), 
            this, SIGNAL(mdeSessionOpened()));
    connect(d, SIGNAL(mdeSessionError(int)),
            this, SIGNAL(mdeSessionError(int)));
    connect(d, SIGNAL(queryComplete(QStringList)),
            this, SIGNAL(queryComplete(QStringList)));
    connect(d, SIGNAL(queryError(int)),
            this, SIGNAL(queryError(int)));
    connect(d, SIGNAL(notifyObjectChanged()),
            this, SIGNAL(notifyObjectChanged()));    
    connect(d, SIGNAL(notifyPreviewEvent(int)),
            this, SIGNAL(notifyPreviewEvent(int)));
}

ToneFetcherEngine::~ToneFetcherEngine()
{
    delete d;
}

void ToneFetcherEngine::getTones()
{
    d->getTones();
}

void ToneFetcherEngine::play(const QString &file)
{    
    d->play(file);
}

bool ToneFetcherEngine::isPlaying()
{
    return d->isPlaying();
}

void ToneFetcherEngine::stopPlaying()
{
    d->stopPlaying();
}

//End of File
