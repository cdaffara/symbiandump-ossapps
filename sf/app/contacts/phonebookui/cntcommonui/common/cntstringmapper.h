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

#ifndef CNTSTRINGMAPPER_H
#define CNTSTRINGMAPPER_H

#include <qtcontacts.h>
#include <hbglobal.h>

QTM_USE_NAMESPACE

class Loc
{
public:
    inline Loc( QString aId, QString aContext, QString aLoc );
    inline ~Loc();
    
public:
    QString mId;
    QString mContext;
    QString mLoc;
};
class CntStringMapper
{

public:

    CntStringMapper() { initMappings(); }
    ~CntStringMapper() {}

public:
    void initMappings()
    {
        //editor icons
        mContactEditorIconList.append(Loc(QContactPhoneNumber::DefinitionName, "", "qtg_small_mobile"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, "qtg_small_mobile_home"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, "qtg_small_mobile_work"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", "qtg_small_mobile"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome, "qtg_small_mobile_home"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork, "qtg_small_mobile_work"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, "", "qtg_small_landline"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, "qtg_small_landline_home"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, "qtg_small_landline_work"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeFax, "", "qtg_small_fax"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, "qtg_small_fax_home"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, "qtg_small_fax_work"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypePager, "", "qtg_small_pager"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeCar, "", "qtg_small_car"));
        mContactEditorIconList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", "qtg_small_assistant"));
        mContactEditorIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", "qtg_small_voip"));
        mContactEditorIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, "qtg_small_voip_home"));
        mContactEditorIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, "qtg_small_voip_work"));
        mContactEditorIconList.append(Loc(QContactOnlineAccount::SubTypeSip, "", "qtg_small_internet"));
        mContactEditorIconList.append(Loc(QContactEmailAddress::DefinitionName, "", "qtg_small_email"));
        mContactEditorIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, "qtg_small_email_home"));
        mContactEditorIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, "qtg_small_email_work"));
        mContactEditorIconList.append(Loc(QContactUrl::DefinitionName, "", "qtg_small_url_address"));
        mContactEditorIconList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, "qtg_small_url_address_home"));
        mContactEditorIconList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, "qtg_small_url_address_work"));
        mContactEditorIconList.append(Loc(QContactAddress::DefinitionName, "", "qtg_small_location"));
      
        //editor list items
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_dblist_mobile")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_mobile_home")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_mobile_work")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "", hbTrId("txt_phob_dblist_phone")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_phone_home")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_phone_work")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_dblist_pager")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_dblist_assistant")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_dblist_car")));
        mContactEditorLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_dblist_internet_telephone")));
        mContactEditorLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_internet_telephone_home")));
        mContactEditorLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_internet_telephone_work")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeFax, "", hbTrId("txt_phob_dblist_fax")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_fax_home")));
        mContactEditorLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_fax_work")));
        mContactEditorLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_dblist_ptt")));

        //editor list item specific menu items
        mContactEditorAddLocList.append(Loc(QContactAddress::DefinitionName, "", hbTrId("txt_phob_menu_add_address")));
        mContactEditorAddLocList.append(Loc(QContactOrganization::DefinitionName, "", hbTrId("txt_phob_menu_add_company_details")));
        mContactEditorAddLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_menu_add_email")));
        mContactEditorAddLocList.append(Loc(QContactFamily::DefinitionName, "", hbTrId("txt_phob_menu_add_family_details")));
        mContactEditorAddLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_menu_add_number")));
        mContactEditorAddLocList.append(Loc(QContactOnlineAccount::DefinitionName, "", hbTrId("txt_phob_menu_add_number")));
        mContactEditorAddLocList.append(Loc(QContactUrl::DefinitionName, "", hbTrId("txt_phob_menu_add_url")));
        mContactEditorAddLocList.append(Loc(QContactNote::DefinitionName, "", hbTrId("txt_phob_menu_add_note")));
        
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_menu_edit_mobile")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_mobile_home")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_mobile_work")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_menu_edit_assistant")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_assistant")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_assistant")));   
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_menu_edit_car")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_car")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_car")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_menu_edit_mobile")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_menu_edit_mobile_home")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_menu_edit_mobile_work")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_menu_edit_phone")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_phone_home")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_phone_work")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeFax, "", hbTrId("txt_phob_menu_edit_fax")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_fax_home")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_fax_work")));
        mContactEditorEditLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_menu_edit_pager")));
        mContactEditorEditLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_menu_edit_internet_telephone")));
        mContactEditorEditLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_internet_telephone_home")));
        mContactEditorEditLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_internet_telephone_work")));
        mContactEditorEditLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_menu_edit_sip")));
        mContactEditorEditLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_menu_edit_email")));
        mContactEditorEditLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_email_home")));
        mContactEditorEditLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_email_work")));
        mContactEditorEditLocList.append(Loc(QContactAddress::DefinitionName, "", hbTrId("txt_phob_menu_edit_address")));
        mContactEditorEditLocList.append(Loc(QContactAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_address_home")));
        mContactEditorEditLocList.append(Loc(QContactAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_address_work")));
        mContactEditorEditLocList.append(Loc(QContactAnniversary::DefinitionName, "", hbTrId("txt_phob_menu_edit_anniversary")));
        mContactEditorEditLocList.append(Loc(QContactBirthday::DefinitionName, "", hbTrId("txt_phob_menu_edit_birthday")));
        mContactEditorEditLocList.append(Loc(QContactNote::DefinitionName, "", hbTrId("txt_phob_menu_edit_note")));
        mContactEditorEditLocList.append(Loc(QContactFamily::FieldChildren, "", hbTrId("txt_phob_menu_edit_children")));
        mContactEditorEditLocList.append(Loc(QContactFamily::FieldSpouse, "", hbTrId("txt_phob_menu_edit_spouse")));
        mContactEditorEditLocList.append(Loc(QContactUrl::DefinitionName, "", hbTrId("txt_phob_menu_edit_url")));
        mContactEditorEditLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_edit_url_home")));
        mContactEditorEditLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_edit_url_work")));
        mContactEditorEditLocList.append(Loc(QContactOrganization::DefinitionName, "", hbTrId("txt_phob_menu_edit_company_details")));     
                
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_menu_delete_mobile")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_mobile_home")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_mobile_work")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_menu_delete_assistant")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_assistant")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_assistant")));   
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_menu_delete_car")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_car")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_car")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_menu_delete_mobile")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_mobile_home")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_mobile_work")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_menu_delete_phone")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_phone_home")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_phone_work")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeFax, "", hbTrId("txt_phob_menu_delete_fax")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_fax_home")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_fax_work")));
        mContactEditorDelLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_menu_delete_pager")));
        mContactEditorDelLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_menu_delete_internet_telephone")));
        mContactEditorDelLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_internet_telephone_home")));
        mContactEditorDelLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_internet_telephone_work")));
        mContactEditorDelLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_menu_delete_sip")));
        mContactEditorDelLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_menu_delete_email")));
        mContactEditorDelLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_email_home")));
        mContactEditorDelLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_email_work")));
        mContactEditorDelLocList.append(Loc(QContactAddress::DefinitionName, "", hbTrId("txt_phob_menu_delete_address")));
        mContactEditorDelLocList.append(Loc(QContactAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_address_home")));
        mContactEditorDelLocList.append(Loc(QContactAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_address_work")));
        mContactEditorDelLocList.append(Loc(QContactAnniversary::DefinitionName, "", hbTrId("txt_phob_menu_delete_anniversary")));
        mContactEditorDelLocList.append(Loc(QContactBirthday::DefinitionName, "", hbTrId("txt_phob_menu_delete_birthday")));
        mContactEditorDelLocList.append(Loc(QContactNote::DefinitionName, "", hbTrId("txt_phob_menu_delete_note")));
        mContactEditorDelLocList.append(Loc(QContactFamily::FieldChildren, "", hbTrId("txt_phob_menu_delete_children")));
        mContactEditorDelLocList.append(Loc(QContactFamily::FieldSpouse, "", hbTrId("txt_phob_menu_delete_spouse")));
        mContactEditorDelLocList.append(Loc(QContactUrl::DefinitionName, "", hbTrId("txt_phob_menu_delete_url")));
        mContactEditorDelLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_delete_url_home")));
        mContactEditorDelLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_delete_url_work")));
        mContactEditorDelLocList.append(Loc(QContactOrganization::DefinitionName, "", hbTrId("txt_phob_menu_delete_company_details")));     
        mContactEditorDelLocList.append(Loc(QContactRingtone::DefinitionName, "", hbTrId("txt_phob_menu_delete_ringing_tone")));
        
        //contact card item specific menu 
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_menu_call_mobile")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_mobile_home")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_mobile_work")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_menu_call_assistant")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_assistant")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_assistant")));   
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_menu_call_car_phone")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_car_phone")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_car_phone")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_menu_call_mobile_home")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_menu_call_mobile_work")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_menu_call_mobile")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_menu_call_phone")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_phone_home")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_phone_work")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFax, "", hbTrId("txt_phob_menu_fax")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, hbTrId("txt_phob_menu_fax_home")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, hbTrId("txt_phob_menu_fax_work")));
        mItemSpecificMenuLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_menu_call_pager")));
        mItemSpecificMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_menu_call_internet_call")));
        mItemSpecificMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_internet_call_home")));
        mItemSpecificMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_internet_call_work")));
        mItemSpecificMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_menu_sip")));
        mItemSpecificMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_menu_email")));
        mItemSpecificMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_email_home")));
        mItemSpecificMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_email_work")));
        mItemSpecificMenuLocList.append(Loc(QContactUrl::DefinitionName, "", hbTrId("txt_phob_menu_url")));
        mItemSpecificMenuLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_url_home")));
        mItemSpecificMenuLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_url_work")));
        
        //contact card list items
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_dblist_call_mobile")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_mobile_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_mobile_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_dblist_call_assistant")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_assistant")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_assistant")));   
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_dblist_call_mobile_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_dblist_call_mobile_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_dblist_call_mobile")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_dblist_call_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_phone_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_phone_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFax, "", hbTrId("txt_phob_dblist_fax")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_fax_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_fax_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_dblist_call_pager")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_formlabel_internet_telephone")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_formlabel_internet_telephone_home")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_formlabel_internet_telephone_work")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_formlabel_sip")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_dblist_email")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_email_home")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_email_work")));
        mContactCardLocList.append(Loc(QContactUrl::DefinitionName, "", hbTrId("txt_phob_dblist_url")));
        mContactCardLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_url_home")));
        mContactCardLocList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_url_work")));
        
        //contact card icons
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, "", "qtg_large_call_mobile"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, "qtg_large_call_mobile_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, "qtg_large_call_mobile_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", "qtg_large_call_mobile"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome, "qtg_large_call_mobile_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork, "qtg_large_call_mobile_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, "", "qtg_large_call_landline"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, "qtg_large_call_landline_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, "qtg_large_call_landline_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFax, "", "qtg_large_call_fax"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextHome, "qtg_large_call_fax_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFax, QContactDetail::ContextWork, "qtg_large_call_fax_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypePager, "", "qtg_large_call_pager"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeCar, "", "qtg_large_call_car"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", "qtg_large_call_assistant"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeDtmfMenu, "", "qtg_large_call_group"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", "qtg_large_call_voip"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, "qtg_large_call_voip_home"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, "qtg_large_call_voip_work"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSip, "", ""));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, "", "qtg_large_email"));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, "qtg_large_email_home"));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, "qtg_large_email_work"));
        mContactCardIconList.append(Loc(QContactUrl::DefinitionName, "", "qtg_large_url_address"));
        mContactCardIconList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextHome, "qtg_large_url_address_home"));
        mContactCardIconList.append(Loc(QContactUrl::DefinitionName, QContactDetail::ContextWork, "qtg_large_url_address_work"));
    }

    //helper function to get string from list
    QString getLocString( QString aDefId, QString aContext, QList<Loc> list)
    {
        QString str;
        foreach ( Loc loc, list )
        {
            if ( loc.mId == aDefId && loc.mContext == aContext )
            {
                str = loc.mLoc;
                break;
            }
        }
        return str;
    }
    
    //get string of editor view item
	QString getContactEditorLocString( QString aDefId, QString aContext )
    {
	    return getLocString( aDefId, aContext, mContactEditorLocList );
    }
	
	//get add string of contect editor
	QString getContactEditorAddLocString( QString aDefId, QString aContext )
    {
	   return getLocString( aDefId, aContext, mContactEditorAddLocList );
	}
	
	//get edit string of contect editor
	QString getContactEditorEditLocString( QString aDefId, QString aContext )
	{
        return getLocString( aDefId, aContext, mContactEditorEditLocList );
    }
	    
	
	//get delete string of contect editor
	QString getContactEditorDelLocString( QString aDefId, QString aContext )
    {
        return getLocString( aDefId, aContext, mContactEditorDelLocList );
    }

	//get string of item specific menu of contact card and editor view
    QString getItemSpecificMenuLocString( QString aDefId, QString aContext )
    {
        return getLocString( aDefId, aContext, mItemSpecificMenuLocList );      
    }
    
    //get string of contact card list item
    QString getContactCardListLocString( QString aDefId, QString aContext )
    {
        return getLocString( aDefId, aContext, mContactCardLocList );
    }
    
    //get string of editor view icon
    QString getContactEditorIconString( QString aDefId, QString aContext )
    {
        return getLocString( aDefId, aContext, mContactEditorIconList );
    }
    
    //get string of contact card icon
    QString getContactCardIconString( QString aDefId, QString aContext )
    {
        return getLocString( aDefId, aContext, mContactCardIconList );
    }
    
private:
    QList<Loc> mContactEditorIconList;
	QList<Loc> mContactEditorLocList; 
	QList<Loc> mContactEditorAddLocList;
	QList<Loc> mContactEditorEditLocList;
	QList<Loc> mContactEditorDelLocList;
	QList<Loc> mItemSpecificMenuLocList;
    QList<Loc> mContactCardLocList;
    QList<Loc> mContactCardIconList;
};

inline Loc::Loc(QString aId, QString aContext, QString aLoc ) :
mId( aId ),
mContext( aContext ),
mLoc( aLoc )
{    
}
inline Loc::~Loc()
{
}
#endif // CNTSTRINGMAPPER_H
