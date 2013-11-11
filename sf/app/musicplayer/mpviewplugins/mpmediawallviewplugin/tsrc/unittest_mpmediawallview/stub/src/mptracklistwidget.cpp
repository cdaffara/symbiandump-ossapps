/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Track List Widget for Music Player Media Wall.
*
*/


#include <QGraphicsSceneMouseEvent>

#include <hbinstance.h>
#include <hblistview.h>
#include <hbframeitem.h>
#include <hbmainwindow.h>
#include <hbstyleloader.h>


#include "mptracklistwidget.h"


/*!
    Stub.
 */
MpTrackListWidget::MpTrackListWidget( QGraphicsItem *parent ) : HbWidget( parent ) 
{

}

/*!
    Stub
 */
MpTrackListWidget::~MpTrackListWidget()
{
}

/*!
    Stub
 */
HbListView *MpTrackListWidget::list()
{
    return new HbListView();
} 

//EOF
