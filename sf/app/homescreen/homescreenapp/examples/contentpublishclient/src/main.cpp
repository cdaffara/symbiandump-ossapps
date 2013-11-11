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
* Description:  Example of home screen content publishing client
*
*/

#include <HbApplication>
#include <HbMainWindow>
#include <HbView>
#include <HbPushButton>
#include <QGraphicsLinearLayout>
#include <QDir>

#include "contentpublishclient.h"

int main(int argc, char *argv[])
{
    // Initialization
    HbApplication app(argc, argv);
        
    QServiceManager manager;
    
    // Create main window.
    HbMainWindow mainWindow;

    // Create content publisher client
	ContentPublishClient contentPublishClient(manager);

    QString buttonString = "Add HelloWorld";
    bool clientOk = contentPublishClient.load(); 
    if (!clientOk) {
        buttonString = "Open failed";
    }

    HbWidget *myView = new HbWidget();
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    
    HbPushButton* button1 = new HbPushButton(buttonString);
    layout->addItem(button1);
    contentPublishClient.connect(button1, SIGNAL(clicked()), SLOT(addHelloworldWidget()));
    
    if (clientOk) {
        HbPushButton* button2 = new HbPushButton("Add Clock widget");
        HbPushButton* button3 = new HbPushButton("Set wallpaper1");
        HbPushButton* button4 = new HbPushButton("Set wallpaper2");
        HbPushButton* button5 = new HbPushButton("Set wallpaper3");
        
        layout->addItem(button2);
        layout->addItem(button3);
        layout->addItem(button4);
        layout->addItem(button5);
        
        contentPublishClient.connect(button2, SIGNAL(clicked()), SLOT(addClockWidget()));
        contentPublishClient.connect(button3, SIGNAL(clicked()), SLOT(setWallpaper1()));
        contentPublishClient.connect(button4, SIGNAL(clicked()), SLOT(setWallpaper2()));
        contentPublishClient.connect(button5, SIGNAL(clicked()), SLOT(setWallpaper3()));
    }
    
    
    myView->setLayout(layout);

	// Add view
    mainWindow.addView(myView);
    
    // Show main window
    mainWindow.show();

    return app.exec();
}
