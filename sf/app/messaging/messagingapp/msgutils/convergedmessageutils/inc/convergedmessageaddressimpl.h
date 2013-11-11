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
 * Description: This is the implementation class for 
 * 				ConvergedMessageAddress
 */

#ifndef CONVERGED_MESSAGE_ADDRESS_IMPL_H
#define CONVERGED_MESSAGE_ADDRESS_IMPL_H

#include <QString>

/**
* Implementation of ConvergedMessageAddress data object.
* Encapsulates message real address and alias.
*/
class ConvergedMessageAddressImpl 
{
public:
	/**
	* Constructor
	*/
	ConvergedMessageAddressImpl(const QString &address = QString(),
			const QString &alias = QString());

	/**
	* Destructor     
	*/
	~ConvergedMessageAddressImpl();

	/**
	* Sets real address.
	* @param address Real address.
	*/
	void setAddress( const QString &address );

	/**
	* Returns real address.
	* @return Real address.
	*/
	const QString& address() const;

	/**
	* Sets alias for the real address.
	* @param alias for the real address.
	*/
	void setAlias( const QString &alias );

	/**
	* Returns alias for the real address.
	* @return alias for the Real address. 
	*/    
	const QString& alias() const;

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

private:  // Data
	/**
	* Real address
	*/
	QString  mRealAddress;

	/**
	* Alias for real address
	*/
	QString  mAlias;
};

#endif //CONVERGED_MESSAGE_ADDRESS_IMPL_H
