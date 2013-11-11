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
* Description: LocationPickerSearchView declaration
*
*/

#ifndef LOCATIONPICKERSEARCHVIEW_H
#define LOCATIONPICKERSEARCHVIEW_H


#include <HbView>

class HbListView;
class QStandardItemModel;
class HbSearchPanel;
class LocationPickerDataManager;
class LocationPickerProxyModel;
class HbDocumentLoader;
class HbTextItem;
class QGraphicsLinearLayout;
class HbAbstractViewItem;
class QPoint;
class HbMenu;

/**  
 * Class defines the search view of location picker
 */
class LocationPickerSearchView : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerSearchView( HbDocumentLoader &aLoader );
    // destructor
    ~LocationPickerSearchView();
    //initialize the action items and connect to slots
    void init( QStandardItemModel *aModel );
private:
    void getData( QModelIndex aIndex, quint32& aValue );
#ifdef LOCPICKER_UNIT_TEST
public slots:
#else    
private slots:
#endif
    // slot to perform search
    void doSearch( QString aCriteria );
    // slot to handle select event on a list item
    void handleActivated(const QModelIndex &aIndex);
    // slot to handle backbutton on search panel
    void handleExit();
	//launch context menu
    void launchPopUpMenu(HbAbstractViewItem *aItem, const QPointF &aPoint);
	//handle long press
    void handleLongPress();
    //delete context menu
    void deleteMenu();
    //handle secondary back action
    void backTriggered();
signals:
    //signals to switch current view
    void switchView();
    //signals when any item is selected
    void selectItem( quint32 aLm );
    //signal to send service complete
    void completeService();
#ifdef LOCPICKER_UNIT_TEST
public:
#else    
private:
#endif
    //proxymodel used for sort and filter
    LocationPickerProxyModel *mProxyModel;
    // model for the view
    QStandardItemModel *mModel;
    // list view
    HbListView  *mListView;
    // search panel
    HbSearchPanel *mSearchPanel;
    //TextItem
    HbTextItem* mEmptyLabel;
    //Graphicslayout
    QGraphicsLinearLayout* mVerticalLayout;
    //Documentloader
    HbDocumentLoader &mDocumentLoader;
    QModelIndex mIndex;
    //context menu
    HbMenu* mLongPressMenu;
    //select action
    HbAction* mSelectAction;
    //back action
    HbAction *mBackAction;
};


#endif // LOCATIONPICKERSEARCHVIEW_H
