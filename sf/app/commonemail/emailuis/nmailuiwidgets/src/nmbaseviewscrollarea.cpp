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

#include "nmailuiwidgetsheaders.h"

/*!
   Constructor
*/
NmBaseViewScrollArea::NmBaseViewScrollArea(QGraphicsItem *parentView) :
    HbScrollArea(parentView)
    {
    NM_FUNCTION;
    }

/*!
   Destructor
*/
NmBaseViewScrollArea::~NmBaseViewScrollArea()
    {
    NM_FUNCTION;
    }

/*!
   Scroll area mouse press event handling. Mouse press event is forwarded
   to scroll area base class and web kit for link handling
*/
void NmBaseViewScrollArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    
    if (event){
        emit handleMousePressEvent(event);
        HbScrollArea::mousePressEvent(event);
    }
}

/*!
   Scroll area mouse release event handling. Mouse release event is forwarded
   to scroll area base class and web kit for link handling
*/
void NmBaseViewScrollArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    
    if (event){
        emit handleMouseReleaseEvent(event);
        HbScrollArea::mouseReleaseEvent(event);
    }
}

/*!
   Scroll area mouse double click event handling. Double click is forwarded
   to scroll area base class and web kit
*/
void NmBaseViewScrollArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    
    if (event){
        emit handleMouseMoveEvent(event);
        HbScrollArea::mouseMoveEvent(event);
    }
}

/*!
   Scroll area long press gesture event handling.
*/
void NmBaseViewScrollArea::longPressGesture(const QPointF &point)
{
    NM_FUNCTION;
    
    emit handleLongPressGesture(point);
}

