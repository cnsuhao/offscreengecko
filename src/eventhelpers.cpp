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

#include "defs_private.h"

#include "eventhelpers.h"
#include "OffscreenGecko/browser.h"

#include GECKO_INCLUDE(dom,nsIDOMKeyEvent.h)

namespace OSGK
{
  namespace Impl
  {
    namespace EventHelpers
    {
      unsigned int KeyOSGKToGecko (unsigned int key, bool& isChar)
      {
        switch (key)
        {
        #define MAP_KEY(oKey, gKey)   \
          case OSGKKey_ ## oKey: isChar = false; return nsIDOMKeyEvent::DOM_VK_ ## gKey;
        #define MAP_KEY_CHAR(oKey, gKey)   \
          case OSGKKey_ ## oKey: isChar = true; return gKey;
        #include "keymapping.inl"
        #undef MAP_KEY
        #undef MAP_KEY_CHAR
        default:
          if (key < OSGKKey_First) return key;
        }
        return 0;
      }

      bool KeyState::ApplyKey (unsigned int key, bool isDown)
      {
        unsigned char oldKeyBits = keyBits;

        unsigned char mask;
        switch (key)
        {
          case OSGKKey_Alt:     mask = 1 << ksAlt;   break;
          case OSGKKey_Control: mask = 1 << ksCtrl;  break;
          case OSGKKey_Shift:   mask = 1 << ksShift; break;
          case OSGKKey_Meta:    mask = 1 << ksMeta;  break;
          default: return false;
        }
        if (isDown)
          keyBits |= mask;
        else
          keyBits &= ~mask;
        return oldKeyBits != keyBits;
      }

    } // namespace EventHelpers
  } // namespace Impl
} // namespace OSGK
