#pragma once
/*!*********************************************************************
\file   Canvas.h
\author
\date   1-November-2024
\brief  Definition of the Canvas component, for UI with transitions

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
namespace Component
{
    struct Canvas
    {
        Canvas()
            : isVisible(true),
            hasTransition(false),
            transitionProgress(1.0f), // Default fully visible
            transitionSpeed(1.0f),
            fadeColor(0.0f, 0.0f, 0.0f, 1.0f), // Default black fade
            fadingOut(false) {}

        /*!*********************************************************************
        \brief
          Resets the component to default values.
        ************************************************************************/
        inline void Clear() noexcept {
            isVisible = true;
            hasTransition = false;
            transitionProgress = 1.0f;
            transitionSpeed = 1.0f;
            fadingOut = false;
            fadeColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }

        bool isVisible;   // UI visibility

        // Fade Transition Variables
        bool hasTransition;          // Is the fade transition active?
        float transitionProgress;    // 0.0 = fully black, 1.0 = fully visible
        float transitionSpeed;       // Speed of fade effect
        glm::vec4 fadeColor;         // Fade color (default black)
        bool fadingOut;              // True = fade to black, False = fade in
    };
} // namespace Component
