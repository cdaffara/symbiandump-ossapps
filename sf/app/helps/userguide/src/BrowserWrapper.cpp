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

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsLinearLayout>
#include <QWebFrame>

#include "HelpDocumentLoader.h"
#include "HelpCommon.h"
#include "BrowserWrapper.h"

#ifdef Q_OS_SYMBIAN
#include <wrtcontroller.h>
#endif

BrowserWrapper::BrowserWrapper()
{
}

BrowserWrapper::~BrowserWrapper()
{
}

void BrowserWrapper::init()
{
	mWebView = new QGraphicsWebView();
    mWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	mWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
	mWebView->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    mWebView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    mWebView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    connect(mWebView, SIGNAL(linkClicked(const QUrl&)), this, SIGNAL(linkClicked(const QUrl&)));
    
    QGraphicsLinearLayout* vLayout = new QGraphicsLinearLayout(this);
    vLayout->setOrientation(Qt::Vertical);
    vLayout->addItem(mWebView);
    vLayout->setContentsMargins(0,0,0,0);
    setLayout(vLayout);
}

void BrowserWrapper::setHtml(const QString& html, const QUrl& url)
{
    mWebView->setHtml(html, url);

    if(!mHistory.count() || mHistory.top()!=url)
    {
        mHistory.append(url);
    }
}

void BrowserWrapper::clearHistory()
{
    mHistory.clear();
}

bool BrowserWrapper::canGoBack()
{
    return (mHistory.count() > 1);
}

void BrowserWrapper::back()
{
    if(canGoBack())
    {
        mHistory.pop();
        QUrl url = mHistory.top();
        emit urlChanged(url);
    }
}


// end of file

