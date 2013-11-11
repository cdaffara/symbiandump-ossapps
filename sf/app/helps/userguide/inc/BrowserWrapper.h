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

#ifndef BROWSERWRAPPER_H
#define BROWSERWRAPPER_H

#include <QStack>
#include <QGraphicsWebView>

#include <hbview.h>

class BrowserWrapper : public HbView
{
    Q_OBJECT
    
public:
    BrowserWrapper();
	~BrowserWrapper();

public:
	void init();
	
public:
	void setHtml(const QString& html, const QUrl& url);
	void clearHistory();
	bool canGoBack();
	void back();
	
signals:
    void linkClicked(const QUrl &url);
    void urlChanged(const QUrl&);

private:
	QGraphicsWebView*      mWebView;
	QStack<QUrl>   mHistory;
	
};

#endif //MYTEXTBROWSER_H
