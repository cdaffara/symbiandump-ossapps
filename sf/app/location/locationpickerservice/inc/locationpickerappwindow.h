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
* Description: LocationPickerAppWindow declaration
*
*/

#ifndef LOCATIONPICKERAPPWINDOW_H
#define LOCATIONPICKERAPPWINDOW_H
#include <HbMainWindow>

#include "locationpickertypes.h"

// Forward declarations
class LocationPickerSearchView;
class LocationPickerDocumentLoader;
class LocationPickerView;
class LocationPickerService;
class LocationPickerProxyModel;
class LocationPickerContent;

class LocationPickerAppWindow: public HbMainWindow
{
    Q_OBJECT
public:
    // constructor
    LocationPickerAppWindow(QWidget *parent=0, Hb::WindowFlags 
            windowFlags=Hb::WindowFlagNone);
    
    // destructor
    ~LocationPickerAppWindow();

#ifdef LOCPICKER_UNIT_TEST
public slots:
#else    
private slots:
#endif
    //Connect the slots for Potrait view
    void connectSlots();
public slots:
    // a list item is selected
    void itemSelected( quint32 aLm );
#ifdef LOCPICKER_UNIT_TEST
public slots:
#else    
private slots:
#endif

    //activate search view
    void activateSearchView();
    //activate locationpicker view
    void activateLocationPickerView();
    //changes the orientation
    void changeOrientation( Qt::Orientation );
    //complete the service
    void serviceComplete();
    //slot to close details dialog (if open)
    void closeDetailsDialog();

#ifdef LOCPICKER_UNIT_TEST
public:
#else    
private:
#endif
    // search view
    LocationPickerSearchView* mLocationPickerSearchView;
    //document loader
    LocationPickerDocumentLoader* mLocationPickerDocumentLoader;
    //location picker potrait view
    LocationPickerView* mLocationPickerView;
	//locationpicker content
    LocationPickerContent* mLocationPickerContent;
    //location picker service;
    LocationPickerService *mService;
};
#endif // LOCATIONPICKERAPPWINDOW_H
