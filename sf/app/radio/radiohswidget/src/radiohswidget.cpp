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
* Description:  FM Radio home screen widget
*
*/

// System includes
#include <HbPushButton>
#include <HbLabel>
#include <HbDocumentLoader>
#include <HbFrameDrawer>
#include <HbIcon>
#include <HbIconAnimationManager>
#include <HbIconAnimationDefinition>
#include <HbColorScheme>
#include <HbDeviceMessageBox>
#include <QGraphicsLinearLayout>
#include <QGraphicsItem>
#include <QDesktopServices>
#include <QSignalMapper>

// User includes
#include "radiohswidget.h"
#include "radiohswidgetprofilereader.h"
#include "radiohswidgetradioserviceclient.h"
#include "radioservicedef.h"
#include "radio_global.h"
#include "radiologger.h"

// Constants
/** Path to docml file */
const QString DOCML(":/ui/resource/fmradiohswidget.docml");

/**  DOCML object name for mainLayout */
const QString DOCML_OBJECT_NAME_MAIN_LAYOUT("mainLayout");
/**  DOCML object name for contentLayout */
const QString DOCML_OBJECT_NAME_CONTENT_LAYOUT("contentLayout");
/**  DOCML object name for tunerBackgroundPushButton */
const QString DOCML_OBJECT_NAME_TUNER_BACKGROUND_BUTTON(
    "tunerBackgroundPushButton");
/**  DOCML object name for tunerStackedLayout */
const QString DOCML_OBJECT_NAME_TUNER_STACKED_LAYOUT("tunerStackedLayout");
/**  DOCML object name for tunerInformationStackedLayout */
const QString DOCML_OBJECT_NAME_TUNER_INFORMATION_STACKED_LAYOUT(
    "tunerInformationStackedLayout");
/**  DOCML object name for controlButtons */
const QString DOCML_OBJECT_NAME_CONTROL_BUTTONS_LAYOUT("controlButtons");
/**  DOCML object name for powerToggleButton */
const QString DOCML_OBJECT_NAME_POWER_BUTTON("powerButton");
/**  DOCML object name for previousPushButton */
const QString DOCML_OBJECT_NAME_PREVIOUS_BUTTON("previousPushButton");
/**  DOCML object name for nextPushButton */
const QString DOCML_OBJECT_NAME_NEXT_BUTTON("nextPushButton");
/**  DOCML object name for twoRowsLayout */
const QString DOCML_OBJECT_NAME_TWO_ROWS_LAYOUT("twoRowsLayout");
/**  DOCML object name for firstRowLabel */
const QString DOCML_OBJECT_NAME_FIRST_ROW_LABEL("firstRowLabel");
/**  DOCML object name for secondRowLabel */
const QString DOCML_OBJECT_NAME_SECOND_ROW_LABEL("secondRowLabel");
/**  DOCML object name for lonelyRowLabel */
const QString DOCML_OBJECT_NAME_LONELY_ROW_LABEL("lonelyRowLabel");
/**  DOCML object name for animationIcon */
const QString DOCML_OBJECT_NAME_ANIMATION_ICON("animationIcon");

/** Unknown favorite station count. */
const int FAVORITE_STATION_COUNT_UNDEFINED(0);
/** Unknown local station count. */
const int LOCAL_STATION_COUNT_UNDEFINED(0);
/** Favorite station count lower boundary including this number. */
const int FAVORITE_STATION_COUNT_LOWER_BOUNDARY(0);
/** Favorite station count upper boundary including this number. */
const int FAVORITE_STATION_COUNT_UPPER_BOUNDARY(100);

// Graphics identifiers for different push button states
const QString CONTROL_BUTTON_GRAPHICS_NORMAL  ("qtg_fr_hsbutton_normal");
const QString CONTROL_BUTTON_GRAPHICS_PRESSED ("qtg_fr_hsbutton_pressed");
const QString CONTROL_BUTTON_GRAPHICS_DISABLED("qtg_fr_hsbutton_disabled");
const QString CONTROL_BUTTON_GRAPHICS_LATCHED ("qtg_fr_hsbutton_latched");

// Push button icon colors for each of the states (normal, pressed, disabled
// and latched)
const QString CONTROL_BUTTON_ICON_COLOR_NORMAL  ("qtc_button_normal");
const QString CONTROL_BUTTON_ICON_COLOR_PRESSED ("qtc_button_pressed");
const QString CONTROL_BUTTON_ICON_COLOR_DISABLED("qtc_button_disabled");
const QString CONTROL_BUTTON_ICON_COLOR_LATCHED ("qtc_button_latched");

// File name suffix lists for push buttons
const QStringList POWER_BUTTON_SUFFIX(
    (QStringList() << "_l" << "_c" << "_cr"));
const QStringList PREVIOUS_BUTTON_SUFFIX(
    (QStringList() << "_cl" << "_c" << "_cr"));
const QStringList NEXT_BUTTON_SUFFIX(
    (QStringList() << "_cl" << "_c" << "_r"));

/** Icon for power button off. */
const QString POWER_BUTTON_ICON_OFF("qtg_mono_power"); 
/** Icon for power button on. */
const QString POWER_BUTTON_ICON_ON ("qtg_mono_power"); 

