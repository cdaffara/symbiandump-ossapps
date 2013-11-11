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
* Description: Example of home screen content publishing client
*
*/

#include <QObject>

#include <qservicemanager.h>

QTM_USE_NAMESPACE

class ContentPublishClient: public QObject
{
    Q_OBJECT
    
public:
    ContentPublishClient(QServiceManager &manager, QObject *parent=0 );
    ~ContentPublishClient();
    bool load();
    
public slots:
    void addHelloworldWidget();
    void addClockWidget();
    void setWallpaper1();
    void setWallpaper2();
    void setWallpaper3();

protected:
    bool addWidget(QString widgetUri);
    bool setWallpaper(QString fileName);
    bool setWallpaper(const QString &portraitFileName, const QString &landscapeFileName);
    
private:
    QObject* mService;
    QServiceManager* mManager;    
};
