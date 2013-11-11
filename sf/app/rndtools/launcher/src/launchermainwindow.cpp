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
#include <hbmainwindow.h>
#include <hbdocumentloader.h>

#include "enginewrapper.h"
#include "commonActions.h"
#include "applicationview.h"
#include "outputview.h"
#include "launchermainwindow.h"

// Here we create custom document loader to be able to use custom views in XML.
class LauncherDocumentLoader : public HbDocumentLoader
{
public:
    virtual QObject *createObject(const QString& type, const QString &name);
};

QObject *LauncherDocumentLoader::createObject(const QString& type, const QString &name)
{
    // Application view
    if (type == ApplicationView::staticMetaObject.className()) {
        QObject *object = new ApplicationView;
        object->setObjectName(name);
        return object;
    }
    
    // Output view
    else if (type == OutputView::staticMetaObject.className()) {
        QObject *object = new OutputView;
        object->setObjectName(name);
        return object;
    }
    
    return HbDocumentLoader::createObject(type, name);
}


LauncherMainWindow::LauncherMainWindow( QWidget *parent ) : HbMainWindow( parent )
    ,mEngineWrapper(0)
    ,mApplicationView(0)
    ,mOutputView(0)
    ,mCommonActions(0)
    {
    }

LauncherMainWindow::~LauncherMainWindow ()
    {
    if(mCommonActions)
        delete mCommonActions;
    if(mEngineWrapper)
        delete mEngineWrapper;
    }

int LauncherMainWindow::init( HbApplication &app )
    {
    
    // Create Engine Wrapper and initialize it
    mEngineWrapper = new EngineWrapper(this);
    if (!mEngineWrapper->init()) {
        return EXIT_FAILURE;
    }
    
    // Create common actions class for actions that are used in all views of Launcher        
    mCommonActions = new CommonActions(mEngineWrapper, &app);

    // Create document loader
    LauncherDocumentLoader loader;
    
    // Create application view
    mApplicationView = ApplicationView::create(loader, mCommonActions, mEngineWrapper);
    // Add view to main window
    addView(mApplicationView);

    loader.reset();
    
    // Create output view
    mOutputView = OutputView::create(loader, mCommonActions);
    // Add view to main window
    addView(mOutputView);
    
    // Show ApplicationView at startup
    setCurrentView(mApplicationView);
    
    // Show HbMainWindow
    show();

    return 0;// no errors
    }

void LauncherMainWindow::openApplicationView()
    {
    setCurrentView(mApplicationView);
    }

void LauncherMainWindow::openOutputView()
    {
    setCurrentView(mOutputView);
    }

void LauncherMainWindow::printText(const QString &text)
    {
    mOutputView->printText(text);
    }
