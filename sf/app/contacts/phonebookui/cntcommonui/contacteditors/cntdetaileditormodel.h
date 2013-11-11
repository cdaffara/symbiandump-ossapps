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

#ifndef CNTDETAILMODEL_H_
#define CNTDETAILMODEL_H_

#include <hbdataformmodel.h>
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntDetailModelItem;

class CntDetailEditorModel : public HbDataFormModel
{
    Q_OBJECT
    
public:
    CntDetailEditorModel( QContact* aContact );
    ~CntDetailEditorModel();
    
public:
    QContact* contact() const;
    virtual QContactDetail detail() const = 0;
    
    // Note that this method should not save anything if there's nothing to change.
    // In otherwords, don't change the mContact pointer if not needed.
    virtual void saveContactDetails() = 0;
    virtual void insertDetailField(){}
    
public slots:
    void emitDataChanged(CntDetailModelItem* changedItem);
    
protected:
    QContact* mContact;
};

#endif /* CNTDETAILMODEL_H_ */
