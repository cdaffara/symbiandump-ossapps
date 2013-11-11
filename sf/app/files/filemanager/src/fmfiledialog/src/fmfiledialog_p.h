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
*     The header file of the file dialog implement
*
*/

#ifndef FMFILEDIALOG_P_H
#define FMFILEDIALOG_P_H

#include "fmfiledialog.h"

#include <QDir>
#include <QEventLoop>

class HbPushButton;
class FmFileWidget;
class HbLineEdit;
class HbLabel;
class HbWidget;
class HbLabel;
class HbPushButton;
class QGraphicsLinearLayout;

class FmFileDialogPrivate
{
public:
    enum DialogCode {
        Accepted,
        Canceled
    };

    enum DialogMode {
        GetDirMode,
        GetFileMode,
        SaveFileMode
    };

    struct FmFileDialogArgs
    {
        FmFileDialogPrivate::DialogMode      mDialogMode;
        QString         mTitle;
        QString         mDirectory;
        QDir::Filters   mDirFilters;
        QStringList     mNameFilters;
        FmFileDialog::Options mOptions;
    };

    FmFileDialogPrivate( FmFileDialog *fileDialog );
    ~FmFileDialogPrivate();

    void init( const FmFileDialogArgs &args );

    QString currentPath() const;
    QString selectedFile() const;
    bool isOkAction( HbAction *action );
    
    void setRetAction( HbAction *action );
    HbAction *retAction();
    QEventLoop &eventLoop();
        
protected:
    void createAndSetActions(const QString & primaryActionText,const QString & secondaryActionText);
    void createContentWidget();
    void createHeadingWidget();
    void populateContentWidget(const QString & directory);
    void createBottomWidget();
    void setProperties();
    void makeConnections();

    void initBottomFileWidget();
    void checkUpButton();
    void checkBottomFileWidget();
    void checkActions();

    void setTitle( const QString &str );        
    void setRootPath( const QString &pathName );
    
public:
    void _q_handleUpButton();
    void _q_handleTextChanged(const QString &text);
    void _q_handlePathChanged( const QString &path );
    void _q_handleFileActivated( const QString &path );

private:
    FmFileDialog *mFileDialog;

    HbWidget *mHeadingWidget;
    HbLabel  *mCurrentPathLabel;
    HbPushButton *mUpButton;
    HbLabel  *mTitleLabel;
    HbWidget *mContentWidget;
    QGraphicsLinearLayout *mContentLayout;
    FmFileWidget *mFileWidget;

    HbAction *mOkAction;
    HbAction *mCancelAction;
    
    QString mDirectory;
    QString mTitleValue;
    QStringList mFilterList;

    HbLabel         *mFileNameTitleLabel;
    HbLineEdit      *mFileNameLineEdit;

    QString mCaptionHead;
    FmFileDialogArgs          mArgs;
    
    HbAction *mRetAction;
    QEventLoop mEventLoop;
};

#endif // FMFILEDIALOG_P_H
