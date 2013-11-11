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
 * Description:  Declaration of the progressive search state.
 *
 */

#ifndef PROGRESSIVE_SEARCH_STATE_H
#define PROGRESSIVE_SEARCH_STATE_H

#include <qstate.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qpixmap.h>
#include <hbicon.h>
#include <displaymode.h>
#include <xqappmgr.h>
#include <xqaiwrequest.h>
#include "search_global.h"
#include <f32file.h>

#ifdef OST_TRACE_COMPILER_IN_USE //defined in Search_global.h 
#define PERF_CAT_API_TIME_RESTART  m_categorySearchApiTime.restart();
#define PERF_CAT_UI_TIME_RESTART  m_categorySearchUiTime.restart();
#define PERF_CAT_TOTAL_TIME_RESTART  m_totalSearchUiTime.restart();
#define PERF_CAT_API_ENDLOG qDebug() << "Search on category (API): " << mTemplist.at( mDatabasecount-1 ) << "took "<< m_categorySearchApiTime.elapsed() << " msec";
#define PERF_CAT_UI_ENDLOG qDebug() << "Search on category (UI): " << mTemplist.at( mDatabasecount-1 ) << "took "<< m_categorySearchUiTime.elapsed() << " msec";
#define PERF_TOTAL_UI_ENDLOG qDebug() << "Search on total (UI): took "<< m_totalSearchUiTime.elapsed() << " msec";
#define PERF_CAT_HITS_ENDLOG qDebug() << "Hits on category: " << mTemplist.at( mDatabasecount-1 ) << ": "<< aResultCount;
#define PERF_CAT_GETDOC_TIME_RESTART m_categoryGetDocumentApiTime.restart();
#define PERF_CAT_GETDOC_TIME_ACCUMULATE m_getDocumentCatergoryTimeAccumulator += m_categoryGetDocumentApiTime.elapsed();
#define PERF_CAT_GETDOC_ACCUMULATOR_RESET m_getDocumentCatergoryTimeAccumulator = 0;
#define PERF_CAT_GETDOC_ACCUMULATOR_ENDLOG qDebug() << "Get Doc on category (API): " << mTemplist.at( mDatabasecount-1 ) << "took " << m_getDocumentCatergoryTimeAccumulator << "msec";
#define PERF_RESULT_ITEM_LAUNCH_TIME_RESTART m_resultItemLaunchTime.restart();
#define PERF_RESULT_ITEM_FOR_LAUNCHING(string) qDebug() <<"Result_Item_Launching: Launching "<<string ;
#define PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG(string) qDebug() <<"Result_Item_Launching:"<<string<<"took "<<m_resultItemLaunchTime.elapsed()<<" msec";
#else

#define PERF_CAT_API_TIME_RESTART  
#define PERF_CAT_UI_TIME_RESTART  
#define PERF_CAT_TOTAL_TIME_RESTART
#define PERF_CAT_API_ENDLOG
#define PERF_CAT_UI_ENDLOG
#define PERF_TOTAL_UI_ENDLOG
#define PERF_CAT_HITS_ENDLOG
#define PERF_CAT_GETDOC_TIME_RESTART
#define PERF_CAT_GETDOC_TIME_ACCUMULATE 
#define PERF_CAT_GETDOC_ACCUMULATOR_RESET
#define PERF_CAT_GETDOC_ACCUMULATOR_ENDLOG
#define PERF_RESULT_ITEM_LAUNCH_TIME_RESTART
#define PERF_RESULT_ITEM_FOR_LAUNCHING(string)
#define PERF_RESULT_ITEM_LAUNCH_TIME_ENDLOG(string)
#endif //OST_TRACE_COMPILER_IN_USE
class HbMainWindow;
class HbView;
class HbSearchPanel;
class CFbsBitmap;
class InDeviceHandler;
class CpixDocument;
class NotesEditorInterface;
class EventViewerPluginInterface;
class HbListWidget;
class HbListWidgetItem;
class QPluginLoader;
class SearchUiLoader;
SEARCH_CLASS( SearchStateProviderTest)
/** @ingroup group_searchstateprovider
 * @brief The state where progressive search state is shown
 *
 * @see StateMachine
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class SearchProgressiveState : public QState
    {
Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.     
     * @param aParent Owner.
     */
    SearchProgressiveState(QState *aParent = 0);

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~SearchProgressiveState();

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

    /**
     * get the application icon  .
     * @since S60 ?S60_version.
     */
    HbIcon getAppIconFromAppIdL(TUid auid);

    /**
     * resizing the symbain icon  .
     * @since S60 ?S60_version.
     */
    CFbsBitmap *copyBitmapLC(CFbsBitmap *input);

    /**
     * Constructing cpix handlers  .
     * @since S60 ?S60_version.
     */
    void constructHandlers();



    /**
     * parse the result documents.
     * @since S60 ?S60_version.
     */
    void parseDocument(CpixDocument* aDoc);

