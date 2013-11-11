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


#include <hbdocumentloader.h>

//#include <hbeffect.h>

#include "cxuidocumentloader.h"
#include "cxuistillprecaptureview.h"
#include "cxuipostcaptureview.h"
#include "cxuivideoprecaptureview.h"
#include "cxuiscenemodeview.h"
#include "cxutils.h" //CX_DEBUG_ASSERT
#include "cxuisettingdialog.h"
#include "cxuisettingslider.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuizoomslider.h"
#include "cxeengine.h"
#include "cxuienums.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuidocumentloaderTraces.h"
#endif // OST_TRACE_COMPILER_IN_USE
#endif // Q_OS_SYMBIAN


CxuiDocumentLoader::CxuiDocumentLoader(CxeEngine *engine) : HbDocumentLoader(), mEngine(engine)
{
}

QObject *CxuiDocumentLoader::createObject(const QString& type, const QString &name)
{
    OstTrace0(camerax_performance, CXUIDOCUMENTLOADER_1, "msg: e_CX_DOCUMENTLOADER_CREATEOBJECT 1");
    QObject *object = NULL;

    if (type == "HbView") {
        if (name == CxUiLayout::STILL_PRE_CAPTURE_VIEW) {
            object = new CxuiStillPrecaptureView();
        } else if (name == CxUiLayout::VIDEO_PRE_CAPTURE_VIEW) {
            object = new CxuiVideoPrecaptureView();
        } else if (name == CxUiLayout::POSTCAPTURE_VIEW) {
            object = new CxuiPostcaptureView();
        } else if (name == CxUiLayout::SCENE_MODE_VIEW) {
            object = new CxuiSceneModeView(NULL);
        }
    } else if (type == "HbDialog") {
        if (name == CxUiLayout::SETTINGS_DIALOG_WIDGET
         || name == CxUiLayout::SETTINGS_SLIDER_DIALOG_WIDGET) {
            object = new CxuiSettingDialog();
        }
    } else if (type == "HbSlider") {
        if (name == CxUiLayout::SETTINGS_SLIDER_DIALOG_CONTENT_WIDGET) {
            object = new CxuiSettingSlider(NULL, mEngine);
        } else if (name == CxUiLayout::STILL_PRE_CAPTURE_ZOOM_SLIDER
         || name == CxUiLayout::VIDEO_PRE_CAPTURE_ZOOM_SLIDER) {
            object = new CxuiZoomSlider();
        }
    } else if (type == "HbRadioButtonList") {
        if (name == CxUiLayout::SETTINGS_DIALOG_CONTENT_WIDGET
         || name == CxUiLayout::SCENE_MODE_VIEW_RADIOBUTTONS) {
            object = new CxuiSettingRadioButtonList(NULL, mEngine);
        }
    }

    if (object) {
        object->setObjectName(name);
    } else {
        object = HbDocumentLoader::createObject(type, name);
    }

    OstTrace0(camerax_performance, CXUIDOCUMENTLOADER_2, "msg: e_CX_DOCUMENTLOADER_CREATEOBJECT 0");
    return object;
}





