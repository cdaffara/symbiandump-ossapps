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

#include "cntdetaileditormodel.h"
#include "cntdetailmodelitem.h"

CntDetailEditorModel::CntDetailEditorModel( QContact* aContact ) :
HbDataFormModel(), 
mContact( aContact )
{   
}

CntDetailEditorModel::~CntDetailEditorModel()
{
    delete mContact;
}

QContact* CntDetailEditorModel::contact() const
{
    return mContact;
}

void CntDetailEditorModel::emitDataChanged(CntDetailModelItem* changedItem)
{
    emit dataChanged(indexFromItem(changedItem), indexFromItem(changedItem));
}

// End of file
