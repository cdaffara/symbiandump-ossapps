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

#ifndef CNTNAMEEDITORMODEL_H_
#define CNTNAMEEDITORMODEL_H_
#include "cntdetaileditormodel.h"
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactName;
class QContactNickname;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntNameEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntNameEditorModel( QContact* aContact );
    ~CntNameEditorModel();
    
    void saveContactDetails();
    QContactDetail detail() const;
    
private:
    QContactName iName;
    QContactNickname iNick;
    };
#endif /* CNTNAMEEDITORMODEL_H_ */
