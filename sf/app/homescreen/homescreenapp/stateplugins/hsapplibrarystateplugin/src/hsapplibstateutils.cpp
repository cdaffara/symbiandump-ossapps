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
 * Description: Utils for application library state plugin.
 *
 */

#include <caentry.h>

#include "hsapplibstateutils.h"
#include "hscontentservice.h"
#include "hsapp_defs.h"

/*!
 \class HsAppLibStateUtils
 \ingroup group_hsmenustateplugin
 \brief Utils.
 Utils for application library state plugin.
 */

/*!
 Return true if given entry is cwrt widget and already on homescreen
 */
bool HsAppLibStateUtils::isCWRTWidgetOnHomeScreen(const CaEntry *entry)
{
    bool result = false;
    if (entry->entryTypeName() == Hs::applicationTypeName && 
        entry->attribute(Hs::swTypeKey) == Hs::HS_CWRT_APP_TYPE &&
        entry->attribute(Hs::widgetUriAttributeName) ==
                Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE) {
        QVariantHash preferences;
        QMap<QString, QString> attributes = entry->attributes();
        QMapIterator<QString, QString> i(attributes);
        while (i.hasNext()) {
            i.next();
            QString key(i.key());
            QString value(i.value());
            if (key.contains(Hs::widgetParam)) {
                preferences.insert(key.remove(Hs::widgetParam),value);
            }
        }
        int count(0);
        HsContentService::instance()->widgets(Hs::HS_WIDGET_URI_ATTRIBUTE_CWRT_VALUE,preferences,count);
        result = count > 0;
    }
    return result;
}
