#include "t_searchstateprovider.h"
#include "searchprogressivestate.h"
#include "searchuiloader.h"
#include <qsignalspy.h>
#include "indevicehandler.h"
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <fbs.h>
#include <AknsUtils.h>
#include <bitdev.h> 
#include <bitstd.h>
#include <qbitmap.h>
#include <fbs.h>
#include <AknInternalIconUtils.h>
#include <AknIconUtils.h> 
#include <apgcli.h>
#include <hbsearchpanel.h>
#include  <qmap.h>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

void SearchStateProviderTest::testProgressiveStateConstruction()
    {
    // HbMainWindow* wind = mainWindow();

    SearchProgressiveState* progressiveState = new SearchProgressiveState();

    QVERIFY(progressiveState != NULL);
    QVERIFY(progressiveState->mView);
    QVERIFY(progressiveState->mListView);    
    QVERIFY(progressiveState->mSearchPanel);
    //QVERIFY(progressiveState->mSearchHandler);

    delete progressiveState;

    //  delete wind;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

void SearchStateProviderTest::testProgressiveStateOnEntryAndExitSignalled()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();

    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);

    progressiveState->onExit(event);

    delete progressiveState;
    }
void SearchStateProviderTest::testgetAppIconFromAppId()
    {

    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    TRAP_IGNORE(progressiveState->getAppIconFromAppIdL(TUid::Uid(0x2002C377)));
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testonAsyncSearchComplete()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);   
    progressiveState->slotonAsyncSearchComplete(-1,0);
    progressiveState->slotonAsyncSearchComplete(0,0);
    progressiveState->slotonAsyncSearchComplete(0,10);
    QVERIFY(progressiveState->mResultcount);
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testonGetDocumentComplete()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    for(int i = 0;i<progressiveState->mUiLoader->ContentInfoList().count();i++)
    {
    if(progressiveState->mUiLoader->ContentInfoList().at(i)->getBaseApp().contains("root applications"))
        progressiveState->mUiLoader->ContentInfoList().at(i)->setSelected(true);
    }    
    
    progressiveState->onEntry(event);
    
    progressiveState->slotonGetDocumentComplete(0, NULL);
    QCOMPARE(progressiveState->mListView->count(),0);

    progressiveState->slotonGetDocumentComplete(-1, NULL);
    QCOMPARE(progressiveState->mListView->count(),0);
    progressiveState->slotstartNewSearch("contact");
    QTest::qWait(2000);
    int i = progressiveState->mListView->count();
    QVERIFY(progressiveState->mListView->count());
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testopenResultitem()
    {    
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);

    progressiveState->onEntry(event);
    progressiveState->slotstartNewSearch("Creat");
    QTest::qWait(2000);

    //code for getting the model index from model
    /*QModelIndex index = progressiveState->mModel->index(0, 0);
     progressiveState->openResultitem(index);
     QTest::qWait(200);
     progressiveState->startNewSearch("jpg");
     QTest::qWait(200);
     progressiveState->cancelSearch();
     //code for getting the model index from model
     index = progressiveState->mModel->index(0, 0);
     progressiveState->openResultitem(index);
     
     QTest::qWait(200);
     progressiveState->startNewSearch("note");
     QTest::qWait(200);
     progressiveState->cancelSearch();
     //code for getting the model index from model
     index = progressiveState->mModel->index(3, 0);
     progressiveState->openResultitem(index);*/
    delete progressiveState;    
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testsetSettings()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    QSignalSpy spy(progressiveState, SIGNAL(switchProToSettingsState()));
    progressiveState->slotsetSettings();
    QCOMPARE(spy.count(), 1);
    delete progressiveState;
    }
