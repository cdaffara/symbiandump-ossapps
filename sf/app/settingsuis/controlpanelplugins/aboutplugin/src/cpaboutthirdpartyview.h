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
#ifndef	CPABOUTTHIRDPARTYVIEW_H
#define	CPABOUTTHIRDPARTYVIEW_H

#include "cpabout_global.h"
#include <HbView>

class CpAboutThirdPartyView : public HbView
{
    Q_OBJECT
    
public:
    explicit CpAboutThirdPartyView(QGraphicsItem *parent = 0);
    virtual ~CpAboutThirdPartyView();
    
private slots: 
    void handleLinkClicked(const QString &anchor);
    
private:
    
    ABOUT_TEST_FRIEND_CLASS(TestAboutPlugin)
};
#endif //CPABOUTTHIRDPARTYVIEW_H
