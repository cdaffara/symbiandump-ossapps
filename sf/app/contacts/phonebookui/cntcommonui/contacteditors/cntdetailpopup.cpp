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

#include "cntdetailpopup.h"

#include <hblabel.h>
#include <QStandardItemModel>

CntDetailPopup::CntDetailPopup(QGraphicsItem *parent,  CntViewIdList aList ):
    HbSelectionDialog(parent),
    mListModel(NULL),
    mViewIdList( aList )
{
    mListModel = new QStandardItemModel(this);

    addListItem("qtg_small_mobile", hbTrId("txt_phob_list_number"), phoneNumberEditorView );
    addListItem("qtg_small_email", hbTrId("txt_phob_list_email"), emailEditorView );
    addListItem("qtg_small_url_address", hbTrId("txt_phob_list_url"), urlEditorView);
    addListItem("qtg_small_location", hbTrId("txt_phob_list_address"), addressEditorView );
    addListItem("qtg_small_note", hbTrId("txt_phob_list_note"), noteEditorView);
    addListItem("qtg_small_sound", hbTrId("txt_phob_list_personal_ringing_tone"), ringToneFetcherView );
    addListItem("qtg_small_calendar", hbTrId("txt_phob_list_date"), dateEditorView);
    addListItem("qtg_small_company_details", hbTrId("txt_phob_list_company_details"), companyEditorView);
    addListItem("qtg_small_family", hbTrId("txt_phob_list_family"), familyDetailEditorView);
    
    HbLabel *label = new HbLabel(this);    
    label->setPlainText(hbTrId("txt_phob_title_add_field"));

    setHeadingWidget(label);
    setTimeout(HbDialog::NoTimeout);
    setDismissPolicy(HbDialog::NoDismiss);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setModal(true);
    setSelectionMode(HbAbstractItemView::NoSelection);
    setModel(mListModel);
}

CntDetailPopup::~CntDetailPopup()
{
    
}

void CntDetailPopup::selectDetail( CntViewIdList aList, QObject *receiver, const char *member  )
{
    CntDetailPopup *popup = new CntDetailPopup( NULL, aList );
    
    popup->open(receiver, member);
}

void CntDetailPopup::addListItem(QString aIcon, QString label, int aId )
{
    if ( !mViewIdList.contains(aId) )
    {
        QStandardItem *labelItem = new QStandardItem(label);
        labelItem->setData(HbIcon(aIcon), Qt::DecorationRole);
        labelItem->setData(aId, Qt::UserRole);

        mListModel->appendRow(labelItem);
    }
}
