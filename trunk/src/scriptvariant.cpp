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

#define _CRT_SECURE_NO_WARNINGS
#include "defs_private.h"

#include "scriptvariant.h"
#include "string.h"

#include GECKO_INCLUDE(xpcom,nsCOMPtr.h)
#include GECKO_INCLUDE(xpcom,nsComponentManagerUtils.h)

OSGK_ScriptVariantType osgk_variant_get_type (OSGK_ScriptVariant* variant)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetType();
}

OSGK_ScriptVariant* osgk_variant_convert (OSGK_ScriptVariant* variant,
                                          OSGK_ScriptVariantType newType)
{
  OSGK_ScriptVariantType oldType = osgk_variant_get_type (variant);
  if (newType == oldType)
  {
    osgk_addref (variant);
    return variant;
  }

  OSGK_Embedding* embedding = 0; // Not used atm anyway
  switch (newType)
  {
  case svtInt:
    {
      switch (oldType)
      {
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          return osgk_variant_create_int (embedding, ui);
        }
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          return osgk_variant_create_int (embedding, f);
        }
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          return osgk_variant_create_int (embedding, d);
        }
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          return osgk_variant_create_int (embedding, b);
        }
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          return osgk_variant_create_int (embedding, ch);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          int val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%d%c", &val, &dummy) == 1)
            return osgk_variant_create_int (embedding, val);
        }
      }
    }
    break;
  case svtUint:
    {
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          return osgk_variant_create_uint (embedding, i);
        }
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          return osgk_variant_create_uint (embedding, f);
        }
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          return osgk_variant_create_uint (embedding, d);
        }
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          return osgk_variant_create_uint (embedding, b);
        }
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          return osgk_variant_create_uint (embedding, ch);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          unsigned int val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%u%c", &val, &dummy) == 1)
            return osgk_variant_create_uint (embedding, val);
        }
      }
    }
    break;
  case svtFloat:
    {
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          return osgk_variant_create_float (embedding, i);
        }
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          return osgk_variant_create_float (embedding, ui);
        }
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          return osgk_variant_create_float (embedding, d);
        }
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          return osgk_variant_create_float (embedding, b);
        }
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          return osgk_variant_create_float (embedding, ch);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          float val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%f%c", &val, &dummy) == 1)
            return osgk_variant_create_float (embedding, val);
        }
      }
    }
    break;
  case svtDouble:
    {
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          return osgk_variant_create_double (embedding, i);
        }
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          return osgk_variant_create_double (embedding, ui);
        }
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          return osgk_variant_create_double (embedding, f);
        }
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          return osgk_variant_create_double (embedding, b);
        }
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          return osgk_variant_create_double (embedding, ch);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          float val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%f%c", &val, &dummy) == 1)
            return osgk_variant_create_double (embedding, val);
        }
      }
    }
    break;
  case svtBool:
    {
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          return osgk_variant_create_bool (embedding, i);
        }
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          return osgk_variant_create_bool (embedding, ui);
        }
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          return osgk_variant_create_bool (embedding, f);
        }
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          return osgk_variant_create_bool (embedding, d);
        }
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          return osgk_variant_create_bool (embedding, ch);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          int val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%d%c", &val, &dummy) == 1)
            return osgk_variant_create_bool (embedding, val);
          // @@@ strings like "true", "false"?
        }
      }
    }
    break;
  case svtChar:
    {
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          return osgk_variant_create_char (embedding, i);
        }
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          return osgk_variant_create_char (embedding, ui);
        }
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          return osgk_variant_create_char (embedding, f);
        }
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          return osgk_variant_create_char (embedding, d);
        }
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          return osgk_variant_create_char (embedding, b);
        }
      case svtString:
        {
          OSGK_String* s;
          if (!osgk_variant_get_string (variant, &s)) return 0;
          char val;
          char dummy;
          if (sscanf (osgk_string_get (s), "%c%c", &val, &dummy) == 1)
            return osgk_variant_create_char (embedding, val);
          // FIXME: should support UTF-8 here as well...
        }
      }
    }
    break;
  case svtString:
    {
      char buf[64];
      switch (oldType)
      {
      case svtInt:
        {
          int i;
          if (!osgk_variant_get_int (variant, &i)) return 0;
          _snprintf (buf, sizeof (buf), "%d", i);
        }
        break;
      case svtUint:
        {
          unsigned int ui;
          if (!osgk_variant_get_uint (variant, &ui)) return 0;
          _snprintf (buf, sizeof (buf), "%u", ui);
        }
        break;
      case svtFloat:
        {
          float f;
          if (!osgk_variant_get_float (variant, &f)) return 0;
          _snprintf (buf, sizeof (buf), "%g", f);
        }
        break;
      case svtDouble:
        {
          double d;
          if (!osgk_variant_get_double (variant, &d)) return 0;
          _snprintf (buf, sizeof (buf), "%g", d);
        }
        break;
      case svtBool:
        {
          int b;
          if (!osgk_variant_get_bool (variant, &b)) return 0;
          _snprintf (buf, sizeof (buf), "%d", b);
        }
        break;
      case svtChar:
        {
          unsigned int ch;
          if (!osgk_variant_get_char (variant, &ch)) return 0;
          // FIXME: should support UTF-8 here as well...
          _snprintf (buf, sizeof (buf), "%c", ch);
        }
        break;
      default:
        return 0;
      }
      OSGK::String str (buf);
      return osgk_variant_create_string (embedding, str.GetObject());
    }
    break;
  }
  return 0;
}

