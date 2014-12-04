#pragma once

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes
#include "Leap/GL/GLTexture2Params.h"
#include "Leap/GL/GLTexture2PixelData.h"
#include "Leap/GL/Texture2Exception.h"

namespace Leap {
namespace GL {

// This is an invaluable resource: http://www.opengl.org/wiki/Common_Mistakes

// This class wraps creation and use of 2-dimensional GL textures.  While the GLTexture2Params value
// used to construct GLTexture2 is persistent (it is stored almost unmodified), the pixel_data is
// not, and is only used to pass in data for texel-loading operations.  The only exceptions that this
// class explicitly throws derive from Leap::GL::Texture2Exception.
class GLTexture2 {
public:
  
  GLTexture2(const GLTexture2& rhs) = delete;

  // Construct a GLTexture2 with the specified parameters and pixel data. The pixel data is only
  // passed into glTexImage2D, and is not stored.  The default value for pixel_data is "empty",
  // which indicates that while the texture memory will be allocated for it, it will not be
  // initialized.  An exception will be thrown upon error.
  GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data = GLTexture2PixelData());
  // Automatically frees the allocated resources.
  ~GLTexture2 ();

  // Returns the assigned GLuint generated by this texture
  GLuint Id () { return m_texture_name; }
  // Returns the GLTexture2Params used to construct this texture.
  const GLTexture2Params &Params() const { return m_params; }

  // This method should be called to bind this shader.
  void Bind () const { glBindTexture(m_params.Target(), m_texture_name); }
  // This method should be called when no shader program should be used.
  void Unbind () const { glBindTexture(m_params.Target(), 0); }

  // Updates the contents of this texture from the specified pixel data.
  void UpdateTexture (const GLTexture2PixelData &pixel_data);
  // Extracts the contents of this texture to the specified pixel data.
  void ExtractTexture (GLTexture2PixelData &pixel_data);
  
private:

  void VerifyPixelDataOrThrow (const GLTexture2PixelData &pixel_data) const;

  GLTexture2Params m_params;
  GLuint m_texture_name;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
