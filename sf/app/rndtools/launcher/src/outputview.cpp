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

#include <hbdocumentloader.h>
#include <launchermainwindow.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbtextedit.h>

#include "enginewrapper.h"
#include "commonActions.h"
#include "outputview.h"

// xml definition of view
const char *OUTPUTVIEW_XML = ":/xml/outputview.xml";

// name of the output view object in the xml file.
const char *OUTPUTVIEW = "outputview";
const char *ACTIONCLEAR = "actionClearWindow";
const char *TEXTOUTPUT = "textEdit";
const char *ACTIONOPENAPPLICATIONVIEW = "actionOpenApplicationView";

// ---------------------------------------------------------------------------


OutputView::OutputView():
    mActionClear(0),
    mActionOpenApplicationView(0),
    mTextOutput(0)
{

}

// ---------------------------------------------------------------------------

OutputView::~OutputView()
{
    if(mActionClear)
        mActionClear->deleteLater();
    if(mActionOpenApplicationView)
        mActionOpenApplicationView->deleteLater();
}


// ---------------------------------------------------------------------------

OutputView* OutputView::create(HbDocumentLoader &loader, 
                        CommonActions *commonActions)
{
    // Load application view from xml-file
    bool ok = false;
    loader.load(OUTPUTVIEW_XML, &ok);
    
    // Output View:
    QGraphicsWidget *widget = loader.findWidget(OUTPUTVIEW);
    Q_ASSERT_X(ok && (widget != 0), "Launcher", "Invalid launcher.xml file");
    OutputView *outputView = qobject_cast<OutputView *>(widget);
    
    // initialize view
    outputView->init(loader, commonActions);
    
    return outputView;
}

// ---------------------------------------------------------------------------


void OutputView::loadItemsFromXml(HbDocumentLoader &loader)
{

    // List widget:
    QGraphicsWidget *widget = loader.findWidget(TEXTOUTPUT);
    Q_ASSERT_X((widget != 0), "Launcher", "Invalid launcher.xml file");
    mTextOutput = qobject_cast<HbTextEdit *>(widget);

    // clear action:
    QObject *object= loader.findObject(ACTIONCLEAR);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionClear = qobject_cast<HbAction *>(object);
    
    // open output view
    object= loader.findObject(ACTIONOPENAPPLICATIONVIEW);
    Q_ASSERT_X((object != 0), "Launcher", "Invalid launcher.xml file");
    mActionOpenApplicationView = qobject_cast<HbAction *>(object);

}

// ---------------------------------------------------------------------------


void OutputView::init(HbDocumentLoader &loader, CommonActions *commonActions)
{
    loadItemsFromXml(loader);
    connectSignalsAndSlots();

    // set text edit components settings
    mTextOutput->setPlainText("Ready.\n\n");
    mTextOutput->setReadOnly(true);
    mTextOutput->setCursorVisibility( Hb::TextCursorHidden );
    mTextOutput->setAttribute(Qt::WA_InputMethodEnabled, false);
    
    // add actions to menu
    commonActions->addActionsToMenu(menu());

}

// ---------------------------------------------------------------------------

void OutputView::connectSignalsAndSlots()
{
    // Clear action
    connect(mActionClear, SIGNAL(triggered()), this, SLOT(clearOutput()));
    connect(mActionOpenApplicationView, SIGNAL(triggered()), this, SLOT(openApplicationView()));
}

// ---------------------------------------------------------------------------

void OutputView::clearOutput()
{
    mTextOutput->setPlainText("");
}

// ---------------------------------------------------------------------------

void OutputView::printText(const QString &text)
{
    QString newText = mTextOutput->toPlainText() + text;
    mTextOutput->setPlainText(newText);
    mTextOutput->setCursorPosition(newText.length()-1);
}

// ---------------------------------------------------------------------------

void OutputView::openApplicationView()
{
    qDebug("openApplicationView");
    LauncherMainWindow *pMainWindow = static_cast< LauncherMainWindow * >( mainWindow() );
    if( pMainWindow )
        {
        pMainWindow->openApplicationView();
        }
}

// ---------------------------------------------------------------------------

