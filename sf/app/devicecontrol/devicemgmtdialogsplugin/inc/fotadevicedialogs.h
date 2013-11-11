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
* Description: 
*
*/

#ifndef FOTADEVICEDIALOGS_H_
#define FOTADEVICEDIALOGS_H_

#include <devicedialogconsts.h>

#include <hbdevicedialoginterface.h>    // HbDeviceDialogInterface
#include <hbdialog.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include "syncmlnotifierprivate.h"
#include "devicemanagementnotifierwidget_p.h"


class fotadevicedialogs:public HbDialog, public HbDeviceDialogInterface {
//class fotadevicedialogs:public devicemanagementnotifierwidget {

Q_OBJECT

public:
/**
  * Construction and setting of observer happens here
  *
  * @since   S60   SF4
  * @param   aType - tell sthe type of the dialog to be shown
  * @param   aObserver is the parent class which owns the notifier and information is
  * passed to the parent class from the device dialog.
  */

fotadevicedialogs(const QVariantMap &parameters);

private:
    /**
      * Function to call the appropriate device dialog based on the dialog ID.
      *
      * @since   S60   SF4
      * @param   aType - tell sthe type of the dialog to be shown
      * @param   parameters - contains the dialogID + information to be shown in the dialogs
      */

     void launchFotaDialog(const QVariantMap &parameters);
     
     /**
       * Function to show the informative dialog of FOTA
       *
       * @since   S60   SF4
       * @param   aType - tell sthe type of the dialog to be shown
       * @param   parameters - contains the dialogID + information to be shown in the dialogs
       */

     void createfotainformativedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);
     
     /**
        * Function to show the error message dialog of FOTA
        *
        * @since   S60   SF4
        * @param   aType - tell sthe type of the dialog to be shown
        * @param   parameters - contains the dialogID + information to be shown in the dialogs
        */

     void createfotamessagedialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);
     
     /**
         * Function to show the confirmative dialog of FOTA
         *
         * @since   S60   SF4
         * @param   aType - tell sthe type of the dialog to be shown
         * @param   parameters - contains the dialogID + information to be shown in the dialogs
         */
     void createfotaconfirmationdialog(TFwUpdNoteTypes aType,const QVariantMap &parameters);

public slots:

/**
  * Construction and setting of observer happens here
  *
  * @since   S60   SF4
  * @param   aObserver is the parent class which owns the notifier and information is
  * passed to the parent class from the device dialog.
  */

    void fotaLSK();
    /**
      * Construction and setting of observer happens here
      *
      * @since   S60   SF4
      * @param   aObserver is the parent class which owns the notifier and information is
      * passed to the parent class from the device dialog.
      */

    void fotaRSK();
    

public: //derived from HbDeviceDialogInterface
    /**
      * Ccalled when device dialog is closed.
      *
      * @since   S60   SF4
      * @param   byClient - The Name of the client which closed the dialog.
      */

	void closeDeviceDialog(bool byClient);
    
    bool setDeviceDialogParameters(const QVariantMap &parameters);
    int deviceDialogError() const;
//    void closeDeviceDialog(bool byClient);
    HbDialog *deviceDialogWidget() const;

signals:
        void deviceDialogClosed();
        void deviceDialogData(QVariantMap data);


signals:
/**
  * servertextmessage
  *
  * @since   S60   SF4
  * @param   aObserver is the parent class which owns the notifier and information is
  * passed to the parent class from the device dialog.
  */

    void servertextmessage(const QString &string);
};

#endif
