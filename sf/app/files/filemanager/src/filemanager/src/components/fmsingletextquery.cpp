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
 *     The source file of the drive rename query of file manager
 */

#include "fmsingletextquery.h"

#include <QGraphicsLinearLayout>
#include <QRegExp>

#include <hbwidget.h>
#include <hblineedit.h>
#include <hbaction.h>

FmSingleTextQuery::FmSingleTextQuery( HbLineEdit::EchoMode echoMode, QGraphicsItem *parent  ) :
    FmDialog( parent ), mEchoMode( echoMode )
{
    init();
}

FmSingleTextQuery::~FmSingleTextQuery()
{
}

void FmSingleTextQuery::init()
{
    setObjectName( "SingleTextQuery " );
    setDismissPolicy( HbPopup::NoDismiss );
    mContentWidget = new HbWidget();
    mContentWidget->setObjectName( "SingleTextContentWidget" );
    setContentWidget( mContentWidget );
	

    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mTextEdit = new HbLineEdit();
    mTextEdit->setEchoMode( mEchoMode );
    mTextEdit->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mTextEdit->setObjectName( "textLineEdit" );
	vLayout->addItem( mTextEdit );

    mContentWidget->setLayout( vLayout );

    setTimeout( NoTimeout );

    connect( mTextEdit, SIGNAL(contentsChanged()), this, SLOT(checkActions()) );
}

void FmSingleTextQuery::setLineEditText( const QString &text )
{
    mTextEdit->setText( text );
    mTextEdit->setSelection( 0, text.length() );
}

QString FmSingleTextQuery::getLineEditText()
{
    QString text = mTextEdit->text();

    return text;
}

void FmSingleTextQuery::setLineEditMaxLength( int length )
{
    mTextEdit->setMaxLength( length );
}

void FmSingleTextQuery::setRegExpStringList( QStringList regExpStringList )
{
	mRegExpStringList = regExpStringList;
}

void FmSingleTextQuery::checkActions()
{
    // check if all regExp match, disable primary action if not match
    bool validateResult = true;
    foreach( const QString &regExpString, mRegExpStringList ) {
        if( !regExpString.isEmpty() ) {
            QRegExp regExp( regExpString );
            if( !regExp.exactMatch( mTextEdit->text() ) ) {
                validateResult =  false;
            }
        }
    }
    if( validateResult ) {
        this->primaryAction()->setEnabled( true );
    } else {
        this->primaryAction()->setEnabled( false );
    }
}
