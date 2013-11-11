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

#ifndef T_HS_MOCK_MODEL_H_
#define T_HS_MOCK_MODEL_H_

#include <qstandarditemmodel.h>
#include "caentry.h"
#include "hsmenuitemmodel.h"


class MockModel: public HsMenuItemModel
{
    Q_OBJECT
    CaEntry mEntry;
public:
    MockModel();

    QModelIndex fixedIndex() const;

    const CaEntry *entry(const QModelIndex &index) const;
};

#endif
