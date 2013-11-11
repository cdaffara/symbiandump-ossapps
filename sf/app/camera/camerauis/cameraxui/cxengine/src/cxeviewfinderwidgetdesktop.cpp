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
* Description:
*
*/
#include "cxeviewfinderwidgetdesktop.h"
#include <QPainter>
#include "cxutils.h"

const double RES_WIDTH = 640.0;
const double RES_HEIGHT = 360.0;

/*!
* Construtor
*/
CxeViewfinderWidgetDesktop::CxeViewfinderWidgetDesktop() :
        QGraphicsWidget()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCurrentPixmap = QPixmap();
    resize(RES_WIDTH, RES_HEIGHT);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destrutor
*/
CxeViewfinderWidgetDesktop::~CxeViewfinderWidgetDesktop()
{

}

/*!
* Paint
*/
void CxeViewfinderWidgetDesktop::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPixmap(rect().toRect(), mCurrentPixmap);
}

/*!
* Handle image change
*/
void CxeViewfinderWidgetDesktop::handleImageChange(const QPixmap &newImage)
{
    CX_DEBUG_ENTER_FUNCTION();
    mCurrentPixmap = newImage;
    update();
    CX_DEBUG_EXIT_FUNCTION();
}
