#include "t_searchstateprovider.h"
#include "searchinitstate.h"
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testinitStateConstruction()
    {
    // HbMainWindow* wind = mainWindow();
    SearchInitState* initState = new SearchInitState();

    QVERIFY(initState != NULL);

    delete initState;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testinitStateOnEntryAndExitSignalled()
    {
    // HbMainWindow* wind = mainWindow();

    SearchInitState* initState = new SearchInitState();

    QEvent *event = new QEvent(QEvent::None);
    initState->onEntry(event);

    initState->onExit(event);

    delete initState;
    }
