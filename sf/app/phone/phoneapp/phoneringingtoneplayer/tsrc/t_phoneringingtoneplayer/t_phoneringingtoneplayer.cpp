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

#include <QtGui/QApplication.h>
#include <QGraphicsLinearLayout>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbpushbutton.h>
#include <hbslider.h>
#include <profile.hrh>
#include "t_phoneringingtoneplayer.h"
#include "cphoneringingtonecontroller.h"
#include "TPhoneCmdParamRingTone.h"


_LIT(KNokiatune, "Z:\\Data\\Sounds\\Digital\\Nokia tune.aac");
_LIT(KSamplemp3, "c:\\Data\\Sounds\\Digital\\sample.mp3");

t_phoneringingtoneplayer::t_phoneringingtoneplayer(QGraphicsItem *parent)
    : HbView(parent), m_volume(10)
{
    setTitle(tr("t_ringingtoneplayer"));
    createMenu();
    createContent();

    m_ringingtone.Format(KNokiatune);
    m_ringingtoneplayer = CPhoneRingingToneController::NewL();
}

t_phoneringingtoneplayer::~t_phoneringingtoneplayer()
{
    delete m_ringingtoneplayer;
}
void t_phoneringingtoneplayer::createMenu()
{
    HbMenu *optionsMenu = menu();
    /*connect(optionsMenu->addAction(tr("EProfileRingingTypeRinging"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneRingning()));
    connect(optionsMenu->addAction(tr("EProfileRingingTypeBeepOnce"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneBeepOnce()));
    connect(optionsMenu->addAction(tr("EProfileRingingTypeSilent"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneSilence()));
    connect(optionsMenu->addAction(tr("EProfileRingingTypeRinging+tts"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneTTS()));  
    connect(optionsMenu->addAction(tr("EProfileRingingTypeAscending"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneAscending()));  
    connect(optionsMenu->addAction(tr("EProfileRingingTypeRingingOnce"))->action(), SIGNAL(triggered()), this, SLOT(PlayRingingtoneOnce()));  
    connect(optionsMenu->addAction(tr("Stop ringingtone"))->action(), SIGNAL(triggered()), this, SLOT(StopRingingtone())); */
    connect(optionsMenu->addAction(tr("sample.mp3")), SIGNAL(triggered()), this, SLOT(SampleMP3()));
    connect(optionsMenu->addAction(tr("nokiatune.aac")), SIGNAL(triggered()), this, SLOT(Nokiatuneaac()));
    //connect(optionsMenu->addAction(tr("Quit"))->action(), SIGNAL(triggered()), qApp, SLOT(quit()));
}

void t_phoneringingtoneplayer::createContent()
{
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    
    HbPushButton *ringingButton = new HbPushButton(tr("EProfileRingingTypeRinging"), this); 
    connect(ringingButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneRingning()));
    
    HbPushButton *beeponceButton = new HbPushButton(tr("EProfileRingingTypeBeepOnce"), this); 
    connect(beeponceButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneBeepOnce()));
    
    HbPushButton *silenceButton = new HbPushButton(tr("EProfileRingingTypeSilent"), this); 
    connect(silenceButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneSilence()));
    
    HbPushButton *ttsButton = new HbPushButton(tr("EProfileRingingTypeRinging+tts"), this); 
    connect(ttsButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneTTS()));
    
    HbPushButton *ascendingButton = new HbPushButton(tr("EProfileRingingTypeAscending"), this); 
    connect(ascendingButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneAscending()));

    HbPushButton *ringingonceButton = new HbPushButton(tr("EProfileRingingTypeRingingOnce"), this); 
    connect(ringingonceButton, SIGNAL(clicked()), this, SLOT(PlayRingingtoneOnce()));

    HbPushButton *stopButton = new HbPushButton(tr("Stop ringingtone"), this); 
    connect(stopButton, SIGNAL(clicked()), this, SLOT(StopRingingtone()));
    
    HbPushButton *quitButton = new HbPushButton(tr("Quit"), this); 
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
    
    HbSlider *volumeslider = new HbSlider(this);
    //volumeslider->setSliderType(HbSlider::VolumeSlider);
    volumeslider->setOrientation( Qt::Horizontal);
    volumeslider->setMinimum(EProfileRingingVolumeLevel1);
    volumeslider->setMaximum(EProfileRingingVolumeLevel10);
    volumeslider->setSingleStep(1);
    volumeslider->setValue(m_volume);
    connect(volumeslider, SIGNAL(valueChanged(int)), this, SLOT(volumeSliderChanged(int)));
    
    mainLayout->addItem(volumeslider);
    mainLayout->addItem(ringingButton);
    mainLayout->addItem(beeponceButton);
    mainLayout->addItem(silenceButton);
    mainLayout->addItem(ttsButton);
    mainLayout->addItem(ascendingButton);
    mainLayout->addItem(ringingonceButton);
    mainLayout->addItem(stopButton);
    mainLayout->addItem(quitButton);
    //mainLayout->addStretch();

    setLayout(mainLayout);
}

void t_phoneringingtoneplayer::PlayRingingtoneRingning()
{
    m_ringingtoneplayer->StopPlaying();
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeRinging);
    // Set the profile ring tone
    ringToneParam.SetRingTone( m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}

void t_phoneringingtoneplayer::PlayRingingtoneBeepOnce()
{
    m_ringingtoneplayer->StopPlaying();
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeBeepOnce);
    // Set the profile ring tone
    ringToneParam.SetRingTone( m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}


void t_phoneringingtoneplayer::PlayRingingtoneSilence()
{
    m_ringingtoneplayer->StopPlaying();
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeSilent);
    // Set the profile ring tone
    ringToneParam.SetRingTone( m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}

void t_phoneringingtoneplayer::PlayRingingtoneTTS()
{
    m_ringingtoneplayer->StopPlaying();
    _LIT(KTextToSay, "Mr. Brownstone");
    TBuf<30> texttosay (KTextToSay);
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeRinging);
    ringToneParam.SetTextToSay(texttosay);
    // Set the profile ring tone
    ringToneParam.SetRingTone( m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}

void t_phoneringingtoneplayer::PlayRingingtoneAscending()
{
    m_ringingtoneplayer->StopPlaying();
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeAscending);
    // Set the profile ring tone
    ringToneParam.SetRingTone( m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}

void t_phoneringingtoneplayer::PlayRingingtoneOnce()
{
    m_ringingtoneplayer->StopPlaying();

    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(m_volume);
    ringToneParam.SetRingingType(EProfileRingingTypeRingingOnce);
    // Set the profile ring tone
    ringToneParam.SetRingTone(m_ringingtone);
    ringToneParam.SetType( EPhoneRingToneProfile );

    m_ringingtoneplayer->PlayRingToneL( &ringToneParam );
}


void t_phoneringingtoneplayer::StopRingingtone()
{
    m_ringingtoneplayer->StopPlaying();
}

void t_phoneringingtoneplayer::SampleMP3()
{
   m_ringingtone.Format(KSamplemp3);
}
void t_phoneringingtoneplayer::Nokiatuneaac()
{
    m_ringingtone.Format(KNokiatune); 
}
void t_phoneringingtoneplayer::volumeSliderChanged(int value)
    {
    m_volume = value;
    }
