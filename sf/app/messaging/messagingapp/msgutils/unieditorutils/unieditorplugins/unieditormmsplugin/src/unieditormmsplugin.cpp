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

//USER INCLUDES
#include "unieditormmsplugin.h"
#include "unieditormmsplugin_p.h"
#include "debugtraces.h"

//---------------------------------------------------------------
// UniEditorMmsPlugin::UniEditorMmsPlugin()
// @see header
//---------------------------------------------------------------
UniEditorMmsPlugin::UniEditorMmsPlugin(QObject* parent) :
    QObject(parent)
{	
    int error;
    QT_TRAP_THROWING( d_ptr = CUniEditorMmsPluginPrivate::NewL()); 
}

//---------------------------------------------------------------
// UniEditorMmsPlugin::~UniEditorMmsPlugin()
// @see header
//---------------------------------------------------------------
UniEditorMmsPlugin::~UniEditorMmsPlugin()
{
    delete d_ptr;
}

//---------------------------------------------------------------
// UniEditorMmsPlugin::messageType()
// @see header
//---------------------------------------------------------------
ConvergedMessage::MessageType UniEditorMmsPlugin::messageType()
{
return ConvergedMessage::Mms;
}		

//---------------------------------------------------------------
// UniEditorMmsPlugin::convertFrom()
// @see header
//---------------------------------------------------------------
ConvergedMessage* UniEditorMmsPlugin::convertFrom( TMsvId aId,
    UniEditorPluginInterface::EditorOperation aOperation )
{
    int error;
    ConvergedMessage* msg = NULL;
    TRAP( error, msg = d_ptr->convertFromL(aId,aOperation));
    QDEBUG_WRITE_FORMAT("Exiting convertFrom with error = ",error);
    return msg;
}

//---------------------------------------------------------------
// UniEditorMmsPlugin::deleteDraftsEntry()
// @see header
//---------------------------------------------------------------
void UniEditorMmsPlugin::deleteDraftsEntry( TMsvId aId )
{
    int error;
    TRAP( error, d_ptr->deleteDraftsEntryL(aId));
    QDEBUG_WRITE_FORMAT("Exiting deleteDraftsEntry with error = ",error);
}

//---------------------------------------------------------------
// UniEditorMmsPlugin::convertTo()
// @see header
//---------------------------------------------------------------
TMsvId UniEditorMmsPlugin::convertTo( ConvergedMessage *aMessage )
{
    int error;
    TMsvId id = -1;
    TRAP( error, id = d_ptr->convertToL(aMessage));
    QDEBUG_WRITE_FORMAT("Exiting convertTo and msgId = ",id);
	return id;	
}    

//---------------------------------------------------------------
// UniEditorMmsPlugin::send()
// @see header
//---------------------------------------------------------------
bool UniEditorMmsPlugin::send(TMsvId aId)
{
    int error = KErrNone;
    TRAP( error, d_ptr->sendL(aId));
    QDEBUG_WRITE_FORMAT("send returning with error= ",error);
    
    return (error != KErrNone) ? false : true; 
}    

//---------------------------------------------------------------
// UniEditorMmsPlugin::isServiceValid()
// @see header
//---------------------------------------------------------------
TBool UniEditorMmsPlugin::isServiceValid()
{
    int error;
    TBool valid = false;
    TRAP( error, valid = d_ptr->isServiceValidL());
    QDEBUG_WRITE_FORMAT("Exiting isServiceValid with validity= ",valid);
    return valid;
}    

//---------------------------------------------------------------
// UniEditorMmsPlugin::validateService()
// @see header
//---------------------------------------------------------------
TBool UniEditorMmsPlugin::validateService( TBool aEmailOverSms)
{
    int error;
    TBool valid = false;
    TRAP( error, valid = d_ptr->validateServiceL(aEmailOverSms));
    QDEBUG_WRITE_FORMAT("Exiting validateService with validity= ",valid);
    return valid;
}		





//---------------------------------------------------------------
// UniEditorMmsPlugin::setEncodingSettings()
// @see header
//---------------------------------------------------------------
void UniEditorMmsPlugin::setEncodingSettings(TBool /*aUnicodeMode*/,
    TSmsEncoding /*aAlternativeEncodingType*/, TInt /*charSupportType*/)
{

}

//---------------------------------------------------------------
// UniEditorMmsPlugin::getNumPDUs()
// @see header
//---------------------------------------------------------------
bool UniEditorMmsPlugin::getNumPDUs(QString& /*aBuf*/,
    TInt& /*aNumOfRemainingChars*/, TInt& /*aNumOfPDUs*/,
    TBool& /*aUnicodeMode*/, TSmsEncoding& /*aAlternativeEncodingType*/)
{
    return true;
}

Q_EXPORT_PLUGIN2(unieditormmsplugin, UniEditorMmsPlugin)

