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
* Description: 
*
*/


#include <hbapplication.h>
#include <hblabel.h>
#include <hbmainwindow.h>
#include <hbdocumentloader.h>
#include <hbview.h>
#include "pimainview.h"
#include "piprofilerengine.h"
#include "notifications.h"
#include "pimainwindow.h"

// Create custom document loader to be able to use custom views in XML.
class PIProfilerDocumentLoader: public HbDocumentLoader
{
public:
    virtual QObject *createObject(const QString& type, const QString &name);
};

int main(int argc, char *argv[])
{

    // Create application and document loader.
    HbApplication app(argc, argv);
    PIProfilerDocumentLoader loader;

    // Create Engine and initialize it
    PIProfilerEngine engine;
    if (!engine.init()) {
        Notifications::showErrorNote(QString("Unable to start PI Profiler engine"));
        return EXIT_FAILURE;
    }

    // create and show main window
    PIMainWindow mainWindow(&engine, loader, &app);
    mainWindow.show();

    return app.exec();
}

QObject *PIProfilerDocumentLoader::createObject(const QString& type, const QString &name)
{
    // Main view
    if (type == PIMainView::staticMetaObject.className()) {
        QObject *object = new PIMainView;
        object->setObjectName(name);
        return object;
    }

    return HbDocumentLoader::createObject(type, name);
}

