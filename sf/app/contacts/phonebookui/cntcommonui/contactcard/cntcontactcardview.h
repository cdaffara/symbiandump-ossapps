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

#ifndef CNTCONTACTCARDVIEW_H
#define CNTCONTACTCARDVIEW_H

#include <cntabstractview.h>
#include "cntglobal.h"
#include <QObject>

class HbView;
class CntContactCardViewPrivate;

QTM_BEGIN_NAMESPACE
class QContact;
class QContactDetail;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntContactCardView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public:
    CntContactCardView(bool isTemporary = false);
    ~CntContactCardView();
  
signals:
    void backPressed(int value); // value is return value for services (from cntservicescontact.h)
    void viewActivated(CntAbstractViewManager* aMgr, const CntViewParameters aArgs);
    void addToContacts();
    
public: // From CntAbstractView
    void activate(const CntViewParameters aArgs);
    void deactivate();
    HbView* view() const;
    
    bool isDefault() const;
    int viewId() const;
     
    void setEngine( CntAbstractEngine& aEngine );
private:
    CntContactCardViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntContactCardView)
    Q_DISABLE_COPY(CntContactCardView)    
};
#endif // CNTCONTACTCARDVIEW_H

// EOF
