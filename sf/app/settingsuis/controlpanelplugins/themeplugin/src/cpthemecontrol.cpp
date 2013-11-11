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

/*!
    \class CpThemeControl
    \brief CpThemeControl creates and controls views for Theme Changer plugin and handles
	user interaction to preview and change the themes.

	This class also connects to the theme server using the HbThemeChanger and sets the theme
	based on user interaction with the views.  

 */

#include <QString>
#include <QModelIndex>
#include <QTranslator>
#include <QSortFilterProxyModel>
#include <QThread>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>

#include <hbmainwindow.h>
#include <hbinstance.h>
#include "cpthemechanger.h"

#include "cpthemecontrol.h"
#include "cpthemelistview.h"
#include "cpthemeinfo.h"
#include "cpthemelistmodel.h"

#include <hbdialog.h>
#include <hblabel.h>

//time out time before showing a processing dialog.
static const int KThemeChangeTimeOutMilliSeconds = 2000;  

/*!
	constructor.
*/
CpThemeControl::CpThemeControl(): mThemeListView(0), 
    mThemeChanger(0),
    mListModel(0),
    mThemeChangeFinished(false),
    mWaitDialog(0)
{
    mThemeChanger = new CpThemeChanger();
       
    QTranslator *translator = new QTranslator(this);
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    translator->load("control_panel_" + lang, path);
    qApp->installTranslator(translator);
    
    connect(mThemeChanger,SIGNAL(themeChangeFinished()), this, SLOT(themeChangeFinished()));
   
}


/*!
	destorys the list view and theme changer objects.
*/
CpThemeControl::~CpThemeControl()
{
    delete mThemeListView;
    mThemeListView = 0;

    delete mThemeChanger;
    mThemeChanger = 0;

    delete mWaitDialog;
    mWaitDialog = 0;
}

/*!
	Creates the theme list view.  Gets the themes, creates a model  and sets the list model.
*/
void CpThemeControl::createThemeList()
{
   
    mThemeListView = new CpThemeListView();
    
    if(!mListModel) {
        mListModel = new CpThemeListModel(this);
    }
    
    // Set the model for theme list.
    mThemeListView->setModel(mListModel);
    
    setActiveThemeIndex();
    
    //connect to signal for selecting a list item.
    connect(mThemeListView,SIGNAL(newThemeSelected(QModelIndex)),
            this,SLOT(newThemeSelected(QModelIndex)));
    
    connect(mThemeListView, SIGNAL(oviClicked()), this, SLOT(getOviTheme()));

	//handle signal for list view closing. (e.g Back softkey pressed)
    connect(mThemeListView,SIGNAL(aboutToClose()),
            this,SLOT(themeListClosed()));
}

/*!
	returns the instance of themelist view.  Used by control panel to set
	the view.  
*/
CpBaseSettingView* CpThemeControl::themeListView()
{
    //If the view was removed before by control panel app, create it again.
    if(!mThemeListView) {
        createThemeList();
    }

    return mThemeListView;
}

/*!
        returns the name of the current theme.
*/
QString CpThemeControl::currentThemeName() const
{
    QString name = "";
    if(mThemeChanger->currentTheme()) {
        name = mThemeChanger->currentTheme()->name();
    }
    return name;
}

/*!
        returns the repersenatative icon of the current theme.
*/
HbIcon CpThemeControl::currentThemeIcon() const
{
    HbIcon icon;
    if(mThemeChanger->currentTheme()) {
        icon = mThemeChanger->currentTheme()->icon();
    }
    return icon;
}

/*!
	Slot called when a list item of the theme list is selected.
*/
void CpThemeControl::newThemeSelected(const QModelIndex& index)
{
    if(!index.isValid()) {
        return;
    }
    
    CpThemeInfo themeInfo;
    QVariant data;

    //reset the current index to active theme, so that the selection remains on current
    //theme even though another list item is selected.
    setActiveThemeIndex();
    
    //get the theme name.
    data = index.data(Qt::DisplayRole);
    if(data.isValid()) {
        themeInfo.setName(data.toString());
    }
    
    //get theme path
    data = index.data(CpThemeListModel::ItemDataRole);
    if(data.isValid()) {
        themeInfo.setItemData(data.toString());
    }
    
    applyTheme(themeInfo);

}

