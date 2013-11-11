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

#ifndef CNTCOMPANYEDITORMODEL_H_
#define CNTCOMPANYEDITORMODEL_H_

#include "cntdetaileditormodel.h"
#include <hbdataformmodelitem.h>
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactOrganization;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntCompanyEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntCompanyEditorModel( QContact* aContact );
    ~CntCompanyEditorModel();
    
    void saveContactDetails();
    QContactDetail detail() const;
    
private:
    QContactOrganization mCompany;
    };
#endif /* CNTCOMPANYEDITORMODEL_H_ */
