#pragma once
#include <set>
#include <string>
namespace IGE {
	namespace Assets {
		const std::set<std::string> cMeshExtensions{
			".obj", ".fbx",    // JPEG format
		};
		const std::set<std::string> cImageExtensions{
			".jpg", ".jpeg",    // JPEG format
			".png",             // PNG format
			".bmp",             // BMP format
			".tga",             // TGA format
			".hdr",             // HDR format
			".dds",             // DDS format (DirectXTex only)
			".tiff", ".tif",    // TIFF format (DirectXTex only)
		};
		const std::set<std::string> cAudioExtensions{
			".wav",    // Waveform Audio File
				".mp3",    // MPEG-1 or MPEG-2 Audio Layer III
				".ogg",    // Ogg Vorbis
				".flac",   // Free Lossless Audio Codec
				".aiff",   // Audio Interchange File Format
				".asf",    // Advanced Systems Format (WMA included)
				".it",     // Impulse Tracker
				".mod",    // Module file format
				".s3m",    // ScreamTracker 3 format
				".xm"      // FastTracker 2 Extended Module
		};
	}
}