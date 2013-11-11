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
 * Description: hsmenuclient_p.h
 *
 */

#ifndef HSMENUCLIENT_PRIVATE_H
#define HSMENUCLIENT_PRIVATE_H

// hidden
const QString hsItemId("id");

class HsMenuClient;

class HsMenuClientPrivate
{

public:

    explicit HsMenuClientPrivate(HsMenuClient *publishPublic);
    ~HsMenuClientPrivate();
    bool add(const QVariantMap &entryPreference = QVariantMap());
    bool remove(const QVariantMap &queryPreference = QVariantMap()) const;    
    QList<QVariantMap > getList(const QVariantMap &queryPreference = QVariantMap());
    
private:
    /**
     * Points to the HsMenuClient instance that uses this private implementation.
     */
    HsMenuClient *const m_q;    
};

#endif //HSMENUCLIENT_PRIVATE_H
