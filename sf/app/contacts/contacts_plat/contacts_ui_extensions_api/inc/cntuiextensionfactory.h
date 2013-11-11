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

//
//  W A R N I N G
//  -------------
//
// FOR INTERNAL USE ONLY!
//
// This file is not the final version. These interfaces may and will
// change without any notice until they are final. Please do not use them.
//

#ifndef CNTUIEXTENSIONFACTORY_H
#define CNTUIEXTENSIONFACTORY_H

#include <QtPlugin>
#include <qcontact.h>

class CntContactCardActionSupplier;
class CntUiGroupSupplier;
class CntUiSocialExtension;
class CntEditViewItemSupplier;
class CntViewSupplier;
class CntUiContactCardExtension;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/**
 * A factory which gives access to UI extension interfaces.
 * The plugin can extend the needed extensions and return null for the
 * unsupported extension types.
 *
 * Plugins must be exported to /resource/qt/plugins/contacts/extensions
 */
class CntUiExtensionFactory
{
 public:
    virtual ~CntUiExtensionFactory() {}

    /**
     * Gives access to an interface for extending Contact Card.
     * @return Returns null if not supported. Ownership is not given.
     */
    virtual CntUiContactCardExtension* contactCardExtension() = 0;

    /**
     * Gives access to an interface for extending UI with new groups.
     * @return Returns null if not supported. Ownership is not given.
     */
    virtual CntUiGroupSupplier* groupSupplier() = 0;

    /**
     * Gives access to an interface for extending edit view list with new items for a given contact.
     * @return Returns null if not supported. Ownership is not given.
     */
    virtual CntEditViewItemSupplier* editViewItemSupplier(QContact &contact) = 0;

    /**
     * Gives access to an interface for creating new views.
     * @return Returns null if not supported. Ownership is not given.
     */
    virtual CntViewSupplier* viewSupplier() = 0;

    virtual CntUiSocialExtension* socialExtension() = 0;

    virtual bool hasService(int serviceId) = 0;
    
    virtual void* ExtensionFactoryExt(
            int /*aExtensionId*/ )
    {
        return NULL;
    }

};

Q_DECLARE_INTERFACE(CntUiExtensionFactory, "com.nokia.contacts.uiextensionfactory/1.0")

#endif // CNTUIEXTENSIONFACTORY_H