// Tuner background button graphics for different states.
const QString TUNER_BUTTON_NORMAL_OFF("qtg_fr_tuner");
const QString TUNER_BUTTON_NORMAL_ON("qtg_fr_tuner");
const QString TUNER_BUTTON_NORMAL_PRESSED("qtg_fr_hsitems2_pressed");

/*!
    \class RadioHsWidget
    \brief Implementation of FM Radio home screen widget.

    RadioHsWidget implements needed functions for the FM Radio home screen
    widget.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructs a widget which is a child of \a parent, with widget flags set
    to \a flags.
    
    Constructor should be empty and all the actual construction should be
    done in onInitialize().
 */
RadioHsWidget::RadioHsWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mInformationAreaBackgroundButton(NULL),
      mPowerButton(NULL),
      mPreviousButton(NULL),
      mNextButton(NULL),
      mInformationAreaTwoRowsLayout(NULL),
      mInformationLonelyRowLabel(NULL),
      mInformationFirstRowLabel(NULL),
      mInformationSecondRowLabel(NULL),
      mAnimationIcon(NULL),
      mFmRadioState(FmRadio::StateUndefined),
      mFavoriteStationCount(FAVORITE_STATION_COUNT_UNDEFINED),
      mLocalStationCount(LOCAL_STATION_COUNT_UNDEFINED),
      mCurrentStationIsFavorite(false),
      mProfileMonitor(NULL),
      mRadioServiceClient(NULL)
{
    LOG_METHOD;
}

/*!
    Destructor
 */
RadioHsWidget::~RadioHsWidget()
{
    LOG_METHOD;
}

/*!
    Handles changes in FM Radio information.

    \param informationType Type of changed information.
    \param information Actual information.
 */
void RadioHsWidget::handleRadioInformationChange(const int informationType,
    const QVariant &information)
{
    LOG_METHOD;
    switch (informationType) {

    case RadioServiceNotification::FavoriteCount:
        LOG("FavoriteCount");
        if (information.canConvert(QVariant::Int) &&
            information.toInt() >= FAVORITE_STATION_COUNT_LOWER_BOUNDARY &&
            information.toInt() <= FAVORITE_STATION_COUNT_UPPER_BOUNDARY) {
            mFavoriteStationCount = information.toInt();
            LOG_FORMAT("mFavoriteStationCount: %d", mFavoriteStationCount);
            enableStationButtons();
        }
        break;

    case RadioServiceNotification::LocalCount:
        LOG("LocalCount");
        if (information.canConvert(QVariant::Int) &&
            information.toInt() >= FAVORITE_STATION_COUNT_LOWER_BOUNDARY &&
            information.toInt() <= FAVORITE_STATION_COUNT_UPPER_BOUNDARY) {
            mLocalStationCount = information.toInt();
            // If there are local stations, enable the next/previous
            // buttons.
            LOG_FORMAT("mLocalStationCount: %d", mLocalStationCount);
            // Enable or disable buttons only if localCount differs
            // from 1.
            enableStationButtons();
        }
        break;

    case RadioServiceNotification::CurrentIsFavorite:
        LOG("CurrentIsFavorite");
        if (information.canConvert(QVariant::Bool)) {
            mCurrentStationIsFavorite = information.toBool();
            LOG_FORMAT("currentIsFavorite: %d", mCurrentStationIsFavorite);
        }
        break;

    case RadioServiceNotification::RadioStatus:
        LOG("RadioStatus");
        if (information.canConvert(QVariant::Int)) {
            const int status = information.toInt();
            switch (status) {
            case RadioStatus::Playing:
                LOG("Playing");
                handleRadioStateChange(FmRadio::StateRunning);
                break;
            case RadioStatus::Muted:
                LOG("Muted");
                break;
            case RadioStatus::Seeking:
                LEVEL2(LOG("Seeking"));
                handleRadioStateChange(FmRadio::StateSeeking);
                break;
            case RadioStatus::NoAntenna:
                LEVEL2(LOG("NoAntenna"));
                handleRadioStateChange(FmRadio::StateAntennaNotConnected);
                break;
            case RadioStatus::PoweringOff:
                LEVEL2(LOG("PoweringOff"));
                handleRadioStateChange(FmRadio::StateClosing);
                break;
            default:
                LOG("default case at case RadioStatus");
                break;
            }
        }
        break;

    case RadioServiceNotification::Frequency:
        LOG("Frequency");
        // TODO: Should information.toString() be checked for too many characters? What's the limit?
        if (information.canConvert(QVariant::String)) {
            LOG_FORMAT("frequency: %s", GETSTRING(information.toString()));
            // TODO: Remove comment when localisation is working on device.
            //frequencyString = hbTrId("txt_fmradiohswidget_rad_list_l1_mhz").arg(freqString);
            bool frequencyCleared = false;

            if (mRadioInformation.contains(Frequency)) {
                // Clear all infromation.
                clearRadioInformation();
                frequencyCleared = true;
            }
            // If widget do not have any frquency information, update it.
            bool frequencyUpdated = updateRadioInformation(Frequency,
                information.toString());
            if (frequencyCleared || frequencyUpdated) {
                // Information changed, update the UI.
                changeInRadioInformation();
                mFmRadioState = FmRadio::StateRunning;
            }
        }
        break;

    case RadioServiceNotification::Name:
        LOG("Name");
        handleSimilarRadioInformation(StationName, information);
        break;

    case RadioServiceNotification::Genre:
        LOG("Genre");
        handleSimilarRadioInformation(Pty, information);
        break;

    case RadioServiceNotification::RadioText:
        LOG("RadioText");
        handleSimilarRadioInformation(RadioText, information);
        break;

    case RadioServiceNotification::DynamicPS:
        LOG("DynamicPS");
        handleSimilarRadioInformation(DynamicPsName, information);
        break;

    default:
        LOG("default case at notificationId");
        break;
    }
}

