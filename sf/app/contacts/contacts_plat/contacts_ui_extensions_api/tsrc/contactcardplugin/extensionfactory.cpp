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

//User include
#include "extensionfactory.h"
#include "contactcardextension.h"

//System includes
#include <Qtplugin>
//#include <cntactionsupplier.h>
//#include <cnteditviewitemsupplier.h>
//#include <cntuigroupsupplier.h>
//#include <cntviewsupplier.h>

ExtensionFactory::ExtensionFactory() : mContactCardExtension(NULL)
{
}

ExtensionFactory::~ExtensionFactory()
{
    delete mContactCardExtension;
}

CntUiContactCardExtension* ExtensionFactory::contactCardExtension()
{
    if (!mContactCardExtension)
    {
        mContactCardExtension = new ContactCardExtension();
    }
    return mContactCardExtension;
}

CntUiGroupSupplier* ExtensionFactory::groupSupplier()
{
    return NULL;
}

CntEditViewItemSupplier* ExtensionFactory::editViewItemSupplier(QContact &/*contact*/)
{
    return NULL;
}

CntViewSupplier* ExtensionFactory::viewSupplier()
{
    return NULL;
}

CntUiSocialExtension* ExtensionFactory::socialExtension()
{
    return NULL;
}

bool ExtensionFactory::hasService(int /*serviceId*/)
{
    return true;
}

Q_EXPORT_PLUGIN2(cntuiextensionexample,ExtensionFactory)
