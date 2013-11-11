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
 *     The source file for tone fetcher engine private class.
 *     
 */


#include "tonefetcherengine_symbian.h"
#include "CToneSelection.h"
#include "CTonePlayer.h"
#include "tonefetcherutils.h"
#include <XQConversions>

ToneFetcherEnginePrivate::ToneFetcherEnginePrivate()
    {
    TRAP_IGNORE( mToneSelection = CToneSelection::NewL( this ) ); 
    TRAP_IGNORE( mTonePlayer = CTonePlayer::NewL( this ) );    
    }

ToneFetcherEnginePrivate::~ToneFetcherEnginePrivate()
    {
    mResultList.clear();
    delete mToneSelection;
    delete mTonePlayer;
    }

void ToneFetcherEnginePrivate::getTones()
    {
    QT_TRAP_THROWING( mToneSelection->QueryTonesL() );
    }

void ToneFetcherEnginePrivate::play(const QString &file)
    {    
    QT_TRAP_THROWING( mTonePlayer->SetAttrL( XQConversions::qStringToS60Desc( ToneFetcherUtils::normalizeSeperator(file) )->Des() ) );
    QT_TRAP_THROWING( mTonePlayer->PlayL() );
    }

bool ToneFetcherEnginePrivate::isPlaying()
    {
    return mTonePlayer->IsPlaying();
    }

void ToneFetcherEnginePrivate::stopPlaying()
    {
    mTonePlayer->Stop();
    }

void ToneFetcherEnginePrivate::HandleMdeSessionError( TInt aError )
    {
    emit mdeSessionError( aError );
    }

void ToneFetcherEnginePrivate::HandleMdeSessionOpened()
    {
    emit mdeSessionOpened();
    }

void ToneFetcherEnginePrivate::HandleQueryError( TInt aError )
    {
    emit queryError(aError);
    }

void ToneFetcherEnginePrivate::HandleQueryComplete( RPointerArray<TDesC>& aResultArray )
    {
    mResultList.clear();
    for ( int i = 0; i < aResultArray.Count(); ++i ) 
        {
        mResultList.append( XQConversions::s60DescToQString( *(aResultArray[i]) ) );
        }
    aResultArray.ResetAndDestroy();
    emit queryComplete(mResultList);
    }

void ToneFetcherEnginePrivate::HandleObjectChanged()
    {
    emit notifyObjectChanged();
    }

void ToneFetcherEnginePrivate::HandlePreviewEvent( TInt event )
    {
    emit notifyPreviewEvent(event);
    }
