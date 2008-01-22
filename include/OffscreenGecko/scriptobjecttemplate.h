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

#ifndef __OFFSCREENGECKO_SCRIPTOBJECTTEMPLATE_H__
#define __OFFSCREENGECKO_SCRIPTOBJECTTEMPLATE_H__

/**\file
 */

#include "baseobj.h"
#include "browser.h"
#include "scriptvariant.h"

OSGK_DERIVEDTYPE(OSGK_ScriptObjectTemplate);

typedef enum OSGK_ScriptResult
{
  srSuccess = 0,
  srFailed = 0x80004005L /* actually NS_ERROR_FAILURE */
} OSGK_ScriptResult;

typedef struct OSGK_ScriptObjectCreateParams_s
{
  void* createParam;
  OSGK_Browser* browser;
} OSGK_ScriptObjectCreateParams;

typedef OSGK_ScriptResult (*OSGK_CreateObjFunc) (
  OSGK_ScriptObjectCreateParams* params, void** objTag);
typedef void (*OSGK_DestroyObjFunc) (void* objTag);

OSGK_EXTERN_C OSGK_API OSGK_ScriptObjectTemplate* osgk_sot_create (
  OSGK_Embedding* embedding,
  OSGK_CreateObjFunc createFunc, OSGK_DestroyObjFunc destroyFunc,
  void* createParam);

typedef OSGK_ScriptVariant* (*OSGK_GetPropertyFunc) (void* objTag,
  void* propTag);
typedef OSGK_ScriptResult (*OSGK_SetPropertyFunc) (void* objTag,
  void* propTag, OSGK_ScriptVariant* val);
  
OSGK_EXTERN_C OSGK_API int osgk_sot_add_property (
  OSGK_ScriptObjectTemplate* templ, const char* propName, void* propTag,
  OSGK_GetPropertyFunc getter, OSGK_SetPropertyFunc setter);

typedef OSGK_ScriptResult (*OSGK_FunctionCallFunc) (void* objTag,
  void* methTag, size_t numParams, OSGK_ScriptVariant** params,
  OSGK_ScriptVariant** returnVal);

OSGK_EXTERN_C OSGK_API int osgk_sot_add_function (
  OSGK_ScriptObjectTemplate* templ, const char* funcName, void* funcTag,
  OSGK_FunctionCallFunc callFunc);

OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_sot_instantiate (
  OSGK_ScriptObjectTemplate* templ, void** objTag);

OSGK_EXTERN_C OSGK_API int osgk_sot_register (
  OSGK_ScriptObjectTemplate* templ, OSGK_Embedding* embedding, 
  const char* name, unsigned int flags);

// TODO: Create a nice C++ interface (notably, try to wrap callbacks)

#endif // __OFFSCREENGECKO_SCRIPTOBJECTTEMPLATE_H__
