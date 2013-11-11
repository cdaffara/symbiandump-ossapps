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
#ifndef CXUIZOOMSLIDER_H
#define CXUIZOOMSLIDER_H

#include <QGraphicsItem>
#include <hbslider.h>

class QGraphicsSceneMouseEvent;

class CxuiZoomSlider : public HbSlider
{
    Q_OBJECT

public:
    explicit CxuiZoomSlider(QGraphicsItem *parentItem = NULL);
    ~CxuiZoomSlider();

public:
    void addZoomButtons();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // CXUIZOOMSLIDER_H
