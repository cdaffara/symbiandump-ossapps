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
* Description:  Example of home screen widget
*
*/
#include <qtDebug>

#include <QGraphicsLinearLayout>
#include <HbLabel>
#include "helloworldwidget.h"

/*!
    \ingroup group_helloworld_widget
    \class HelloWorldWidget
    \brief Example implementation for home screen widget.

    HelloWorldWidget derived from the HbWidget and implements 
    needed functions for the home screen widget. 
*/

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
// Start of snippet 1
HelloWorldWidget::HelloWorldWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags)
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    setLayout(layout);
    HbLabel *helloWorld = new HbLabel("Hello World!");
    helloWorld->setPreferredSize(QSizeF(130.0,20.0));
    layout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    layout->addItem(helloWorld);
}
// End of snippet 1

/*!
    Destructor
*/
HelloWorldWidget::~HelloWorldWidget()
{
}

/*!
    Called when widget is shown in the home screen
*/
// Start of snippet 2
void HelloWorldWidget::onShow()
{
}
// End of snippet 2

/*!
    Called when widget is hidden from the home screen
*/
// Start of snippet 3
void HelloWorldWidget::onHide()
{
}
// End of snippet 3
