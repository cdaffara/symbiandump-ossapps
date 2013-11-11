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

#ifndef UNIEDITORMMSPLUGIN_H_
#define UNIEDITORMMSPLUGIN_H_


#include <QObject>
#include "unieditorplugininterface.h"

/**
 * MMS data model plugin.
 * Implements the processing of a MMS message in Message Store.
 */

class CUniEditorMmsPluginPrivate;

class UniEditorMmsPlugin : public QObject, public UniEditorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(UniEditorPluginInterface)

public:
    /**
     * constructor
     */
    UniEditorMmsPlugin(QObject* parent = 0);

    /**
     * Destructor
     */
    ~UniEditorMmsPlugin();

    /**
     * @see UniEditorPluginInterface
     */
    ConvergedMessage::MessageType messageType();
    
    /**
     * @see UniEditorPluginInterface
     */
    ConvergedMessage* convertFrom( TMsvId aId,
        UniEditorPluginInterface::EditorOperation aOperation
        =UniEditorPluginInterface::Default );

    /**
     * @see UniEditorPluginInterface
     */
    void deleteDraftsEntry( TMsvId aId );

    /**
     * @see UniEditorPluginInterface
     */
    TMsvId convertTo( ConvergedMessage *aMessage );
    
    /**
     * @see UniEditorPluginInterface
     */    
    bool send(TMsvId aId);    
    
    /**
     * @see UniEditorPluginInterface
     */
    TBool isServiceValid();

    /**
     * @see UniEditorPluginInterface
     */
    TBool validateService( TBool aEmailOverSms = EFalse );
    
    /**
     * @see UniEditorPluginInterface
     */
    void setEncodingSettings(TBool aUnicodeMode,
        TSmsEncoding aAlternativeEncodingType, TInt charSupportType);

    /**
     * @see UniEditorPluginInterface
     */
    bool getNumPDUs(QString& aBuf, TInt& aNumOfRemainingChars,
        TInt& aNumOfPDUs, TBool& aUnicodeMode,
        TSmsEncoding& aAlternativeEncodingType);
    
private:
	
    CUniEditorMmsPluginPrivate* d_ptr;
};

#endif
    

