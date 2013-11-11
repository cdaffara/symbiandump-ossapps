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
* Description: Playback widget stub for Music Player playback view.
*
*/

#include <QTime>

#include <hbprogressslider.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <hbfontspec.h>
#include <hbstackedlayout.h>

#include "..\inc\mpplaybackwidget.h"
#include "mpplaybackdata.h"
#include "mpalbumcoverwidget.h"
#include "mpplaybackdocumentloader.h"
#include "mptrace.h"




/*!
 Constructs the collection view plugin.
 */
MpPlaybackWidget::MpPlaybackWidget(MpPlaybackData *data, QGraphicsItem *parent )
    : HbWidget(parent),
      mPlaybackData(data),
      mDocumentLoader(0),
      mProgreesBarDragging(false),
      mDuration(0)
{
}

/*!
 Constructs the collection view plugin.
 */
MpPlaybackWidget::~MpPlaybackWidget()
{

}

/*!
 Handle Repeat changed.
 */
void MpPlaybackWidget::repeatChanged( bool )
{ 
}
