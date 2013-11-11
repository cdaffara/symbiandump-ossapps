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
#ifndef SEARCH_STATES_ONLINE_H
#define SEARCH_STATES_ONLINE_H
#include <qstate.h>
#include "search_global.h"
#include <hbicon.h>
SEARCH_CLASS( SearchStateProviderTest)

class HbListWidget;
class HbView;
class HbSearchPanel;
class HbMainWindow;
class OnlineHandler;
class SearchUiLoader;

/** @ingroup group_searchonlinestate
 * @brief The state handles wizard activation and interaction.
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class SearchOnlineState : public QState
    {
Q_OBJECT
public:

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    SearchOnlineState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~SearchOnlineState();

protected:

    /**
     * @copydoc QState::onEntry()
     */
    void onEntry(QEvent *event);

    /**
     * @copydoc QState::onExit()
     */
    void onExit(QEvent *event);

private:
    /**
     * deactivates the signals .
     * @since S60 ?S60_version.
     */
    void deActivateSignals();

    /**
     * activates the signals .
     * @since S60 ?S60_version.
     */
    void activateSignals();

public slots:

    /**
     * slot connects to search panel to initiate the online search
     * @since S60 ?S60_version.
     * @param aKeyword search keyword.
     */
    void startOnlineSearch(const QString &aKeyword);

    /**
     * slot connects to search panel to launch the settings delimiter
     * @since S60 ?S60_version.     
     */
    void setSettings();

    /**
     * slot connects to search panel to cancel the outstanding search
     * @since S60 ?S60_version.     
     */
    void cancelSearch();

    /**
     * slot connects to online handler to get notified when plugin is intialized 
     * @since S60 ?S60_version.     
     */
    void slotpluginIntialized(bool);

    /**
     * slot connects to online handler to get notified when plugin is shutdown 
     * @since S60 ?S60_version.     
     */
    void slotpluginShutdown(bool);

    /**
     * slot connects to online handler to get notified when view changed  
     * @since S60 ?S60_version.     
     */
    void slotbackEventTriggered();

    /**
     * slot connects to progressive search state to get notified criteria in search panel  
     * @since S60 ?S60_version.
     * @param aKeyword search keyword.     
     */
    void slotIndeviceQuery(QString);

    /**
     * slot connects to progressive search state to launch the suggestion links  
     * @since S60 ?S60_version.
     * @param aKeyword search keyword.
     * @param id search provider id.     
     */
    void slotlaunchLink(int, QString);

    /**
     * slot connects to main window to get notify when view is ready  
     * @since S60 ?S60_version.     
     */
    void slotviewReady();

signals:

    /**
     * Signalled when user selects an to switch the settings state
     * setting state will be  activated.
     */
    void switchOnlineToSettingsState();

    /**
     * Signalled when user change the state to progressive state
     * setting state will be  activated.
     */
    void onlineSearchQuery(QString);

    /**
     * Signalled when UI is ready on online state        
     */
    void applicationReady();

private:
    HbMainWindow* mMainWindow;
    /**
     * main view.
     * Own.
     */
    HbView* mView;

    /**
     * The searchpanel widget.
     * Own.
     */
    HbSearchPanel* mSearchPanel;

    /**
     * The List View widget.    
     */
    HbListWidget* mListView;

    /**
     * OnlineHandler instance.
     */
    OnlineHandler* mInternetHandler;

    SearchUiLoader* mUiLoader;

    /**
     * search keyword.
     */
    QString mSearchQuery;
    bool mSearchReady;
    bool mIndeviceQueryAvailable;
   
    /**
     * friend class for unit testing
     */
    SEARCH_FRIEND_CLASS (SearchStateProviderTest)
    };

#endif //SEARCH_STATES_ONLINE_H
