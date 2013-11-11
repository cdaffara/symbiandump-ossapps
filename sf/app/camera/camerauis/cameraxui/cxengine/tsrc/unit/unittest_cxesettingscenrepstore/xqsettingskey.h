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
* Description: 
*
*/
#ifndef XQSETTINGSKEY_H
#define XQSETTINGSKEY_H

// INCLUDES


// CLASS DECLARATION
class XQSettingsKey
{
public:
    enum Target
    {
        TargetCentralRepository = 0,
        TargetPublishAndSubscribe
    };

    XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key);
    ~XQSettingsKey();

    XQSettingsKey::Target target() const;
    long int uid() const;
    unsigned long int key() const;

protected:
    XQSettingsKey::Target mTarget;
    long int mUid;
    unsigned long int mKey;
};

#endif //XQSETTINGSKEY_H

