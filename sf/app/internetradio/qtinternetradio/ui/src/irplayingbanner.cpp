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

#include <HbTapGesture>
#include <HbEvent>
#include <HbLabel>
#include <HbMarqueeItem>
#include <HbColorScheme>
#include <QGraphicsLayout>

#include "irplayingbanner.h"
#include "iruidefines.h"

IRPlayingBanner::IRPlayingBanner() : iStationName(NULL), iArtistSongName(NULL)
{
    grabGesture(Qt::TapGesture);
}

void IRPlayingBanner::gestureEvent(QGestureEvent *aEvent)
{
    HbTapGesture *tapGesture = qobject_cast<HbTapGesture *>(aEvent->gesture(Qt::TapGesture));
    if (!tapGesture)
    {
        return;
    }
    
    if (Qt::GestureStarted == tapGesture->state())
    {
        setPlayingBannerTextColor(KNowPlayingBannerColorPressed);
    }
    else if (Qt::GestureFinished == tapGesture->state())
    {
        setPlayingBannerTextColor(KNowPlayingBannerColorNormal);
        emit playingBannerTapFinished();
    }
    else if (Qt::GestureCanceled == tapGesture->state())
    {
        setPlayingBannerTextColor(KNowPlayingBannerColorNormal);
    }
}

void IRPlayingBanner::changeEvent(QEvent *event)
{
    if (HbEvent::ThemeChanged == event->type())
    {
        setPlayingBannerTextColor(KNowPlayingBannerColorNormal);
    }
    
    HbWidget::changeEvent(event);
}

void IRPlayingBanner::setPlayingBannerTextColor(const QString &aColor)
{
    init();
    QColor color = HbColorScheme::color(aColor);
    iStationName->setTextColor(color);
    iArtistSongName->setTextColor(color);
}

void IRPlayingBanner::init()
{
    if (iStationName && iArtistSongName)
    {
        return;
    }

    int max = layout()->count();
    for (int i = 0; i < max; i++)
    {
        QObject *obj = dynamic_cast<QObject *>(layout()->itemAt(i));
        if (obj->objectName() == ABSTRACT_LIST_VIEW_BASE_OBJECT_STATIONNAME)
        {
            iStationName =  static_cast<HbLabel *>(obj);
        }
        else if (obj->objectName() == ABSTRACT_LIST_VIEW_BASE_OBJECT_ARTISTSONGNAME)
        {
            iArtistSongName = static_cast<HbMarqueeItem *>(obj);;
        }
    }
}
