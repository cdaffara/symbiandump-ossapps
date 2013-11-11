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
* Description:  Container of properties for audio effect
*
*/

 

#ifndef CMPXAUDIOEFFECTPROPERTIES_H
#define CMPXAUDIOEFFECTPROPERTIES_H


#include <e32base.h>
#include <s32strm.h>


/**
*  Supports 'standard' a attributes. Streamable object
*/
class CMPXAudioEffectProperties : public CBase
    {
public: 
    inline virtual ~CMPXAudioEffectProperties();
public:
    inline TInt Balance() const;
    inline TInt Reverb() const;
    inline TBool Stereo() const;  
    inline TBool BassBoost() const;
    inline TBool Loudness() const;

public:
    inline void SetBalance(TInt aBalance);
    inline void SetReverb(TInt aReverb);
    inline void SetStereo(TBool aStereo);  
    inline void SetBassBoost(TBool aBassBoost);  
    inline void SetLoudness(TBool aLoudness);  
    inline void Reset();
public:
    inline void ExternalizeL(RWriteStream& aStream) const;
    inline void InternalizeL(RReadStream& aStream);
    inline void SaveToFileL() const;
    inline void LoadFromFileL();
private:
    enum TDesProperty 
        { 
        //EDesPresetName, // Not used yet. EQ
        EDesNumItems = 0
        };

    enum TIntProperty 
        { 
        EIntBalance, //(From -100 to 100, 0 = center)
        EIntReverb, //(from 1 to 8, 0 = default)
        EIntStereo, //(default off)
        EIntBassBoost, //(default off)
        EIntLoudness, //(default off)
        EIntNumItems 
        };
private:
    inline const TDesC& Value(TDesProperty aProperty) const;
    inline TInt Value(TIntProperty aProperty) const;
    inline void SetValue(TDesProperty aProperty,const TDesC& aValue);
    inline void SetValue(TIntProperty aProperty,TInt aValue);
private:
    //TFixedArray<HBufC*,EDesNumItems> iDesValues;
    TFixedArray<TInt,EIntNumItems> iIntValues;
    };

#include "mpxaudioeffectproperties.inl"

#endif // CMPXAUDIOEFFECTPROPERTIES_H
