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

#ifndef __DEFS_PRIVATE_H__
#define __DEFS_PRIVATE_H__

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#define _GECKO_INCLUDE_STRINGIZE(X)             _GECKO_INCLUDE_STRINGIZE2(#X)
#define _GECKO_INCLUDE_STRINGIZE2(X)            X
#ifdef OSGK_GECKOINC_FLAT
# define GECKO_INCLUDE(dir, header)		_GECKO_INCLUDE_STRINGIZE(header)
#else
# define GECKO_INCLUDE(dir, header)		_GECKO_INCLUDE_STRINGIZE(dir/header)
#endif

#if defined(_MSC_VER)
  #if _MSC_VER >= 1400
    inline int snwprintf (wchar_t* out, int n, const wchar_t* fmt, ...)
    {
      va_list args;
      va_start (args, fmt);
      int r = _vsnwprintf_s (out, n, n, fmt, args);
      va_end (args);
      return r;
    }
  #else
    #define snwprintf _snwprintf
  #endif
#elif !defined(XP_WIN)
# define snwprintf swprintf
# define _snprintf snprintf
#endif

#endif // __DEFS_PRIVATE_H__
