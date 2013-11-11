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

#ifndef HELPBATHVIEW_H
#define HELPBATHVIEW_H

#include <hbview.h>

#include "HelpCommon.h"
#include "HelpDocumentLoader.h"

class HbToolBar;

class HelpBaseView : public HbView
{
	Q_OBJECT
	
public:
	HelpBaseView();
	~HelpBaseView();

signals:
    void activateView(HelpViewName viewName);

signals: // from tollbar event
    void showAllList();
    void showFindList();

private slots: // handle tollbar event
    void onToolbarAll()		{ emit showAllList(); };
    void onToolbarFind()	{ emit showFindList(); };
    void onToolbarOnlineSupport();

protected:
	void initBaseDocMl();

protected:
	HelpUIBuilder	mBuilder;
	HbToolBar*		mToolBar;
};

#endif //HELPBATHVIEW_H
