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

#include <glxcommondialogs.h>

#include <hbaction.h>
#include <QEventLoop>
const int MAXSTRINGLENGHT = 256; 

// this is regexp for valid file/folder name: no \/:*?"<>| and is not totally empty characters.
// file name can not end with "." , but it is not include in this RegExp. It should be checked in Regex_ValidNotEndWithDot
// this expression is composed by two expressions:
// ^.*[^\\s].*$  used to match un-empty string and is not totally empty characters.
// [^\\\\/:*?\"<>|] used to math valid file/folder name
// merge the two regex together:
// valid file/folder name and is not totally empty.
#define Regex_ValidFileFolderName QString( "^[^\\\\/:*?\"<>|]*[^\\\\/:*?\"<>|\\s][^\\\\/:*?\"<>|]*$" )

// is not end with dot( trim blank characters in the end first )
#define Regex_ValidNotEndWithDot QString( "^.*[^\\.\\s][\\s]*$" )

GlxTextInputDialog::GlxTextInputDialog(bool disableOkForEmptyText) 
    : mDialog ( NULL ),
      mEventLoop ( 0 ),
      mResult ( false ),
      mDisableOkForEmptyText(disableOkForEmptyText)
{
}

GlxTextInputDialog::~GlxTextInputDialog()
{
}

QString GlxTextInputDialog::getText(const QString &label,
        const QString &text, bool *ok)
{
    QEventLoop eventLoop;
    mEventLoop = &eventLoop;
    
    mDialog = new HbInputDialog();
    mDialog->setPromptText(label);
    mDialog->setInputMode(HbInputDialog::TextInput);
    mDialog->setValue(text);
    mDialog->lineEdit(0)->setMaxLength(MAXSTRINGLENGHT);
    if(mDisableOkForEmptyText){
        connect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
                this, SLOT( textChanged (const QString &)));
    }
    mDialog->open( this, SLOT( dialogClosed( HbAction* ) ) ); 
    eventLoop.exec( );
    mEventLoop = 0 ;
    
    if ( ok ) {
        *ok = mResult ;
    }
    QString retText = NULL;
    if ( mResult ) {
        retText = mDialog->value().toString().trimmed();
    }
    if(mDisableOkForEmptyText){
        disconnect(mDialog->lineEdit(0), SIGNAL( textChanged (const QString &) ),
                    this, SLOT( textChanged (const QString &)));
    }
    delete mDialog;
    mDialog = NULL;
    return retText;
}

void GlxTextInputDialog::textChanged(const QString &text)
{
    // check if all regExp match, disable primary action if not match
    QStringList regExpList = (QStringList() << Regex_ValidFileFolderName << Regex_ValidNotEndWithDot );
    
    bool validateResult = true;
    foreach( const QString &regExpString, regExpList ) {
        if( !regExpString.isEmpty() ) {
            QRegExp regExp( regExpString );
            if( !regExp.exactMatch( text ) ) {
                validateResult =  false;
            }
        }
    }
    
    if(!validateResult){
        mDialog->actions().first()->setEnabled(false);
    }
    else {
        mDialog->actions().first()->setEnabled(true);
    }
}

void GlxTextInputDialog::dialogClosed(HbAction *action)
{
    HbInputDialog *dlg = static_cast<HbInputDialog*>(sender());
    if( action == dlg->actions().first() ) {
        mResult = true ;
    }
    else {
        mResult = false ;
    }
    if ( mEventLoop && mEventLoop->isRunning( ) ) {
        mEventLoop->exit( 0 );
    }
}
