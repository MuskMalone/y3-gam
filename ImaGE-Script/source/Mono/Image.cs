/******************************************************************************/
/*!
\par        Image Engine
\file       .cs

\author     
\date       

\brief      


Copyright (C) 2024 DigiPen Institute of Technology. Reproduction
or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;      // For Bitmap (or use other graphics libraries)
using System.Drawing.Imaging;

namespace IGE.Utils
{
  public class Sprite
  {
    public Bitmap Image { get; private set; }
    public Point Position { get; set; }
    public Size Size { get; set; }

    // Constructor to load sprite from file
    public Sprite(string imagePath, int x, int y)
    {
      Image = new Bitmap(imagePath);
      Position = new Point(x, y);
      Size = Image.Size;
    }

    // Draw method to render the sprite
    public void Draw(Graphics graphics)
    {
      if (Image != null)
      {
        graphics.DrawImage(Image, Position.X, Position.Y, Size.Width, Size.Height);
      }
    }

    // Optional method to change position
    public void SetPosition(int x, int y)
    {
      Position = new Point(x, y);
    }

    // Clean up resources
    public void Dispose()
    {
      Image?.Dispose();
    }
  }
}