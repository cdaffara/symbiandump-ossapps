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
* Description:  Trash bin widget.
*
*/

#ifndef HSTRASHBINWIDGET_H
#define HSTRASHBINWIDGET_H

#include <hbwidget.h>

#include "hsutils_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(t_hsUtils)

class HbIconItem;

class HSUTILS_EXPORT HsTrashBinWidget : public HbWidget
{
	Q_OBJECT

public:
    HsTrashBinWidget(QGraphicsItem *parent = 0);
	virtual ~HsTrashBinWidget();

public:
    void activate();
    void deactivate();

private:
    HbIconItem *mIconItem;
    HbIcon *mNormalIcon;
    HbIcon *mHighlightIcon;
    bool mNormalIconVisible;

    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};
 
#endif // HSTRASHBINWIDGET_H
