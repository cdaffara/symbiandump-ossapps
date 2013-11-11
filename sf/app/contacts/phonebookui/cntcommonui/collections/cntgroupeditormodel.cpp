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
#include "cntgroupeditormodel.h"
#include "cntdetailconst.h"
#include "cntdetailorderinghelper.h"
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include "cntglobal.h"

CntGroupEditorModel::CntGroupEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactName> nameDetails = mContact->details<QContactName>();
    if (nameDetails.isEmpty())
    {
        QContactName emptyName;
        nameDetails.append(emptyName);
    }
    
    QList<QContactPhoneNumber> numberDetails = mContact->details<QContactPhoneNumber>();
    if (numberDetails.isEmpty())
    {
        QContactPhoneNumber emptyNumber;
        numberDetails.append(emptyNumber);
    }
    
    iGroupName = nameDetails.first();
    iGroupConfCallNumber = numberDetails.first();
    
    HbDataFormModelItem::DataItemType type = HbDataFormModelItem::TextItem;
    iNameItem = new HbDataFormModelItem(type, hbTrId("txt_phob_formlabel_group_name"));
    iNumberItem = new HbDataFormModelItem(type, hbTrId("txt_phob_formlabel_conference_number"));
    
    iNameItem->setContentWidgetData("text", iGroupName.customLabel());
    iNameItem->setContentWidgetData("maxLength", CNT_GROUPNAME_MAXLENGTH);
    iNumberItem->setContentWidgetData("text", iGroupConfCallNumber.number() );
    iNumberItem->setContentWidgetData("maxLength", CNT_PHONENUMBER_EDITOR_MAXLENGTH);
    
    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem( iNameItem, root);
    appendDataFormItem( iNumberItem, root);
}

CntGroupEditorModel::~CntGroupEditorModel()
{
}

bool CntGroupEditorModel::isConferenceNumber( const QModelIndex& aIndex )
{
    return ( itemFromIndex( aIndex ) == iNumberItem );
}

void CntGroupEditorModel::saveContactDetails()
{
    iGroupName.setCustomLabel( iNameItem->contentWidgetData("text").toString().trimmed());
    iGroupConfCallNumber.setNumber( iNumberItem->contentWidgetData("text").toString().trimmed());
    
    mContact->saveDetail( &iGroupName );
    mContact->saveDetail( &iGroupConfCallNumber );
    
    // remove empty details
    if (iGroupName.customLabel().isEmpty())
    {
        mContact->removeDetail( &iGroupName );
    }

    if(iGroupConfCallNumber.number().isEmpty())
    {
        mContact->removeDetail( &iGroupConfCallNumber );
    }
}

// This function is from API and is not used currently. "Implementation side-effect".
QContactDetail CntGroupEditorModel::detail() const
{
    return iGroupName;
}
// End of File
