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

#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

#include "defs_private.h"
#include <direct.h>
#include <string>

#include GECKO_INCLUDE(xpcom,nscore.h)
#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)

#if defined(XP_WIN) || defined(XP_OS2)
  #define FILE_PATH_SEPARATOR       '\\'
#else
  #define FILE_PATH_SEPARATOR       '/'
#endif

#include GECKO_INCLUDE(xpcom,nsStringAPI.h)

namespace OSGK
{
  namespace Impl
  {
    static inline bool IsPathSeparator (unsigned int ch)
    {
      return (ch == '/') || (ch == '\\');
    }

#ifdef _MSC_VER
#define getcwd  _getcwd
#define chdir   _chdir
#endif

    static void PathExpand (std::string& path)
    {
#ifdef _MSC_VER
      char* pathExpanded = _fullpath (0, path.c_str(), 0);
      path = pathExpanded;
      free (pathExpanded);
#else
      char* saveCwd = getcwd (0, MAX_PATH);
      chdir (path.c_str());
      char* newCwd = getcwd (0, MAX_PATH);
      chdir (saveCwd);
      path = newCwd;
      free (saveCwd);
      free (newCwd);
#endif
    }

    template<typename T>
    nsresult GetLocalFile (const char* str, T**_retval)
    {
      if (str == 0)
      {
        *_retval = 0;
        return NS_ERROR_NULL_POINTER;
      }

      NS_ConvertASCIItoUTF16 nsPath (str);
      nsCOMPtr<nsILocalFile> pathObj;
      nsresult res = NS_NewLocalFile (nsPath, false, getter_AddRefs (pathObj));
      if (NS_FAILED (res)) return res;

      NS_ADDREF(*_retval = pathObj);
      return NS_OK;
    }
  } // namespace Impl
} // namespace OSGK

#endif // __PATHUTIL_H__