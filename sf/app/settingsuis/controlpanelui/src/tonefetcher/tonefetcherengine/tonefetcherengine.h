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
 *     The header file for tone fetcher engine.
 *     
 */

#ifndef TONEFETCHERENGINE_H
#define TONEFETCHERENGINE_H
#include <QObject>
#include <QStringList>

class ToneFetcherEnginePrivate;

// CLASS DECLARATION
/**
 *  This class is used for interacting with platform based codes
 *  including fetching tones from MDE (Metadata Engine) and playing tones
 *  using platform-dependant interface.
 *
 */
class  ToneFetcherEngine : public QObject
{
    Q_OBJECT
    
public:    
    explicit ToneFetcherEngine( QObject* parent = 0 );
    ~ToneFetcherEngine();
    /*
     * search the tone using MDE, not including rom files.
     */
    void getTones();
    
    /*
     * preview the tone
     * @param file the absolute path of the file.
     */
    void play(const QString &file);
    
    bool isPlaying();
    
    /*
     * stop playing
     */
    void stopPlaying();
    
signals:
    void mdeSessionOpened();
    void mdeSessionError(int error);
    void queryComplete(const QStringList &uriList);
    void queryError(int error);
    void notifyPreviewEvent(int event);
    void notifyObjectChanged();
    
private:
    ToneFetcherEnginePrivate *d;
};
#endif
