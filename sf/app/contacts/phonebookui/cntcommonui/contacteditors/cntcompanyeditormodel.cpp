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
#include "cntcompanyeditormodel.h"
#include "cntdetailconst.h"
#include <qcontactorganization.h>

CntCompanyEditorModel::CntCompanyEditorModel(QContact* aContact) :
    CntDetailEditorModel(aContact)
{
    QList<QContactOrganization> orgList = mContact->details<QContactOrganization> ();
    if (orgList.isEmpty()) {
        QContactOrganization organization;
        orgList << organization;
    }

    mCompany = orgList.first();

    HbDataFormModelItem* organization = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        hbTrId("txt_phob_formlabel_company"));
    organization->setContentWidgetData("text", mCompany.name());
    organization->setContentWidgetData("maxLength", CNT_ORGANIZATION_MAXLENGTH);

    HbDataFormModelItem* jobTitle = new HbDataFormModelItem(HbDataFormModelItem::TextItem, 
        hbTrId("txt_phob_formlabel_job_title"));
    jobTitle->setContentWidgetData("text", mCompany.title());
    jobTitle->setContentWidgetData("maxLength", CNT_JOBTITLE_MAXLENGTH);

    HbDataFormModelItem* department = new HbDataFormModelItem(HbDataFormModelItem::TextItem,
        hbTrId("txt_phob_formlabel_department"));
    department->setContentWidgetData("text", mCompany.department());
    department->setContentWidgetData("maxLength", CNT_DEPARTMENT_MAXLENGTH);

    HbDataFormModelItem* assistant = new HbDataFormModelItem(HbDataFormModelItem::TextItem, 
        hbTrId("txt_phob_formlabel_assistant"));
    assistant->setContentWidgetData("text", mCompany.assistantName());
    assistant->setContentWidgetData("maxLength", CNT_ASSISTANT_MAXLENGTH);

    HbDataFormModelItem* root = invisibleRootItem();
    appendDataFormItem(organization, root);
    appendDataFormItem(jobTitle, root);
    appendDataFormItem(department, root);
    appendDataFormItem(assistant, root);
}

CntCompanyEditorModel::~CntCompanyEditorModel()
{
}

void CntCompanyEditorModel::saveContactDetails()
{
    HbDataFormModelItem* root = invisibleRootItem();
    QString name = root->childAt(0)->contentWidgetData("text").toString();
    QString title = root->childAt(1)->contentWidgetData("text").toString();
    QString department = root->childAt(2)->contentWidgetData("text").toString();
    QString assistant = root->childAt(3)->contentWidgetData("text").toString();
    
    if ( mCompany.name() != name ||
         mCompany.title() != title || 
         mCompany.department().join(", ") != department ||
         mCompany.assistantName() != assistant )
    {
        mCompany.setName( name );
        mCompany.setTitle( title );
        mCompany.setDepartment( department.split(", ", QString::SkipEmptyParts) );
        mCompany.setAssistantName( assistant );
        mContact->saveDetail( &mCompany );
    }
    
    if ( mCompany.name().isEmpty() && 
         mCompany.title().isEmpty() &&
         mCompany.department().isEmpty() &&
         mCompany.assistantName().isEmpty() )
    {
        mContact->removeDetail( &mCompany );
    }
}

QContactDetail CntCompanyEditorModel::detail() const
{
    return mCompany;
}
// End of File
