/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#ifndef FACTORY_H
#define FACTORY_H

#include <QMap>

/*! Simple class factory.
  Example usage: <code>Factory<ContentView, QWidget, QString> viewFactory;</code>
  \param AbstractProduct The base class of the objects that are to be created by the factory.
  \param ParamType The class of the object to be passed to the constructor of the created objects.
  \param IdentifierType The type of the class identifier, usually a string or int.
  \param ProductCreator The type of the creator method of the target class.
*/
template <
    class AbstractProduct,
    class ParamType,
    class IdentifierType,
    class ProductCreator = AbstractProduct* (*)(ParamType *)
>
class Factory
{
public:
    void subscribe(const IdentifierType& id, ProductCreator creator)
    {
        m_associations[id] = creator;
    }

    void unsubscribe(const IdentifierType& id)
    {
        m_associations.remove(id);
    }

    AbstractProduct* createObject(const IdentifierType& id, ParamType *parent)
    {
        if(m_associations.contains(id))
        {
            return (m_associations[id])(parent);
        }
        return 0;
    }

private:
    QMap<QString, ProductCreator> m_associations;
};

#endif // FACTORY_H
