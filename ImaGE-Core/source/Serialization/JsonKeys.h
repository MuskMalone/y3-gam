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

// Names of Keys used when serializing to file
// Both Serializer and Deserializer use these to determine the name of the keys
#define JSON_ID_KEY                 "ID"
#define JSON_ENTITY_STATE_KEY       "isActive"
#define JSON_PARENT_KEY             "Parent"
#define JSON_CHILD_ENTITIES_KEY     "Child Entities"
#define JSON_COMPONENTS_KEY         "Components"
#define JSON_PREFAB_KEY             "Prefab"
#define JSON_PFB_POS_KEY            "Position"
#define JSON_GUID_KEY               "GUID"
#define JSON_PFB_NAME_KEY           "Name"
#define JSON_PFB_ACTIVE_KEY         "isActive"
#define JSON_PFB_DATA_KEY           "SubData"
#define JSON_ASSOCIATIVE_KEY        "key"
#define JSON_ASSOCIATIVE_VALUE      "value"

#define JSON_SCENE_KEY              "Scene"
#define JSON_LAYERS_KEY             "Layers"

#define JSON_SCRIPT_LIST_KEY        "scriptList"
#define JSON_SCRIPT_NAME_KEY        "scriptName"
#define JSON_SCRIPT_FIELD_LIST_KEY  "scriptFieldInstList"

#define JSON_SCRIPT_DMI_DATA_KEY    "data"
#define JSON_SCRIPT_DMI_TYPE_KEY    "type"
#define JSON_SCRIPT_DMI_SF_KEY      "scriptField"