public slots:

    /**
     * slot connects to CSearchHandler to get the status of search result asynchronously 
     * @since S60 ?S60_version.
     * @param aError error code.
     * @param aResultCount number of results
     */
    void slotonAsyncSearchComplete(int aError, int aResultCount);

    /**
     * slot connects to CSearchHandler to get the result item asynchronously 
     * @since S60 ?S60_version.
     * @param aError error code.
     * @param aDoc result item
     */
    void slotonGetDocumentComplete(int aError, CpixDocument* aDoc);

    /**
     * slot connects to CSearchHandler to get the result item asynchronously 
     * @since S60 ?S60_version.
     * @param aError error code.
     * @param aDoc result item
     */
    void slotonGetBatchDocumentComplete(int aError, int aCount,
            CpixDocument** aDoc);

    /**
     * slot connects to list view to launch the respective application
     * @since S60 ?S60_version.
     * @param aIndex index of the activated item.
     */
    void slotopenResultitem(HbListWidgetItem * item);
    
    /**
     * slot connects to action to change the current state to settings state
     * @since S60 ?S60_version. 
     */
    void slotsetSettings();

    /**
     * slot connects to search panel to initiate the fresh search
     * @since S60 ?S60_version.
     * @param aKeyword search keyword.
     */
    void slotstartNewSearch(const QString &aKeyword);

    /**
     * slot implemented to avoid repeated search for the same category 
     * selection when user search for mutiple times
     * @since S60 ?S60_version.
     */
    void slotsettingsaction(bool avalue);

    /**
     * slot connects to search state  for internet search
     * @since S60 ?S60_version.
     */

    void slothandleOk(const QVariant& var);

    /**
     * slot added for Application manager
     * @since S60 ?S60_version.
     */

    void slothandleError(int ret, const QString& var);

    /**
     * Slot implemented to delete the calenderviewer plugin  
     * @since S60 ?S60_version.
     */
    void slotviewingCompleted();

    /**
     * Slot to notify when view is ready   
     * @since S60 ?S60_version.
     */
    void slotviewReady();

    /**
     * Slot to notify form query update form online state   
     * @since S60 ?S60_version.
     */
    void slotOnlineQuery(QString);

    /**
     * Slot to update the online service providers for suggestion links 
     * @since S60 ?S60_version.
     */
    void slotISProvidersIcon(int, HbIcon);

    /**
     * Slot to launch the search result screen with the activity URI  
     * @since S60 ?S60_version.
     */
    void slotactivityRequested(const QString &name);

    /**
     * Slot to notify when theme is changed
     * @since S60 ?S60_version.
     */    
    void slotdbChanged();
