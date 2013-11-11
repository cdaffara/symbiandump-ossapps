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

#ifndef CntCommonDetailViewItem_H
#define CntCommonDetailViewItem_H

#include "hbwidget.h"

class HbComboBox;
class HbLineEdit;

class CntCommonDetailViewItem : public HbWidget
{
    Q_OBJECT
    
public:
    CntCommonDetailViewItem(QGraphicsItem *aParent = 0);
    
    HbComboBox* comboBox() const;
    HbLineEdit* editor() const;

private:
    HbComboBox*   mPhoneTypeSelector;
    HbLineEdit*   mDataEditor;
    
    friend class T_CommonDetailViewItemTest;
};

#endif // CntCommonDetailViewItem_H
