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

#ifndef CNTNAMESVIEW_H
#define CNTNAMESVIEW_H

#include <cntabstractview.h>
#include <QObject>

class HbView;
class CntNamesViewPrivate;

class CntNamesView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public:
    CntNamesView();
    ~CntNamesView();
   
public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    HbView* view() const;
    
    bool isDefault() const;
    int viewId() const;
  
    void setEngine( CntAbstractEngine& aEngine );
    
private:
    CntNamesViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, CntNamesView)
    Q_DISABLE_COPY(CntNamesView)    
};

#endif // CNTNAMESVIEW_H

// EOF
