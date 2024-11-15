#pragma once
/*!*********************************************************************
\file   Canvas.h
\author
\date   1-November-2024
\brief  Definition of the Canvas component, for UI

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
namespace Component
{
    struct Canvas
    {
        Canvas() : isVisible(true)/*, anchor{ 0.5f, 0.5f }, renderOrder(0)*/ {}

        /*!*********************************************************************
        \brief
          Resets the component to default values.
        ************************************************************************/
        inline void Clear() noexcept { isVisible = true;/* anchor = { 0.5f, 0.5f }; renderOrder = 0;*/ }

        bool isVisible;
        //glm::vec2 anchor; // Anchor point relative to the canvas (0.5, 0.5 is center)
        //int renderOrder;  // Rendering order, in case you need layering for UI
    };

} // namespace Component