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
 *     The source file of the time query of file manager
 */

#include "fmtimequery.h"

#include <QGraphicsItem>
#include <QGraphicsLinearLayout>

#include <hbdatetimepicker.h>


FmTimeQuery::FmTimeQuery( QGraphicsItem *parent ) : FmDialog( parent )
{
    setObjectName( "timeQueryDialog" );
    mContentWidget = new QGraphicsWidget();
	setContentWidget(mContentWidget);
	mContentWidget->setObjectName( "timeQueryContentWidget" );
	QGraphicsLinearLayout *vLayout = new QGraphicsLinearLayout();
    vLayout->setOrientation( Qt::Vertical );

    mTimeWidget = new HbDateTimePicker();
    mTimeWidget->setObjectName( "timeQueryTimeWidget" );
	vLayout->addItem( mTimeWidget );

    mTimeWidget->setFontSpec( HbFontSpec( HbFontSpec::Primary ) );
    mTimeWidget->setDisplayFormat( "hh:mm" );
    mTimeWidget->setDateTime( QDateTime::currentDateTime() );

	this->setTimeout( NoTimeout );
	mContentWidget->setLayout( vLayout );
}

FmTimeQuery::~FmTimeQuery(void)
{
}

void FmTimeQuery::setTime( const QTime &time )
{
    QDateTime dateTime = QDateTime::currentDateTime();
    dateTime.setTime( time );
    mTimeWidget->setDateTime( dateTime );
}

QTime FmTimeQuery::getTime()
{
    return mTimeWidget->dateTime().time();
}
