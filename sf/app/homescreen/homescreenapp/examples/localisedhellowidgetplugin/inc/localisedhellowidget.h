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


class HbPushButton;
class QGraphicsPixmapItem;
class HbIconItem;

class LocalisedHelloWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
public:
    LocalisedHelloWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~LocalisedHelloWidget();

public:
    QString rootPath()const;
    void setRootPath(const QString &rootPath);
public slots:
    void onInitialize();
    void onShow();
    void onHide();
private slots:
    void onButtonPressed();
  
private:
    Q_DISABLE_COPY(LocalisedHelloWidget)
   
    HbPushButton *mButton;
    QString mText;
    QString mRootPath;
    HbIconItem *mIcon;
};

#endif // HELLOWORLDWIDGET_H
