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

#ifndef CNTPDETAILMODELITEM_H_
#define CNTDETAILMODELITEM_H_

#include <QObject>
#include <QModelIndex>
#include <hbdataformmodelitem.h>
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntDetailModelItem : public QObject, public HbDataFormModelItem
    {
Q_OBJECT

public:
    CntDetailModelItem( QContactDetail aDetail );
    CntDetailModelItem( QContactDetail aDetail, QString aLabel );
    ~CntDetailModelItem();
    
public:
    void setDetail( QContactDetail aDetail );
    QContactDetail detail();
    
signals:
    void itemDataChanged(CntDetailModelItem* changedItem);
    
private:
    QContactDetail mDetail;
    };
#endif /* CNTDETAILMODELITEM_H_ */