private:

    /**
     * initiate the fresh search for selected category separately 
     * 
     * @param aKeyword search keyword.
     */
    void searchOnCategory(const QString aKeyword);

    /**
     * clears the model  
     *              
     */
    void clear();

    /**
     * prepares the item on result list view for no result case 
     * 
     * @param aKeyword search keyword.
     */
    void noResultsFound(QString aKeyword);

    /**
     * prepares the suggestion link item on result list view to provide suggestion links     
     * 
     * @param aKeyword search keyword.
     */
    void createSuggestionLink();

    /**
     * Function to include corrrect Qimage format to be taken from bitmap
     *  @param mode Bitmap display mode.
     */
    QImage::Format TDisplayMode2Format(TDisplayMode mode);

    /**
     * Function to convert the s60 based CFbsBitmap into Qt specific QPixmap
     *  @param aBitmap Bitmap to be converted.
     */
    QPixmap fromSymbianCFbsBitmap(CFbsBitmap *aBitmap);

    /**
     * Function to retrive drive number from the provided mediaId
     *  @param aMediaId Unique media Id.
     */
    QString getDrivefromMediaId(QString aMediaId);

    /**
     * Function to launch the result item for application category 
     *  @param aUid Unique app Id.
     */
    void LaunchApplicationL(const QString aUid);

    /**
     * Function to parse the  CpixDocument with the given filter       
     */
    QString filterDoc(const CpixDocument* aDoc, const QString& filter);

    /**
     * Function to parse the  CpixDocument with the given filters       
     */
    QStringList filterDoc(const CpixDocument* aDoc, const QString& filter1,
            const QString& filter2, const QString& filter3 = QString());

    /**
     * Function to convert bitmap to pixmap       
     */
    void fromBitmapAndMaskToPixmapL(CFbsBitmap* fbsBitmap,
            CFbsBitmap* fbsMask, QPixmap& pixmap);

    /**
     * Function to get pixmap       
     */
    void GetPixmapByFilenameL(TDesC& fileName, const QSize &size,
            QPixmap& pixmap);
    
    /**
     * function to load the category translator files       
     */
    void loadTranslator(QString localizationpath);

signals:

    /**
     * Signalled when user selects an to switch the settings state
     * setting state will be  activated.
     */
    void switchProToSettingsState();

    void inDeviceSearchQuery(QString);

    void launchLink(int, QString);

    /**
     * Signalled when UI is ready on progressive state        
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
     * The List View widget.
     * Own.
     */
    HbListWidget* mListView;

    /**
     * The searchpanel widget.
     * Own.
     */
    HbSearchPanel* mSearchPanel;

    /**
     * qt interface for CPix engine
     * Own.
     */
    QList<InDeviceHandler*> mSearchHandlerList;

    InDeviceHandler* mSearchHandler;

    /**
     * Search Keyword                        
     */

    QString mSearchString;

    /**
     * Search Keyword                        
     */

    QString mOriginalString;
    
    /**
     * number of categories selected
     * 
     */
    int mDatabasecount;

    /**
     * index of link item
     */
    int mLinkindex;

    /**
     * number of hits
     */
    int mResultcount;

    /**
     * result parser 
     * 
     */
    int mResultparser;

    /**
     * setting loaded or not variable
     * 
     */
    bool loadSettings;

    
    /**
     * Hbicon to be created 
     * 
     */
    HbIcon mIcon;

    /**
     * to get drive info
     * 
     */
    RFs iFs;

    /**
     * Calendar plugin info
     * 
     */
    EventViewerPluginInterface *calAgandaViewerPluginInstance;

    /**
     * to create Notes editor 
     * 
     */
    NotesEditorInterface *mNotesEditor;

    /**
     * to create Notes plugin loader 
     * 
     */
    QPluginLoader *mNotespluginLoader;

    QMap<int, HbIcon> mISprovidersIcon;

    bool mOnlineQueryAvailable;
    
    QMap<int,bool> mSelectedCategory;

private:
    /**
     * Application manager handler to perform resultitem opening.
     * 
     */
    XQApplicationManager* mAiwMgr;

    /**
     * Request handler to to open resultItems
     * 
     */
    XQAiwRequest* mRequest;

    /**
     * ListView icon Size.
     * 
     */
    QSize mListViewIconSize;

    SearchUiLoader* mUiLoader;

    bool mStateStatus;

    bool mValidateHandlerCreation;
    
    /**
     * to handle exception when searchserver get terminated aburptly.
     * 
     */
    bool mContinuationSearch;

#ifdef OST_TRACE_COMPILER_IN_USE
    QTime m_totalSearchUiTime;
    QTime m_categorySearchUiTime;
    QTime m_categorySearchApiTime;
    QTime m_categoryGetDocumentApiTime;
    QTime m_resultItemLaunchTime;
    //use long to safeguard overflow from long running operations.
    long m_getDocumentCatergoryTimeAccumulator;
#endif

    SEARCH_FRIEND_CLASS (SearchStateProviderTest)

    };

#endif //PROGRESSIVE_SEARCH_STATE_H
