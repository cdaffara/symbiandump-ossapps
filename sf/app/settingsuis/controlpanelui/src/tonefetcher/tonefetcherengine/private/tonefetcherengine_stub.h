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
 *     The header file for tone fetcher engine private class of other platforms.
 *     
 */

#ifndef TONEFETCHERENGINE_STUB_H
#define TONEFETCHERENGINE_STUB_H

#include <QObject>

class ToneFetcherEnginePrivate : public QObject
{
    Q_OBJECT
    
public:
    ToneFetcherEnginePrivate();
    virtual ~ToneFetcherEnginePrivate();
    void getTones();    
    void play( const QString & );
    bool isPlaying();
    void stopPlaying();
};

#endif /* TONEFETCHERENGINE_STUB */