int osgk_variant_get_int (OSGK_ScriptVariant* variant, int* val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetInt (*val);
}

int osgk_variant_get_uint (OSGK_ScriptVariant* variant, unsigned int* val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetUint (*val);
}

int osgk_variant_get_float (OSGK_ScriptVariant* variant, float* val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetFloat (*val);
}

int osgk_variant_get_double (OSGK_ScriptVariant* variant, double* val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetDouble (*val);
}

int osgk_variant_get_bool (OSGK_ScriptVariant* variant, int* val)
{
  bool b;
  int ret = static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetBool (b);
  if (ret) *val = b;
  return ret;
}

int osgk_variant_get_char (OSGK_ScriptVariant* variant, unsigned int* val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetChar (*val);
}

// Does not increase ref count
int osgk_variant_get_string (OSGK_ScriptVariant* variant, OSGK_String** val)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetString (*val);
}

// Does not increase ref count
//int osgk_variant_get_isupports (OSGK_ScriptVariant* variant, OSGK_CLASSTYPE_REF nsISupports** val);
int osgk_variant_get_script_object (OSGK_ScriptVariant* variant, void** tag)
{
  return static_cast<OSGK::Impl::ScriptVariant*> (variant)->GetScriptObject (*tag);
}

//int osgk_variant_get_array_size (OSGK_ScriptVariant* variant, size_t* size);
//int osgk_variant_get_array_item (OSGK_ScriptVariant* variant, OSGK_ScriptVariant** val);

OSGK_ScriptVariant* osgk_variant_create_empty (OSGK_Embedding* embedding)
{
  return new OSGK::Impl::ScriptVariant ();
}

OSGK_ScriptVariant* osgk_variant_create_int (OSGK_Embedding* embedding, int val)
{
  return new OSGK::Impl::ScriptVariant (val);
}

OSGK_ScriptVariant* osgk_variant_create_uint (OSGK_Embedding* embedding, unsigned int val)
{
  return new OSGK::Impl::ScriptVariant (val);
}

OSGK_ScriptVariant* osgk_variant_create_float (OSGK_Embedding* embedding, float val)
{
  return new OSGK::Impl::ScriptVariant (val);
}

OSGK_ScriptVariant* osgk_variant_create_double (OSGK_Embedding* embedding, double val)
{
  return new OSGK::Impl::ScriptVariant (val);
}

