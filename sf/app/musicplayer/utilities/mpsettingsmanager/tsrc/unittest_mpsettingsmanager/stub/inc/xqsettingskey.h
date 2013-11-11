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
* Description: xqsettingitem stub for testing MpSettingsManager.
*
*/

#ifndef XQSETTINGSITEM_H
#define XQSETTINGSITEM_H

class XQSettingsKey
{
public:
    enum Target
    {
        TargetCentralRepository
    };

    XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key);
    ~XQSettingsKey();

    XQSettingsKey::Target target() const;
    unsigned long int key() const;

protected:
    unsigned long int m_key;
};

#endif //XQSETTINGSITEM_H
