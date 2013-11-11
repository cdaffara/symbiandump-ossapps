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

#include "cntaddressmodel.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"
#include <maptileservice.h> 

CntAddressModel::CntAddressModel( QContact* aContact ):
CntDetailEditorModel( aContact ),
mAddress(NULL),
mAddressHome(NULL),
mAddressWork(NULL),
mIsLocationPickerEnabled( false )
    {
    mMaptileInterface = new MapTileService;
    HbDataFormModelItem* address = appendDataFormGroup(hbTrId("txt_phob_formlabel_address"), invisibleRootItem());
    HbDataFormModelItem* addressHome = appendDataFormGroup(hbTrId("txt_phob_formlabel_address_home"), invisibleRootItem());
    HbDataFormModelItem* addressWork = appendDataFormGroup(hbTrId("txt_phob_formlabel_address_work"), invisibleRootItem());
    
    foreach ( QContactAddress a, mContact->details<QContactAddress>() )
        {
        QStringList context = a.contexts();
        QString type = context.isEmpty() ? "" : context.first();
        // Context HOME
        if (type == QContactAddress::ContextHome && !mAddressHome )
            {
            mAddressHome = new QContactAddress( a );
            createAddressItems( addressHome, mAddressHome );
            }
        // Context WORK
        else if (type == QContactAddress::ContextWork && !mAddressWork )
            {
            mAddressWork = new QContactAddress( a );
            createAddressItems( addressWork, mAddressWork );
            }
        // Context WHATEVER
        else if ( !mAddress ) // no context
            {
            mAddress = new QContactAddress( a );
            createAddressItems( address, mAddress );
            }
        }
    
    // Check that all items (address, addressHome, addressWork) are created
    if ( !mAddress )
        {
        mAddress = new QContactAddress();
        createAddressItems( address, mAddress );
        }
    
    if ( !mAddressHome )
        {
        QStringList context;
        context << QContactAddress::ContextHome;
                
        mAddressHome = new QContactAddress();
        mAddressHome->setContexts( context );
        
        createAddressItems( addressHome, mAddressHome );
        }
    
    if ( !mAddressWork )
        {
        mAddressWork = new QContactAddress();
        QStringList context;
        context << QContactAddress::ContextWork;
        mAddressWork->setContexts( context );
        createAddressItems( addressWork, mAddressWork );
        }
    }

CntAddressModel::~CntAddressModel()
    {
    delete mAddress;
    delete mAddressHome;
    delete mAddressWork;
    delete mMaptileInterface;
    }

void CntAddressModel::createAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress )
    {
	//Show the location picker button only if location feature enabled
    if( mMaptileInterface->isLocationFeatureEnabled(MapTileService::AppTypeContacts) )
    {
        // custom item for map button
        HbDataFormModelItem* mapButton = new HbDataFormModelItem( HbDataFormModelItem::CustomItemBase );
        appendDataFormItem( mapButton, aGroup );
        mIsLocationPickerEnabled = true;
    }
    
    // default items for rest of fields
    HbDataFormModelItem* street = new HbDataFormModelItem( HbDataFormModelItem::TextItem, hbTrId("txt_phob_formlabel_street"));
    street->setContentWidgetData( "text", aAddress->street() );
    street->setContentWidgetData( "maxLength", CNT_STREET_MAXLENGTH );
    
    HbDataFormModelItem* postal = new HbDataFormModelItem( HbDataFormModelItem::TextItem, hbTrId("txt_phob_formlabel_postal_codezip_code"));
    postal->setContentWidgetData( "text", aAddress->postcode() );
    postal->setContentWidgetData( "maxLength", CNT_POSTCODE_MAXLENGTH );
        
    HbDataFormModelItem* city = new HbDataFormModelItem( HbDataFormModelItem::TextItem, hbTrId("txt_phob_formlabel_city"));
    city->setContentWidgetData( "text", aAddress->locality() );
    city->setContentWidgetData( "maxLength", CNT_LOCALITY_MAXLENGTH );
    
    HbDataFormModelItem* region = new HbDataFormModelItem( HbDataFormModelItem::TextItem, hbTrId("txt_phob_formlabel_province"));
    region->setContentWidgetData( "text", aAddress->region() );
    region->setContentWidgetData( "maxLength", CNT_REGION_MAXLENGTH );
    
    HbDataFormModelItem* country = new HbDataFormModelItem( HbDataFormModelItem::TextItem, hbTrId("txt_phob_formlabel_countryregion"));
    country->setContentWidgetData( "text", aAddress->country() );
    country->setContentWidgetData( "maxLength", CNT_COUNTRY_MAXLENGTH );
    
    appendDataFormItem( street, aGroup );
    appendDataFormItem( postal, aGroup );
    appendDataFormItem( city, aGroup );
    appendDataFormItem( region, aGroup );
    appendDataFormItem( country, aGroup );
    }

