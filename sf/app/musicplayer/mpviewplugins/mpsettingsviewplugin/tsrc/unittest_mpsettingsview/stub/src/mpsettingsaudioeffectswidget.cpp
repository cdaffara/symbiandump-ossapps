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
* Description: Stub for MpSettingsAudioEffectsWidget.
*
*/

// User includes
#include "mpsettingsaudioeffectswidget.h"
#include "mptrace.h"


MpSettingsAudioEffectsWidget::MpSettingsAudioEffectsWidget( QGraphicsItem *parent  )
    : HbWidget( parent )
{
    TX_LOG
}


MpSettingsAudioEffectsWidget::~MpSettingsAudioEffectsWidget()
{

    TX_LOG
}


void MpSettingsAudioEffectsWidget::initialize()
{
}

void MpSettingsAudioEffectsWidget::filterSignals( QModelIndex startIn, QModelIndex endIn )
{
    Q_UNUSED(startIn);
    Q_UNUSED(endIn);
}
