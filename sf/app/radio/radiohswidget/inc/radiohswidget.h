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

#ifndef RADIOHSWIDGET_H
#define RADIOHSWIDGET_H

// System includes
#include <HbWidget>

// Forward declarations
class HbLabel;
class HbPushButton;
class XQSettingsManager;
class XQSettingsKey;
class RadioHsWidgetProfileReader;
class RadioHsWidgetRadioServiceClient;
class QGraphicsLinearLayout;

/*!
    \namespace FmRadio
    \brief Wraps enumerators for radio state and service request visibility. 
 */
namespace FmRadio
{
    /*! Enum for radio application states. */
    enum State
    {
        StateUndefined,
        StateNotRunning,
        StateStarting,
        StateRunning,
        StateSeeking,
        StateAntennaNotConnected,
        StateClosing
    };
    
    /*! Enum for controlling the visibility of the radio application. */
    enum VisibiltyAfterRequest
    {
        VisibiltyDoNotChange,
        VisibiltyToForeground,
        VisibiltyToBackground
    };

}

class RadioHsWidget : public HbWidget
{
    Q_OBJECT
    
public:
    RadioHsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~RadioHsWidget();
    
    void handleRadioInformationChange(const int informationType,
        const QVariant &information);
    void handleRadioStateChange(const QVariant &value);

private:
    /*! Enum for information area layout states. */
    enum InformationAreaLayout
    {
        OneRow,
        TwoRows,
        Animation
    };

    /*! Enum for control button identifier. */
    enum ControlButtonIdentifier{
        Power,
        Previous,
        Next
    };
    
    /*! Enum for control button state. */
    enum ControlButtonState{
        Normal,
        Pressed,
        Disabled,
        Latched
    };


    /*! Enum for information types published by radio. */
    enum FmRadioInformationType {
        Frequency,
        StationName,
        RadioText,
        DynamicPsName,
        Pty
    };
    
signals:

    /*
       This signal allows your widget to save its properties persistently.
       Once properties are saved, the user may close and reopen the widget,
       and the stored properties will still be available. The names
       parameter contains names for the properties you want to store. The
       home screen framework reads the corresponding property values from
       the widget by calling QObject::property() method. Currently only
       QString type properties are supported. The home screen framework
       writes widget's properties just before calling the onInitialize slot.
       Both compile-time and dynamic properties are supported. Optional.
     */
    //void setPreferences(const QStringList &names);

    /*
       Your widget can notify the home screen framework about its completion
       by emitting this signal. With the current implementation, the
       framework removes the widget after receiving this signal. Optional.
     */
    //void finished();

    /*!
       Widget can notify the home screen framework about errors in its
       execution by emitting this signal. With the current implementation,
       the framework removes the widget after receiving this signal.
       Optional.
     */
    void error();

public slots:
    /* These slots (prefixed with "on") are called by the home screen
     * framework for controlling the widget's state during its lifetime.
     * They are automatically connected by the framework.
     */

    /* Implement this slot if you want to be notified when your widget is
     * added to the home screen. Upon receiving this event, your widget can
     * perform any necessary initialization operations, such as preparing
     * connections and allocating resources. Optional.
     */
    void onInitialize();
    
    /* This slot is called when your widget is shown in the home screen. You
     * use this slot to activate your widget and begin processing data again
     * after being quiescent. Each home screen compatible widget must define
     * this slot. Mandatory.
     */
    void onShow();
    
    /* This slot is called when your widget is hidden from the home screen.
     * You use this function to deactivate your widget and put it into a
     * quiescent state. Each home screen compatible widget must define this
     * slot. Mandatory.
     */
    void onHide();
    
    /* Implement this slot if you want to be notified when your widget is
     * removed from the home screen. Upon receiving this event, your widget
     * should perform any necessary cleanup operations, such as remove cache
     * files, and release any resources it currently holds. Optional.
     */
    //void onUninitialize(); 
    
    
    void changeButtonToPressed(int controlButtonId);
    void changeButtonToReleased(int controlButtonId);

private slots:
    void closeRadio();
    void changeToPreviousStation();
    void changeToNextStation();
    void changeRadioToForeground();
    void changeRadioToBackground();
    void toggleRadioPower();

private:
    void load(const QString &docml);
    
    void handleSimilarRadioInformation(
        const FmRadioInformationType informationType,
        const QVariant &information);
    bool updateRadioInformation(const FmRadioInformationType informationType,
        const QString &information);
    void changeInRadioInformation();
    void clearRadioInformation();
    
    void changeInformationAreaLayout(const InformationAreaLayout layout);
    void changePowerButtonOn(const bool isPowerOn);
    void enableStationButtons();
    void defineButton(HbPushButton &target, const QString &graphicsId,
        const QStringList &suffix, const QString &icon,
        const QString &iconColor);
    void buttonEvent(ControlButtonIdentifier buttonId,
        const ControlButtonState state);
    void changeButtonToDisabled(int controlButtonId);
    void changeButtonToEnabled(int controlButtonId);
    
    bool radioStartPermission();

private:
    // Data
    Q_DISABLE_COPY(RadioHsWidget)
    
    // UI components.
    /*! Button for capturing taps on information area. */
    HbPushButton *mInformationAreaBackgroundButton;
    /*! Control button for power. */
    HbPushButton *mPowerButton;
    /*! Control button for previous station. */
    HbPushButton *mPreviousButton;
    /*! Control button for next station. */
    HbPushButton *mNextButton;
    /*! Layout for information area. */
    QGraphicsWidget *mInformationAreaTwoRowsLayout;
    /*! Label that is shown when there is only one row of information. */
    HbLabel *mInformationLonelyRowLabel;
    /*! Label for first row when there is two rows of information. */
    HbLabel *mInformationFirstRowLabel;
    // TODO: This label should be modified to support marquee scrolling when Orbit supports it.
    /*! Label for first second when there is two rows of information. */
    HbLabel *mInformationSecondRowLabel;
    /*! Label for displaying animation. */
    HbLabel *mAnimationIcon;
    
    /*! Stores the state of the FM Radio application. */
    FmRadio::State mFmRadioState;

    /*! Stores the count of favorite stations. */
    int mFavoriteStationCount;
    /*! Stores the count of local stations. */
    int mLocalStationCount;
    /*! Stores the value whether the current station is favorite or not. */
    bool mCurrentStationIsFavorite;

    /*! Stores the radio information. */
    QHash<FmRadioInformationType, QString> mRadioInformation;
    /*! 
       String is used to format the text shown on first or only row from
       radio information.
     */
    QString mRadioInformationFirstRow;
    /*! 
       String is used to format the text shown on second row from
       radio information.
     */
    QString mRadioInformationSecondRow;

    /*! Profile monitor is used to read P&S keys and profile information. */
    RadioHsWidgetProfileReader *mProfileMonitor;
    
    /*! For communicating with the FM Radio through Qt Highway. */
    RadioHsWidgetRadioServiceClient *mRadioServiceClient;
    
};

#endif // RADIOHSWIDGET_H
