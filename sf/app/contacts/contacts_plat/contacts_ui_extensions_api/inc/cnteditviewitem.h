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

#ifndef CNTEDITVIEWITEM_H
#define CNTEDITVIEWITEM_H

#include <QObject>
#include <QVariant>
#include <QPointF>
#include <hbnamespace.h>
#include <cnteditviewitemcallback.h>

// Value enum for EditViewItemRole::ERoleItemType
enum EditViewItemType
{
    ETypeUiExtension = 0,
    ETypeDetailItem
};

enum EditViewItemRole
{
    ERoleItemType = Hb::UserRole,
    ERoleEditorViewId,
    ERoleContactDetailFields,
    ERoleContactDetail,
    ERoleContactDetailDef
};

/**
 * Handle to specific item's data.
 */
class CntEditViewItem
{
public:

    virtual ~CntEditViewItem() {}

    /**
     * Returns data for a given role.
     * ETypeUiExtension must be returned for ERoleItemType
     *
     * Other supported roles and restrictions:
     * Qt::DisplayRole -> QStringList with 1-2 QStrings
     * Qt::DecorationRole -> 0-1 HbIcon
     *
     * @param int, role
     * @return QVariant, data for the given role
     */
    virtual QVariant data(int role) const = 0;

    /**
     * Called when this item is activated (tapped) in the list.
     *
     * @param CntEditViewItemCallback*, callback interface for possible view switching and list refreshing requests, ownership NOT given
     */   
    virtual void activated(CntEditViewItemCallback* interface) = 0;

    /**
     * Called when this item is longpressed in the list.
     *
     * @param QPointF&, coordinates from where the longpress was initiated
     * @param CntEditViewItemCallback*, callback interface for possible view switching and list refreshing requests, ownership NOT given
     */   
    virtual void longPressed(const QPointF& coords, CntEditViewItemCallback* interface) = 0;
};

#endif // CNTEDITVIEWITEM_H

