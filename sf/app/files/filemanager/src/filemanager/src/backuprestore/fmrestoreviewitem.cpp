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
 *     The source file of the restore view list item of file manager
 */
#include "fmrestoreviewitem.h"
#include "fmfileiconprovider.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <hblabel.h>
#include <hbcheckbox.h>
#include <hbwidget.h>
#include <hblineedit.h>

//FmRestoreViewItem
FmRestoreViewItem::FmRestoreViewItem( QGraphicsItem *parent )
  : HbListViewItem( parent ),
    mRestoreContentLabel( 0 ),
    mDateTimeLabel( 0 ),
    mCheckBox( 0 ),
    hLayout( 0 ),
    mParentWidget((HbWidget *)parent)
{
	//init();
}

FmRestoreViewItem::~FmRestoreViewItem()
{
}


HbAbstractViewItem *FmRestoreViewItem::createItem()
{
	return new FmRestoreViewItem( *this );
}

void FmRestoreViewItem::polish(HbStyleParameters& params)
{
    Q_UNUSED(params);
}

void FmRestoreViewItem::updateChildItems()
{
    if( !hLayout ) {
       init();
    }
	QString string = modelIndex().data( Qt::DisplayRole ).toString();	

	QStringList stringList = string.split( '\t' );

	if( stringList.count() == 0 ){
		return;
	}

    mRestoreContentLabel->setPlainText(stringList.first());

    if (stringList.size() > 0)
        {
        mDateTimeLabel->setPlainText(stringList.at(1));
        }
    if (stringList.size() > 1)
        {
        QIcon icon = mIconProvider->icon(QFileInfo(stringList.at(2)));
        // FmFileIconProvider already handle null icon issue
        mIconLabel->setIcon(HbIcon(icon));
        }
    
    connect(this, SIGNAL(stateChanged(int)), mParentWidget,
            SIGNAL(stateChanged(int)));

}

void FmRestoreViewItem::init()
{
    mIconProvider = new FmFileIconProvider(); 
    hLayout = new QGraphicsLinearLayout();
    hLayout->setOrientation(Qt::Horizontal);
    hLayout->addItem(layout());

    mCheckBox = new HbCheckBox(this);
    hLayout->addItem(mCheckBox);
    hLayout->setAlignment(mCheckBox, Qt::AlignVCenter);

    QGraphicsGridLayout *vLayout = new QGraphicsGridLayout();

    mRestoreContentLabel = new HbLabel("");
    mRestoreContentLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
    
    mIconLabel = new HbLabel();    

    mDateTimeLabel = new HbLabel("");
    mDateTimeLabel->setFontSpec(HbFontSpec(HbFontSpec::Secondary));

    vLayout->addItem(mRestoreContentLabel, 0, 0);
    vLayout->addItem(mIconLabel, 0, 1);
    vLayout->addItem(mDateTimeLabel, 1, 0);

    HbWidget *textWidget = new HbWidget();
    textWidget->setLayout(vLayout);

    hLayout->addItem(textWidget);
    hLayout->setAlignment(textWidget, Qt::AlignVCenter);
    
    connect(mCheckBox, SIGNAL(stateChanged(int)), this,
            SIGNAL(stateChanged(int)));
    setLayout(hLayout);
}

void FmRestoreViewItem::setCheckBoxState()
{
	if ( mCheckBox->checkState() ==  Qt::Unchecked ){
		mCheckBox->setCheckState( Qt::Checked );
		setSelected( true );		
	}
	else if( mCheckBox->checkState() ==  Qt::Checked ){
		mCheckBox->setCheckState( Qt::Unchecked );
		setSelected( false );
	}	
}

bool FmRestoreViewItem::getCheckBoxState()
{
    if( mCheckBox->checkState() == Qt::Unchecked ) {
        return false;
    } else {
        return true;
    }
}

