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
#include "mozilla-config.h"

#include "DirectoryService.h"
#include "pathutil.h"

#include GECKO_INCLUDE(xpcom,nsAppDirectoryServiceDefs.h)
#include GECKO_INCLUDE(xpcom,nsDirectoryServiceDefs.h)
#include GECKO_INCLUDE(xpcom,nsILocalFile.h)
#include GECKO_INCLUDE(xulapp,nsXULAppAPI.h)

namespace OSGK
{
  namespace Impl
  {
    NS_IMPL_ISUPPORTS1(DirectoryService::DirEnum,
      nsISimpleEnumerator)

    NS_IMETHODIMP DirectoryService::DirEnum::HasMoreElements (PRBool* _retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;
      *_retval = pos < paths.size();
      return NS_OK;
    }

    NS_IMETHODIMP DirectoryService::DirEnum::GetNext (nsISupports **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;

      return GetLocalFile (paths[pos++].c_str(), _retval);
    }

    NS_IMPL_ISUPPORTS2(DirectoryService, 
      nsIDirectoryServiceProvider,
      nsIDirectoryServiceProvider2)

    NS_IMETHODIMP DirectoryService::GetFile (const char *prop, PRBool *persistent, 
                                             nsIFile **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;

      if ((strcmp (prop, NS_APP_USER_PROFILE_50_DIR) == 0)
        || (strcmp (prop, NS_APP_PROFILE_DIR_STARTUP) == 0))
      {
        *persistent = true;
        if (!profileDir.empty())
        {
          nsresult rc = GetLocalFile (profileDir.c_str(), _retval);
          if (NS_SUCCEEDED(rc)) return NS_OK;
        }
      }
      else if ((strcmp (prop, NS_APP_USER_PROFILE_LOCAL_50_DIR) == 0)
        || (strcmp (prop, NS_APP_PROFILE_LOCAL_DIR_STARTUP) == 0))
      {
        *persistent = true;
        if (!localProfileDir.empty())
        {
          nsresult rc = GetLocalFile (localProfileDir.c_str(), _retval);
          if (NS_SUCCEEDED(rc)) return NS_OK;
        }
        if (!profileDir.empty())
        {
          nsresult rc = GetLocalFile (profileDir.c_str(), _retval);
          if (NS_SUCCEEDED(rc)) return NS_OK;
        }
      }

      *_retval = 0;
      return NS_ERROR_NOT_AVAILABLE;
    }

    NS_IMETHODIMP DirectoryService::GetFiles (const char *prop, 
                                              nsISimpleEnumerator **_retval)
    {
      if (!_retval) return NS_ERROR_NULL_POINTER;
      if (strcmp (prop, NS_XPCOM_COMPONENT_DIR_LIST) == 0)
      {
        *_retval = new DirEnum (this, componentsPaths);
        NS_ADDREF (*_retval);
        return NS_OK;
      }

      *_retval = 0;
      return NS_ERROR_NOT_AVAILABLE;
    }

    void DirectoryService::AddComponentsPath (const char* path)
    {
      std::string pathStr (path);
      PathExpand (pathStr);
      componentsPaths.push_back (pathStr);
    }

    void DirectoryService::SetProfileDirectory (const char* path)
    { 
      if (path != 0)
      {
	profileDir = path; 
        PathExpand (profileDir);
      }
      else
        profileDir.clear();
    }

    void DirectoryService::SetLocalProfileDirectory (const char* path)
    { 
      if (path != 0)
      {
        localProfileDir = path;
        PathExpand (localProfileDir);
      }
      else
        localProfileDir.clear();
    }

    void DirectoryService::CreateProfileDirectories ()
    {
      PRBool dummy;
      nsCOMPtr<nsIFile> dir;
      if (NS_SUCCEEDED (GetFile (NS_APP_PROFILE_DIR_STARTUP,
          &dummy, getter_AddRefs (dir))))
        dir->Create (nsIFile::DIRECTORY_TYPE, 0700);
      if (NS_SUCCEEDED (GetFile (NS_APP_PROFILE_LOCAL_DIR_STARTUP,
          &dummy, getter_AddRefs (dir))))
        dir->Create (nsIFile::DIRECTORY_TYPE, 0700);
    }
  } // namespace Impl
} // namespace OSGK
