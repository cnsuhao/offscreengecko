/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is OffscreenGecko code.
 *
 * The Initial Developer of the Original Code is
 * Frank Richter <frank.richter@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "tgawrite.h"

namespace OSGK
{
  bool TGAWriter::WriteBGRAImage (int width, int height, const void* data,
                                  std::ostream& out)
  {
    TGAWriter w (out);

    // TGA Header
    w.WriteByte (0);  // ID length
    w.WriteByte (0);  // Color map type
    w.WriteByte (2);  // Image type - TrueColor
    w.WriteWord (0);  // Color map stuff
    w.WriteWord (0); 
    w.WriteByte (0); 
    w.WriteWord (0);  // X/Y offset
    w.WriteWord (0); 
    w.WriteWord (width); 
    w.WriteWord (height); 
    w.WriteByte (32); // Image depth
    w.WriteByte (1 | 32); // 1 attribute; origin top left

    out.write ((const char*)data, width * height * 4);

    return out.good();
  }
} // namespace OSGK