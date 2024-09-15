/*!*********************************************************************
\file   JsonKeys.h
\author chengen.lau\@digipen.edu
\date   15-September-2024
\brief  Contains the relevant keys in string used for json
        serialization, which are referenced by the Serializer and
        Deserializer. These are for internal usage (what its named in
        the file) and most likely will not have to be cared about or
        changed by external users.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
#pragma once
namespace Serialization
{
  // Names of Keys used when serializing to file
  // Both Serializer and Deserializer uses these to determine the name of the keys
  constexpr char JsonIdKey[] = "ID";
  constexpr char JsonEntityStateKey[] = "isActive";
  constexpr char JsonParentKey[] = "Parent";
  constexpr char JsonChildEntitiesKey[] = "Child Entities";
  constexpr char JsonComponentsKey[] = "Components";
  constexpr char JsonPrefabKey[] = "Prefab";
  constexpr char JsonPfbVerKey[] = "Version";
  constexpr char JsonPfbActiveKey[] = "isActive";
  constexpr char JsonPfbDataKey[] = "SubData";
  constexpr char JsonRemovedChildrenKey[] = "Removed Children";
  constexpr char JsonRemovedCompKey[] = "Removed Components";
  constexpr char ScriptFieldInstListTypeKey[] = "type";
  constexpr char JsonAssociativeKey[] = "key", JsonAssociativeValue[] = "value";
}
