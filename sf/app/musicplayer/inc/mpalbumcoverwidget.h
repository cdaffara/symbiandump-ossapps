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
* Description:  Album Cover Widget for Music Player Media Wall.
*
*/
#ifndef MPALBUMCOVERWIDGET_H
#define MPALBUMCOVERWIDGET_H

#include <QPixmap>
#include <QGesture>

#include <hbwidget.h>
#include <hbicon.h>

#ifdef BUILD_MPALBUMCOVERWIDGET
#define MPALBUMCOVERWIDGET_EXPORT Q_DECL_EXPORT
#else
#define MPALBUMCOVERWIDGET_EXPORT Q_DECL_IMPORT
#endif

class MPALBUMCOVERWIDGET_EXPORT MpAlbumCoverWidget : public HbWidget

{
    Q_OBJECT

public:

    MpAlbumCoverWidget( QGraphicsItem *parent = 0 );
    virtual ~MpAlbumCoverWidget();
    void setIcon(const HbIcon &icon );
    void setDefaultIcon( const HbIcon &icon );
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, 
                QWidget *widget = 0 );
    void gestureEvent(QGestureEvent *event);

signals:

    void clicked();
    
private:

    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );
    
private:
    
    HbIcon    mIcon;
    HbIcon    mDefaultIcon;
    QPixmap   mPixmap;
};


#endif //MPALBUMCOVERWIDGET_H

// EOF
