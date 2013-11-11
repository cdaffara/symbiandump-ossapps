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

#ifndef _RADIOVIEWBASE_H_
#define _RADIOVIEWBASE_H_

// System includes
#include <HbView>
#include <QScopedPointer>
#include <QSharedPointer>

// User includes

// Forward declarations
class RadioWindow;
class RadioStationModel;
class RadioUiLoader;
class HbAction;
class RadioUiEngine;
class HbSelectionDialog;

// Constants
namespace MenuItem
{
    enum CommonMenuItem
    {
        UseLoudspeaker,
        Exit
    };
}

// Class declaration
class RadioViewBase : public HbView
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioViewBase )

public:
    explicit RadioViewBase( bool transient = true );

    virtual ~RadioViewBase();

    void setMembers( RadioWindow* mainWindow, RadioUiLoader* uiLoader );

    virtual void preLazyLoadInit();

    void initialize( QSharedPointer<RadioUiEngine> uiEngine );

    bool isInitialized() const;

    bool isTransient() const;

    void updateOrientation( Qt::Orientation orientation, bool forceUpdate = false );

    void bringToForeground();

protected slots:

    void updateAudioRouting( bool loudspeaker );
    void activatePreviousView();
    void quit();

private slots:

    void handleUserAnswer( HbAction* answer );   // Needed by HbMessageBox inconvenience API

protected:

// New functinos

    void initBackAction();

    void connectCommonMenuItem( int menuItem );

    void connectXmlElement( const QString& name, const char* signal, QObject* receiver, const char* slot );

    void connectViewChangeMenuItem( QString name, const char* slot );

    void loadSection( const QString& docml, const QString& section );

    void askQuestion( const QString& question );
    
    void showSelectionDialog( QAbstractItemModel* model, 
                              const QString& titleTxt, 
                              const QString& confirmBtnTxt );

private:

    virtual void init() = 0;

    virtual void setOrientation();
    virtual void userAccepted();

protected: // data

    /**
     * Pointer to the main window.
     * Not own.
     */
    RadioWindow*                        mMainWindow;

    /**
     * Pointer to the XML UI (DocML) loader
     * Own.
     */
    QScopedPointer<RadioUiLoader>       mUiLoader;

    /**
     * Pointer to the ui engine
     * Shared among all views and the radio window
     */
    QSharedPointer<RadioUiEngine>       mUiEngine;

    /**
     * Flag indicating whether or not the view is transient
     * Transient views are deleted when they are hidden.
     */
    bool                                mTransientView;

    /**
     * Route audio to Loudspeaker/Headset menu item
     */
    HbAction*                           mUseLoudspeakerAction;

    /**
     * View orientation.
     */
    Qt::Orientation                     mOrientation;

};


#endif // _RADIOVIEWBASE_H_
