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
 * Description: hsmenuserviceutils.cpp
 *
 */

#include "hsmenuserviceutils.h"

/*!
 Gets sort attribute
 \param sortAttribute menu sort order
 */
SortAttribute HsMenuServiceUtils::sortBy(
    Hs::HsSortAttribute menuSortAttribute)
{
    SortAttribute sortAttribute(DefaultSortAttribute);
    switch (menuSortAttribute) {
    case Hs::AscendingNameHsSortAttribute:
    case Hs::DescendingNameHsSortAttribute:
        sortAttribute = NameSortAttribute;
        break;
    case Hs::NoHsSortAttribute:
    case Hs::LatestOnTopHsSortAttribute:
    case Hs::OldestOnTopHsSortAttribute:
    case Hs::CustomHsSortAttribute:
        sortAttribute = DefaultSortAttribute;
        break;
    }
    return sortAttribute;
}

/*!
 Gets sort attribute
 \param sortAttribute menu sort order
 */
Qt::SortOrder HsMenuServiceUtils::sortOrder(
    Hs::HsSortAttribute menuSortAttribute)
{
    Qt::SortOrder sortOrder(Qt::AscendingOrder);
    switch (menuSortAttribute) {
    case Hs::NoHsSortAttribute:
    case Hs::AscendingNameHsSortAttribute:
    case Hs::LatestOnTopHsSortAttribute:
    case Hs::CustomHsSortAttribute:
        sortOrder = Qt::AscendingOrder;
        break;
    case Hs::DescendingNameHsSortAttribute:
    case Hs::OldestOnTopHsSortAttribute:
        sortOrder = Qt::DescendingOrder;
        break;
    }
    return sortOrder;
}
