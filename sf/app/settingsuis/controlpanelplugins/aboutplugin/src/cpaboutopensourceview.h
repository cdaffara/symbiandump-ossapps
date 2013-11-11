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
#ifndef	CPABOUTOPENSOURCEVIEW_H
#define	CPABOUTOPENSOURCEVIEW_H

#include "cpabout_global.h"
#include <HbView>

class CpAboutOpenSourceView : public HbView
{
    Q_OBJECT
public:
    explicit CpAboutOpenSourceView(QGraphicsItem *parent = 0);
    virtual ~CpAboutOpenSourceView();
    
private:
    ABOUT_TEST_FRIEND_CLASS(TestAboutPlugin)
};
#endif //CPABOUTOPENSOURCEVIEW_H
