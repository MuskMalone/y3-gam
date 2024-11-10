using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IGE.Utils
{
  public class Color
  {
    // Fields for red, green, blue, and alpha channels
    public float r;
    public float g;
    public float b;
    public float a;

    // Constructors
    public Color(float r, float g, float b, float a = 1.0f)
    {
      this.r = Mathf.Clamp01(r);
      this.g = Mathf.Clamp01(g);
      this.b = Mathf.Clamp01(b);
      this.a = Mathf.Clamp01(a);
    }

    // Static Colors (like Unity’s Color.red, Color.green, etc.)
    public static Color Red => new Color(1, 0, 0);
    public static Color Green => new Color(0, 1, 0);
    public static Color Blue => new Color(0, 0, 1);
    public static Color White => new Color(1, 1, 1);
    public static Color Black => new Color(0, 0, 0);
    public static Color Clear => new Color(0, 0, 0, 0);
    public static Color Yellow => new Color(1, 1, 0);

    // Get and set individual color components
    public float this[int index]
    {
      get
      {
        switch (index)
        {
          case 0: return r;
          case 1: return g;
          case 2: return b;
          case 3: return a;
          default: throw new System.IndexOutOfRangeException("Invalid Color index!");
        }
      }
      set
      {
        switch (index)
        {
          case 0: r = Mathf.Clamp01(value); break;
          case 1: g = Mathf.Clamp01(value); break;
          case 2: b = Mathf.Clamp01(value); break;
          case 3: a = Mathf.Clamp01(value); break;
          default: throw new System.IndexOutOfRangeException("Invalid Color index!");
        }
      }
    }

    // Methods for color manipulation
    public static Color Lerp(Color a, Color b, float t)
    {
      t = Mathf.Clamp01(t);
      return new Color(
          a.r + (b.r - a.r) * t,
          a.g + (b.g - a.g) * t,
          a.b + (b.b - a.b) * t,
          a.a + (b.a - a.a) * t
      );
    }

    public override string ToString()
    {
      return $"Color(r: {r}, g: {g}, b: {b}, a: {a})";
    }
  };
}
