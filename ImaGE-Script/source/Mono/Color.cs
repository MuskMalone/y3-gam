using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Image.Utils
{
  public struct Color
  {
    public float r;
    public float g;
    public float b;
    public float a;

    public Color(float red, float green, float blue, float alpha = 1.0f)
    {
      r = red;
      g = green;
      b = blue;
      a = alpha;
    }

    public static Color red => new Color(1f, 0f, 0f);
    public static Color green => new Color(0f, 1f, 0f);
    public static Color blue => new Color(0f, 0f, 1f);
    public static Color white => new Color(1f, 1f, 1f);
    public static Color black => new Color(0f, 0f, 0f);
    public static Color clear => new Color(0f, 0f, 0f, 0f);
    public static Color yellow => new Color(1f, 1f, 0f); // RGB(1, 1, 0) with full opacity

    public override string ToString() => $"Color(r: {r}, g: {g}, b: {b}, a: {a})";
  }
}
