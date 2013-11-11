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

#ifndef RADIOCAROUSELITEMOBSERVER_H
#define RADIOCAROUSELITEMOBSERVER_H

class RadioStation;
class QString;
class HbIcon;

class RadioCarouselItemObserver
{
public:

    virtual void handleIconClicked( const RadioStation& station ) = 0;
    virtual void handleRadiotextClicked( const RadioStation& station ) = 0;
    virtual void handleUrlClicked( const RadioStation& station ) = 0;
    virtual QString localizeGenre( int genre ) = 0;
    virtual bool isInManualSeek() const = 0;
    virtual HbIcon favoriteIcon() const = 0;
    virtual HbIcon nonFavoriteIcon() const = 0;
    virtual RadioStation findStation( uint frequency ) = 0;

};

#endif // RADIOCAROUSELITEMOBSERVER_H
