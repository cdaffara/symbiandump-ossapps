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

#ifndef FBMAINWINDOW_H_
#define FBMAINWINDOW_H_

#include <hbmainwindow.h>

class HbApplication;
class HbView;

class EngineWrapper;
class FbDriveView;
class FbFileView;
class FbSettingsView;
class FbEditorView;
class FbSearchView;
class FbAttributesView;

class FbMainWindow : public HbMainWindow
{
    Q_OBJECT

public:
    explicit FbMainWindow(QWidget *parent = 0);
    virtual ~FbMainWindow();
    
    void init();

private slots:
    void openPreviousBrowserView();
    void openFileBrowserView(bool);
    void openDriveView();
    void openFileView();
    void openSettingsView();
    void openEditorView(const QString &, bool);
    void openSearchView(const QString &);

    void openAttributesView(const QString &, quint32 &, quint32 &, bool &);
    void closeAttributesView(bool);
    
private:
    EngineWrapper *mEngineWrapper;
    FbDriveView *mDriveView;
    FbFileView *mFileView;
    FbSettingsView *mSettingsView;
    FbEditorView *mEditorView;
    FbSearchView *mSearchView;
    FbAttributesView *mAttributesView;
    HbView *mPreviousView;
};

#endif /* FBMAINWINDOW_H_ */
