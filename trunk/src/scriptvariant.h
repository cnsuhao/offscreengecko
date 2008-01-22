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

#ifndef __SCRIPTVARIANT_H__
#define __SCRIPTVARIANT_H__

#include "baseobj_private.h"

#include GECKO_INCLUDE(xpcom,nsIVariant.h)

#include "OffscreenGecko/scriptvariant.h"

namespace OSGK
{
  namespace Impl
  {
    class ScriptVariant : public BaseObject<OSGK_ScriptVariant>
    {
      OSGK_ScriptVariantType dataType;
      union
      {
        int intVal;
        unsigned int uintVal;
        float floatVal;
        double doubleVal;
        bool boolVal;
        unsigned int charVal;
        OSGK_String* stringVal;
        nsISupports* isupportsVal;
        void* scriptObjectVal;
        struct ArrayStruct
        {
          size_t itemNum;
          OSGK_ScriptVariant** items;
        } arrayVal;
      };
    public:
      ScriptVariant () : dataType (svtEmpty) {}
      ScriptVariant (nsIVariant* value);
      ~ScriptVariant ();

      OSGK_ScriptVariantType GetType() const { return dataType; }
      
      nsresult ConvertToIVariant (nsIVariant** out);

    #define VARTYPE(GetterName, Type, VType, Value)                 \
      bool Get##GetterName (Type& val)                              \
      {                                                             \
        if (dataType != VType) return false;                        \
        val = Value;                                                \
        return true;                                                \
      }                                                             \
      ScriptVariant (Type val) : dataType (VType) { Value = val; }
    #include "scriptvariant_types.inl"
    #undef VARTYPE
      bool GetChar (unsigned int& val)
      {
        if (dataType != svtChar) return false;
        val = charVal;
        return true;
      }
      ScriptVariant (unsigned int val, bool isChar) : dataType (svtChar)
      { charVal = val; }

      bool GetString (OSGK_String*& val)
      {
        if (dataType != svtString) return false;
        val = stringVal;
        return true;
      }
      ScriptVariant (OSGK_String* val) : dataType (svtString)
      { stringVal = val; osgk_addref (stringVal); }
    };
  } // namespace Impl
} // namespace OSGK

#endif // __SCRIPTVARIANT_H__
