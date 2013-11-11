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

#include "cntfetchmarkall.h"

#include <QGraphicsLinearLayout>
#include <hbcheckbox.h>
#include <hbtextitem.h>
#include <hbframeitem.h>

CntFetchMarkAll::CntFetchMarkAll(QGraphicsItem *aParent)
: HbWidget(aParent),
  mCheckBox(NULL),
  mCounterLabel(NULL),
  mContactCount( 0 ),
  mSelectionCount( 0 )
{
    mCheckBox = new HbCheckBox(this);
    mCheckBox->setText(hbTrId("txt_common_list_mark_all_items"));
    connect(mCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(markAll(int)) );
    
    mCounterLabel = new HbTextItem( this );
    
    mFrame = new HbFrameItem(this);
    mFrame->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox");
    mFrame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
    mFrame->setZValue(-5);
    
    style()->setItemName(mFrame, "background");
           
    HbStyle::setItemName(mCheckBox,"checkbox");
    HbStyle::setItemName(mCounterLabel,"counter");
}

void CntFetchMarkAll::setMaxContactCount( int aContactCount )
{
    mContactCount = aContactCount;
    mCounterLabel->setText( QString("%1/%2").arg(mSelectionCount).arg(mContactCount) );
}

void CntFetchMarkAll::setSelectedContactCount( int aSelectedCount )
{
    mSelectionCount = aSelectedCount;
    mCounterLabel->setText( QString("%1/%2").arg(mSelectionCount).arg(mContactCount) );
    
    if ( mSelectionCount < mContactCount )
    {
        blockSignals( true );
        mCheckBox->setCheckState( Qt::Unchecked );
        blockSignals( false );
    }
    else if (mSelectionCount == mContactCount)
    {
        blockSignals( true );
        mCheckBox->setCheckState( Qt::Checked );
        blockSignals( false );
    }
}

CntFetchMarkAll::~CntFetchMarkAll()
{
    delete mCheckBox;
    mCheckBox = NULL;
    delete mCounterLabel;
    mCounterLabel = NULL;
    delete mFrame;
    mFrame = NULL;
}

// EOF
