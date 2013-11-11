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

#ifndef CNTEDITVIEWITEMSUPPLIER_H
#define CNTEDITVIEWITEMSUPPLIER_H

#include <QObject>

#include <cnteditviewitem.h>

/**
 * UI extension class that supplies items for the edit view list
 */
class CntEditViewItemSupplier
{
 public:

    /**
     * Returns number of items provided by the extension.
     *
     * @return int, item count
     */      
    virtual int itemCount() const = 0;

    /**
     * Returns specific edit view item.
     *
     * @param int, item index 
     * @return CntEditViewItem, item
     */      
    virtual CntEditViewItem* itemAt(int aIndex) = 0;

protected:
    // prevent deleting by client
    virtual ~CntEditViewItemSupplier() {}
};

#endif //CNTEDITVIEWITEMSUPPLIER_H

