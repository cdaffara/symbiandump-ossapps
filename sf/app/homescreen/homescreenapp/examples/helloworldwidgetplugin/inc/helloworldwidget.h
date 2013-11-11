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

#ifndef HELLOWORLDWIDGET_H
#define HELLOWORLDWIDGET_H

#include <HbWidget>

// Start of snippet 1
class HelloWorldWidget : public HbWidget
// End of snippet 1
{
    Q_OBJECT

public:
    HelloWorldWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HelloWorldWidget();

// Start of snippet 2
public slots:
    void onShow();
    void onHide();
// End of snippet 2

private:
    Q_DISABLE_COPY(HelloWorldWidget)
};

#endif // HELLOWORLDWIDGET_H
