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

#include "nmuiengineheaders.h"

static HbIcon *icons[NmIcons::NmLastItem];

/*!
    \struct IconRes
    \brief Icon resource structure
*/
struct IconRes {
    NmIcons::Icon id;
    QString resName;
    QString animXmlName;
};

/*!
    Icon array
*/
static IconRes icon_res[] = {
    {NmIcons::NmIconDefaultMailbox, "qtg_large_email", ""},
    {NmIcons::NmIconPlusSign, "qtg_small_expand", ""},
    {NmIcons::NmIconMinusSign, "qtg_small_collapse", ""},
    {NmIcons::NmIconAttachment, "qtg_small_attachment", ""},
    {NmIcons::NmIconPriorityHigh, "qtg_small_priority_high", ""},
    {NmIcons::NmIconPriorityLow, "qtg_small_priority_low", ""},
    {NmIcons::NmIconAttach, "qtg_mono_attach", ""},
    {NmIcons::NmIconSend, "qtg_mono_send", ""},
    {NmIcons::NmIconContacts, "qtg_mono_contacts", ""},
    {NmIcons::NmIconNewEmail, "qtg_mono_create_email", ""},
    {NmIcons::NmIconDelete, "qtg_mono_delete", ""},
    {NmIcons::NmIconForward, "qtg_mono_forward", ""},
    {NmIcons::NmIconReply, "qtg_mono_reply", ""},
    {NmIcons::NmIconReplyAll, "qtg_mono_reply_all", ""},
    {NmIcons::NmIconSynching, "qtg_anim_small_loading", "qtg_anim_small_loading.axml"},
    {NmIcons::NmIconOnline, "qtg_small_connected", ""},
    {NmIcons::NmIconDisconnected, "qtg_small_disconnected", ""},
    {NmIcons::NmIconOffline, "qtg_small_fail", ""},
    {NmIcons::NmLastItem, ""} // Last item definion.
};

/*!
    \class NmIcons
    \brief handles icons loading and destruction
*/


/*!
    Get icon function. Use this function with enumeration value to get
    corresponding icon. Icon is created if it does not exist previously.
*/
HbIcon &NmIcons::getIcon(NmIcons::Icon icon)
{
    NM_FUNCTION;
    
    if (!icons[icon]) {
        for (int i(0); icon_res[i].id != NmIcons::NmLastItem; i++) {
            if (icon_res[i].id == icon) {
                // Branding icon check here. If branded icon is not found
                // default icon is to be loaded.
            
                if (icon_res[i].animXmlName.length() > 0) {
                    HbIconAnimationManager::global()->addDefinitionFile(icon_res[i].animXmlName);
                }
                icons[icon] = new HbIcon(icon_res[i].resName);
                break;
            }
        }
    }
    if (!icons[icon]) {
        NM_ERROR(1,QString("nmailuiengine: cannot open icon file: %1").arg(__FILE__));
    }
    return *icons[icon];
}

/*!
    Icon array reset functionality. Called when application exists.
*/
void NmIcons::freeIcons()
{
    NM_FUNCTION;
    
    for (int i(0); icon_res[i].id != NmIcons::NmLastItem; i++) {
        if (icons[i]) {
            delete icons[i];
            icons[i] = NULL;
        }
    }
}

