/*
 * Copyright 2008 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Sun designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Sun in the LICENSE file that accompanied this code.
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

package build.tools.charsetmapping;

import java.io.*;

public class GenerateMapping {

    public static void genMapping(String argv[]) throws IOException {
        genDataJIS0213(new FileInputStream(argv[0]),
                       new FileOutputStream(argv[1]));
    }

    // regex pattern to parse the "jis0213.map" file
    static Pattern sjis0213 = Pattern.compile("0x(\\p{XDigit}++)\\s++U\\+(\\p{XDigit}++)(?:\\+(\\p{XDigit}++))?\\s++#.*");
    private static void genDataJIS0213(InputStream in, OutputStream out)
    {
        int[] sb = new int[0x100];                         // singlebyte
        int[] db = new int[0x10000];                       // doublebyte
        int[] indexC2B = new int[256];
        CharsetMapping.Entry[] supp = new CharsetMapping.Entry[0x10000];
        CharsetMapping.Entry[] comp = new CharsetMapping.Entry[0x100];
        int suppTotal = 0;
        int compTotal = 0;

        int b1Min1 = 0x81;
        int b1Max1 = 0x9f;
        int b1Min2 = 0xe0;
        int b1Max2 = 0xfc;
        int b2Min = 0x40;
        int b2Max = 0xfe;

        //init
        for (int i = 0; i < 0x80; i++) sb[i] = i;
        for (int i = 0x80; i < 0x100; i++) sb[i] = CharsetMapping.UNMAPPABLE_DECODING;
        for (int i = 0; i < 0x10000; i++) db[i] = CharsetMapping.UNMAPPABLE_DECODING;
        try {
            CharsetMapping.Parser p = new CharsetMapping.Parser(in, sjis0213);
            CharsetMapping.Entry  e = null;
            while ((e = p.next()) != null) {
                if (e.cp2 != 0) {
                    comp[compTotal++] = e;
                } else {
                    if (e.cp <= 0xffff) {
                        if (e.bs <= 0xff)
                            sb[e.bs] = e.cp;
                        else
                            db[e.bs] = e.cp;
                        indexC2B[e.cp>>8] = 1;
                    } else {
                        supp[suppTotal++] = e;
                    }
                }
            }
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            // c2b Index Table, always the first one
            CharsetMapping.writeINDEXC2B(baos, indexC2B);
            CharsetMapping.writeSINGLEBYTE(baos, sb);
            CharsetMapping.writeDOUBLEBYTE1(baos, db, b1Min1, b1Max1, b2Min, b2Max);
            CharsetMapping.writeDOUBLEBYTE2(baos, db, b1Min2, b1Max2, b2Min, b2Max);
            CharsetMapping.writeSUPPLEMENT(baos, supp, suppTotal);
            CharsetMapping.writeCOMPOSITE(baos, comp, compTotal);
            CharsetMapping.writeSIZE(out, baos.size());
            baos.writeTo(out);
            out.close();
        } catch (Exception x) {
            x.printStackTrace();
        }
    }
}