void CntAddressModel::saveContactDetails()
{
    // No Context
    HbDataFormModelItem* addressRoot = invisibleRootItem()->childAt( 0 );
    if ( saveAddressItems( addressRoot, mAddress ) )
    {
        mContact->saveDetail( mAddress );    
    }
    
    // Home 
    HbDataFormModelItem* addressHomeRoot = invisibleRootItem()->childAt( 1 );
    if ( saveAddressItems( addressHomeRoot, mAddressHome ) )
    {
        mContact->saveDetail( mAddressHome );    
    }
    
    // Business
    HbDataFormModelItem* addressWorkRoot = invisibleRootItem()->childAt( 2 );
    if ( saveAddressItems( addressWorkRoot, mAddressWork ) )
    {
        mContact->saveDetail( mAddressWork );    
    }
    
    if ( isAddressEmpty(mAddress) ) 
    {
        mContact->removeDetail( mAddress );
    }
    
    if ( isAddressEmpty(mAddressHome) ) 
    {
        mContact->removeDetail( mAddressHome );
    }
    
    if ( isAddressEmpty(mAddressWork) ) 
    {
        mContact->removeDetail( mAddressWork );
    }
}

QContactDetail CntAddressModel::detail() const
{
    if ( !isAddressEmpty(mAddressWork) )
    {
        return *mAddressWork;
    }
    
    if ( !isAddressEmpty(mAddressHome) )
    {
        return *mAddressHome;
    }
    
    if ( !isAddressEmpty(mAddress) )
    {
        return *mAddress;
    }
    
    return QContactAddress();
}

bool CntAddressModel::saveAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress )
{
    int offset = 0;
    if( mMaptileInterface->isLocationFeatureEnabled(MapTileService::AppTypeContacts) )
    {
        offset = 1;
    }
    
	QString street = aGroup->childAt( 0 + offset  )->contentWidgetData("text").toString().trimmed();
	QString postcode = aGroup->childAt( 1 + offset )->contentWidgetData("text").toString().trimmed();
	QString locality = aGroup->childAt( 2 + offset  )->contentWidgetData("text").toString().trimmed();
	QString region = aGroup->childAt( 3 + offset  )->contentWidgetData("text").toString().trimmed();
	QString country = aGroup->childAt( 4 + offset  )->contentWidgetData("text").toString().trimmed();
	
	bool changed = ( 
	        street != aAddress->street() || 
            postcode != aAddress->postcode() || 
            locality != aAddress->locality() ||
            region != aAddress->region() ||
            country != aAddress->country() 
            );
	
	if ( changed )
	{
        aAddress->setStreet( street );
        aAddress->setPostcode( postcode );
        aAddress->setLocality( locality );
        aAddress->setRegion( region );
        aAddress->setCountry( country );
	}
    return changed;
}

bool CntAddressModel::isAddressEmpty( QContactAddress* aAddress ) const
{
    return ( aAddress->street().length() == 0 &&
             aAddress->postcode().length() == 0 &&
             aAddress->locality().length() == 0 &&
             aAddress->region().length() == 0 &&
             aAddress->country().length() == 0 );
}
