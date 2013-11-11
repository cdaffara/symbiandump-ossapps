/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QApplication>
#include <QGraphicsProxyWidget>
// needed for localization
#include <QLocale>
#include <HbTranslator>
#include <hbmainwindow.h>
#include <afactivation.h>

#ifdef Q_OS_SYMBIAN
#include <coemain.h>
#include <eikenv.h>
#include <XQSettingsManager>
#endif // Q_OS_SYMBIAN

#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxuiapplication.h"
#include "cxuiviewmanager.h"
#include "cxuiapplicationstate.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "cxuiserviceprovider.h"
#include "cxuidisplaypropertyhandler.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif // OST_TRACE_COMPILER_IN_USE
#endif // Q_OS_SYMBIAN

using namespace Cxe;

// CONSTANTS
const QString TRANSLATIONS_PATH = "/resource/qt/translations/";
const QString TRANSLATIONS_FILE = "camera";

const long int CAMERAX_USE_RASTER_PS_UID = 0x2726beef;
const unsigned long int CAMERAX_USE_RASTER_PS_KEY = 0;

int main(int argc, char *argv[])
{
    CX_DEBUG(("CxUI: entering main()"));
    OstTrace0( camerax_performance, _MAIN, "msg: e_CX_STARTUP 1" );

    Q_INIT_RESOURCE(cxui);

#ifdef Q_OS_SYMBIAN
    XQSettingsManager settingManager;
    XQPublishAndSubscribeSettingsKey useRasterGraphicsSystemKey(
            CAMERAX_USE_RASTER_PS_UID, CAMERAX_USE_RASTER_PS_KEY);
    QVariant useRasterGraphicsSystemValue =
            settingManager.readItemValue(useRasterGraphicsSystemKey,
                                         XQSettingsManager::TypeInt);

    if (useRasterGraphicsSystemValue.isNull()
        || !useRasterGraphicsSystemValue.toInt()) {
        // Either there was an error reading the value or the value was
        // zero. Raster graphics system should be forced
        CX_DEBUG(("CxUI: Take raster graphics system into use"));
        QApplication::setGraphicsSystem("raster");
    }
#endif

    OstTrace0( camerax_performance, DUP1__MAIN, "msg: e_CX_HBAPP_CREATION 1" );
    CxuiApplication app(argc, argv);
    OstTrace0( camerax_performance, DUP2__MAIN, "msg: e_CX_HBAPP_CREATION 0" );

    // Creating and initializing engine as early as possible.
    // Reserve and power on can then proceed in parallel with
    // ui construction.
    OstTrace0( camerax_performance, DUP7__MAIN, "msg: e_CX_CREATE_ENGINE 1" );
    CxeEngine *engine = CxeEngine::createEngine();
    OstTrace0( camerax_performance, DUP8__MAIN, "msg: e_CX_CREATE_ENGINE 0" );

    AfActivation activation;
    if (activation.reason() == Af::ActivationReasonService) {
        CX_DEBUG(("CxUI: Camera started as service"));
        // Embedded mode.  Engine is inited to correct mode
        // by service provider when request arrives
        CX_DEBUG(("CxUI: creating serviceprovider"));
        CxuiServiceProvider::create(engine);
        CX_DEBUG(("CxUI: done"));
    } else if (activation.reason() == Af::ActivationReasonActivity) {
        CX_DEBUG(("CxUI: Camera started as activity"));
        Cxe::CameraMode mode = Cxe::ImageMode;
        QString activityId = activation.name();
        if (activityId == CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY ||
            activityId == CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY) {
            mode = Cxe::VideoMode;
        }

        if (activityId == CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY ||
            activityId == CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY) {
            // init engine only if going to pre-capture
            engine->initMode(mode);
        } else {
            // in post-capture don't init but set the correct mode to engine
            // so init can be done later
            engine->setMode(mode);
        }
    } else {
        CX_DEBUG(("CxUI: Camera started as normal app"));
        // normal start
        engine->initMode(engine->mode());
    }

    // Load language specific application localization file, e.g. "camera_en.qm"
    // Translations need to be loaded before any widgets are created.
    CX_DEBUG(("CxUI: Load translations.."));
    OstTrace0( camerax_performance, DUP3__MAIN, "msg: e_CX_LOAD_TRANSLATIONS 1" );
    HbTranslator translator(TRANSLATIONS_PATH, TRANSLATIONS_FILE);
    // Load language specific common localization file
    translator.loadCommon();
    OstTrace0( camerax_performance, DUP4__MAIN, "msg: e_CX_LOAD_TRANSLATIONS 0" );
    CX_DEBUG(("CxUI: ..translations loaded"));

    // Create main window
    OstTrace0( camerax_performance, DUP5__MAIN, "msg: e_CX_MAINWINDOW_CREATION 1" );
    HbMainWindow *mainWindow = new HbMainWindow(0, Hb::WindowFlagTransparent | Hb::WindowFlagNoBackground);
    mainWindow->setAttribute(Qt::WA_NoBackground);
    OstTrace0( camerax_performance, DUP6__MAIN, "msg: e_CX_MAINWINDOW_CREATION 0" );

    // Set main window to landscape and full screen
    OstTrace0( camerax_performance, DUP13__MAIN, "msg: e_CX_MAINWINDOW_SETORIENTATION 1" );
    mainWindow->setOrientation(Qt::Horizontal);
    OstTrace0( camerax_performance, DUP14__MAIN, "msg: e_CX_MAINWINDOW_SETORIENTATION 0" );
    OstTrace0( camerax_performance, DUP15__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 1" );

#ifdef Q_OS_SYMBIAN
    mainWindow->showFullScreen();
#else
    /*
     * todo : check if this is an Orbit bug or if there's a better solution
    */
    mainWindow->resize(640, 360);
    mainWindow->setOrientation(Qt::Vertical, false);
    mainWindow->show();
    mainWindow->setOrientation(Qt::Horizontal, false);
#endif //Q_OS_SYMBIAN
    OstTrace0( camerax_performance, DUP16__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 0" );

    // Create view manager
    OstTrace0( camerax_performance, DUP11__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 1" );
    CxuiViewManager *viewManager = new CxuiViewManager(app, *mainWindow, *engine);
    OstTrace0( camerax_performance, DUP12__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 0" );

    // Give main window id to engine for setting up viewfinder window behind it
    OstTrace0( camerax_performance, DUP17__MAIN, "msg: e_CX_PREPAREWINDOW 1" );
    engine->viewfinderControl().setWindow(mainWindow->effectiveWinId());
    OstTrace0( camerax_performance, DUP18__MAIN, "msg: e_CX_PREPAREWINDOW 0" );

    // connect display handler to vf state changes
    CxuiDisplayPropertyHandler displayHandler;
    QObject::connect(&engine->viewfinderControl(), SIGNAL(stateChanged(CxeViewfinderControl::State, CxeError::Id)),
                     &displayHandler, SLOT(handleVfStateChanged(CxeViewfinderControl::State, CxeError::Id)));

    int returnValue = app.exec();

    delete viewManager;
	viewManager = NULL;
    CxuiServiceProvider::destroy(); // delete service provider instance
    engine->cameraDeviceControl().release();
    delete mainWindow;
	mainWindow = NULL;
    delete engine;
	engine = NULL;

    return returnValue;
}
