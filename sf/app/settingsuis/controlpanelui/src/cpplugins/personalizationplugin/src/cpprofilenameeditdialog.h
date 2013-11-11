/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#ifndef CPPROFILENAMEEDITDIALOG_H
#define CPPROFILENAMEEDITDIALOG_H

#include <hbdialog.h>

class HbWidget;
class HbLineEdit;

class CpProfileNameEditDialog : public HbDialog
{
    Q_OBJECT
    
public:
    CpProfileNameEditDialog( QGraphicsItem *parent = 0 );
    virtual ~CpProfileNameEditDialog();
    void setLineEditText(const QString &text);
    QString lineEditText();
    void setLineEditMaxLength(int length);
    
private slots:
    void checkPrimaryAction();
    
private:
    void init();
   
private:    
    HbWidget *mContentWidget;
    HbLineEdit  *mLineEdit;
};

#endif /* CPPROFILENAMEEDITDIALOG_H */
