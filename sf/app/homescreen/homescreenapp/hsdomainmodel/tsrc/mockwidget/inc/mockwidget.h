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

#ifndef MOCKWIDGET_H
#define MOCKWIDGET_H

#include <hbwidget.h>

class MockWidget: public HbWidget
    {
    Q_OBJECT
    Q_PROPERTY(bool isOnline READ isOnline WRITE setOnline)
    
public:
    MockWidget();
    MockWidget(QSizeF widgetSize);
    bool isOnline(){ return mOnline; }
    void setOnline(bool online){ mOnline = online; }
    QPainterPath shape() const;
public slots:

    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();
    
signals:
    void setPreferences(const QStringList &names);
    // for testing
    void initializeCalled();
    void showCalled();
    void hideCalled();
    void uninitializeCalled();
private:
    Q_DISABLE_COPY(MockWidget)         
private:    
    bool mOnline;
 
    };

#endif //MOCKWIDGET_H
