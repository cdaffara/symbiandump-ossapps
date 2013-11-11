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
 * 
 * Description:
 *     The header file of the multi text query of file manager
 */

#ifndef FMMULTITEXTQUERY_H
#define FMMULTITEXTQUERY_H

#include "fmcommon.h"
#include "fmdialog.h"

#include <hblineedit.h>

class HbWidget;
class HbLabel;

class FmMultiTextQuery : public FmDialog
{
Q_OBJECT
public:
    explicit FmMultiTextQuery( HbLineEdit::EchoMode echoMode = HbLineEdit::Normal, QGraphicsItem *parent = 0 );
    ~FmMultiTextQuery();

public:
    void setLineEditMaxLength( int length );
    void setFirstEditText( const QString &text );
    void setSecondEditText( const QString &text );
    void setFirstLabelText( const QString &text );
    void setSecondLabelText( const QString &text );
    QString getFirstEditText();
    QString getSecondEditText();

public slots:
    void checkActions();

private:
    void init();

private:
    HbWidget    *mContentWidget;
    HbLineEdit  *mFirstEdit;
    HbLineEdit  *mSecondEdit;
    HbLabel     *mFirstLabel;
    HbLabel     *mSecondLabel;
    HbLineEdit::EchoMode mEchoMode;

};

#endif
