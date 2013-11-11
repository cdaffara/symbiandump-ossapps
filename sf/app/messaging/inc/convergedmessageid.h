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
 * Description: This is the domain header API for Conversations 
 *                  Message Id details
 */

#ifndef CONVERGED_MESSAGE_ID_H
#define CONVERGED_MESSAGE_ID_H

#include <QList>

#include "msgutilsapidefines.h"

/**
* ConvergedMessageId data object
* Encapsulates Conversations Message Id details
*/
class MSG_UTILS_API_EXPORT ConvergedMessageId 
{

public:

	/**
	* Constructor
	* @param id QString type id
	*/
	ConvergedMessageId (int id = 0);

	/**
	* Copy Constructor 
	* @param other another ConvergedMessageId     
	*/
	ConvergedMessageId(const ConvergedMessageId &other);

	//TODO:OPEN recheck with QT mobility if == is required
	/**
	* Overloaded == operator
	* Compares two message id's
	* @param other another ConvergedMessageId
	* @return bool true or false if the id's are same
	*/
	bool operator==(const ConvergedMessageId &other) const;  

	/**
	* Overloaded = operator
	* Assigning other id into object id
	* @param other another ConvergedMessageId
	* @return type is reference to ConvergedMessageId
	*/
	ConvergedMessageId& operator=(const ConvergedMessageId &other);

	/**
	* Get the id
	* @return id 
	*/
	int getId() const;

	/**
	* Set the id
	* @param id of QString type
	*/
	void setId(int id);

	/**
	* Serialize the data memebers into the stream.
	* @param stream data stream to which data is serialized. 
	*/
	void serialize(QDataStream &stream) const;

	/**
	* Deserialize the stream to data members.
	* @param stream data stream from which data is deserialized. 
	*/
	void deserialize(QDataStream &stream);

private: //Data

	/**
	* Converge message id
	*/
	int mId;
}; 

typedef QList<ConvergedMessageId> ConvergedMessageIdList;

#endif // CONVERGED_MESSAGE_ID_H
