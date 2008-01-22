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

#ifndef __OFFSCREENGECKO_STRING_H__
#define __OFFSCREENGECKO_STRING_H__

/**\file
 * String wrapper
 */

#include "baseobj.h"

/**
 * String wrapper. Used to return strings from OffscreenGecko.
 */
OSGK_DERIVEDTYPE(OSGK_String);

/**
 * Get the string data.
 * \param str The string object.
 * \return The C string wrapped by the string object.
 */
OSGK_EXTERN_C OSGK_API const char* osgk_string_get (OSGK_String* str);

/**
 * Create a new OSGK string from a given UTF-8 C string.
 * \param str The UTF-8 C string from which to create the OSGK string.
 */
OSGK_EXTERN_C OSGK_API OSGK_String* osgk_string_create (const char* str);

#ifdef __cplusplus

namespace OSGK
{
  /**
   * String wrapper. Used to return strings from OffscreenGecko.
   */
  struct String : public CXXAPI::Wrap<OSGK_String>
  {
    /**
     * Create a new OSGK string from a given UTF-8 C string.
     * \param str The UTF-8 C string from which to create the OSGK string.
     */
    String (const char* str) : WrapperType (osgk_string_create (str)) { }

    explicit String (WrappedType* obj) : WrapperType (obj) {}
    String& operator=(const String& other)
    {
      WrapperType::operator=(other);
      return *this;
    }
    
    //@{
    /**
     * Get the string data.
     * \return The C string wrapped by the string object.
     */
    const char* Get()
    {
      return osgk_string_get (GetObject());
    }
    
    operator const char*() { return Get(); }
    //@}
  };
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_STRING_H__
