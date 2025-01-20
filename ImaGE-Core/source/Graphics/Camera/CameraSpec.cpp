#include <pch.h>
#include "CameraSpec.h"
#include <Graphics/Utils.h>

namespace Graphics {
  void CameraSpec::ComputeFrustum() {
    frustum = Utils::Culling::ComputeFrustum(viewProjMatrix);
  }
}
