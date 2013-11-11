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
#include "cntfamilyeditormodel.h"
#include "cntdetailconst.h"
#include <qcontactfamily.h>
#include <hbdataformmodelitem.h>

CntFamilyEditorModel::CntFamilyEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactFamily> familyList = mContact->details<QContactFamily> ();
    if (familyList.isEmpty()) {
        QContactFamily family;
        familyList << family;
    }

    mFamily = familyList.first();
    
    HbDataFormModelItem* spouseItem = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        hbTrId("txt_phob_formlabel_spouse"));
    spouseItem->setContentWidgetData("text", mFamily.spouse());
    spouseItem->setContentWidgetData("maxLength", CNT_SPOUSE_MAXLENGTH);

    HbDataFormModelItem* childrenItem = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        hbTrId("txt_phob_formlabel_children"));
    childrenItem->setContentWidgetData("text", mFamily.children().join(", "));
    childrenItem->setContentWidgetData("maxLength", CNT_CHILDREN_MAXLENGTH);

    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem(spouseItem, root);
    appendDataFormItem(childrenItem, root);
}

CntFamilyEditorModel::~CntFamilyEditorModel()
{
}

void CntFamilyEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    QString spouse = root->childAt(0)->contentWidgetData("text").toString();
    QString children = root->childAt(1)->contentWidgetData("text").toString();
    
    if ( spouse != mFamily.spouse() || children != mFamily.children().join(", "))
    {
        mFamily.setSpouse( spouse );
        QString children = root->childAt(1)->contentWidgetData("text").toString();
        mFamily.setChildren( children.split(", ", QString::SkipEmptyParts) );
        
        mContact->saveDetail( &mFamily );
    }
    
    if ( mFamily.spouse().isEmpty() && mFamily.children().isEmpty() )
    {
        mContact->removeDetail( &mFamily );
    }
    
}

QContactDetail CntFamilyEditorModel::detail() const
{
    return mFamily;
}
// End of File
