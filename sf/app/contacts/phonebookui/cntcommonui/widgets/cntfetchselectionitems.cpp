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

#include "cntfetchselectionitems.h"

#include <QGraphicsLinearLayout>

#include <hbcheckbox.h>
#include <hblabel.h>


CntFetchSelectionItems::CntFetchSelectionItems(QGraphicsItem *aParent)
: HbWidget(aParent),
  mCheckBox(NULL),
  mCounterLabel(NULL),
  mLayout(NULL)
{
    if (!mLayout) {
        mLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    }

    if (!mCheckBox) {
        mCheckBox = new HbCheckBox(hbTrId("txt_common_list_mark_all_items"));
        connect(mCheckBox, SIGNAL(stateChanged(int)), this, SLOT(handleStateChange(int)), Qt::UniqueConnection);
        mLayout->addItem(mCheckBox);
    }

    if (!mCounterLabel) {
        mCounterLabel = new HbLabel();
        mLayout->addItem(mCounterLabel);
    }
    
    setLayout(mLayout);
}

CntFetchSelectionItems::~CntFetchSelectionItems()
{
}

HbLabel* CntFetchSelectionItems::counter() const
{
    return mCounterLabel;   
}

void CntFetchSelectionItems::handleStateChange(int aState)
{
    emit passStateChanged(aState);
}

// EOF
