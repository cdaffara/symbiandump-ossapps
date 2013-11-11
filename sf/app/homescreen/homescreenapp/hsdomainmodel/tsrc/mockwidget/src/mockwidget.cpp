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
* Description:  Mocked widget.
*
*/

#include <qserviceinterfacedescriptor.h>
#include <qabstractsecuritysession.h>
#include <qservicecontext.h>

#include "mockwidget.h"

MockWidget::MockWidget():HbWidget()
{
    setProperty("my_preference1",QString("sun"));
    setProperty("my_preference2",QString("moon"));
    setProperty("my_preference3",QString("earth"));
}

MockWidget::MockWidget(QSizeF widgetSize):HbWidget()
{
    setProperty("my_preference1",QString("sun"));
    setProperty("my_preference2",QString("moon"));
    setProperty("my_preference3",QString("earth"));
    
    resize(widgetSize);
}

void MockWidget::onInitialize()
{
    emit initializeCalled();
}

void MockWidget::onShow()
{
    setProperty("testProperty",QString("dynamic_widget_property"));
    emit showCalled();
}

void MockWidget::onHide()
{
    emit hideCalled();
}

void MockWidget::onUninitialize()
{
    emit uninitializeCalled();
}

QPainterPath MockWidget::shape() const
{   
    QPainterPath path;
    path.addRect(rect());
    return path;
}