/*!
    Handles changes in FM Radio state.

    \param value New state of the radio application.
*/
void RadioHsWidget::handleRadioStateChange(const QVariant &value)
{
    LOG_METHOD;
    int state;
    if (value.canConvert(QVariant::Int)) {
        state = value.toInt();
    } else {
        return;
    }

    if (state == mFmRadioState) {
        // State did not change, so return.
        return;
    }

    switch (state) {
    case FmRadio::StateUndefined:
        LOG("FmRadio::StateUndefined");
        // Something went wrong. Widget should not be in this state after onInitialize().
        mFmRadioState = FmRadio::StateUndefined;
        break;
    case FmRadio::StateNotRunning:
        LOG("FmRadio::StateNotRunning");
        mFmRadioState = FmRadio::StateNotRunning;
        mRadioServiceClient->stopMonitoring();
        changePowerButtonOn(false);
        mFavoriteStationCount = FAVORITE_STATION_COUNT_UNDEFINED;
        mLocalStationCount = LOCAL_STATION_COUNT_UNDEFINED;
        mCurrentStationIsFavorite = false;
        enableStationButtons();
        clearRadioInformation();
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId("txt_fmradiohswidget_rad_list_fm_radio"));
        changeInformationAreaLayout(OneRow);
        break;
    case FmRadio::StateStarting:
        LOG("FmRadio::StateStarting");
        mFmRadioState = FmRadio::StateStarting;
        changePowerButtonOn(true);
        mFavoriteStationCount = FAVORITE_STATION_COUNT_UNDEFINED;
        mLocalStationCount = LOCAL_STATION_COUNT_UNDEFINED;
        mCurrentStationIsFavorite = false;
        enableStationButtons();
        changeInformationAreaLayout(Animation);
        break;
    case FmRadio::StateRunning:
        LOG("FmRadio::StateRunning");
        mFmRadioState = FmRadio::StateRunning;
        // Stop timer if it is running because radio is now running.
        mRadioServiceClient->startMonitoring(
            FmRadio::VisibiltyDoNotChange);
        changeInRadioInformation();
        changePowerButtonOn(true);
        enableStationButtons();
        changeInformationAreaLayout(OneRow);
        break;
    case FmRadio::StateSeeking:
        LOG("FmRadio::StateSeeking");
        mFmRadioState = FmRadio::StateSeeking;
        mCurrentStationIsFavorite = false;
        enableStationButtons();
        changeInformationAreaLayout(Animation);
        break;
    case FmRadio::StateAntennaNotConnected:
        LOG("FmRadio::StateAntennaNotConnected");
        mFmRadioState = FmRadio::StateAntennaNotConnected;
        mCurrentStationIsFavorite = false;
        enableStationButtons();
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId(
            "txt_fmradiohswidget_rad_info_connect_wired_headset"));
        changeInformationAreaLayout(OneRow);
        break;
    case FmRadio::StateClosing:
        LOG("FmRadio::StateClosing");
        mFmRadioState = FmRadio::StateClosing;
        changePowerButtonOn(false);
        mFavoriteStationCount = FAVORITE_STATION_COUNT_UNDEFINED;
        mLocalStationCount = LOCAL_STATION_COUNT_UNDEFINED;
        mCurrentStationIsFavorite = false;
        enableStationButtons();
        clearRadioInformation();
        mInformationFirstRowLabel->setPlainText("");
        mInformationSecondRowLabel->setPlainText("");
        mInformationLonelyRowLabel->setPlainText(hbTrId(
            "txt_fmradiohswidget_rad_list_fm_radio"));
        changeInformationAreaLayout(OneRow);
        break;
    default:
        LOG_FORMAT("default case at state. State: %d", state);
        break;
    }
}

