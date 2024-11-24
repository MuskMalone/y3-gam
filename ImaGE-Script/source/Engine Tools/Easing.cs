using System.Numerics;

public static class Easing
{
    public static float EaseInQuart(float start, float end, float value)
    {
        end -= start;
        return end * value * value * value * value + start;
    }

    public static float EaseOutBounce(float start, float end, float value)
    {
        value /= 1.0f;
        end -= start;
        if (value < (1 / 2.75f))
        {
            return end * (7.5625f * value * value) + start;
        }
        else if (value < (2 / 2.75f))
        {
            value -= (1.5f / 2.75f);
            return end * (7.5625f * (value) * value + .75f) + start;
        }
        else if (value < (2.5 / 2.75))
        {
            value -= (2.25f / 2.75f);
            return end * (7.5625f * (value) * value + .9375f) + start;
        }
        else
        {
            value -= (2.625f / 2.75f);
            return end * (7.5625f * (value) * value + .984375f) + start;
        }
    }

    public static float EaseInBounce(float start, float end, float value)
    {
        end -= start;
        float d = 1.0f;
        return end - EaseOutBounce(0, end, d - value) + start;
    }

    public static float EaseInOutBounce(float start, float end, float value)
    {
        end -= start;
        float d = 1.0f;
        if (value < d * 0.5f) return EaseInBounce(0, end, value * 2) * 0.5f + start;
        else return EaseOutBounce(0, end, value * 2 - d) * 0.5f + end * 0.5f + start;
    }

    public static float EaseInBack(float start, float end, float value)
    {
        end -= start;
        value /= 1;
        float s = 1.70158f;
        return end * (value) * value * ((s + 1) * value - s) + start;
    }

    public static float EaseOutBack(float start, float end, float value)
    {
        float s = 1.70158f;
        end -= start;
        value = (value) - 1;
        return end * ((value) * value * ((s + 1) * value + s) + 1) + start;
    }

    public static float QuickSpikeEaseOut(float start, float end, float value)
    {
        if (value <= .2f)
            return Linear(start, end, value / .2f);

        return EaseInQuint(end, start, value / .8f);
    }

    public static float Linear(float start, float end, float value)
    {
        float a = (1.0f - value) * start;
        float b = value * end;
        return a + b;
    }

    public static Vector2 Linear(Vector2 start, Vector2 end, float t)
    {
      float x = start.X + (end.X - start.X) * t;
      float y = start.Y + (end.Y - start.Y) * t;
      return new Vector2(x, y);
    }

  public static float EaseInQuint(float start, float end, float value)
    {
        end -= start;
        return end * value * value * value * value * value + start;
    }

    public static float Remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
    {
        float normalizedValue = (originalValue - originalMin) / (originalMax - originalMin);
        return normalizedValue * (newMax - newMin) + newMin;
    }

    public static float LinearInOut(float start, float end, float value)
    {
        if (value <= 0.5f)
        {
            // In the first half, interpolate from start to end
            return Linear(start, end, value / 0.5f);
        }
        else
        {
            // In the second half, interpolate from end back to start.
            // Adjust the value to map from 0.5-1 range to 0-1 range for interpolation
            return Linear(end, start, (value - 0.5f) / 0.5f);
        }
    }
}