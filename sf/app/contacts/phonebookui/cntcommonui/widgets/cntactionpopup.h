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

#ifndef CNTACTIONPOPUP_H
#define CNTACTIONPOPUP_H


#include <QObject>
#include "cntglobal.h"
#include <qtcontactsglobal.h>
#include <qcontactdetail.h>

class CntActionPopupPrivate;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*
* Class for showing popup for Call, Message and Email actions if preffered number is not set
*/
class QTPBK_EXPORT CntActionPopup : public QObject
{
    Q_OBJECT
    
public:
    CntActionPopup( QContact *contact );
    ~CntActionPopup();
    
    //returns true if the action is supported else false
    bool showActionPopup(QString action);
    
signals:
    //Emit signal for the perticular action with the detail selected from the popup
    void executeContactAction( QContact& aContact,QContactDetail contactDetail, QString actionName );
    void actionPopupCancelPressed();
  
private:
    CntActionPopupPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntActionPopup)
    Q_DISABLE_COPY(CntActionPopup)    
};

#endif // CNTACTIONPOPUP_H

// EOF
