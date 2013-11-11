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
* Description:  Mocked Mocked widget plugin, which trys 
*               to changes it's size illegally.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "mockwidgetcheater.h"

MockWidgetCheater::MockWidgetCheater():HbWidget()
{
    setProperty("my_preference1",QString("sun"));
    setProperty("my_preference2",QString("moon"));
    setProperty("my_preference3",QString("earth"));
}

MockWidgetCheater::MockWidgetCheater(QSizeF widgetSize):HbWidget()
{
    setProperty("my_preference1",QString("sun"));
    setProperty("my_preference2",QString("moon"));
    setProperty("my_preference3",QString("earth"));
    
    resize(widgetSize);
}

void MockWidgetCheater::onInitialize()
{    
    emit initializeCalled();
}

void MockWidgetCheater::onShow()
{
    setProperty("testProperty",QString("dynamic_widget_property"));
    emit showCalled();
}

void MockWidgetCheater::onHide()
{
    emit hideCalled();
}

void MockWidgetCheater::onUninitialize()
{
    emit uninitializeCalled();
}

QPainterPath MockWidgetCheater::shape() const
{   
    QPainterPath path;
    QRectF currentRect(rect());
    currentRect.setHeight(currentRect.height() + 10);
    path.addRect(currentRect);    
    return path;
}
