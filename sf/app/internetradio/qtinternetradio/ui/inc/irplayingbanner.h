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

#ifndef IRPLAYINGBANNER_H_
#define IRPLAYINGBANNER_H_

#include <HbWidget>

class HbLabel;
class HbMarqueeItem;

/**
 * This class is to overload the gestureevent function.
 */
class IRPlayingBanner : public HbWidget
{
    Q_OBJECT
public:
    IRPlayingBanner();

signals:
    void playingBannerTapFinished();

private:
    // from HbWidgetBase
    void gestureEvent(QGestureEvent *aEvent);

    // from HbWidget
    void changeEvent(QEvent *event);

    void setPlayingBannerTextColor(const QString &aColor);

    void init();

private:
    HbLabel       *iStationName;
    HbMarqueeItem *iArtistSongName;
};

#endif /* IRPLAYINGBANNER_H_ */