/*!
    Called when widget is initialized. Constructs objects and connects them.
*/
void RadioHsWidget::onInitialize()
{
    LOG_METHOD_ENTER;
    mProfileMonitor = new RadioHsWidgetProfileReader(this);
    mRadioServiceClient = new RadioHsWidgetRadioServiceClient(this);
    
    load(DOCML);
    
    // Use signal mapper to indicate button identifiers to button event
    // slots. 
    QSignalMapper* signalMapperPressed = new QSignalMapper(this); 
    signalMapperPressed->setMapping(mPowerButton, Power);
    signalMapperPressed->setMapping(mPreviousButton, Previous);
    signalMapperPressed->setMapping(mNextButton, Next);

    // Need to use different signal mapper for pressed and released events, 
    // both have same mappings but they are mapped to different slots.
    QSignalMapper* signalMapperReleased = new QSignalMapper(this);
    signalMapperReleased->setMapping(mPowerButton, Power);
    signalMapperReleased->setMapping(mPreviousButton, Previous);
    signalMapperReleased->setMapping(mNextButton, Next);

    // Connect button events to signal maps.
    Radio::connect(mPowerButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    Radio::connect(mPowerButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));
    Radio::connect(mPreviousButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    Radio::connect(mPreviousButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));
    Radio::connect(mNextButton, SIGNAL(pressed()), signalMapperPressed, SLOT (map()));
    Radio::connect(mNextButton, SIGNAL(released()), signalMapperReleased, SLOT (map()));
    
    // Connect mapper signals to self implemented slots.
    Radio::connect(signalMapperPressed, SIGNAL(mapped(int)), this, SLOT(changeButtonToPressed(int)));
    Radio::connect(signalMapperReleased, SIGNAL(mapped(int)), this, SLOT(changeButtonToReleased(int)));
    
    mProfileMonitor->startMonitoringRadioRunningStatus();
}

/*!
    Called when widget is shown in the home screen
*/
void RadioHsWidget::onShow()
{
    LOG_METHOD_ENTER;
}

/*!
    Called when widget is hidden from the home screen
*/
void RadioHsWidget::onHide()
{
    LOG_METHOD_ENTER;
}

/*!
    Emited from HbPushButton:pressed() signal, changes the button layout to
    pressed state.
 
    \param hsButtonIdentifier Identifies the button which was pressed.
 */
void RadioHsWidget::changeButtonToPressed(int hsButtonIdentifier)
{
    LEVEL2(LOG_METHOD);
    buttonEvent(static_cast<ControlButtonIdentifier>(hsButtonIdentifier), Pressed);
}

/*!
    Emited from HbPushButton:released() signal, changes the button layout to
    normal state.
 
    \param hsButtonIdentifier Identifies the button which was released.
 */
void RadioHsWidget::changeButtonToReleased(int hsButtonIdentifier)
{
    LEVEL2(LOG_METHOD);
    buttonEvent(static_cast<ControlButtonIdentifier>(hsButtonIdentifier), Normal);
}

/*!
    Slot for closing FM Radio application from power button.
 */
void RadioHsWidget::closeRadio()
{
    LOG_SLOT_CALLER;
    mRadioServiceClient->commandFmRadio(RadioServiceCommand::PowerOff);
}

/*!
    Slot for previous button clicked.
 */
void RadioHsWidget::changeToPreviousStation()
{
    LOG_SLOT_CALLER;
    clearRadioInformation();
    if (mFavoriteStationCount > 1) {
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::PreviousFavorite);
    }
    else {
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::Previous);
    }
}

/*!
    Slot for next button clicked.
 */
void RadioHsWidget::changeToNextStation()
{
    LOG_SLOT_CALLER;
    clearRadioInformation();
    if (mFavoriteStationCount > 1) {
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::NextFavorite);
    }
    else {
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::Next);
    }

}

/*!
    Slot for bringing the radio application to foreground.
 */
void RadioHsWidget::changeRadioToForeground()
{
    LOG_SLOT_CALLER;
    // If radio is not running start it to foreground by monitor request.
    if (mFmRadioState == FmRadio::StateNotRunning) {
        bool okToStartRadio = radioStartPermission();
        if (okToStartRadio) {
            handleRadioStateChange(FmRadio::StateStarting);
            mRadioServiceClient->startMonitoring(
                FmRadio::VisibiltyToForeground);
        }
    }
    else {
        if (mFmRadioState == FmRadio::StateClosing) {
            // Radio is closing but user wants to power it up again.
            mRadioServiceClient->commandFmRadio(RadioServiceCommand::PowerOn);
            // Stop and start monitoring to get refresh.
            mRadioServiceClient->stopMonitoring();
            mRadioServiceClient->startMonitoring(
                FmRadio::VisibiltyToBackground);
            handleRadioStateChange(FmRadio::StateRunning);
        }
        // If radio is running, bring it to the foreground.
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::Foreground);
    }
}

/*!
    Slot for putting the radio application to the background.
 */
void RadioHsWidget::changeRadioToBackground()
{
    LOG_SLOT_CALLER;
    // If radio is not running start it to background by monitor request.
    if (mFmRadioState == FmRadio::StateNotRunning) {
        bool okToStartRadio = radioStartPermission();
        if (okToStartRadio) {
            handleRadioStateChange(FmRadio::StateStarting);
            mRadioServiceClient->startMonitoring(
                FmRadio::VisibiltyToBackground);
        }
    }
    else if (mFmRadioState == FmRadio::StateStarting) {
        // Do nothing if radio is starting.
    }
    else if (mFmRadioState == FmRadio::StateClosing) {
        // Radio is closing but user wants to power it up again.
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::PowerOn);
        // Stop and start monitoring to get refresh.
        mRadioServiceClient->stopMonitoring();
        mRadioServiceClient->startMonitoring(
            FmRadio::VisibiltyToBackground);
        handleRadioStateChange(FmRadio::StateRunning);
    }
    else {
        // If radio is running, put it to the background.
        // This is little bit useless because the radio is in background if
        // user is able to click the widget.
        mRadioServiceClient->commandFmRadio(RadioServiceCommand::Background);
    }
}

