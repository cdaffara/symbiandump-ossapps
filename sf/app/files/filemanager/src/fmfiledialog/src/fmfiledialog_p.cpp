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
*     The source file of the file dialog implement
*
*/

#include "fmfiledialog_p.h"
#include "fmfilewidget.h"
#include "fmcommon.h"

#include "hbfontspec.h"
#include <hbaction.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hblineedit.h>
#include <QGraphicsLinearLayout>

#define backButtonIcon  "qtg_mono_app_exit"

/*!
    constructor
*/
FmFileDialogPrivate::FmFileDialogPrivate( FmFileDialog *fileDialog ) :
    mFileDialog( fileDialog ),
    mHeadingWidget( 0 ),
    mCurrentPathLabel( 0 ),
    mUpButton( 0 ),
    mTitleLabel( 0 ),
    mContentWidget( 0 ),
    mContentLayout( 0 ),
    mFileWidget( 0 ),
    mOkAction( 0 ),
    mCancelAction( 0 ),
    mFileNameTitleLabel( 0 ),
    mFileNameLineEdit( 0 )
{
 }

/*!
    destructor
*/
FmFileDialogPrivate::~FmFileDialogPrivate()
{

}

/*!
    init
    \param directory .
*/
void FmFileDialogPrivate::init( const FmFileDialogArgs &args )
{
    mArgs = args;
    setProperties();
    createHeadingWidget();
    createContentWidget();
    createBottomWidget();

    if( !mFileDialog->primaryAction() && !mFileDialog->secondaryAction() ) {
        createAndSetActions(mFileDialog->tr("Ok"),mFileDialog->tr("Cancel"));
    }

    makeConnections();
    setRootPath( mArgs.mDirectory );
    mFileWidget->setModelFilter( mArgs.mDirFilters );
    mFileWidget->setNameFilters( mArgs.mNameFilters );
}

/*!
    createAndSetActions

    \param primaryActionText
           secondaryActionText
*/
void FmFileDialogPrivate::createAndSetActions(const QString & primaryActionText,
                                                       const QString & secondaryActionText)
{
    // Create action for ok button and assign it to the primary action of popup
    mOkAction = new HbAction( primaryActionText, mFileDialog );
    mOkAction->setObjectName( "okAction" );
    Q_ASSERT( mOkAction );
    mFileDialog->setPrimaryAction( mOkAction );

    // Create action for cancel button and assign it to the secondary action of popup
    mCancelAction = new HbAction( secondaryActionText, mFileDialog );
    mCancelAction->setObjectName( "cancelAction" );
    Q_ASSERT( mCancelAction );
    mFileDialog->setSecondaryAction( mCancelAction );
}

/*!
    setProperties
    sets the properties of the file dialog like timeout,
    Dismiss policy,modal etc.
*/
void FmFileDialogPrivate::setProperties()
{
    // set the file dialog as Modal.
    mFileDialog->setModal(true);

    // no dismiss for File dialog.
    mFileDialog->setDismissPolicy( HbPopup::NoDismiss );

    // File dialog will not time out.
    mFileDialog->setTimeout( HbPopup::NoTimeout );
}

/*!
    makeConnections
*/
void FmFileDialogPrivate::makeConnections()
{
    mFileDialog->connect( mUpButton,SIGNAL( clicked() ),
        mFileDialog, SLOT( _q_handleUpButton() ) );

    if( mFileNameLineEdit ) {
        mFileDialog->connect( mFileNameLineEdit, SIGNAL( textChanged( QString ) ),
            mFileDialog, SLOT( _q_handleTextChanged( QString ) ) );
    }

    mFileDialog->connect( mFileWidget, SIGNAL( pathChanged( QString ) ),
        mFileDialog, SLOT( _q_handlePathChanged( QString ) ) );

    mFileDialog->connect( mFileWidget, SIGNAL( fileActivated( QString ) ),
        mFileDialog, SLOT( _q_handleFileActivated( QString ) ) );
}

