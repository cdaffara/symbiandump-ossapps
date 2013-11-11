/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: LocationPickerAppWindow implementation
*
*/

#include "locationpickerappwindow.h"

#include "qlocationpickeritem.h"

#include "locationpickersearchview.h"
#include "locationpickerview.h"
#include "locationpickerservice.h"
#include "locationpickerdatamanager.h"
#include "locationpickerdocumentloader.h"
#include "locationpickercontent.h"

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::LocationPickerAppWindow( QWidget *parent, Hb::WindowFlags windowFlags )
	:HbMainWindow(parent, windowFlags),
	mLocationPickerSearchView(NULL),
    mLocationPickerDocumentLoader(NULL),
	mLocationPickerView(NULL),
	mLocationPickerContent(NULL),
    mService(NULL)
{
    // create the service object;
    mService = new LocationPickerService(this);
    mLocationPickerContent = new LocationPickerContent();
    
    //create document loader object
    mLocationPickerDocumentLoader = new LocationPickerDocumentLoader();
    bool ok = false;
    //load the Locationpicker view
    mLocationPickerDocumentLoader->load(":/locationpickerview.docml", &ok);
    Q_ASSERT_X(ok, "locationpickerservice", "invalid DocML file");
    //find graphics location picker potrait view
    QGraphicsWidget *locationPickerWidget = mLocationPickerDocumentLoader->findWidget("LocationPickerView");
    Q_ASSERT_X((locationPickerWidget != 0), "locationpickerservice", "invalid DocML file");
    mLocationPickerView = qobject_cast<LocationPickerView*>(locationPickerWidget);
    bool populated = mLocationPickerContent->populateModel();
    connectSlots();
    if(!populated)
    {
        mLocationPickerView->disableTabs();
        addView(mLocationPickerView);
        setCurrentView(mLocationPickerView);
    }
    else
    {    
    //initialize potrait widgets and connect to respective signals 
    mLocationPickerView->init( this->orientation(), mLocationPickerContent->getStandardModel());
    addView( mLocationPickerView );
    setCurrentView(mLocationPickerView);
    //connect to orientationChanged signal
     connect(this, SIGNAL(orientationChanged(Qt::Orientation)),this, 
     	SLOT(changeOrientation(Qt::Orientation)));
    connect(this, SIGNAL(aboutToChangeOrientation()),this, SLOT(closeDetailsDialog()));
     
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::~LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::~LocationPickerAppWindow()
{
    delete mService;
    delete mLocationPickerSearchView;
    delete mLocationPickerDocumentLoader;
    delete mLocationPickerView;
    delete mLocationPickerContent;
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::itemSelected()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::itemSelected( quint32 aLmid )
{
    QLocationPickerItem item;
    LocationPickerDataManager::getInstance()->getLocationItem(aLmid, item);
    // complete the request
    mService->complete(item);
}

void LocationPickerAppWindow::serviceComplete()
{
    // Other views request complete with a valid location picker item
    QLocationPickerItem item;
    item.mIsValid = false;
    mService->complete(item);
}
// ----------------------------------------------------------------------------
// LocationPickerAppWindow::activateSearchView()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::activateSearchView()
{   
    //load the LocationPickerSearchView
    bool ok = false;
    if(!mLocationPickerSearchView)
    {
        mLocationPickerDocumentLoader->load(":/locationpickersearchview.docml", &ok);
        Q_ASSERT_X(ok, "locationpickerService", "invalid DocML file");
        //find the LocationPickerSearchView
        QGraphicsWidget *locationPickerSearchWidget = 
        	mLocationPickerDocumentLoader->findWidget("LocationPickerSearchView");
        Q_ASSERT_X((locationPickerSearchWidget != 0), "locationpickerService", "invalid DocML file");
        mLocationPickerSearchView = qobject_cast<LocationPickerSearchView*>(locationPickerSearchWidget);
        Q_ASSERT_X((mLocationPickerSearchView != 0), "mLocationPickerSearchView", 
            "qobject cast failure");
        //initialize the action items and connect to slots
        mLocationPickerSearchView->init(mLocationPickerContent->getStandardModel());
        connect(mLocationPickerSearchView,SIGNAL(switchView()),
        	this,SLOT(activateLocationPickerView()));
        connect(mLocationPickerSearchView,SIGNAL(selectItem( quint32 )),
        	this,SLOT(itemSelected( quint32 )));
        connect(mLocationPickerSearchView,SIGNAL(completeService()),
        	this,SLOT(serviceComplete()));
        addView(mLocationPickerSearchView);
    }
    //set LocationPickerSearchview as current view
    setCurrentView(mLocationPickerSearchView);
   
}


// ----------------------------------------------------------------------------
// LocationPickerAppWindow::activateLocationPickerView()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::activateLocationPickerView()
{ 
    mLocationPickerView->setViewType( ELocationPickerContent );
    //check the orientation and load view accordingly 
    if(this->orientation() == Qt::Horizontal)
    {
        mLocationPickerView->manageHgWidget();
    }
    else
    { 
        mLocationPickerView->manageListView();
    }
    setCurrentView(mLocationPickerView);
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::changeOrientation()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::changeOrientation( Qt::Orientation )
{   
    //check the orientation and load view accordingly 
    if(mLocationPickerView->getViewType()== ELocationPickerCollectionContent || 
            mLocationPickerView->getViewType()== ELocationPickerContent )
    {
        if(this->orientation() == Qt::Vertical)
       {
            mLocationPickerView->manageListView();
      
       }
       
   else if(this->orientation() == Qt::Horizontal)
       {
           mLocationPickerView->manageHgWidget();
       }
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::closeDetailsDialog()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::closeDetailsDialog()
{
    mLocationPickerView->closeDetailsDialog();
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::connectSlots()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::connectSlots()
{
    connect(mLocationPickerView,SIGNAL(switchToSearchView()),this,SLOT(activateSearchView()));
    connect(mLocationPickerView,SIGNAL(selectItem( quint32 )),this,SLOT(itemSelected( quint32 )));
    connect(mLocationPickerView,SIGNAL(completeService()),this,SLOT(serviceComplete()));
}

Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)
