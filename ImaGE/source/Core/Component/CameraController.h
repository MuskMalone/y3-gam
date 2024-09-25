#pragma once
namespace Component {
    struct CameraController {
        CameraController(float speed = 2.5f, float sense = 0.1f)
            : moveSpeed{ speed }, mouseSense{sense} {}

    private:
        float moveSpeed;
        float mouseSense;
    };
}
