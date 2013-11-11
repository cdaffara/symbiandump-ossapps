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
#include <HbApplication>
#include <HbMainWindow>

#include "fbmainwindow.h"
#include "enginewrapper.h"
#include "fbfileview.h"
#include "fbdriveview.h"
#include "fbsettingsview.h"
#include "fbeditorview.h"
#include "fbsearchview.h"
#include "fbattributesview.h"

FbMainWindow::FbMainWindow(QWidget *parent)
    : HbMainWindow(parent),
    mEngineWrapper(0),
    mDriveView(0),
    mFileView(0),
    mSettingsView(0),
    mEditorView(0),
    mSearchView(0),
    mAttributesView(0),
    mPreviousView(0)
{
}

FbMainWindow::~FbMainWindow ()
{
    if (mEngineWrapper) {
        delete mEngineWrapper;
    }
}

void FbMainWindow::init()
{
    // Create Engine Wrapper and initialize it
    mEngineWrapper = new EngineWrapper();
    int error = mEngineWrapper->init();
    Q_ASSERT_X(error == 1, "FileBrowser", "Engine initialization failed");

    // Create drive view
    mDriveView = new FbDriveView();
    connect(mDriveView, SIGNAL(aboutToShowSettingsView()), this, SLOT(openSettingsView()));
    connect(mDriveView, SIGNAL(aboutToShowFileView()), this, SLOT(openFileView()));
    mDriveView->init(mEngineWrapper);
    addView(mDriveView);

    // Create file view
    mFileView = new FbFileView();
    connect(mFileView, SIGNAL(aboutToShowSettingsView()), this, SLOT(openSettingsView()));
    connect(mFileView, SIGNAL(aboutToShowDriveView()), this, SLOT(openDriveView()));
    mFileView->init(mEngineWrapper);
    addView(mFileView);

    // Create settings view
    mSettingsView = new FbSettingsView(*mEngineWrapper);
    connect(mSettingsView, SIGNAL(finished(bool)), this, SLOT(openPreviousBrowserView()));
    addView(mSettingsView);

    // Create editor view
    mEditorView = new FbEditorView();
    connect(mFileView, SIGNAL(aboutToShowEditorView(const QString &, bool)), this, SLOT(openEditorView(const QString &, bool)));
    connect(mEditorView, SIGNAL(finished(bool)), this, SLOT(openFileView()));
    addView(mEditorView);

    // Create Search view
    mSearchView = new FbSearchView(*mEngineWrapper);
    connect(mDriveView, SIGNAL(aboutToShowSearchView(QString)), this, SLOT(openSearchView(QString)));
    connect(mFileView, SIGNAL(aboutToShowSearchView(QString)), this, SLOT(openSearchView(QString)));
    connect(mSearchView, SIGNAL(finished(bool)), this, SLOT(openFileBrowserView(bool)));
    addView(mSearchView);

    connect(mFileView, SIGNAL(aboutToShowAttributesView(const QString &, quint32 &, quint32 &, bool &)),
            this, SLOT(openAttributesView(const QString &, quint32 &, quint32 &, bool &)));

    // Show ApplicationView at startup
    setCurrentView(mDriveView);
    mPreviousView = mDriveView;

    // Show HbMainWindow
    show();
}


void FbMainWindow::openPreviousBrowserView()
{
    mDriveView->refreshList();
    mFileView->refreshList();
    setCurrentView(mPreviousView);
}

void FbMainWindow::openFileBrowserView(bool accepted)
{
    mDriveView->refreshList();
    mFileView->refreshList();
    if (accepted) {
        setCurrentView(mFileView);
    } else {
        setCurrentView(mPreviousView);
    }
}

void FbMainWindow::openDriveView()
{
    mDriveView->refreshList();
    setCurrentView(mDriveView);
    mPreviousView = mDriveView;
}

void FbMainWindow::openFileView()
{
    mDriveView->refreshList();
//    mFileView->setCurrentpath(path);
    mFileView->refreshList();
    setCurrentView(mFileView);
    mPreviousView = mFileView;
}

void FbMainWindow::openSettingsView()
{
    mSettingsView->initDataForm();
    setCurrentView(mSettingsView);
}

void FbMainWindow::openEditorView(const QString &fileName, bool flagReadOnly)
{
    mEditorView->open(fileName, flagReadOnly);
    setCurrentView(mEditorView);
}

void FbMainWindow::openSearchView(const QString &path)
{
    mSearchView->open(path);
    setCurrentView(mSearchView);
}

void FbMainWindow::openAttributesView(const QString &attributesViewTitle,
                                      quint32 &setAttributesMask,
                                      quint32 &clearAttributesMask,
                                      bool &recurse)
{
    if (!mAttributesView) {
        // Create attributes view
        mAttributesView = new FbAttributesView(setAttributesMask, clearAttributesMask, recurse);
        connect(mAttributesView, SIGNAL(finished(bool)), this, SLOT(closeAttributesView(bool)));
        mAttributesView->setTitle(attributesViewTitle);
        addView(mAttributesView);
        setCurrentView(mAttributesView);
    }
}

void FbMainWindow::closeAttributesView(bool accepted)
{
    if (accepted) {
        mEngineWrapper->setAttributes(mAttributesView->setAttributesMask(), mAttributesView->clearAttributesMask(), mAttributesView->recurse());
    }
    openFileView();
    removeView(mAttributesView);
    mAttributesView->deleteLater();
    mAttributesView = 0;
}

// End of file
