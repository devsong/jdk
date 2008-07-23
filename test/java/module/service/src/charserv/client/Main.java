/*
 * Copyright 2008 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 */

module charserv.client;
package charserv.client;

import java.nio.charset.spi.CharsetProvider;
import java.util.Iterator;
import java.util.ServiceLoader;
import java.module.ModuleDefinition;
import java.module.Version;
import java.util.NoSuchElementException;

/**
 * Checks that the test's provider for java.nio.charset.spi.CharsetProvider,
 * which is defined in java.se.core, is available.
 */
public class Main {
    public static void realMain(String[] args) throws Throwable {
        Iterator<CharsetProvider> loader =
            ServiceLoader.load(CharsetProvider.class).iterator();

        CharsetProvider cp = loader.next();

        // Check by name rather than by java.lang.class to ensure that we
        // don't load the class by other than the ServiceLoader
        check(cp.getClass().getName().equals("charserv.provider.CharsetServiceProvider"));

        // Check that we can get a provider from the classpath
        boolean found = false;
        while (!found && loader.hasNext()) {
            cp = loader.next();
            if (cp.getClass().getName().equals("charserv.other.CharsetServiceProviderOnClasspath")) {
                found = true;
                break;
            }
        }
        check(found);
    }

    //--------------------- Infrastructure ---------------------------
    static volatile int passed = 0, failed = 0;
    static boolean pass() {passed++; return true;}
    static boolean fail() {failed++; Thread.dumpStack(); return false;}
    static boolean fail(String msg) {System.out.println(msg); return fail();}
    static void unexpected(Throwable t) {failed++; t.printStackTrace();}
    static boolean check(boolean cond) {if (cond) pass(); else fail(); return cond;}
    static boolean equal(Object x, Object y) {
        if (x == null ? y == null : x.equals(y)) return pass();
        else return fail(x + " not equal to " + y);}
    public static void main(String[] args) throws Throwable {
        try {realMain(args);} catch (Throwable t) {unexpected(t);}
        System.out.println("\nPassed = " + passed + " failed = " + failed);
        if (failed > 0) throw new AssertionError("Some tests failed");}
}
