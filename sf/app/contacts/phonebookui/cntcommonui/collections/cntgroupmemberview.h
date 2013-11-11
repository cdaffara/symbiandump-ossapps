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

#ifndef CNTGROUPMEMBERVIEW_H
#define CNTGROUPMEMBERVIEW_H

#include <QObject>
#include <cntabstractview.h>
#include "cntglobal.h"

class CntGroupMemberViewPrivate;
class HbView;
class CntAbstractEngine;

class QTPBK_EXPORT CntGroupMemberView : public QObject, public CntAbstractView
{
    Q_OBJECT

public:
    CntGroupMemberView();
    ~CntGroupMemberView();
    
public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const;
    HbView* view() const;
    int viewId() const;
    void setEngine( CntAbstractEngine& aEngine );
    
signals:
    void backPressed();
        
private:
    friend class TestCntGroupMemberView;
    CntGroupMemberViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntGroupMemberView)
    Q_DISABLE_COPY(CntGroupMemberView)
};

#endif // CNTGROUPMEMBERVIEW_H

// EOF
