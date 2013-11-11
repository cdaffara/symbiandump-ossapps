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
* Description:  Mocked Mocked widget plugin, which trys 
*               to changes it's size illegally.
*
*/

#ifndef MOCKWIDGETCHEATER_H
#define MOCKWIDGETCHEATER_H

#include <hbwidget.h>

class MockWidgetCheater: public HbWidget
    {
    Q_OBJECT
    Q_PROPERTY(bool isOnline READ isOnline WRITE setOnline)
    
public:
    MockWidgetCheater();
    MockWidgetCheater(QSizeF widgetSize);
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
    Q_DISABLE_COPY(MockWidgetCheater)         
private:    
    bool mOnline;    
 
    };

#endif //MOCKWIDGETCHEATER_H
