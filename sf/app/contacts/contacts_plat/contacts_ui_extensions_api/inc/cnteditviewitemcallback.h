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

#ifndef CNTEDITVIEWITEMCALLBACK_H
#define CNTEDITVIEWITEMCALLBACK_H

#include <cntviewparams.h>

/**
 * Class for asynchronously handling action callbacks from cnteditviewitems
 */
class CntEditViewItemCallback
{
public:

    /**
     * Handles the possibly needed view switching callbacks from CntEditViewItem::activated(...)
     * and longPressed(...)
     *
     * @param CntViewParameters&, callback for view switching
     */
    virtual void openView(CntViewParameters& viewParams) = 0;

    /**
     * Handles the refresh requests from editviewitems
     */
    virtual void requestRefresh() = 0;

protected:
    // prevent deleting by client
    virtual ~CntEditViewItemCallback() {}
};

#endif //CNTEDITVIEWITEMCALLBACK_H
