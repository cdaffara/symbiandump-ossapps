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
 * Description: hsmenuserviceutils.h
 *
 */

#ifndef HSMENUSERVICEUTILS_H
#define HSMENUSERVICEUTILS_H

// System includes
#include <QSize>

// User includes
#include "hsmenuservice.h"

// Class declaration
class MENUSERVICE_EXPORT HsMenuServiceUtils
{

public:

    // Function declarations
    static SortAttribute sortBy(
        Hs::HsSortAttribute menuSortAttribute);
    static Qt::SortOrder sortOrder(
        Hs::HsSortAttribute menuSortAttribute);

};

#endif // HSMENUSERVICEUTILS_H
