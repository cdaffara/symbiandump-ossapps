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
*
*/

#ifndef T_PHONERINGINGTONEPLAYER_H
#define T_PHONERINGINGTONEPLAYER_H

#include <QGraphicsItem>
#include <hbview.h>

class CPhoneRingingToneController;

class t_phoneringingtoneplayer : public HbView
{
    Q_OBJECT

public:
     t_phoneringingtoneplayer(QGraphicsItem *parent = 0);
    ~t_phoneringingtoneplayer();

private: 
    void createMenu();
    void createContent();
    
protected slots:
    void PlayRingingtoneRingning();
    void StopRingingtone();  
    void PlayRingingtoneBeepOnce();
    void PlayRingingtoneSilence();
    void PlayRingingtoneTTS();
    void PlayRingingtoneAscending();
    void PlayRingingtoneOnce();
    void SampleMP3();
    void Nokiatuneaac();
    void volumeSliderChanged(int value);

private:
    
    TBuf<50> m_ringingtone;
    int m_volume;
    CPhoneRingingToneController *m_ringingtoneplayer;
};

#endif // T_PHONERINGINGTONEPLAYER_H
