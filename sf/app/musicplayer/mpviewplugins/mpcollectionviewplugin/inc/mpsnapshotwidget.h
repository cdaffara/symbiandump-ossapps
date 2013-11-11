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
* Description: Music Player snapshot widget.
*
*/

#ifndef MPSNAPSHOTWIDGET_H
#define MPSNAPSHOTWIDGET_H


#include <QtGlobal>
#include <hbwidget.h>


class QGraphicsView;
class QImage;

class  MpSnapshotWidget : public HbWidget
{
    Q_OBJECT

public:
    explicit MpSnapshotWidget( QGraphicsItem *parent=0 );
    virtual ~MpSnapshotWidget();
    
    void capture(QGraphicsView *graphicsView, QGraphicsItem *item);
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, 
                QWidget *widget = 0 );

private:
    Q_DISABLE_COPY(MpSnapshotWidget)
    QImage *mSnapshot;

};

#endif    // SNAPSHOTWIDGET_H
