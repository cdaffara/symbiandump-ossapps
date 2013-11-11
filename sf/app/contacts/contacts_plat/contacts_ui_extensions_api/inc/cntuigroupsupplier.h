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

#ifndef CNTUIGROUPSUPPLIER_H
#define CNTUIGROUPSUPPLIER_H

#include <QObject>
#include <cntuiextensiongroup.h>

/**
 * UI extension class that supplies groups to phonebook groups UI.
 */
class CntUiGroupSupplier : public QObject
{
public:
    CntUiGroupSupplier() {}

    /**
     * Returns true if this supplier should be handled asynchronously and
     * false otherwise.
     *
     * @return bool, asynchronity
     */  
    virtual bool isAsynch() const = 0;

    /**
     * Returns number of groups provided by the extension.
     *
     * @return int, group count
     */      
    virtual int groupCount() const = 0;

    /**
     * Returns specific group handle which can be used to access
     * members of the group.
     *
     * @param int, group index 
     * @return CntUiExtensionGroup&, group handle
     */      
    virtual CntUiExtensionGroup& groupAt( int aIndex ) const = 0;

    virtual void* GroupSupplierExtension(
            int /*aExtensionId*/ )
    {
        return NULL;
    }

signals:
    /**
     * Asynchronous suppliers should re-implement this signal and emit it
     * once group loading is ready.
     */  
    void groupsReady();

protected:
    // prevent deleting by client
    virtual ~CntUiGroupSupplier() {}
};

#endif //CNTUIGROUPSUPPLIER_H

