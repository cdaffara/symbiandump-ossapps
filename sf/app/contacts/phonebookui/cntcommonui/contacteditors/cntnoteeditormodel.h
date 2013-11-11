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

#ifndef CNTNOTEEDITORMODEL_H_
#define CNTNOTEEDITORMODEL_H_

#include "cntdetaileditormodel.h"
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntNoteEditorModel : public CntDetailEditorModel
    {
    Q_OBJECT
    
public:
    CntNoteEditorModel( QContact* aContact );
    ~CntNoteEditorModel();
    
    void saveContactDetails();
    void insertDetailField();
    QContactDetail detail() const;
    
private:
    QList<QContactNote> mNoteList;
    };

#endif /* CNTNOTEEDITORMODEL_H_ */
