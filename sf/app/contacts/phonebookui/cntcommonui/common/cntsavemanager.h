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

#ifndef CNTSAVEMANAGER_H
#define CNTSAVEMANAGER_H

#include <QObject>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContact;
class QContactAvatar;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    Contact saving helper class. Used mainly in edit view for contact saving,
    but should also be used in other (special) cases a contact needs to be saved.
    For example "forcing" phonebook to shut down (end key, from task swapper etc..)
    in detail editors.
*/
class CntSaveManager : public QObject
{
    friend class TestCntSaveManager;
    Q_OBJECT
    
public:
    enum CntSaveResult
    {
        ESaved = 0,
        EUpdated,
        EDeleted,
        EFailed,
        ENothingDone
    };
    
    enum CntContactType
    {
        EContact = 0,
        EMyCard,
        EGroup
    };
    
public:
    CntSaveManager(CntContactType type = EContact, QObject* parent = NULL);
    ~CntSaveManager();
    
    CntSaveResult saveContact(QContact* aContact, QContactManager* aManager);
    
private:
    void setPreferredDetails(QContact* aContact);
    
private:
    CntContactType   mContactType;
    
};

#endif // CNTIMAGEEDITORVIEW_H
