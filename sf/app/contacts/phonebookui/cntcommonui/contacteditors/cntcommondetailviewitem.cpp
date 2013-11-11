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
#include "cntcommondetailviewitem.h"

#include <hbcombobox.h>
#include <hblineedit.h>


CntCommonDetailViewItem::CntCommonDetailViewItem(QGraphicsItem *aParent):
HbWidget(aParent)
{
    mPhoneTypeSelector = new HbComboBox(this);
    style()->setItemName(mPhoneTypeSelector, "combobox");

    mDataEditor = new HbLineEdit(this);
    style()->setItemName(mDataEditor, "editbox");
    mDataEditor->setInputMethodHints(Qt::ImhNoPredictiveText);
}

HbComboBox* CntCommonDetailViewItem::comboBox() const
{
    return mPhoneTypeSelector;
}

HbLineEdit* CntCommonDetailViewItem::editor() const
{
    return mDataEditor;
}

// End of File
