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

#ifndef CNTPHONENUMBERMODELITEM_H_
#define CNTPHONENUMBERMODELITEM_H_

#include "cntdetaileditormodel.h"
#include <QPointer>

#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactPhoneNumber;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntPhoneNumberModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    // takes ownership of aContact
    CntPhoneNumberModel( QContact* aContact, QObject* aParent = 0 );
    ~CntPhoneNumberModel();
    
    void saveContactDetails();
    void insertDetailField();
    QContactDetail detail() const;
    
private:
    QList<QContactDetail> mNumberList;
    QContactPhoneNumber mMobileTemplate;
    QContactPhoneNumber mLandlineTemplate;
    };
#endif /* CNTPHONENUMBERMODELITEM_H_ */
