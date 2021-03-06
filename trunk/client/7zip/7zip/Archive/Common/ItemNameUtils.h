// Archive/Common/ItemNameUtils.h

#ifndef __ARCHIVE_ITEMNAMEUTILS_H
#define __ARCHIVE_ITEMNAMEUTILS_H

#include "../../../Common/String.h"

namespace NArchive {
namespace NItemName {

  UString MakeLegalName(const UString &name);
  UString GetOSName(const UString &name);
  UString GetOSName2(const UString &name);
  bool HasTailSlash(const AString &name, UINT codePage);

}}

#endif