/*!
    creates the heading wiget elements that include
    a label for showing the current path and a push button for
    going to the parent directory.
*/
void FmFileDialogPrivate::createHeadingWidget()
{
    mHeadingWidget = new HbWidget( mFileDialog );
    mHeadingWidget->setObjectName( "headingWidget" ); 

    QGraphicsLinearLayout *headingLayout = new QGraphicsLinearLayout;
    headingLayout->setOrientation(Qt::Horizontal);
    mHeadingWidget->setLayout(headingLayout);
    

    QGraphicsLinearLayout *titleLayout = new QGraphicsLinearLayout;
    titleLayout->setOrientation(Qt::Vertical);    

    mTitleLabel = new HbLabel();
    mTitleLabel->setObjectName( "titleLabel" );
    if( mArgs.mTitle.isEmpty() ){
        mTitleLabel->setPlainText( QString( FmPlaceholderString ) );
    } else {
        mTitleLabel->setPlainText( mArgs.mTitle );
    }

    mCurrentPathLabel = new HbLabel( QString( FmPlaceholderString ) );
    mCurrentPathLabel->setObjectName( "currentPathLabel" );
    mCurrentPathLabel->setElideMode(Qt::ElideRight);

    mUpButton  = new HbPushButton;
    mUpButton->setObjectName( "upButton" );
    mUpButton->setIcon(HbIcon(backButtonIcon));

    titleLayout->addItem( mTitleLabel );
    titleLayout->setAlignment( mTitleLabel, Qt::AlignLeft);

    titleLayout->addItem( mCurrentPathLabel );
    titleLayout->setAlignment( mCurrentPathLabel, Qt::AlignLeft);

    headingLayout->addItem(titleLayout);
    headingLayout->addItem( mUpButton ) ;
    headingLayout->setAlignment(mUpButton,Qt::AlignRight);

    mFileDialog->setHeadingWidget( mHeadingWidget );
}

/*!
    Creates the content widget. It includes the listview and model.

*/

void FmFileDialogPrivate::createContentWidget()
{
    mContentWidget = new HbWidget( mFileDialog );
    mContentWidget->setObjectName( "contentWidget" );

    mContentLayout = new QGraphicsLinearLayout;
    mContentLayout->setOrientation(Qt::Vertical);
    
    mContentWidget->setLayout( mContentLayout );

    mFileWidget = new FmFileWidget( mContentWidget );
    mFileWidget->setObjectName( "fileWidget" );
    mContentLayout->addItem( mFileWidget );

    mFileDialog->setContentWidget( mContentWidget );
}

/*!
    Creates the content widget. It includes the listview and model.

*/

void FmFileDialogPrivate::createBottomWidget()
{
    if( mArgs.mDialogMode == GetDirMode ) {
        return;
    }
    HbWidget *bottomWidget = new HbWidget( mContentWidget );
    bottomWidget->setObjectName( "bottomWidget" );
    mContentLayout->addItem( bottomWidget );

    QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout;
    bottomLayout->setOrientation( Qt::Horizontal );

    mFileNameTitleLabel = new HbLabel( mFileDialog->tr( "file name:" ), bottomWidget );
    mFileNameTitleLabel->setObjectName( "fileNameTitleLabel" );
    bottomLayout->addItem( mFileNameTitleLabel );

    mFileNameLineEdit = new HbLineEdit( bottomWidget );
    mFileNameLineEdit->setObjectName( "fileNameLineEdit" );
    bottomLayout->addItem( mFileNameLineEdit );    

    bottomWidget->setLayout( bottomLayout );

}