void CpThemeControl::getOviTheme()
{
    QString url = QString("http://lr.ovi.mobi/store/themes");
    // Launch the URL in the browser and 
    // continue to Preview if not successful
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
                              
}

/*!
	Slot called when a Select key is pressed in theme preview view.
*/
void CpThemeControl::applyTheme(const CpThemeInfo& theme)
{
    QThread::currentThread()->setPriority(QThread::HighPriority);  
    
    if(mThemeChanger->changeTheme(theme)) {
    
        //Start a timer. If theme change takes more than 1 seconds,
        //we will show a dialog (mWaitDialog) until theme change
        //is done (themeChangeFinished is called).
        QTimer::singleShot(KThemeChangeTimeOutMilliSeconds, this, SLOT(themeWaitTimeout()));
        
        mThemeChangeFinished = false;
    } else {
        //theme change failed, go back to control panel.
        setActiveThemeIndex();
    }
   
}

/*!
    Slot for when the theme list view is closed. Ownership of the theme list was given to
    control panel, so the class won't delete it.
    
*/
void CpThemeControl::themeListClosed()
{
    mThemeListView = 0;
}

/*!
    asks the theme list view to close.  
*/
void CpThemeControl::triggerThemeListClose()
{
    mThemeListView->closeView();
}

void CpThemeControl::themeChangeTimeout()
{
    //Theme change is finished and idle timer has timed out,
    //so revert back the application priority to normal
    //and go back to control panel view.
    if(mWaitDialog && mWaitDialog->isVisible()) {
        mWaitDialog->hide();
    }
    setActiveThemeIndex();
    QThread::currentThread()->setPriority(QThread::NormalPriority); 
}

void CpThemeControl::themeWaitTimeout()
{
    //If after this timeOut, theme change is still in progress,
    //show a processing dialog.
    if(!mThemeChangeFinished){
        if(!mWaitDialog) {
            mWaitDialog = new HbDialog();
            mWaitDialog->setDismissPolicy(HbPopup::NoDismiss);
            mWaitDialog->setModal(false);
            mWaitDialog->setTimeout(HbPopup::NoTimeout);
            // Create and set HbLabel as content widget.
            QString processingText = hbTrId("txt_common_info_processing") + QString("...");
            HbLabel *label = new HbLabel(processingText);
            label->setAlignment(Qt::AlignCenter);
            mWaitDialog->setContentWidget(label);
        }
       // as we do not need any signals, calling show() instead of open()
       mWaitDialog->show();
    }
}

void CpThemeControl::themeChangeFinished()
{
    //Theme change is done. Start an idle timer to let the UI
    //finish remaining tasks.
    QTimer::singleShot(0, this, SLOT(themeChangeTimeout()));
    mThemeChangeFinished = true;
    
    if(mThemeChanger->currentTheme()) {
        emit themeUpdated(mThemeChanger->currentTheme()->name(), mThemeChanger->currentTheme()->icon());
    }
    
}

/*!
 * Private function that sets the current index of theme list view to indicate
 * the active theme.
 */
void CpThemeControl::setActiveThemeIndex()
{
    //Get the index of current theme.
    CpThemeListModel* themeListModel = dynamic_cast<CpThemeListModel*>(mListModel);
    const CpThemeInfo* currentTheme = mThemeChanger->currentTheme();
    if(themeListModel && currentTheme) {
        QModelIndex sourceIndex = mListModel->index(themeListModel->indexOf(*currentTheme),0);
        //set current index.
        mThemeListView->themeList()->setCurrentIndex(sourceIndex, QItemSelectionModel::SelectCurrent);
    }
    else {
        mThemeListView->themeList()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Clear);
    }
}
    





