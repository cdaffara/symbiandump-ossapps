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

#ifndef HELPKEYWORDVIEW_H
#define HELPKEYWORDVIEW_H

#include <HelpBaseView.h>

#include "HelpDocumentLoader.h"

class HbListView;
class HbSearchPanel;
class HbStaticVkbHost;
class HbLabel;

class HelpKeywordView : public HelpBaseView
{
	Q_OBJECT
	
public:
	HelpKeywordView();
	~HelpKeywordView();
	void init();
	void loadAllContent();

private:
	void initDocMl();
	void initSearchList();
	void initSearchPanel();
	void initBackAction();
	void initVirtualKeyboard();

private:
	HbLabel* label();
    
private:
	void updateVisibleItems(bool visible);
	void ResetSearchPanel();	
	void updateLabelPos();
	void showToolBar(bool visible);

private slots:
	void onOrientationChanged(Qt::Orientation orientation);

private slots: // handle system event
    void onViewReady();

private slots: // handle button action
    void onBackAction();
    
private slots: // handle list event
    void onSearchListActivated(const QModelIndex& index);

private slots: // handle search panel event
	void onSearchPanelExitClicked();
	void onSearchPanelCriteriaChanged(const QString &criteria);

private slots: // handle virtual keyboard event
    void onHandleKeypadOpen();
    void onHandleKeypadClose();

private:
	HbListView*			mListSearch;
	HbSearchPanel*		mSearchPanel;
	HbAction*			mSoftKeyAction;
	HbStaticVkbHost*	mVirtualKeyboard;
};

#endif //HELPKEYWORDVIEW_H
