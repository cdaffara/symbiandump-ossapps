/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "GSuperWebPage.h"

namespace GVA {

GSuperWebPage::GSuperWebPage(WebPageWrapper *page, ChromeWidget *chromeWidget)
    : GWebPage(page),
    m_chromeWidget(chromeWidget)
{
    if (!m_page) {
        m_page = new WebPageWrapper(this, "Superpage javascript error");
    }
    //qDebug() << "GSuperWebPage::GSuperWebPage: page=" << GWebPage::page();
    connect(GWebPage::page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(onJavaScriptWindowObjectCleared()));
}

void GSuperWebPage::load(const QString &url) {
    //qDebug() << "GSuperWebPage::load: " << url;
    QString tmpUrl = url;
    if(tmpUrl.left(2) == ":/") // resource path have to be converted to resource url
        tmpUrl = "qrc:///" + tmpUrl.mid(2);
    page()->mainFrame()->load(tmpUrl);
}

void GSuperWebPage::onJavaScriptWindowObjectCleared() {
    //qDebug() << "GSuperWebPage::onJavaScriptWindowObjectCleared: " << objectName();
    if (m_chromeWidget)
        m_chromeWidget->exportJSObjectsToPage(m_page);
}

}  // GVA namespace

