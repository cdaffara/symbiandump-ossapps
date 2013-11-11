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
* Description: 
*
*/

#include <QUrl>
#include <QProcess>
#include <QLatin1String>
#include <QDebug>

#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbmenu.h>

#include "BrowserWrapper.h"
#include "HelpDataProvider.h"
#include "HelpUtils.h"
#include "HelpContentsView.h"

HelpContentsView::HelpContentsView() : mBrowser(NULL)
{
}

HelpContentsView::~HelpContentsView()
{
}

void HelpContentsView::init()
{
	initDocMl();
    initBackAction();
    mBrowser = mBuilder.findWidget<BrowserWrapper*>(DOCML_BROWSER_CONTENTS);
    mBrowser->init();

    connect(mBrowser, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onLinkClicked(const QUrl&)));
    connect(mBrowser, SIGNAL(urlChanged(const QUrl&)), this, SLOT(onUrlChanged(const QUrl&)));
    connect(mainWindow(), SIGNAL(viewReady()), this, SLOT(onViewReady()));
}

void HelpContentsView::initDocMl()
{
	initBaseDocMl();
    mBuilder.load(QRC_DOCML_CONTENTS);
}

void HelpContentsView::initBackAction()
{
    mSoftKeyAction = new HbAction(Hb::BackNaviAction  );
    connect(mSoftKeyAction, SIGNAL(triggered()), this, SLOT(onBackAction()));
}

///////////////////////////////////////////////////////////////////////////////////////

bool HelpContentsView::openApplication(const QUrl& url)
{
	QString str = url.toString();
	if(str.startsWith(URL_HEADER_APP))
	{
        //app://cmd@localhost/APP_NAME/APP_UID
        QString appUid = str.section(BACKSLASH, -1,-1);
        if(appUid.contains("0x", Qt::CaseInsensitive))
        {
            appUid.remove(0,2);
        }
        int error = HelpUtils::launchApplication(appUid);
        if(error != 0)
        {
            qDebug() << "AIW-ERROR: AppMgrClient:test: Send failed" << error;
        }
        return true;
	}
	
	return false;
}

bool HelpContentsView::openExternalLink(const QUrl& url)
{
	QString str = url.toString();
	if(str.startsWith(URL_HEADER_HTTP) || 
	   str.startsWith(URL_HEADER_HTTPS) || 
	   str.startsWith(URL_HEADER_FTP))
	{
		return true;
	}
	
    return false;
}

void HelpContentsView::openHelpContent(const QUrl& url)
{
    QString html;
    QString urlStr = url.toString();
    HelpDataProvider::instance()->getHelpContentData(html, urlStr);
	mBrowser->setHtml(html, urlStr);
}

////////////////////////////////////////////////////////////////////////////////////////////

void HelpContentsView::onViewReady()
{
    if(isVisible())
    {
        setNavigationAction(mSoftKeyAction);
        openHelpContent();
    }
    else
    {
        mBrowser->clearHistory();
    }    
}

///////////////////////////////////////////////////////////////////////////////////////

void HelpContentsView::onBackAction()
{
	if(mBrowser->canGoBack())
	{
		mBrowser->back();
	}
	else
	{
		emit activateView(PreviousView);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void HelpContentsView::onLinkClicked(const QUrl& url)
{
    // try to open as application
    if(openApplication(url))
    {
        return;
    }

    // try to open as remote link
    if(openExternalLink(url))
    {
        return;
    }

    // try to open as local link
    {
        openHelpContent(url);
        return;
    }
}

void HelpContentsView::onUrlChanged(const QUrl& url)
{
    openHelpContent(url);
}


// end of file