/*!
    Powering off or on the radio.
 */
void RadioHsWidget::toggleRadioPower()
{
    LOG_SLOT_CALLER;
    // If radio is not running start it to background by monitor request.
    if (mFmRadioState == FmRadio::StateNotRunning || mFmRadioState == FmRadio::StateClosing) {
        LEVEL2(LOG("Power on"));
        // Start radio
        changeRadioToBackground();
    } else {
        LEVEL2(LOG("Power off"));
        // Close radio
        closeRadio();
    }
}

/*!
    Loads docml file.
 
    \param docml Docml filename to be loaded.
 */
void RadioHsWidget::load(const QString &docml)
{
    LOG_METHOD_ENTER;

    QScopedPointer<HbDocumentLoader> documentLoader(new HbDocumentLoader());
    bool loaded = false;
    documentLoader->load(docml, &loaded);
    if (loaded) {
        // Find mainLayout
        HbWidget *mainLayout = qobject_cast<HbWidget*> (
            documentLoader->findWidget(DOCML_OBJECT_NAME_MAIN_LAYOUT));

        if (mainLayout) {
            QGraphicsLinearLayout *widgetLayout = new QGraphicsLinearLayout(
                Qt::Vertical, this);
            widgetLayout->addItem(mainLayout);
            setLayout(widgetLayout);
        }

        // Find contentLayout
        HbWidget *contentLayout = qobject_cast<HbWidget*> (
            documentLoader->findWidget(DOCML_OBJECT_NAME_CONTENT_LAYOUT));
        if (contentLayout) {

            // Find stacked layout for tuner area.
            HbWidget *tunerStackedLayout = qobject_cast<HbWidget*> (
                documentLoader->findWidget(
                    DOCML_OBJECT_NAME_TUNER_STACKED_LAYOUT));
            if (tunerStackedLayout) {

                // Find stacked layout for information area.
                HbWidget *tunerInformationStackedLayout = qobject_cast<
                    HbWidget*> (documentLoader->findWidget(
                    DOCML_OBJECT_NAME_TUNER_INFORMATION_STACKED_LAYOUT));
                if (tunerInformationStackedLayout) {
                }

                // Find lonely label
                mInformationLonelyRowLabel = qobject_cast<HbLabel *> (
                    documentLoader->findWidget(
                        DOCML_OBJECT_NAME_LONELY_ROW_LABEL));
                if (mInformationLonelyRowLabel) {
                    // TODO: Set the color in docml when application designer supports it.
                    QColor color = HbColorScheme::color(
                        "qtc_radio_tuner_normal");
                    mInformationLonelyRowLabel->setTextColor(color);
                }

                // Find layout for two rows
                mInformationAreaTwoRowsLayout = qobject_cast<
                    QGraphicsWidget *> (documentLoader->findObject(
                    DOCML_OBJECT_NAME_TWO_ROWS_LAYOUT));
                if (mInformationAreaTwoRowsLayout) {
                    // Find first row
                    mInformationFirstRowLabel = qobject_cast<HbLabel *> (
                        documentLoader->findWidget(
                            DOCML_OBJECT_NAME_FIRST_ROW_LABEL));
                    if (mInformationFirstRowLabel) {
                        // TODO: Set the color in docml when application designer supports it.
                        QColor color = HbColorScheme::color(
                            "qtc_radio_tuner_normal");
                        mInformationFirstRowLabel->setTextColor(color);
                    }

                    // Find second row
                    mInformationSecondRowLabel = qobject_cast<HbLabel *> (
                        documentLoader->findWidget(
                            DOCML_OBJECT_NAME_SECOND_ROW_LABEL));
                    if (mInformationSecondRowLabel) {
                        // TODO: Set the color in docml when application designer supports it.
                        QColor color = HbColorScheme::color(
                            "qtc_radio_tuner_normal");
                        mInformationSecondRowLabel->setTextColor(color);
                    }
                }

                mAnimationIcon = qobject_cast<HbLabel *> (
                    documentLoader->findWidget(
                        DOCML_OBJECT_NAME_ANIMATION_ICON));
                if (mAnimationIcon) {
                    // Use animation manager to access anim loading animation.
                    HbIconAnimationManager *animationManager =
                        HbIconAnimationManager::global();
                    // TODO: Axml extension can be removed after wk24 release.
                    animationManager->addDefinitionFile(QLatin1String(
                        "qtg_anim_loading.axml"));
                    mAnimationIcon->setIcon(HbIcon("qtg_anim_loading"));
                }
            }

            // Find push button for tuner area.
            mInformationAreaBackgroundButton = qobject_cast<HbPushButton*> (
                documentLoader->findWidget(
                    DOCML_OBJECT_NAME_TUNER_BACKGROUND_BUTTON));
            if (mInformationAreaBackgroundButton) {
                // Use the frame background.
                HbFrameDrawer *tunerBackgroundButtonFrameDrawer =
                    new HbFrameDrawer("qtg_fr_tuner",
                        HbFrameDrawer::ThreePiecesHorizontal);
                tunerBackgroundButtonFrameDrawer->setFillWholeRect(true);
                mInformationAreaBackgroundButton->setFrameBackground(
                    tunerBackgroundButtonFrameDrawer);
                // Connect the button's clicked signal. 
                Radio::connect(mInformationAreaBackgroundButton,
                    SIGNAL(clicked()), this, SLOT(changeRadioToForeground()));
            }

            // Find layout for control buttons.
            HbWidget *controlButtonsLayout = qobject_cast<HbWidget*> (
                documentLoader->findWidget(
                    DOCML_OBJECT_NAME_CONTROL_BUTTONS_LAYOUT));
            if (controlButtonsLayout) {

                // Find power button.
                mPowerButton
                    = qobject_cast<HbPushButton *> (
                        documentLoader->findWidget(
                            DOCML_OBJECT_NAME_POWER_BUTTON));
                if (mPowerButton) {
                    defineButton(*mPowerButton,
                        CONTROL_BUTTON_GRAPHICS_NORMAL, POWER_BUTTON_SUFFIX,
                        POWER_BUTTON_ICON_ON,
                        CONTROL_BUTTON_ICON_COLOR_NORMAL);
                    // Connect the button's clicked signal.
                    Radio::connect(mPowerButton, SIGNAL(clicked()),
                                   this,         SLOT(toggleRadioPower()));
                }

                // Find previous button.
                mPreviousButton = qobject_cast<HbPushButton *> (
                    documentLoader->findWidget(
                        DOCML_OBJECT_NAME_PREVIOUS_BUTTON));
                if (mPreviousButton) {
                    defineButton(*mPreviousButton,
                        CONTROL_BUTTON_GRAPHICS_DISABLED,
                        PREVIOUS_BUTTON_SUFFIX, NULL,
                        CONTROL_BUTTON_ICON_COLOR_DISABLED);
                    // Connect the button's clicked signal.
                    Radio::connect(mPreviousButton, SIGNAL(clicked()),
                                   this,            SLOT(changeToPreviousStation()));
                }

                // Find next button.
                mNextButton
                    = qobject_cast<HbPushButton *> (
                        documentLoader->findWidget(
                            DOCML_OBJECT_NAME_NEXT_BUTTON));
                if (mNextButton) {
                    defineButton(*mNextButton,
                        CONTROL_BUTTON_GRAPHICS_DISABLED, NEXT_BUTTON_SUFFIX,
                        NULL, CONTROL_BUTTON_ICON_COLOR_DISABLED);
                    // Connect the button's clicked signal.
                    Radio::connect(mNextButton, SIGNAL(clicked()),
                                   this,        SLOT(changeToNextStation()));
                }
            }
        }
    }
    else {
        // Docml was not succesfully loaded. UI cannot be displayed.
        // Emit error to home screen framework, which removes the widget. 
        emit error();
    }
}

