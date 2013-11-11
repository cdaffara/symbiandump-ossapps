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
* Description: stub for MpAudioEffectsFrameworkWrapper
*
*/

#ifndef MPAUDIOEFFECTSFRAMEWORKWRAPPER_H
#define MPAUDIOEFFECTSFRAMEWORKWRAPPER_H

#include <QObject>


class MpAudioEffectsFrameworkWrapper : public QObject
{
    Q_OBJECT

public:
    
    explicit MpAudioEffectsFrameworkWrapper();
    virtual ~MpAudioEffectsFrameworkWrapper();
    
    int balance();
    bool loudness();
    void setBalance( int balance );
    void setLoudness( bool mode );   

public:
    int mBalance;
    bool mLoudness;
};

#endif /*MPAUDIOEFFECTSFRAMEWORKWRAPPER_H*/
