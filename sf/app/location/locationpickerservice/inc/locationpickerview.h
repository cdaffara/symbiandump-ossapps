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
* Description: LocationPickerView declaration
*
*/

#ifndef LOCATIONPICKERVIEW_H
#define LOCATIONPICKERVIEW_H


#include <HbView>
#include <hbdocumentloader.h>
#include "locationpickertypes.h"
#include <QGraphicsLinearLayout>
#include <hgwidgets/hgmediawall.h>

#include <HbLabel>
#include <HbTextItem>
//forward declarations
class HbListView;
class QStandardItemModel;
class LocationPickerProxyModel;
class LocationPickerCollectionListContent;
class LocationPickerCollectionContent;
class HgWidgetDataModel;
class HbListViewItem;
class HbAction;
class HbAbstractViewItem;
class QPoint;
class HbDialog;
class HbLabel;
/**  
 * Class defines the location picker view
 */
class LocationPickerView : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerView( HbDocumentLoader* aLoader );
    // destructor
    ~LocationPickerView();
public:
    //disable the tabs
    void disableTabs();
    //get the items from docml and connect to respective slots
    void init( Qt::Orientation aOrientation, QStandardItemModel *aModel );
    //Set the appropriate model on list view
    void manageListView();
    //create hurriganes widget
    void createHurriganesWidget();
    //manage hurriganes view
    void manageHgWidget();
    //Create collection list and sets to list view
    void setCollectionData( quint32 acategoryId );
    //Get the view type
    TViewType getViewType();
    //set the view type
    void setViewType( TViewType aViewType );
    //clear collection Model
    void clearContentModel();
    void removeDetailsLabel();
    //close the popupdialog
    void closeDetailsDialog();
#ifdef LOCPICKER_UNIT_TEST
public:
#else    
private:
#endif
    void displayNoEntries();
#ifdef LOCPICKER_UNIT_TEST
public slots:
#else    
private slots:
#endif
    //slot to handle list item actions     
    void handleActivated( const QModelIndex &aIndex );
    //slots to handle menu action items     
    void sortDescending();
    void sortAscending();
    //handle secondary back action
    void backTriggered();
    //slot to handle search tab
    void searchTabTriggered();
    //context menu
    void launchPopUpMenu( HbAbstractViewItem *aItem, const QPointF &aPoint );
    //launch context menu
    void launchPopUpMenu( const QModelIndex &aIndex, const QPointF &aPoint );
    //handle select event
    void handleSelect();
    //handle details event
    void handleDetails();
    //slot to handle all tab
    void allTabTriggered();
    //slot to handle collection tab
    void colectionTabTriggered();
    //delete context menu
    void deleteMenu();
signals:
    void switchToSearchView();
    void selectItem( quint32 aLm );
    void completeService();
#ifdef LOCPICKER_UNIT_TEST
public:
#else    
private:
#endif
    //document loader
    HbDocumentLoader* mDocumentLoader;
    // collection list content
    LocationPickerCollectionListContent* mLocationPickerCollectionListContent;
    //locationPickerProxyModel
    LocationPickerProxyModel *mProxyModel;
	//standard model
    QStandardItemModel *mModel;
    //actions
    HbAction *mAllAction;
    HbAction *mCollectionAction;
    HbAction *mSearchAction;
    HbAction *mAscendingAction;
    HbAction *mDescendingAction;
    //secondary back action
    HbAction *mBackAction;
    // list view
    HbListView  *mListView;
    //list item
    HbListViewItem *mListItem;
    //collection content 
    LocationPickerCollectionContent *mCollectionContent;
    //ViewType
    TViewType mViewType ;
    // get the collection/category id
    quint32 mCategoryId;
    //label to show current view
    QModelIndex mIndex;
    //linear Layout
    QGraphicsLinearLayout *mLinerLayout;
    //model for hurriganes
    HgWidgetDataModel       *mHgModel;
    //hurriganes 
    HgWidget                *mWidget;
    // label to show view details
    HbLabel *mColllabel;
    //text item to show no entries
    HbTextItem*             mEmptyLabel;
    //pop up dialog to show details
    HbDialog* mDialog;
    HbLabel* mMapIconLabel;
    HbLabel* mTitleLabel;
    HbLabel* mAddressMiddle;
    HbLabel* mAddressBottom;
    HbAction* mDone;
    HbMenu* mLongPressMenu;
    HbAction* mSelectAction; 
    HbAction* mDetailsAction; 
    bool mPopulated;
};


#endif // LOCATIONPICKERVIEW_H