/*!
    Groups handling of similar kind of radio information (textual) to one
    function.

    \param informationType Type of changed information.
    \param information Actual textual information.
 */
void RadioHsWidget::handleSimilarRadioInformation(
    const FmRadioInformationType informationType, const QVariant &information)
{
    LOG_METHOD_ENTER;
    // TODO: Should information.toString() be checked for too many characters? What's the limit?
    if (information.canConvert(QVariant::String) && updateRadioInformation(
        informationType, information.toString())) {
        LOG_FORMAT("informationType: %d, information: %s", informationType, GETSTRING(information.toString()));
        changeInRadioInformation();
    }
}

/*!
    Check if the the radio information is changed. If it is changed, it is
    also updated.

    \param informationType Type of the information.
    \param information  Information text.

    \returns Returns \c true, if information is updated. Returns \c false otherwise.
 */
bool RadioHsWidget::updateRadioInformation(const FmRadioInformationType informationType,
    const QString &information)
{
    LOG_METHOD_RET("%d");
    // If hash contains this type of information.
    if (mRadioInformation.contains(informationType)) {
        // If new information is empty.
        if (information.isEmpty()) {
            // Remove old information from the hash.
            LEVEL2(LOG_FORMAT("informationType: %s removed", GETSTRING(informationType)));
            mRadioInformation.remove(informationType);
            // Return true to indicate the change.
            return true;
        }
        // If new information differs from the old one.
        if (mRadioInformation[informationType].compare(information) != 0) {
            // Update the information.
            LEVEL2(LOG_FORMAT("informationType: %s = %s", GETSTRING(informationType), GETSTRING(information)));
            mRadioInformation[informationType] = information;
            // And return true to indicate the change.
            return true;
        }
    } else { // Hash do not contain this type of information.
        // If new information is not empty.
        if (!information.isEmpty()) {
            // Add it to the hash.
            LEVEL2(LOG_FORMAT("informationType: %s = %s", GETSTRING(informationType), GETSTRING(information)));
            mRadioInformation[informationType] = information;
            // Return true to indicate the change.
            return true;
        }
    }
    // Return false to indicate that nothing changed.
    return false;
}

