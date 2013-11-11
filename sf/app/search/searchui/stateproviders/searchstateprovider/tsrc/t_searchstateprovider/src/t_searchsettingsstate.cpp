#include "t_searchstateprovider.h"
#include "searchsettingsstate.h"

// ---------------------------------------------------------------------------
//SearchStateProviderTest::testsettingsStateConstruction()
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testsettingsStateConstruction()
    {
    SearchSettingsState* settingsState = new SearchSettingsState();

    QVERIFY(settingsState != NULL);
    QVERIFY(settingsState->mWidget);
    delete settingsState;
    }
// ---------------------------------------------------------------------------
//SearchStateProviderTest::testsettingsStateOnEntryAndExitSignalled()
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testsettingsStateOnEntryAndExitSignalled()
    {
    SearchSettingsState* settingsState = new SearchSettingsState();

    QEvent *event = new QEvent(QEvent::None);
    settingsState->onEntry(event);

    settingsState->onExit(event);

    delete settingsState;
    }

// ---------------------------------------------------------------------------
//SearchStateProviderTest::testhandleBackEvent()
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testhandleBackEvent()
    {
    SearchSettingsState* settingsState = new SearchSettingsState();

    QSignalSpy spy(settingsState, SIGNAL(clickstatus(bool)));
    settingsState->handleBackEvent(true);
    QCOMPARE(spy.count(), 1);

    delete settingsState;
    }
// ---------------------------------------------------------------------------
//SearchStateProviderTest::testisInternetOn()
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testisInternetOn()
    {
    SearchSettingsState* settingsState = new SearchSettingsState();
    QSignalSpy spy(settingsState,
                SIGNAL(switchToProState()));
    
    settingsState->isInternetOn();    
    
    QCOMPARE(spy.count(), 1);
    delete settingsState;
    }
