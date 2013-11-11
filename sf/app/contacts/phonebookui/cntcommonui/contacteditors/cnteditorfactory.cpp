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

#include "cnteditorfactory.h"
#include "cntnameeditormodel.h"
#include "cntphonenumbermodel.h"
#include "cntphonenumberviewitem.h"
#include "cntemaileditormodel.h"
#include "cntemaileditorviewitem.h"
#include "cntaddressmodel.h"
#include "cntaddressviewitem.h"
#include "cnturleditormodel.h"
#include "cnturleditorviewitem.h"
#include "cntnoteeditormodel.h"
#include "cntnoteeditorviewitem.h"
#include "cntdateeditormodel.h"
#include "cntdateeditorviewitem.h"
#include "cntcompanyeditormodel.h"
#include "cntfamilyeditormodel.h"
#include "cntdetaileditor.h"
#include "cntgroupeditormodel.h"
#include "cntabstractview.h"
#include "cntdetaileditor.h"

CntEditorFactory::CntEditorFactory( QObject* aParent ) :
    QObject(aParent)
{
}

CntEditorFactory::~CntEditorFactory()
{
}

/*!
The View's Id should have been set before calling this function.
*/
void CntEditorFactory::setupEditorView(CntDetailEditor &aEditor, QContact &aContact)
{
    int viewId = aEditor.viewId();
    
    switch ( viewId )
    {
    case emailEditorView: 
    {
        aEditor.setDetails(new CntEmailEditorModel(new QContact(aContact)), new CntEmailEditorViewItem);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_email_address"));
        aEditor.setInsertAction(hbTrId("txt_phob_opt_add_email"));
        break;
    }
        
    case namesEditorView:
    {
        aEditor.setDetails(new CntNameEditorModel(new QContact(aContact)), NULL);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_contact_name"));
        break;
    }

    case urlEditorView:
    {
        aEditor.setDetails(new CntUrlEditorModel(new QContact(aContact)), new CntUrlEditorViewItem);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_web_address"));
        aEditor.setInsertAction(hbTrId("txt_phob_opt_add_url"));
        break;
    }
    
    case companyEditorView:
    {
        aEditor.setDetails(new CntCompanyEditorModel(new QContact(aContact)), NULL);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_company_details"));
        break;
    }
    
    case phoneNumberEditorView:
    {
        aEditor.setDetails(new CntPhoneNumberModel(new QContact(aContact)), new CntPhoneNumberViewItem);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_phone_number"));
        aEditor.setInsertAction(hbTrId("txt_phob_opt_add_number"));
        break;
    }
    
    case noteEditorView:
    {
        aEditor.setDetails(new CntNoteEditorModel(new QContact(aContact)), new CntNoteEditorViewItem);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_note"));
        aEditor.setInsertAction(hbTrId("txt_phob_opt_add_note"));
        break;
    }

    case familyDetailEditorView:
    {
        aEditor.setDetails(new CntFamilyEditorModel(new QContact(aContact)), NULL);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_family_details"));
        break;
    }

    case addressEditorView:
    {
        aEditor.setDetails(new CntAddressModel(new QContact(aContact)),  new CntAddressViewItem() );
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_address_details"));
        break;
    }

    case dateEditorView:
    {
        aEditor.setDetails(new CntDateEditorModel(new QContact(aContact)), new CntDateEditorViewItem);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_date"));
        break;
    }
        
    case groupEditorView:
    {
        aEditor.setDetails(new CntGroupEditorModel(new QContact(aContact)), NULL);
        aEditor.setHeader(hbTrId("txt_phob_subtitle_edit_group_details"));
        break;
    }

    default:
       break;
    }
}
// End of File
