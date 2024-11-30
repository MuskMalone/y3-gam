#pragma once
/*!*********************************************************************
\file   Interactive.h
\author k.choa@digipen.edu
\date   24-November-2024
\brief  Component to add interactivity like hover and clicks to UI elements.

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
namespace Component
{
    struct Interactive
    {
        Interactive() : isHovered(false){}

        /*!*********************************************************************
        \brief
          Updates the hover state of the UI element based on the mouse position.

        \param[in] mousePos
          The current position of the mouse.

        \param[in] elementBounds
          The bounding rectangle of the UI element.
        ************************************************************************/
        //void UpdateHoverState(const glm::vec2& mousePos, const glm::vec4& bounds)
        //{
        //    //bool previouslyHovered = isHovered;
        //    isHovered = mousePos.x >= bounds.x &&
        //        mousePos.x <= bounds.x + bounds.z &&
        //        mousePos.y >= bounds.y &&
        //        mousePos.y <= bounds.y + bounds.w;
        //}
        /*!*********************************************************************
        \brief
          Resets the component to default values.
        ************************************************************************/
        inline void Clear() noexcept {
            isHovered = false;
        }

        bool IsMouseOver(const glm::vec2& mousePos, const glm::vec4& bounds) {
            return mousePos.x >= bounds.x && mousePos.x <= bounds.x + bounds.z && // x range
                mousePos.y >= bounds.y && mousePos.y <= bounds.y + bounds.w;  // y range
        }
        void OnPointerEnter() {

        }

        void OnPointerExit() {

        }


        // Hover state
        bool isHovered;

    };
} // namespace Component