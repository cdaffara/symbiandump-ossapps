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
 *     The source file of the multi text query of file manager
 */

#include "fmmultitextquery.h"

#include <QGraphicsLinearLayout>

#include <hbwidget.h>
#include <hblineedit.h>
#include <hblabel.h>
#include <hbaction.h>

FmMultiTextQuery::FmMultiTextQuery( HbLineEdit::EchoMode echoMode, QGraphicsItem *parent ) :
    FmDialog( parent ), mEchoMode( echoMode )
{
    init();
}

FmMultiTextQuery::~FmMultiTextQuery()
{
}

void FmMultiTextQuery::init()
{
    setObjectName( "MultiTextQuery" );
    setDismissPolicy( HbPopup::NoDismiss );
    mContentWidget = new HbWidget();
    mContentWidget->setObjectName( "MultiTextContentWidget" );
	setContentWidget( mContentWidget );

    QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mFirstLabel = new HbLabel();
    mFirstLabel->setObjectName( "MutiTextFirstLabel" );
    mFirstLabel->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    vLayout->addItem( mFirstLabel );

    mFirstEdit = new HbLineEdit();
    mFirstEdit->setEchoMode( mEchoMode );
    mFirstEdit->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mFirstEdit->setObjectName("firstLineEdit");
	vLayout->addItem( mFirstEdit );

    mSecondLabel = new HbLabel();
    mSecondLabel->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mSecondLabel->setObjectName( "MutiTextSecondLabel" );
    vLayout->addItem( mSecondLabel );

    mSecondEdit = new HbLineEdit();
    mSecondEdit->setEchoMode( mEchoMode );
    mSecondEdit->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mSecondEdit->setObjectName("secondLineEdit");
    vLayout->addItem( mSecondEdit );

    mContentWidget->setLayout( vLayout );

    setTimeout( NoTimeout );
    connect( mFirstEdit, SIGNAL(contentsChanged()), this, SLOT(checkActions()) );
    connect( mSecondEdit, SIGNAL(contentsChanged()), this, SLOT(checkActions()) );
        
}

void FmMultiTextQuery::setLineEditMaxLength( int length )
{
    mFirstEdit->setMaxLength( length );
    mSecondEdit->setMaxLength( length );
}

void FmMultiTextQuery::setFirstEditText( const QString &text )
{
    mFirstEdit->setText( text );
}

void FmMultiTextQuery::setSecondEditText( const QString &text )
{
    mSecondEdit->setText( text );
}

QString FmMultiTextQuery::getFirstEditText()
{
    QString text = mFirstEdit->text();

    return text;
}

QString FmMultiTextQuery::getSecondEditText()
{
    QString text = mSecondEdit->text();

    return text;
}

void FmMultiTextQuery::setFirstLabelText( const QString &text )
{
    mFirstLabel->setPlainText( text );
}

void FmMultiTextQuery::setSecondLabelText( const QString &text )
{
    mSecondLabel->setPlainText( text );
}

void FmMultiTextQuery::checkActions()
{
    if( !mFirstEdit->text().isEmpty() && 
        mFirstEdit->text().compare( mSecondEdit->text(), Qt::CaseSensitive ) == 0 ){
        this->primaryAction()->setEnabled( true );
    } else {
        this->primaryAction()->setEnabled( false );
    }
}
