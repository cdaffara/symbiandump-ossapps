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

#ifndef CNTFAMILYEDITORMODEL_H_
#define CNTFAMILYEDITORMODEL_H_

#include "cntdetaileditormodel.h"
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
class QContactFamily;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntFamilyEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntFamilyEditorModel( QContact* aContact );
    ~CntFamilyEditorModel();
    
    void saveContactDetails();
    QContactDetail detail() const;
    
private:
    QContactFamily mFamily;
    };

#endif /* CNTFAMILYEDITORMODEL_H_ */
