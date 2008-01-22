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

#ifndef __OFFSCREENGECKO_SCRIPTVARIANT_H__
#define __OFFSCREENGECKO_SCRIPTVARIANT_H__

/**\file
 */

#include "baseobj.h"
#include "embedding.h"
#include "string.h"

OSGK_CLASSTYPE_DEF nsISupports;

OSGK_DERIVEDTYPE(OSGK_ScriptVariant);

typedef enum OSGK_ScriptVariantType
{
  svtEmpty,
  svtInt,
  svtUint,
  svtFloat,
  svtDouble,
  svtBool,
  svtChar,
  svtString,
  svtISupports,
  svtScriptObject,
  svtArray
} OSGK_ScriptVariantType;

OSGK_EXTERN_C OSGK_API OSGK_ScriptVariantType osgk_variant_get_type (
  OSGK_ScriptVariant* variant);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_convert (
  OSGK_ScriptVariant* variant, OSGK_ScriptVariantType newType);

OSGK_EXTERN_C OSGK_API int osgk_variant_get_int (OSGK_ScriptVariant* variant,
  int* val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_uint (OSGK_ScriptVariant* variant,
  unsigned int* val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_float (OSGK_ScriptVariant* variant,
  float* val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_double (OSGK_ScriptVariant* variant,
  double* val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_bool (OSGK_ScriptVariant* variant,
  int* val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_char (OSGK_ScriptVariant* variant,
  unsigned int* val);
// Does not increase ref count
OSGK_EXTERN_C OSGK_API int osgk_variant_get_string (OSGK_ScriptVariant* variant,
  OSGK_String** val);
// Does not increase ref count
OSGK_EXTERN_C OSGK_API int osgk_variant_get_isupports (OSGK_ScriptVariant* variant,
  OSGK_CLASSTYPE_REF nsISupports** val);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_script_object (OSGK_ScriptVariant* variant,
  void** tag);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_array_size (OSGK_ScriptVariant* variant,
  size_t* size);
OSGK_EXTERN_C OSGK_API int osgk_variant_get_array_item (OSGK_ScriptVariant* variant,
  OSGK_ScriptVariant** val);

OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_empty (
  OSGK_Embedding* embedding);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_int (
  OSGK_Embedding* embedding, int val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_uint (
  OSGK_Embedding* embedding, unsigned int val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_float (
  OSGK_Embedding* embedding, float val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_double (
  OSGK_Embedding* embedding, double val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_bool (
  OSGK_Embedding* embedding, int val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_char (
  OSGK_Embedding* embedding, unsigned int val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_string (
  OSGK_Embedding* embedding, OSGK_String* val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_isupports (
  OSGK_Embedding* embedding, OSGK_CLASSTYPE_REF nsISupports* val);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_script_object (
  OSGK_Embedding* embedding, void* tag);
OSGK_EXTERN_C OSGK_API OSGK_ScriptVariant* osgk_variant_create_array (
  OSGK_Embedding* embedding, size_t numItems, OSGK_ScriptVariant** items);

#ifdef __cplusplus

namespace OSGK
{
  /**
   */
  struct ScriptVariant : public CXXAPI::Wrap<OSGK_ScriptVariant>
  {
    ScriptVariant (Embedding& embedding) : 
      WrapperType (osgk_variant_create_empty (embedding.GetObject())) {}
    ScriptVariant (Embedding& embedding, int v) : 
      WrapperType (osgk_variant_create_int (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, unsigned int v) : 
      WrapperType (osgk_variant_create_uint (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, float v) : 
      WrapperType (osgk_variant_create_float (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, double v) : 
      WrapperType (osgk_variant_create_double (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, bool v) : 
      WrapperType (osgk_variant_create_bool (embedding.GetObject(), v ? 1 : 0)) {}
    ScriptVariant (Embedding& embedding, char v) : 
      WrapperType (osgk_variant_create_char (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, wchar_t v) : 
      WrapperType (osgk_variant_create_char (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, String& v) : 
      WrapperType (osgk_variant_create_string (embedding.GetObject(), v.GetObject())) {}
    ScriptVariant (Embedding& embedding, nsISupports* v) : 
      WrapperType (osgk_variant_create_isupports (embedding.GetObject(), v)) {}
    ScriptVariant (Embedding& embedding, void* tag) : 
      WrapperType (osgk_variant_create_script_object (embedding.GetObject(), tag)) {}
    ScriptVariant (Embedding& embedding, size_t numItems, ScriptVariant items[]) : 
      WrapperType (osgk_variant_create_array (embedding.GetObject(), numItems,
	reinterpret_cast<OSGK_ScriptVariant**> (items))) {}
    
    explicit ScriptVariant (WrappedType* obj) : WrapperType (obj) {}
    ScriptVariant& operator=(const ScriptVariant& other)
    {
      WrapperType::operator=(other);
      return *this;
    }
    
    OSGK_ScriptVariantType GetType()
    {
      return osgk_variant_get_type (GetObject());
    }
    ScriptVariant Convert (OSGK_ScriptVariantType newType)
    {
      ScriptVariant newVar (Null ());
      newVar.SetObject (osgk_variant_convert (GetObject(), newType));
      return newVar;
    }

    bool GetInt (int& val)
    {
      return osgk_variant_get_int (GetObject(), &val) != 0;
    }
    bool GetUint (unsigned int& val)
    {
      return osgk_variant_get_uint (GetObject(), &val) != 0;
    }
    bool GetFloat (float& val)
    {
      return osgk_variant_get_float (GetObject(), &val) != 0;
    }
    bool GetDouble (double& val)
    {
      return osgk_variant_get_double (GetObject(), &val) != 0;
    }
    bool GetBool (bool& val)
    {
      int v;
      if (osgk_variant_get_bool (GetObject(), &v) == 0) return false;
      val = v != 0;
      return true;
    }
    bool GetChar (unsigned int& val)
    {
      return osgk_variant_get_char (GetObject(), &val) != 0;
    }
    bool GetString (String& val)
    {
      OSGK_String* s;
      if (osgk_variant_get_string (GetObject(), &s) == 0) return false;
      osgk_addref (s);
      val.SetObject (s);
      return true;
    }
    bool GetISupports (nsISupports*& val)
    {
      return osgk_variant_get_isupports (GetObject(), &val) != 0;
    }
    bool GetScriptObject (void*& tag)
    {
      return osgk_variant_get_script_object (GetObject(), &tag) != 0;
    }
    bool GetArraySize (size_t& val)
    {
      return osgk_variant_get_array_size (GetObject(), &val) != 0;
    }
    bool GetArrayItem (ScriptVariant& val)
    {
      OSGK_ScriptVariant* v;
      if (osgk_variant_get_array_item (GetObject(), &v) == 0) return false;
      osgk_addref (v);
      val.SetObject (v);
      return true;
    }
  };
} // namespace OSGK
#endif

#endif // __OFFSCREENGECKO_SCRIPTVARIANT_H__