/*!
    Formatting radio information texts after a change.
 */
void RadioHsWidget::changeInRadioInformation()
{
    LOG_METHOD_ENTER;
    // Clear the rows.
    mRadioInformationFirstRow.clear();
    mRadioInformationSecondRow.clear();

    // First row contains station name.
    if (mRadioInformation.contains(StationName)) {
        mRadioInformationFirstRow.append(mRadioInformation.value(StationName));
    }
    else if (mRadioInformation.contains(Frequency)) {
        // Or frequency.
        mRadioInformationFirstRow.append(mRadioInformation.value(Frequency));
    } LEVEL2(LOG_FORMAT("mRadioInformationFirstRow: %s", GETSTRING(mRadioInformationFirstRow)));

    // Second row of information contains radio text.
    if (mRadioInformation.contains(RadioText)) {
        mRadioInformationSecondRow.append(mRadioInformation.value(RadioText));
    }
    else if (mRadioInformation.contains(DynamicPsName)) {
        // Or Dynamic PS name.
        mRadioInformationSecondRow.append(mRadioInformation.value(DynamicPsName));
    }
    else if (mRadioInformation.contains(Pty)) {
        // Or PTY.
        mRadioInformationSecondRow.append(mRadioInformation.value(Pty));
    }
    LEVEL2(LOG_FORMAT("mRadioInformationSecondRow: %s", GETSTRING(mRadioInformationSecondRow)));
    
    // If second row is empty.
    if (mRadioInformationSecondRow.isEmpty()) {
        // Show only the lonely row.
        mInformationLonelyRowLabel->setPlainText(mRadioInformationFirstRow);
        changeInformationAreaLayout(OneRow);
    }
    else {
        // Else display both rows.
        mInformationFirstRowLabel->setPlainText(mRadioInformationFirstRow);
        mInformationSecondRowLabel->setPlainText(mRadioInformationSecondRow);
        changeInformationAreaLayout(TwoRows);
    }
}

/*!
    Clears the radio station information. For example, when the station is
    changed, old information should be cleared.
 */
void RadioHsWidget::clearRadioInformation()
{
    LOG_METHOD_ENTER;
    mRadioInformation.clear();
}

/*!
    Changes visible widgets of information area stacked layout.

    \param layout The layout to switch visible.
 */
void RadioHsWidget::changeInformationAreaLayout(const InformationAreaLayout layout)
{
    LOG_METHOD_ENTER;
    mInformationLonelyRowLabel->setVisible(layout == OneRow);
    mInformationAreaTwoRowsLayout->setVisible(layout == TwoRows);
    mAnimationIcon->setVisible(layout == Animation);
}

/*!
    Changes state of power button.
    
    \param isPowerOn \c true the power is on and \c false the power is off.
 */
void RadioHsWidget::changePowerButtonOn(const bool isPowerOn)
{
    LEVEL2(LOG_METHOD);
    if (isPowerOn) {
        LEVEL2(LOG("Power on"));
        buttonEvent(Power, Latched);
    } else {
        LEVEL2(LOG("Power off"));
        buttonEvent(Power, Normal);
    }
}

/*!
    Changes enabled state of station buttons.
 */
void RadioHsWidget::enableStationButtons()
{
    LEVEL2(LOG_METHOD_ENTER);
    LOG_FORMAT("RadioHsWidget::enableStationButtons count: %d", mLocalStationCount);
    if (mFmRadioState == FmRadio::StateAntennaNotConnected){
        changeButtonToDisabled(Next);
        changeButtonToDisabled(Previous);
    }
    else if (mLocalStationCount + mFavoriteStationCount > 1) {
        changeButtonToEnabled(Next);
        changeButtonToEnabled(Previous);
    }
    else {
        changeButtonToDisabled(Next);
        changeButtonToDisabled(Previous);
    }
}

/*!
    Makes homescreen specific push button based on parameters.
 
    \param target Target push button to modify. Must not be NULL.
    \param graphicsId Defines the target button background graphics.
    \param suffix Defines the suffix for the target button background graphics.
    \param icon Defines the icon for the target button. 
    \param iconColor Defines the icon color for the target button. 
 */