void SearchStateProviderTest::teststartNewSearch()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    for(int i = 0;i<progressiveState->mUiLoader->ContentInfoList().count();i++)
       {
       if(progressiveState->mUiLoader->ContentInfoList().at(i)->getBaseApp().contains("root applications"))
           progressiveState->mUiLoader->ContentInfoList().at(i)->setSelected(true);
       }    
      
    progressiveState->slotstartNewSearch("contact");
    QTest::qWait(2000);
    QVERIFY(progressiveState->mListView->count());
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testsettingsaction()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    for(int i = 0;i<progressiveState->mUiLoader->ContentInfoList().count();i++)
           {
           if(progressiveState->mUiLoader->ContentInfoList().at(i)->getBaseApp().contains("root applications"))
               progressiveState->mUiLoader->ContentInfoList().at(i)->setSelected(true);
           }  
    
    progressiveState->mOriginalString = "contact";
    progressiveState->slotsettingsaction(false);
    QCOMPARE(progressiveState->mResultcount, 0);
    progressiveState->slotsettingsaction(true);
    QTest::qWait(50);
    QCOMPARE(progressiveState->mResultcount, 0);
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
//SearchStateProviderTest::testcancelsearch()
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testcancelSearch()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    for(int i = 0;i<progressiveState->mUiLoader->ContentInfoList().count();i++)
               {
               if(progressiveState->mUiLoader->ContentInfoList().at(i)->getBaseApp().contains("root applications"))
                   progressiveState->mUiLoader->ContentInfoList().at(i)->setSelected(true);
               }  
        
    progressiveState->slotstartNewSearch("contact");
    delete progressiveState;

    }

void SearchStateProviderTest::testhandleOkError()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    QVariant var;
    progressiveState->slothandleOk(var);
    int ret = 0;
    progressiveState->slothandleError(ret, QString());
    delete progressiveState;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testclear()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);

    progressiveState->noResultsFound("aaa");
    QCOMPARE(progressiveState->mListView->count(),1);
    progressiveState->clear();
    QCOMPARE(progressiveState->mListView->count(),0);

    delete progressiveState;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testnoResultsFound()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);

    progressiveState->noResultsFound(NULL);
    QCOMPARE(progressiveState->mListView->count(),0);

    progressiveState->noResultsFound("aaa");
    QCOMPARE(progressiveState->mListView->count(),1);

    delete progressiveState;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testcreateSuggestionLink()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    TRAP_IGNORE(progressiveState->mISprovidersIcon.insert(1, progressiveState->getAppIconFromAppIdL(TUid::Uid(0x2002C377))));
    progressiveState->createSuggestionLink();
    QCOMPARE(progressiveState->mListView->count(), 1);
    delete progressiveState;
    }
void SearchStateProviderTest::testTDisplayMode2Format()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    TDisplayMode mode = EGray2;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor256;
    progressiveState->TDisplayMode2Format(mode);
    mode = EGray256;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor4K;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor64K;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor16M;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor16MU;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor16MA;
    progressiveState->TDisplayMode2Format(mode);
    mode = EColor16MAP;
    progressiveState->TDisplayMode2Format(mode);
    mode = ENone;
    progressiveState->TDisplayMode2Format(mode);

    delete progressiveState;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testgetDrivefromMediaId()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->mSelectedCategory.insert(1, false);
    progressiveState->mSelectedCategory.insert(3, false);
    progressiveState->mSelectedCategory.insert(4, false);
    progressiveState->mSelectedCategory.insert(5, false);
    progressiveState->mSelectedCategory.insert(6, false);
    progressiveState->mSelectedCategory.insert(7, false);
    
    progressiveState->mSelectedCategory.insert(2, true);
    progressiveState->onEntry(event);
    progressiveState->slotstartNewSearch("3gpp");
    QTest::qWait(200);  
    
    HbListWidgetItem* item = progressiveState->mListView->item(0);
    if(item)
    QString uid = progressiveState->getDrivefromMediaId(item->data(
            Qt::UserRole + 2).toString());
    //QVERIFY(uid.length());
    delete progressiveState;
    }
void SearchStateProviderTest::testLaunchApplication()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->onEntry(event);
    // progressiveState->LaunchApplicationL(TUid::Uid(0x20011383));
    delete progressiveState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testfilterDoc()
    {
    SearchProgressiveState* progressiveState = new SearchProgressiveState();
    QEvent *event = new QEvent(QEvent::None);
    progressiveState->mSelectedCategory.insert(5, true);
    progressiveState->onEntry(event);
    progressiveState->slotstartNewSearch("d");
    QTest::qWait(200);
    delete progressiveState;
    }
