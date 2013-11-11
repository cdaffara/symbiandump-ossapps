/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package javax.microedition.io;

import java.security.Permission;
import java.security.PermissionCollection;

/**
 * This class represents access rights to connections via the "http" protocol.
 * A HttpsProtocolPermission consists of a URI string but no actions list.
 * <p>
 * The URI string specifies a data resource accessible via http. It takes the
 *following general form:
 * <p>
 * http://{host}[:{portspec}][{pathname}][?{query}][#{fragment}]
 * <p>
 * (See RFCs 2818 and 3986  for more information.) The value of the {host}
 * field must be a symbolic hostname, a literal IPv4 address or a literal
 * IPv6 address, as specified by RFC 3986. If the {host} string is a
 * DNS name, an asterisk may appear in the leftmost position to indicate
 * a match of 1 or more entire domain labels. Partial domain label
 * matches are not permitted, therefore "*.sun.com" is valid, but
 * "*sun.com" is not. An asterisk by itself matches all hosts.
 * <p>
 * The {portspec} string takes the following form:
 * <p>
 * portnumber | -portnumber | portnumber-[portnumber] | "*" | empty string
 * <p>
 * A {portspec} specification of the form "N-" (where N is a port number)
 * signifies all ports numbered N and above, while a specification of the form
 * "-N" indicates all ports numbered N and below. A single asterisk in the
 * place of the {portspec} field matches all ports; therefore, the URI
 * "http://*:*" matches http connections to all hosts on all ports. If the
 * {portspec} field is omitted, default port 80 is assumed.
 *
 * @see Connector.open(java.lang.String)
 * @see "javax.microedition.io.HttpConnection" in MIDP 3.0 Specification
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public final class HttpsProtocolPermission
        extends GCFPermission
{
    /**
     * Creates a new HttpsProtocolPermission with the specified URI as its
     * name. The URI string must conform to the specification given above,
     * and is normalized to facilitate subsequent comparisons.
     *
     * @param uri the URI string.
     * @throws IllegalArgumentException if uri is malformed.
     * @throws NullPointerException if uri is null.
     */
    public HttpsProtocolPermission(String uri)
    {
        super(uri);
    }

    /**
     * Checks if this HttpsProtocolPermission object "implies" the specified
     * permission.
     * <p>
     * Because the {host} field of the permission URI may indicate an
     * individual host by means of either a symbolic hostname or an IP address,
     * a thorough evaluation of the implies relation between two
     * HttpsProtocolPermission objects requires the underlying platform to be
     * able to transform symbolic hostnames into IP addresses (via address
     * lookup) and vice-versa (via name lookup). Either service may be
     * unavailable on a given platform. As a result, the comparison of URI
     * strings from two permissions can only be best-effort. For each
     * HttpsProtocolPermission constructed with a URI that contains a {host}
     * field, the implementation will attempt to determine both the
     * fully-qualified domain name (the "canonical name") and the IP
     * address(es) of the host. However, either value may be unknown.
     * <p>
     * To determine whether this object implies another permission, p, this
     * method first ensures that all of the following are true (and returns
     * false if any of them are not):
     * <ul>
     * <li>p is an instanceof HttpsProtocolPermission, and</li>
     * <li>p's port range is included in this port range.</li>
     * </ul>
     * Then implies checks each of the following, in order, and for each
     * returns true if the stated condition is true:
     * <ul>
     * <li>If this object has a single IP address and one of p's IP addresses
     * is equal to this object's IP address.</li>
     * <li>If this object has a wildcard domain (such as *.sun.com), and p's
     * absolute name (the name with any preceding * removed) ends with this
     * object's aboslute name. For example, *.sun.com implies *.eng.sun.com.
     * </li>
     * <li>If this object has multiple IP addresses, and one of this object's
     * IP addresses equals one of p's IP addresses.</li>
     * <li>If this object's canonical name equals p's canonical name.</li>
     * </ul>
     * If none of the above are true, implies returns false.
     * <p>
     * Note that the {pathname}, {query} and {fragment} components are not
     * used when evaluating the 'implies' relation.
     *
     * @param p the permission to check against.
     * @return true if the specified permission is implied by this object,
     *        false if not.
     */
    public boolean implies(Permission p)
    {
        if (p instanceof HttpsProtocolPermission)
        {
            return true;
        }
        return false;
    }

    /**
     * Checks two HttpsProtocolPermission objects for equality.
     *
     * @param obj the object we are testing for equality with this object.
     * @return true if obj is a HttpsProtocolPermission and has the same URI
     *         string as this HttpsProtocolPermission object.
     */
    public boolean equals(Object obj)
    {
        return true;
    }

    /**
     * Returns the hash code value for this object.
     *
     * @return a hash code value for this object.
     */
    public int hashCode()
    {
        return 0;
    }

    /**
     * Returns the canonical string representation of the actions, which
     * currently is the empty string "", since there are no actions
     * defined for HttpsProtocolPermission.
     *
     * @return the empty string "".
     */
    public String getActions()
    {
        return "";
    }

    /**
     * Returns a new PermissionCollection for storing
     * HttpsProtocolPermission objects.
     * <p>
     * HttpsProtocolPermission objects must be stored in a manner that allows
     * them to be inserted into the collection in any order, but that also
     * enables the PermissionCollection implies method to be implemented in
     * an efficient (and consistent) manner.
     *
     * @return a new PermissionCollection suitable for storing
     *         HttpsProtocolPermission objects.
     */
    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
