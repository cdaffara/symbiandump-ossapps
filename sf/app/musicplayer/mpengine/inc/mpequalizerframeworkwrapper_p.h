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
* Description: Wrapper for equalizer framework - private implementation.
*
*/

#ifndef MPEQUALIZERFRAMEWORKWRAPPER_P_H
#define MPEQUALIZERFRAMEWORKWRAPPER_P_H

// System includes
#include <e32base.h>
#include <mdaaudiosampleplayer.h>

// FORWARD DECLARATIONS
class CAudioEqualizerUtility;

class MpEqualizerFrameworkWrapperPrivate : public CBase,
                                           public MMdaAudioPlayerCallback
    {
public:

    MpEqualizerFrameworkWrapperPrivate( MpEqualizerFrameworkWrapper *wrapper );
    virtual ~MpEqualizerFrameworkWrapperPrivate();

    void init();
    int getPresetNameKey( int presetIndex );
    int getPresetIndex( int presetNameKey );
    QStringList presetNames();

private:

    void DoInitL();
    TInt DoGetPresetNameKeyL( const TInt presetIndex ) const; 
    void MapcInitComplete( TInt aError, 
                           const TTimeIntervalMicroSeconds& aDuration );
    void MapcPlayComplete( TInt aError );
    
private: 

    MpEqualizerFrameworkWrapper *q_ptr;    
    CMdaAudioPlayerUtility* mMdaPlayUtility;
    CAudioEqualizerUtility* mEqualizerUtility;
    };

#endif // MPEQUALIZERFRAMEWORKWRAPPER_P_H

//End of File
