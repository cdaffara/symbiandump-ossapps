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
*     The source file of the file dialog
*
*/

#include "fmfiledialog.h"
#include "fmfiledialog_p.h"
#include "fmfilewidget.h"
#include "fmlogger.h"
#include "fmutils.h"
#include "fmdrivemodel.h"
#include "fmcommon.h"

#include <hbwidget.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hbtranslator.h>
    
#include <QGraphicsLinearLayout>
#include <QEventLoop>

FmFileDialog::FmFileDialog( QGraphicsItem *parent ) : 
    HbDialog( parent ), d_ptr( new FmFileDialogPrivate( this ) )
{
}

FmFileDialog::~FmFileDialog()
{
    delete d_ptr;
}

QString FmFileDialog::getExistingDirectory( HbWidget *parent,
                                    const QString &title,
                                    const QString &dir,
                                    const QStringList &nameFilters,
                                    Options options )
{
    HbTranslator translator("filemanager");
    translator.loadCommon();
    
    QString ret;

    FmFileDialogPrivate::FmFileDialogArgs args;
    args.mDialogMode = FmFileDialogPrivate::GetDirMode;
    args.mTitle = title;
    args.mDirectory = dir;
    if( options & DisplayAllDirs ) {
        args.mDirFilters = QDir::AllDirs | QDir::NoDotAndDotDot;
    } else {
        args.mDirFilters = QDir::Dirs | QDir::NoDotAndDotDot;
    }
    args.mNameFilters = nameFilters;
    args.mOptions = options;

    FmFileDialog dialog( parent );
    dialog.d_ptr->init( args );
    if( dialog.exec() ) {
        ret = FmUtils::fillPathWithSplash( dialog.d_ptr->currentPath() );
    }
    return ret;
}

QString FmFileDialog::getOpenFileName( HbWidget *parent,
                               const QString &title,
                               const QString &dir,
                               const QStringList &nameFilters,
                               Options options )
{
    HbTranslator translator("filemanager");
    translator.loadCommon();
    
    QString ret;

    FmFileDialogPrivate::FmFileDialogArgs args;
    args.mDialogMode = FmFileDialogPrivate::GetFileMode;
    args.mTitle = title;
    args.mDirectory = dir;
    if( options & DisplayAllDirs ) {
        args.mDirFilters = QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files;
    } else {
        args.mDirFilters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files;
    }
    args.mNameFilters = nameFilters;
    args.mOptions = options;

    FmFileDialog dialog( parent );
    dialog.d_ptr->init( args );
    if( dialog.exec() ) {
        ret = FmUtils::fillPathWithSplash( dialog.d_ptr->currentPath() )
            + dialog.d_ptr->selectedFile();
    }
    return ret;
}


QString FmFileDialog::getSaveFileName( HbWidget * parent,
                               const QString &title,
                               const QString &dir,
                               const QStringList &nameFilters,
                               Options options )
{
    HbTranslator translator("filemanager");
    translator.loadCommon();
    
    QString ret;

    FmFileDialogPrivate::FmFileDialogArgs args;
    args.mDialogMode = FmFileDialogPrivate::SaveFileMode;
    args.mTitle = title;
    args.mDirectory = dir;
    if( options & DisplayAllDirs ) {
        args.mDirFilters = QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Files;
    } else {
        args.mDirFilters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files;
    }
    args.mNameFilters = nameFilters;
    args.mOptions = options;

    FmFileDialog dialog( parent );
    dialog.d_ptr->init( args );
    if( dialog.exec() ) {
        ret = FmUtils::fillPathWithSplash( dialog.d_ptr->currentPath() )
            + dialog.d_ptr->selectedFile();
    }
    return ret;
}



bool FmFileDialog::exec()
{
    HbDialog::open( this, SLOT(dialogClosed(HbAction*)) );
    d_ptr->eventLoop().exec();
    
    if ( d_ptr->isOkAction( d_ptr->retAction() ) ) {
        return true;
    } else {
        return false ;
    }
}

void FmFileDialog::dialogClosed(HbAction *action)
{
    d_ptr->setRetAction( action );
    d_ptr->eventLoop().exit();
}


HbAction *FmFileDialog::primaryAction() const
{
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() > 0) {
        return (HbAction *)(actionList.at(0));
    } else {
        return 0;
    }
    
}

void FmFileDialog::setPrimaryAction( HbAction *action )
{
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

HbAction *FmFileDialog::secondaryAction() const
{
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() > 1) {
        return (HbAction *)(actionList.at(1));
    } else {
        return 0;
    }    
}

void FmFileDialog::setSecondaryAction( HbAction *action )
{
    QList<QAction *> actionList = QGraphicsWidget::actions();
    if (actionList.size() == 0) {
        QGraphicsWidget::addAction(new HbAction(hbTrId("Ok")));
        QGraphicsWidget::addAction(action);          
    } else if (actionList.size() == 1) {
        QGraphicsWidget::addAction(action);
    } else if (actionList.size() == 2) {
        actionList.removeAt(1);
        actionList.insert(1, action);
    }
        
}

