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
 *      The source file of filemanager base dialog class
 */

#include "fmdialog.h"
#include <hbdialog.h>

FmDialog::FmDialog( QGraphicsItem *parent) : HbDialog( parent ), mRetAction( 0 )
{

}

HbAction *FmDialog::exec()
{
    HbDialog::open( this, SLOT(dialogClosed(HbAction*)) );
    mEventLoop.exec();
    return mRetAction;
}

void FmDialog::dialogClosed(HbAction *action)
{
    mRetAction = action;
    mEventLoop.exit();
}

HbAction *FmDialog::primaryAction() const
{
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() > 0) {
        return (HbAction *)(actionList.at(0));
    } else {
        return 0;
    }
    
}

void FmDialog::setPrimaryAction( HbAction *action )
{
    action->setObjectName( "primaryAction" );
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() == 0) {
        QGraphicsWidget::addAction(action);
    } else if (actionList.size() ==  1) {
        actionList.clear();
        QGraphicsWidget::addAction(action);
    } else if (actionList.size() == 2) {
        actionList.removeAt(0);
        actionList.insert(0, action);
    }   
}

HbAction *FmDialog::secondaryAction() const
{
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() > 1) {
        return (HbAction *)(actionList.at(1));
    } else {
        return 0;
    }    
}

void FmDialog::setSecondaryAction( HbAction *action )
{
    action->setObjectName( "secondaryAction" );
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() == 0) {
        HbAction *primaryAction = new HbAction(hbTrId("txt_common_button_ok"));
        primaryAction->setObjectName( "primaryAction" );
        QGraphicsWidget::addAction(primaryAction);
        QGraphicsWidget::addAction(action);          
    } else if (actionList.size() == 1) {
        QGraphicsWidget::addAction(action);
    } else if (actionList.size() == 2) {
        actionList.removeAt(1);
        actionList.insert(1, action);
    } 
}
