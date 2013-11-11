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
*     Zhiqiang Yang <zhiqiang.yang@nokia.com>
* 
* Description:
*     The header file of the split view of file manager
*
*/
#ifndef FMSPLITVIEW_H
#define FMSPLITVIEW_H

#include "fmcommon.h"
#include "fmviewbase.h"

class FmDriverListWidget;
class FmFileBrowseWidget;

class FmSplitView : public FmViewBase
{
	Q_OBJECT
public:
    FmSplitView();
    virtual ~FmSplitView();

private slots:
	void on_normalViewAction_triggered();
	void on_rotateAction_triggered();

private:
    void initMenu();
    void initMainWidget();


    FmDriverListWidget *mDriverWidget;
    FmFileBrowseWidget *mFileWidget;
};

#endif
