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
#include "cpdivertitemdata.h"

CpDivertItemData::CpDivertItemData(
                HbDataFormModelItem::DataItemType type,
                const QString &label,
                CpItemDataHelper &helper,
                const HbDataFormModelItem *parent) :
                CpSettingFormItemData(
                        static_cast<HbDataFormModelItem::DataItemType>(type),
                        label, parent ),
                m_helper(helper)
{
    m_helper.addConnection(
        this, SIGNAL(clicked()),
        this, SLOT(thisItemClicked()));
}

CpDivertItemData::~CpDivertItemData()
{

}

void CpDivertItemData::thisItemClicked()
{
    emit itemClicked(*this);
}


// end of file
