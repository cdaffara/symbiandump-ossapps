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
* Description:   HbLineEdit stub
* 
*/

// INCLUDE FILES
#include "hblineedit.h"

// ---------------------------------------------------------------------------
// HbLineEdit
// ---------------------------------------------------------------------------
//
HbLineEdit::HbLineEdit(QGraphicsItem *parent) : HbWidget(parent)
{
}

// ---------------------------------------------------------------------------
// HbLineEdit
// ---------------------------------------------------------------------------
//
HbLineEdit::HbLineEdit(const QString &text, QGraphicsItem *parent) : HbWidget(parent)
{
    Q_UNUSED(text);
}

// ---------------------------------------------------------------------------
// ~HbLineEdit
// ---------------------------------------------------------------------------
//
HbLineEdit::~HbLineEdit()
{
}

// ---------------------------------------------------------------------------
// setValidator
// ---------------------------------------------------------------------------
//
void HbLineEdit::setValidator(HbValidator *validator)
{
    mValidator = validator;
}

// ---------------------------------------------------------------------------
// setValidator
// ---------------------------------------------------------------------------
//
void HbLineEdit::setMaxLength(int max)
{
    mMaxLength = max;
}
