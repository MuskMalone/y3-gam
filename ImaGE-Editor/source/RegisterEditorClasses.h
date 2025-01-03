#include <rttr/registration>
#include <policy.h>
#include <GUI/Helpers/HierarchyConfig.h>
#include <ImGui/imgui.h>

static void rttr_auto_register_reflection_function4_(); namespace {
  struct rttr__auto__register4__ {
    rttr__auto__register4__() {
      rttr_auto_register_reflection_function4_();
    }
  };
} static const rttr__auto__register4__ auto_register__79; static void rttr_auto_register_reflection_function4_()
{
  rttr::registration::class_<GUI::HierarchyEntry>("HierarchyEntry")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("entityId", &GUI::HierarchyEntry::entityId)
    .property("stackId", &GUI::HierarchyEntry::stackId)
    .property("isOpen", &GUI::HierarchyEntry::isOpen);

  rttr::registration::class_<std::set<GUI::HierarchyEntry>>("HierarchyEntrySet")
    .constructor<>()(rttr::policy::ctor::as_object);

  rttr::registration::class_<GUI::HierarchyConfig>("HierarchyConfig")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("collapsedNodes", &GUI::HierarchyConfig::collapsedNodes);
}
