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
 *     The header file for tone fetcher engine private class of symbian platform.
 *     
 */

#ifndef TONEFETCHERENGINEPRIVATE_H
#define TONEFETCHERENGINEPRIVATE_H

//#include <e32base.h>
#include "MToneSelectionWatcher.h"
#include "MTonePlayingWatcher.h"
#include <QObject>
#include <QStringList>

class CToneSelection;
class CTonePlayer;

class ToneFetcherEnginePrivate : public QObject,                                 
                                 public MToneSelectionWatcher,
                                 public MTonePlayingWatcher
{
    Q_OBJECT
    
public:
    ToneFetcherEnginePrivate();
    virtual ~ToneFetcherEnginePrivate();
    void getTones();    
    void play( const QString &file );
    bool isPlaying();
    void stopPlaying();
    
public:
    //from MToneSelectionWatcher
    void HandleMdeSessionError( TInt aError );
    void HandleMdeSessionOpened();
    void HandleQueryError( TInt aError );
    void HandleQueryComplete( RPointerArray<TDesC>& aResultArray );
    void HandleObjectChanged();
    //from MTonePlayingWatcher
    void HandlePreviewEvent( TInt event );
    
signals:
    void mdeSessionOpened();
    void mdeSessionError(int error);
    void queryComplete(const QStringList& uriList);
    void queryError(int error);
    void notifyObjectChanged();  
    void notifyPreviewEvent(int event);

private:
    QStringList mResultList;
    CToneSelection* mToneSelection;
    CTonePlayer* mTonePlayer;
};
#endif /* TONEFETCHERENGINEPRIVATE_H */
