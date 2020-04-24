
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

enum ShaderCategory {
    Trivial,
    Normal,
    Texture
}

public class CustomShaderGUI : ShaderGUI
{

    public override void OnGUI(MaterialEditor materialEditor, MaterialProperty[] properties)
    {
        Material target = materialEditor.target as Material;

        ShaderCategory choice = ShaderCategory.Trivial;
        if (target.IsKeywordEnabled("USE_TRIVIAL")) {
            choice = ShaderCategory.Trivial;
        } else if (target.IsKeywordEnabled("USE_NORMAL")) {
            choice = ShaderCategory.Normal;
        } else if (target.IsKeywordEnabled("USE_TEXTURE")) {
            choice = ShaderCategory.Texture;
        }
        EditorGUI.BeginChangeCheck();
        choice = (ShaderCategory)EditorGUILayout.EnumPopup(
            new GUIContent("Shader Category"), choice
        );

        if (EditorGUI.EndChangeCheck())
        {
            if (choice == ShaderCategory.Trivial) {
                target.EnableKeyword("USE_TRIVIAL");
                target.DisableKeyword("USE_NORMAL");
                target.DisableKeyword("USE_TEXTURE");
            }
            else if (choice == ShaderCategory.Normal) {
                target.DisableKeyword("USE_TRIVIAL");
                target.EnableKeyword("USE_NORMAL");
                target.DisableKeyword("USE_TEXTURE");
            } else if (choice == ShaderCategory.Texture) {
                target.DisableKeyword("USE_TRIVIAL");
                target.DisableKeyword("USE_NORMAL");
                target.EnableKeyword("USE_TEXTURE");
            } else {
                target.DisableKeyword("USE_TRIVIAL");
                target.DisableKeyword("USE_NORMAL");
                target.DisableKeyword("USE_TEXTURE");
            }
        }
        // render the default gui
        base.OnGUI(materialEditor, properties);
    }
}
