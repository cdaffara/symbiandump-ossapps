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
 *     The header file of filemanager base dialog class
 */

#ifndef FMDIALOG_H
#define FMDIALOG_H

#include <QEventLoop>
#include <hbaction.h>
#include <hbdialog.h>
#include <hbmenu.h>
class FmDialog : public HbDialog
{
Q_OBJECT
public:
    explicit FmDialog( QGraphicsItem *parent = 0 );
    HbAction *exec();
    
    HbAction *primaryAction() const;
    void setPrimaryAction( HbAction *action );

    HbAction *secondaryAction() const;
    void setSecondaryAction( HbAction *action );
public slots:
    void dialogClosed(HbAction *action);
private:
    QEventLoop mEventLoop;
    HbAction   *mRetAction; 
};

#endif