void FmFileDialogPrivate::checkUpButton()
{
    QString currentPath = mFileWidget->currentPath().absoluteFilePath();
    bool isPreventDirUp = mArgs.mOptions & FmFileDialog::PreventDirUp;

    if( mFileWidget->currentViewType() == FmFileWidget::DirView ) {
        if( isPreventDirUp &&
            mArgs.mDirectory.contains( currentPath, Qt::CaseInsensitive ) ) {
            // disable up when client lock top leve folder
            mUpButton->setEnabled( false );
        } else {
            mUpButton->setEnabled( true );
        }
    } else {
        // disable up button when at drive view
        mUpButton->setEnabled( false );
    }
}

void FmFileDialogPrivate::checkBottomFileWidget()
{
    switch( mArgs.mDialogMode )
    {
    case GetDirMode:
        break;
    case GetFileMode:
        if( mFileNameLineEdit ){
            mFileNameLineEdit->setReadOnly( true );
        }
        break;
    case SaveFileMode:
        if( mFileNameLineEdit ){
            if( mFileWidget->currentPath().absoluteFilePath().isEmpty() ) {
                mFileNameLineEdit->setReadOnly( true );
            } else {
                mFileNameLineEdit->setReadOnly( false );
            }
        }
        break;
    }
}

void FmFileDialogPrivate::checkActions()
{
    switch( mArgs.mDialogMode )
    {
    case GetDirMode:
        if( mFileWidget->currentPath().absoluteFilePath().isEmpty() ) {
            mOkAction->setDisabled( true );
        } else {
            mOkAction->setDisabled( false );
        }
        break;
    case GetFileMode:
    case SaveFileMode:
        if( mFileWidget->currentPath().absoluteFilePath().isEmpty() ||
            ( mFileNameLineEdit && mFileNameLineEdit->text().isEmpty() ) ) {
            mOkAction->setDisabled( true );
        } else {
            mOkAction->setDisabled( false );
        }
        break;
    default:
        Q_ASSERT( false );
    }
}

void FmFileDialogPrivate::_q_handleUpButton()
{
    bool isPreventDirUp = mArgs.mOptions & FmFileDialog::PreventDirUp;
    QString currentPath = mFileWidget->currentPath().absoluteFilePath();
    if( isPreventDirUp &&
        mArgs.mDirectory.contains( currentPath, Qt::CaseInsensitive ) ) {
        return;
    } else {
        mFileWidget->cdUp();
    }
}

void FmFileDialogPrivate::_q_handleTextChanged(const QString &text)
{
    Q_UNUSED( text );
    checkActions();
}

void FmFileDialogPrivate::_q_handlePathChanged( const QString &path )
{
    if( path.isEmpty() ) {
        mCurrentPathLabel->setPlainText( QString( FmPlaceholderString ) );
    } else {
        mCurrentPathLabel->setPlainText( path );
    }
    
    if( mFileNameLineEdit && mArgs.mDialogMode == GetFileMode ) {
        mFileNameLineEdit->setText( QString() );
    }

    checkUpButton();
    checkBottomFileWidget();
    checkActions();
}

void FmFileDialogPrivate::_q_handleFileActivated( const QString &path )
{
    if( mFileNameLineEdit ) {
        mFileNameLineEdit->setText( path );
    }
    checkActions();
}

QString FmFileDialogPrivate::currentPath() const
{
    return mFileWidget->currentPath().absoluteFilePath();
}

QString FmFileDialogPrivate::selectedFile() const
{
    return mFileNameLineEdit->text();
}

void FmFileDialogPrivate::setRootPath( const QString &pathName )
{
    mFileWidget->setRootPath( pathName );
}

bool FmFileDialogPrivate::isOkAction( HbAction *action )
{
    if( mOkAction == action ) {
        return true;
    }

    return false;
}

void FmFileDialogPrivate::setRetAction( HbAction *action )
{
    mRetAction = action;
}

HbAction *FmFileDialogPrivate::retAction()
{
    return mRetAction;
}

QEventLoop &FmFileDialogPrivate::eventLoop()
{
    return mEventLoop;
}
    
#include "moc_fmfiledialog.cpp"
