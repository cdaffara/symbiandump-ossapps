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

#ifndef CNTEDITORFACTORY_H_
#define CNTEDITORFACTORY_H_

#include <hbdataformviewitem.h>
#include <QObject>
#include "cntdetaileditormodel.h"
#include <cntviewparams.h>
#include <qtcontacts.h>
#include "cntglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntAbstractView;
class CntDetailEditor;

class CntEditorFactory : public QObject 
    {
    Q_OBJECT
    
public:
    CntEditorFactory( QObject* aParent = 0 );
    virtual ~CntEditorFactory();
    
    void setupEditorView(CntDetailEditor &aEditor, QContact &aContact);
    };
#endif /* CNTEDITORFACTORY_H_ */
