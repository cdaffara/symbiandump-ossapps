/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Indicator information.
*
*/

#ifndef SNSRINDICATORINFO_H
#define SNSRINDICATORINFO_H

//  INCLUDES
#include <hbindicatorinterface.h>
#include <QString>

/*!
    \class SnsrIndicatorInfo
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Data structure used within big clock to handle indicator data.
 */

struct SnsrIndicatorInfo {

    inline SnsrIndicatorInfo() :
                type(QString()),
                iconPath(QString()),
                category(HbIndicatorInterface::NotificationCategory) {}

    inline SnsrIndicatorInfo(const QString &type,
        const QString &iconPath,
        HbIndicatorInterface::Category category) :
            type(type),
            iconPath(iconPath),
            category(category){}

    inline SnsrIndicatorInfo(const SnsrIndicatorInfo &copySource) :
        type(copySource.type),
        iconPath(copySource.iconPath),
        category(copySource.category){}

    inline bool isIconPathEmpty() const {
        return !iconPath.isEmpty();
    }
    
    // Indicator type string that is defined by the indicator provider
    QString type;
    // Should be fetched by using HbIndicatorInterface::MonoDecorationNameRole
    QString iconPath;
    HbIndicatorInterface::Category category;

};

#endif // SNSRINDICATORINFO_H

// End of File