OSGK_ScriptVariant* osgk_variant_create_bool (OSGK_Embedding* embedding, int val)
{
  return new OSGK::Impl::ScriptVariant (val != 0);
}

OSGK_ScriptVariant* osgk_variant_create_char (OSGK_Embedding* embedding, unsigned int val)
{
  return new OSGK::Impl::ScriptVariant (val, true);
}

OSGK_ScriptVariant* osgk_variant_create_string (OSGK_Embedding* embedding, OSGK_String* val)
{
  return new OSGK::Impl::ScriptVariant (val);
}

//OSGK_ScriptVariant* osgk_variant_create_isupports (OSGK_Embedding* embedding, OSGK_CLASSTYPE_REF nsISupports* val)
OSGK_ScriptVariant* osgk_variant_create_script_object (OSGK_Embedding* embedding, void* tag)
{
  return new OSGK::Impl::ScriptVariant (tag);
}

//OSGK_ScriptVariant* osgk_variant_create_array (OSGK_Embedding* embedding, size_t numItems, OSGK_ScriptVariant** items)

namespace OSGK
{
  namespace Impl
  {
    ScriptVariant::ScriptVariant (nsIVariant* value) : dataType (svtEmpty)
    {
      if (value == 0) return;

      PRUint16 varType;
      if (NS_FAILED(value->GetDataType (&varType))) return;

      switch (varType)
      {
      case nsIDataType::VTYPE_INT8:
        {
          PRUint8 v;
          if (NS_FAILED(value->GetAsInt8 (&v))) return;
          dataType = svtInt;
          intVal = PRInt8 (v);
        }
        break;
      case nsIDataType::VTYPE_INT16:
        {
          PRInt16 v;
          if (NS_FAILED(value->GetAsInt16 (&v))) return;
          dataType = svtInt;
          intVal = v;
        }
        break;
      case nsIDataType::VTYPE_INT32:
        {
          PRInt32 v;
          if (NS_FAILED(value->GetAsInt32 (&v))) return;
          dataType = svtInt;
          intVal = v;
        }
        break;
      case nsIDataType::VTYPE_INT64:
        {
          PRInt64 v;
          if (NS_FAILED(value->GetAsInt64 (&v))) return;
          dataType = svtInt;
          intVal = v;
        }
        break;
      case nsIDataType::VTYPE_UINT8:
        {
          PRUint8 v;
          if (NS_FAILED(value->GetAsUint8 (&v))) return;
          dataType = svtUint;
          uintVal = v;
        }
        break;
      case nsIDataType::VTYPE_UINT16:
        {
          PRUint16 v;
          if (NS_FAILED(value->GetAsUint16 (&v))) return;
          dataType = svtUint;
          uintVal = v;
        }
        break;
      case nsIDataType::VTYPE_UINT32:
        {
          PRUint32 v;
          if (NS_FAILED(value->GetAsUint32 (&v))) return;
          dataType = svtUint;
          uintVal = v;
        }
        break;
      case nsIDataType::VTYPE_UINT64:
        {
          PRUint64 v;
          if (NS_FAILED(value->GetAsUint64 (&v))) return;
          dataType = svtUint;
          uintVal = v;
        }
        break;
      case nsIDataType::VTYPE_FLOAT:
        {
          float v;
          if (NS_FAILED(value->GetAsFloat (&v))) return;
          dataType = svtFloat;
          floatVal = v;
        }
        break;
      case nsIDataType::VTYPE_DOUBLE:
        {
          double v;
          if (NS_FAILED(value->GetAsDouble (&v))) return;
          dataType = svtDouble;
          doubleVal = v;
        }
        break;
      case nsIDataType::VTYPE_BOOL:
        {
          PRBool v;
          if (NS_FAILED(value->GetAsBool (&v))) return;
          dataType = svtBool;
          boolVal = v != 0;
        }
        break;
      case nsIDataType::VTYPE_CHAR:
        {
          char v;
          if (NS_FAILED(value->GetAsChar (&v))) return;
          dataType = svtChar;
          charVal = v;
        }
        break;
      case nsIDataType::VTYPE_WCHAR:
        {
          PRUnichar v;
          if (NS_FAILED(value->GetAsWChar (&v))) return;
          dataType = svtChar;
          charVal = v;
        }
        break;
      case nsIDataType::VTYPE_CHAR_STR:
        {
          char* v;
          if (NS_FAILED(value->GetAsString (&v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = v;
          stringVal = wrapStr;
        }
        break;
      case nsIDataType::VTYPE_WCHAR_STR:
        {
          PRUnichar* v;
          if (NS_FAILED(value->GetAsWString (&v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = NS_ConvertUTF16toUTF8 (v);
          stringVal = wrapStr;
        }
        break;
      //VTYPE_ARRAY
      case nsIDataType::VTYPE_STRING_SIZE_IS:
        {
          char* v;
          PRUint32 size;
          if (NS_FAILED(value->GetAsStringWithSize (&size, &v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr().Assign (v, size);
          stringVal = wrapStr;
        }
        break;
      case nsIDataType::VTYPE_WSTRING_SIZE_IS:
        {
          PRUnichar* v;
          PRUint32 size;
          if (NS_FAILED(value->GetAsWStringWithSize (&size, &v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = NS_ConvertUTF16toUTF8 (v, size);
          stringVal = wrapStr;
        }
        break;
      case nsIDataType::VTYPE_UTF8STRING:
        {
          nsCString v;
          if (NS_FAILED(value->GetAsAUTF8String (v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = v;
          stringVal = wrapStr;
        }
        break;
      case nsIDataType::VTYPE_CSTRING:
        {
          nsCString v;
          if (NS_FAILED(value->GetAsACString (v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = v;
          stringVal = wrapStr;
        }
        break;
      case nsIDataType::VTYPE_ASTRING:
        {
          nsString v;
          if (NS_FAILED(value->GetAsAString (v))) return;
          dataType = svtString;
          Str_WrapNSCStr* wrapStr = new Str_WrapNSCStr ();
          wrapStr->GetNSCStr() = NS_ConvertUTF16toUTF8 (v);
          stringVal = wrapStr;
        }
        break;
      //VTYPE_INTERFACE
      //VTYPE_INTERFACE_IS
      //VTYPE_EMPTY_ARRAY
      case nsIDataType::VTYPE_EMPTY:
        // Nothing to do
        break;
      }
    }

    ScriptVariant::~ScriptVariant()
    {
      switch (dataType)
      {
      case svtString:
        osgk_release (stringVal);
        break;
      }
    }

    nsresult ScriptVariant::ConvertToIVariant (nsIVariant** out)
    {
      nsresult rv;
      nsCOMPtr<nsIWritableVariant> v = do_CreateInstance("@mozilla.org/variant;1", &rv);
      NS_ENSURE_SUCCESS(rv, rv);
      
      switch (dataType)
      {
      case svtEmpty:      rv = v->SetAsEmpty(); break;
      case svtInt:        rv = v->SetAsInt32 (intVal); break;
      case svtUint:       rv = v->SetAsUint32 (uintVal); break;
      case svtFloat:      rv = v->SetAsFloat (floatVal); break;
      case svtDouble:     rv = v->SetAsDouble (doubleVal); break;
      case svtBool:       rv = v->SetAsBool (boolVal); break;
      case svtChar:       rv = v->SetAsWChar (charVal); break;
      case svtString:     rv = v->SetAsAUTF8String (nsCString (osgk_string_get (stringVal))); break;
      //svtISupports,
      //svtArray
      case svtScriptObject:
      default:
        return NS_ERROR_CANNOT_CONVERT_DATA;
      }
      NS_ENSURE_SUCCESS(rv, rv);

      nsCOMPtr<nsIVariant> value = do_QueryInterface (v, &rv);
      NS_ENSURE_SUCCESS(rv, rv);
      
      *out = value;
      NS_ADDREF(*out);
      return NS_OK;
    }
  } // namespace Impl
} // namespace OSGK
