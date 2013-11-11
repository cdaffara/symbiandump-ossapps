/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#ifndef	CPABOUTVIEW_H
#define	CPABOUTVIEW_H

#include <cpbasesettingview.h>
#include "cpabout_global.h"

class CpAboutThirdPartyView;
class CpAboutOpenSourceView;

class CpAboutView : public CpBaseSettingView
{
    Q_OBJECT
    
public:
    explicit CpAboutView(QGraphicsItem *parent = 0);
    virtual ~CpAboutView();
    
private slots:
    void openThirdPartyView();
    void openOpenSourceView();
    void handleThirdPartyViewBackAction();
    void handleOpenSourceViewBackAction();
    
private:    
    CpAboutThirdPartyView *mThirdPartyView;
    HbAction *mThirdPartyBackAction;
    CpAboutOpenSourceView *mOpenSourceView;
    HbAction *mOpenSourceBackAction;    
    ABOUT_TEST_FRIEND_CLASS(TestAboutPlugin)
};
#endif //CPABOUTVIEW_H
