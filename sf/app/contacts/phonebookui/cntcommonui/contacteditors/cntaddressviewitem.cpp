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
#include "cntaddressviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qcontactaddress.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbstyleloader.h>
#include <hbpushbutton.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbtextitem.h>

#include "qlocationpickeritem.h"
#include <xqaiwrequest.h>
#include <xqservicerequest.h>
#include <xqappmgr.h>
CntAddressViewItem::CntAddressViewItem(QGraphicsItem* aParent) :
    /*CntDetailViewItem(aParent),*/
    HbDataFormViewItem(aParent),
    mAppManager(NULL),
    mRequest(NULL),
    mRequestPending(false)
{
}

CntAddressViewItem::~CntAddressViewItem()
{
    if(mAppManager)
    {
        delete mAppManager;
        mAppManager = NULL;
    }
    if(mRequest)
    {
        delete mRequest;
        mRequest = NULL;
    }
}

HbAbstractViewItem* CntAddressViewItem::createItem()
{
    return new CntAddressViewItem(*this);
}

bool CntAddressViewItem::canSetModelIndex( const QModelIndex &index ) const 
{ 
    HbDataFormModelItem::DataItemType itemType = 
        static_cast<HbDataFormModelItem::DataItemType>( 
        index.data(HbDataFormModelItem::ItemTypeRole).toInt() ); 

    if( itemType == HbDataFormModelItem::CustomItemBase )       
    {  
        return true; 
    } 
    else 
    { 
        return false; 
    } 

}

HbWidget* CntAddressViewItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>( 
              modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    
    HbWidget* widget = NULL;
    
    if( itemType ==  HbDataFormModelItem::CustomItemBase )
    {
        HbPushButton* locationButton = new HbPushButton(this);
        
        // Naming UI components for automation testability
        locationButton->setObjectName("Select location button");
        locationButton->setText( hbTrId("txt_phob_button_select_location") );
        locationButton->setTextAlignment( Qt::AlignCenter );
        locationButton->setContentsMargins(0,0,0,0);
    
        connect(locationButton, SIGNAL(clicked()), this, SLOT(launchLocationPicker()));
    
        widget = locationButton;
        
    }
    return widget;
}

void CntAddressViewItem::launchLocationPicker()
{
    if( !mAppManager )
    {
    	mAppManager = new XQApplicationManager();
    }
    if(!mRequest)
    {	
    	mRequest = mAppManager->create("com.nokia.symbian", "ILocationPick", "pick()", true);
    	mRequest->setSynchronous(false);
    	connect(mRequest, SIGNAL(requestOk(const QVariant&)), this, SLOT(handleLocationChange(const QVariant&)));
    }
    if(!mRequestPending)
    {
        if( mRequest->send() )
    	{
            mRequestPending = true;
    	}
    }
}

void CntAddressViewItem::handleLocationChange(const QVariant& aValue)
{
    mRequestPending = false;
    QLocationPickerItem selectedLocation = aValue.value<QLocationPickerItem>();
    if( selectedLocation.mIsValid )
    {
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
    
        QModelIndex nextIndex = modelIndex().sibling( modelIndex().row() + 1 , modelIndex().column() );
        HbDataFormModelItem* street = model->itemFromIndex( nextIndex );
        street->setContentWidgetData( "text", selectedLocation.mStreet );
        street->setContentWidgetData( "maxLength", CNT_STREET_MAXLENGTH );

        nextIndex = modelIndex().sibling( modelIndex().row() + 2 , modelIndex().column() );

        HbDataFormModelItem* postal =  model->itemFromIndex( nextIndex );
        postal->setContentWidgetData( "text", selectedLocation.mPostalCode );
        postal->setContentWidgetData( "maxLength", CNT_POSTCODE_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 3 , modelIndex().column() );
        HbDataFormModelItem* locality =  model->itemFromIndex( nextIndex );
        locality->setContentWidgetData( "text", selectedLocation.mCity );
        locality->setContentWidgetData( "maxLength", CNT_LOCALITY_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 4 , modelIndex().column() );
        HbDataFormModelItem* province =  model->itemFromIndex( nextIndex );
        province->setContentWidgetData( "text", selectedLocation.mState );
        province->setContentWidgetData( "maxLength", CNT_REGION_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 5 , modelIndex().column() );
        HbDataFormModelItem* country =  model->itemFromIndex( nextIndex );
        country->setContentWidgetData( "text", selectedLocation.mCountry );
        country->setContentWidgetData( "maxLength", CNT_COUNTRY_MAXLENGTH );
        

    }
   
}

Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)

// End of File
