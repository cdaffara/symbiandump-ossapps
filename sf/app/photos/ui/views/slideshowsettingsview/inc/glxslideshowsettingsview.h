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
* Description:   View for Slide show Settings. Slide Show settings consists
* of Drop down box for settings the trnasition effect and the transition delay.

*
*/

#ifndef GLXSLIDESHOWSETTINGSVIEW_H
#define GLXSLIDESHOWSETTINGSVIEW_H

#include "glxview.h"

//Qt/Orbit forward declarations
class HbLabel;
class HbView;
class HbMainWindow;
class HbComboBox;
class HbPushButton;
class QAbstractItemModel;
class GlxSettingInterface;

class GlxSlideShowSettingsView: public GlxView
{
Q_OBJECT

public:
    GlxSlideShowSettingsView( HbMainWindow *window );
    ~GlxSlideShowSettingsView();
    void activate();
    void deActivate();
    void setModel( QAbstractItemModel *model );

private slots:
    void orientationChanged( Qt::Orientation );
    void playSlideShow( );

private:
    void setLayout();

private:
    HbComboBox *mEffect;              //Drop down box to display transition effect
    HbComboBox *mDelay;               //Drop down box to display transition delay
    HbMainWindow *mWindow;            //no ownership
    HbLabel *mContextlabel;           //Display SlideShow label
    HbLabel *mEffectlabel;            //Display "Transition Effect: "
    HbLabel *mDelaylabel;             //Display "Transition delay: "
    HbPushButton *mPlaySlideShow;     //For playing slide show
    GlxSettingInterface *mSettings;   //no ownership
};

#endif /* GLXGRIDVIEW_H_ */
