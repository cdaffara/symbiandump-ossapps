/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  audio effect properties stub for testing mpmpxasframeworkwrapper 
*
*/
 
#ifndef CMPXAUDIOEFFECTPROPERTIES_H
#define CMPXAUDIOEFFECTPROPERTIES_H

#include <e32base.h>

class CMPXAudioEffectProperties : public CBase
    {
public: 
    explicit CMPXAudioEffectProperties();   
    virtual ~CMPXAudioEffectProperties();

public:
    
    // Stub functions
    TInt Balance();
    TBool Loudness();
    void SetBalance(TInt aBalance);
    void SetLoudness(TBool aLoudness);
    void Reset();
    void SaveToFileL();
    void LoadFromFileL();

public:
    
    TInt    iBalance;
    TBool   iLoudness;
    TBool   iLoadFromFile;
    TBool   iSaveToFile;
    TBool   iLeave;
};

#endif // CMPXAUDIOEFFECTPROPERTIES_H