void RadioHsWidget::defineButton(HbPushButton &target, const QString &graphicsId,
    const QStringList &suffix, const QString &icon, const QString &iconColor)
{
    LEVEL2(LOG_METHOD);
    HbFrameDrawer* drawer = NULL;

    // First check if the drawer is already created for this push button
    if (!target.frameBackground()) {
        LEVEL2(LOG("Creating new frame background."));
        // Nope, create one now
        drawer = new HbFrameDrawer(graphicsId,
            HbFrameDrawer::ThreePiecesHorizontal);
        target.setFrameBackground(drawer);
    }
    else {
        // Frame drawer already created, only need to update frame graphics
        drawer = target.frameBackground();
        drawer->setFrameGraphicsName(graphicsId);
    }

    // Set file name suffix list, so that drawer can load correct 3-piece graphic files 
    drawer->setFileNameSuffixList(suffix);
    
    // Set the icon, if it is not NULL
    if (!icon.isNull()) {
        target.setIcon(HbIcon(icon));
    }

    // Update also the icon color
    QColor color = HbColorScheme::color(iconColor);
    target.icon().setColor(color);

    // Lastly, check if the buttton is disabled
    if (iconColor == CONTROL_BUTTON_ICON_COLOR_DISABLED) {
        target.setEnabled(false);
    }
    else {
        target.setEnabled(true);
    }
}

/*!
    Prepares the information needed for displaying the button correctly
    reflecting its state.
 
    \param buttonId Identifies the button.
    \param state Tells in what state the button is.
 */
void RadioHsWidget::buttonEvent(ControlButtonIdentifier buttonId,
    const ControlButtonState state)
{
    LEVEL2(LOG_METHOD);
    HbPushButton* target = NULL;
    QStringList suffix;
    QString icon;

    switch (buttonId) {
    case Power:
        LEVEL2(LOG("Power"));
        target = mPowerButton;
        icon = POWER_BUTTON_ICON_ON;
        suffix = POWER_BUTTON_SUFFIX;
        break;
    case Previous:
        LEVEL2(LOG("Previous"));
        target = mPreviousButton;
        suffix = PREVIOUS_BUTTON_SUFFIX;
        break;
    case Next:
        LEVEL2(LOG("Next"));
        target = mNextButton;
        suffix = NEXT_BUTTON_SUFFIX;
        break;
    default:
        LOG("default case at buttonId");
        break;
    }
    
    QString buttonBackgroundGraphics;
    QString buttonIconColors;
    switch (state) {
    case Normal:
        LEVEL2(LOG("Normal"));
        target->setProperty("state", "normal");
        buttonBackgroundGraphics = CONTROL_BUTTON_GRAPHICS_NORMAL;
        buttonIconColors = CONTROL_BUTTON_ICON_COLOR_NORMAL;
        break;
    case Pressed:
        LEVEL2(LOG("Pressed"));
        target->setProperty("state", "pressed");
        buttonBackgroundGraphics = CONTROL_BUTTON_GRAPHICS_PRESSED;
        buttonIconColors = CONTROL_BUTTON_ICON_COLOR_PRESSED;
        break;
    case Disabled:
        LEVEL2(LOG("Disabled"));
        target->setProperty("state", "disabled");
        buttonBackgroundGraphics = CONTROL_BUTTON_GRAPHICS_DISABLED;
        buttonIconColors = CONTROL_BUTTON_ICON_COLOR_DISABLED;
        break;
    case Latched:
        LEVEL2(LOG("Latched"));
        target->setProperty("state", "latched");
        buttonBackgroundGraphics = CONTROL_BUTTON_GRAPHICS_LATCHED;
        buttonIconColors = CONTROL_BUTTON_ICON_COLOR_LATCHED;
        break;
    default:
        LOG("default case at button state");
        break;
    }

    RadioHsWidget::defineButton(*target, buttonBackgroundGraphics,
        suffix, icon, buttonIconColors);
}

/*!
    Disables specified push button.
 
    \param hsButtonIdentifier Which button is to be disabled.
 */
void RadioHsWidget::changeButtonToDisabled(int hsButtonIdentifier)
{
    LEVEL2(LOG_METHOD_ENTER);
    buttonEvent(static_cast<ControlButtonIdentifier>(hsButtonIdentifier), Disabled);
}

/*!
    Enabled specified push button.
 
    \param hsButtonIdentifier Which button is to be enabled.
 */
void RadioHsWidget::changeButtonToEnabled(int hsButtonIdentifier)
{
    LEVEL2(LOG_METHOD_ENTER);
    changeButtonToReleased(hsButtonIdentifier);
}

/*!
    Checks the profile of the device. If profile is offline, user is asked a
    permission to start the radio in offline profile. Dialog is shown on
    behalf of the radio asking the permission.    
    
    \returns \c true if radio can be started, \c false if device is in
    offline profile and user didn't gave permission to start the radio.  
 */
bool RadioHsWidget::radioStartPermission()
{
    LOG_METHOD_ENTER;
    // This is true by default because we might not be in offline profile and
    // starting the radio is allowed in other profiles without asking a
    // permission.
    bool radioStartPermssion = true;
    if (mProfileMonitor->isInOfflineMode()) {
        // Device is in offline profile, ask the user for permission to start
        HbDeviceMessageBox box(hbTrId(
            "txt_fmradiohswidget_rad_info_activate_radio_in_offline_mode_hs"),
            HbMessageBox::MessageTypeQuestion);
        box.setTimeout(HbPopup::NoTimeout);
        box.exec();
        // radioStartPermssion is now true or false, depending what the user
        // selected. If user didn't gave permission, then radio is not
        // started.
        radioStartPermssion = box.isAcceptAction(box.triggeredAction());
    }
    return radioStartPermssion;
}
