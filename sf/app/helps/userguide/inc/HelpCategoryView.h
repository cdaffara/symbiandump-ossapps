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

#ifndef HELPCATEGORYVIEW_H
#define HELPCATEGORYVIEW_H

#include <HelpBaseView.h>

#include "HelpDocumentLoader.h"

class HbTreeView;
class QStandardItem;

class HelpCategoryView : public HelpBaseView
{
	Q_OBJECT
	
public:
	HelpCategoryView();
	~HelpCategoryView();
	void init();

private:
	void initDocMl();
	void initAllList();
	
signals:
    // Automation is using MATTI tool and to do performance measuring we need signals as measuring point.
    void applicationReady();

private:
	void expandCollapseAllList(QStandardItem* item, bool expand);
	void updateExpandCollapseAction(bool expand);
    
private slots: // handle list event
    void onAllListActivated(const QModelIndex& index);

private slots: // handle menu event
	void onExpandOrCollapseAll();

private:
	HbTreeView*	mListAll;
	int			mExpandCount;
};

#endif //HELPCATEGORYVIEW_H
