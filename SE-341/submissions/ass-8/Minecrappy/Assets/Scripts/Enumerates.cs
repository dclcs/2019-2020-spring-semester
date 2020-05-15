using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum HighlightColor
{
    Cyan = 0,
    Magenta = 1,
    Yellow = 2,
    Red = 3,
    Green = 4,
    Blue = 5
}

public enum BlockType
{
    Trivial = 0,
    Custom = 1,
    Outline = 2,
    Wave = 3,
    Furry = 4,
    Grass = 5,
    Hatching = 6
}

public enum RenderStyle
{
    Normal = 0,
    Blinn_Phong = 1
}

public class Utilities
{
    public static Color generateColor(HighlightColor colorType)
    {
        switch (colorType)
        {
            case HighlightColor.Blue:
                return new Color(0f, 0f, 1f, 1f);
            case HighlightColor.Cyan:
                return new Color(0f, 1f, 1f, 1f);
            case HighlightColor.Green:
                return new Color(0f, 1f, 0f, 1f);
            case HighlightColor.Magenta:
                return new Color(1f, 0f, 1f, 1f);
            case HighlightColor.Red:
                return new Color(1f, 0f, 0f, 1f);
            case HighlightColor.Yellow:
                return new Color(1f, 1f, 0f, 1f);
            default:
                return new Color(1f, 1f, 1f, 1f);
        }
    }
}