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

#ifndef HELPCONTENTSVIEW_H
#define HELPCONTENTSVIEW_H

#include <QUrl>

#include <HelpBaseView.h>

#include "HelpDocumentLoader.h"

class BrowserWrapper;

class HelpContentsView : public HelpBaseView
{
	Q_OBJECT
	
public:
	HelpContentsView();
	~HelpContentsView();
	
public:
	void init();
	
private:
	void initDocMl();
    void initBackAction();	
 
private:
	bool openApplication(const QUrl& url);
	bool openExternalLink(const QUrl& url);
	void openHelpContent(const QUrl& url=QUrl());

private slots: // handle system event
    void onViewReady();
    
private slots: // handle button action
    void onBackAction();
    
private slots: // handle browser event
	void onLinkClicked(const QUrl& url);
	void onUrlChanged(const QUrl& url);

private:
    BrowserWrapper*     mBrowser;
    HbAction* mSoftKeyAction;
};

#endif //HELPCONTENTSVIEW_H
