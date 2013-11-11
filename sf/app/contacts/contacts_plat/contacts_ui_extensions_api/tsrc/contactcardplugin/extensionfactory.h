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

#ifndef EXTENSIONFACTORY_H
#define EXTENSIONFACTORY_H

//System includes
#include <QObject>

//User includes
#include <cntuiextensionfactory.h>

class ContactCardExtension;

/**
 * Factory class for plugin
 */
class ExtensionFactory : public QObject, public CntUiExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(CntUiExtensionFactory)

public:
    /**
     * Constructor
     */
    ExtensionFactory();

    /**
     * Desctructor
     */
    ~ExtensionFactory();

public: // from CntUiExtensionFactory

    /**
     * Gives access to an interface for extending Contact Card.
     * @return Returns null if not supported. Ownership is not given.
     */
    CntUiContactCardExtension* contactCardExtension();

    /**
     * Gives access to an interface for extending UI with new groups.
     * @return Returns null if not supported. Ownership is not given.
     */
    CntUiGroupSupplier* groupSupplier();

    /**
     * Gives access to an interface for extending edit view list with new items for a given contact.
     * @return Returns null if not supported. Ownership is not given.
     */
    CntEditViewItemSupplier* editViewItemSupplier(QContact &contact);

    /**
     * Gives access to an interface for creating new views.
     * @return Returns null if not supported. Ownership is not given.
     */
    CntViewSupplier* viewSupplier();

    CntUiSocialExtension* socialExtension();
        
    bool hasService(int serviceId);

private:
    ContactCardExtension* mContactCardExtension;
};

#endif // EXTENSIONFACTORY_H
