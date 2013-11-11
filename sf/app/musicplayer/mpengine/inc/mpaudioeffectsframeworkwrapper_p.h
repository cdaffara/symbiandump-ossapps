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
* Description: Framework wraper for Audio Effects - Private.
*
*/

#ifndef MPAUDIOEFFECTSFRAMEWORKWRAPPER_P_H
#define MPAUDIOEFFECTSFRAMEWORKWRAPPER_P_H

class CMPXAudioEffectProperties;

class MpAudioEffectsFrameworkWrapperPrivate 
{
public:
    
    explicit MpAudioEffectsFrameworkWrapperPrivate(); 
    virtual ~MpAudioEffectsFrameworkWrapperPrivate();
    
public:
    
    void init();
    void setBalance( int balance );
    void setLoudness( bool mode );
    int  balance();
    bool loudness();
            
private:
    
    void doInitL();
    void saveToFile();
             
private: // data
    
    CMPXAudioEffectProperties     *mAudioEffectProperties; // owned
    
};

#endif /*MPAUDIOEFFECTSFRAMEWORKWRAPPER_P_H*/
