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
using System.IO;
using System.Text;
using System.Threading.Tasks;

namespace IGE.Utils
{
  public class TextAsset
  {
    public string Name { get; private set; } // Asset name
    public string Text { get; private set; } // Text content
    public byte[] Bytes { get; private set; } // Binary content

    // Constructor
    public TextAsset(string name, string filePath)
    {
      Name = name;
      LoadFromFile(filePath);
    }

    // Load text and binary data from a file
    private void LoadFromFile(string filePath)
    {
      if (!File.Exists(filePath))
      {
        throw new FileNotFoundException($"File not found: {filePath}");
      }


      // Load binary data
      Bytes = File.ReadAllBytes(filePath);

      // Attempt to load as text (fallback if binary is not directly usable)
      Text = Encoding.UTF8.GetString(Bytes);
    }
  }

}