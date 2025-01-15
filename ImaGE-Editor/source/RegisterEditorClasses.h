#include <rttr/registration>
#include <policy.h>
#include <GUI/Helpers/SceneEditorConfig.h>
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

  {
    using T = Graphics::EditorCamera;
    rttr::registration::class_<T>("EditorCamera")
      .constructor<>()(rttr::policy::ctor::as_object)
      .property("position", &T::mPosition)
      .property("fov", &T::mFov)
      .property("pitch", &T::mPitch)
      .property("yaw", &T::mYaw)
      .property("nearClip", &T::mNearClip)
      .property("farClip", &T::mFarClip);
  }
  rttr::registration::class_<GUI::SceneEditorConfig>("SceneEditorConfig")
    .constructor<>()(rttr::policy::ctor::as_object)
    .property("editorCam", &GUI::SceneEditorConfig::editorCam)
    .property("collapsedNodes", &GUI::SceneEditorConfig::collapsedNodes);
}
