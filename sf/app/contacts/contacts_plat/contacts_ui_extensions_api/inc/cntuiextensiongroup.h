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

#ifndef CNTUIEXTENSIONGROUP_H
#define CNTUIEXTENSIONGROUP_H

#include <QObject>
#include <cntviewparams.h>
#include <cntextensiongroupcallback.h>

class QPointF;

/**
 * Handle to specific group provided by the group supplier
 *
 * Example usage for CntViewParameters callbacks:
 *
 * MyExtensionGroup::activated(CntViewParameters& callBack) const {
 *     callBack.insert(EViewId, customView); <- when item is tapped, it opens the view specified here
 *     callBack.insert(ECustomParam, myCustomParam); <- custom parameters can also be used if needed
 * }
 * 
 * see cntviewparams.h for more information on view parameters
 */
class CntUiExtensionGroup
{
public:

    virtual ~CntUiExtensionGroup() {}
    
    /**
     * Returns name of the group.
     *
     * @return QString, name of the group item
     */     
    virtual QString name() const = 0;

    /**
     * Returns extra text for the group.
     *
     * @return QString, extra text for the group item
     */         
    virtual QString extraText() const = 0;

    /**
     * Returns member count for the group. This is displayed
     * in the top or bottom right corner of the list item depending
     * if the item has an extra icon. (portrait)
     * Wont be displayed if return value is < 1
     *
     * @return int, member count for the group item
     */         
    virtual int memberCount() const = 0;

    /**
     * Returns group icon.
     *
     * @return QString, icon for the group
     */             
    virtual QString groupIcon() const = 0;

    /**
     * Returns extra icon. This is displayed in top right corner
     * of the list item.
     *
     * @return QString, extra icon for the group
     */             
    virtual QString extraIcon() const = 0;

    /**
     * Returns service id for the group. This must be unique from
     * Service Settings Provider.
     *
     * @return int, unique service id for the group
     */               
    virtual int serviceId() const = 0;

    /**
     * Called when this item is activated (tapped) in the list. The callback is handled 
     * synchronously and therefore it is assumed that this should be used for
     * opening a new view.
     *
     * @param CntViewParameters&, callback for view switching
     */   
    virtual void activated(CntViewParameters& callBack) = 0;

    /**
     * Called when this item is longpressed in the list.
     *
     * @param QPointF&, coordinates from where the longpress was initiated
     * @param CntExtensionGroupCallback*, callback interface for view switching, ownership NOT given
     */   
    virtual void longPressed(const QPointF& coords, CntExtensionGroupCallback *interface) = 0;

    virtual void* GroupExtension(
            int /*aExtensionId*/ )
    {
        return NULL;
    }
};

#endif // CNTUIEXTENSIONGROUP_H
