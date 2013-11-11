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
 *     The header file of the drive rename query of file manager
 */

#ifndef FMSINGLETEXTQUERY_H
#define FMSINGLETEXTQUERY_H

#include "fmdialog.h"
#include <hblineedit.h>

class HbWidget;

class FmSingleTextQuery : public FmDialog
{
Q_OBJECT
public:
    
    explicit FmSingleTextQuery( HbLineEdit::EchoMode echoMode = HbLineEdit::Normal, QGraphicsItem *parent = 0 );
    ~FmSingleTextQuery();

public:
    void setLineEditText( const QString &text );
    QString getLineEditText();
    void setLineEditMaxLength( int length );
	void setRegExpStringList( QStringList regExpStringList );

public slots:
    void checkActions();

private:
    void init();

private:
    HbWidget *mContentWidget;
    HbLineEdit  *mTextEdit;
    HbLineEdit::EchoMode mEchoMode;

    // used to check if input text is valid. Disable primary action when text is not valid
    QStringList mRegExpStringList;
};

#endif
