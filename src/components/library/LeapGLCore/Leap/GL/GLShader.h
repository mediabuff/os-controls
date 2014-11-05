#pragma once

#include <cassert>
#include <cstdint>
#include <map> // TODO: use unordered_map, but wait until Cinder is cut out of FreeForm so it can use C++11.
#include <string>
#include <vector>

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes
#include "Leap/GL/GLError.h"
#include "Leap/GL/ShaderException.h"

namespace Leap {
namespace GL {

// helper metafunction for simplifying the uniform modifiers
template <typename GLType_, size_t COMPONENT_COUNT_> struct UniformFunction { static const bool exists = false; };

// helper metafunction for simplifying the uniform matrix modifiers
template <size_t ROWS_, size_t COLUMNS_> struct UniformMatrixFunction { static const bool exists = false; };

enum MatrixStorageConvention { COLUMN_MAJOR, ROW_MAJOR };

enum class VariableIs { REQUIRED, OPTIONAL_NO_WARN, OPTIONAL_BUT_WARN };

/// @brief This class wraps compiling and binding GLSL shaders, as well as discovering and
/// setting their uniforms and attributes.
/// @details Some of the code was initially taken from Jerry Coffin's answer at
/// http://stackoverflow.com/questions/2795044/easy-framework-for-opengl-shaders-in-c-c
/// Currently only the OpenGL 2.1 standard uniform types are supported (in particular, this
/// is missing unsigned ints, unsigned int EigenTypes::Vectors, and a bunch of sampler types).
///
/// Upon successful linking, the shader program will be queried for all its active uniforms
/// and attributes, storing the relevant info (name, location, size, type) in a map which is
/// indexed by name.  These maps can be accessed via the UniformInfoMap and AttributeInfoMap
/// methods.
///
/// The only exceptions that this class explicitly throws derive from Leap::GL::ShaderException.
class GLShader {
public:

  // Stores information about a named variable in a shader program.
  class VarInfo {
  public:

    VarInfo () { } // TEMP until C++11 compatibility allows use of std::map::emplace
    VarInfo (const std::string &name, GLint location, GLint size, GLenum type);

    const std::string &Name () const { return m_name; }
    GLint Location () const { return m_location; }
    GLint Size () const { return m_size; }
    // The Type defines what uniform modifier function can be used with each variable.
    // Note that sampler types must be set using integer values indicating which texture
    // unit is bound to it.  See http://www.opengl.org/wiki/Sampler_(GLSL)#Binding_textures_to_samplers
    GLenum Type () const { return m_type; }

  private:

    std::string m_name;
    GLint m_location;
    GLint m_size;
    GLenum m_type;
  };

  typedef std::map<std::string,VarInfo> VarInfoMap;

  // TODO: make GLShader-specific std::exception subclass?

  // Construct a shader with given vertex and fragment programs.
  GLShader (const std::string &vertex_shader_source, const std::string &fragment_shader_source);
  // Automatically frees the allocated resources.
  ~GLShader ();

  // Returns the shader program handle, which is the integer "name" of this shader program in OpenGL.
  GLint ProgramHandle () const { return m_program_handle; }
  // This method should be called to bind this shader.
  void Bind () const {
    GL_THROW_UPON_ERROR(glUseProgram(m_program_handle));
  }
  // This method should be called when no shader program should be used.
  static void Unbind () {
    GL_THROW_UPON_ERROR(glUseProgram(0));
  }
  // Returns the currently bound shader program (the integer handle generated by OpenGL).
  // This should only generate a GL error if it is called between glBegin and glEnd.
  static GLint CurrentlyBoundProgramHandle () {
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program); // We know that GL_CURRENT_PROGRAM is a valid param.
    return current_program;
  }

  // Checks for the uniform with given name and type.  If the variable is not found, then the behavior
  // depends on the value of check_type:
  //   VariableIs::OPTIONAL_NO_WARN: Do nothing.
  //   VariableIs::OPTIONAL_BUT_WARN: Print a message indicating the missing variable.
  //   VariableIs::REQUIRED: Throw an exception with information indicating the missing variable.
  // This shader does not need to be bound for this call to succeed.
  void CheckForTypedUniform (const std::string &name, GLenum type, VariableIs check_type) const;
  // Checks for the attribute with given name and type.  If the variable is not found, then the behavior
  // depends on the value of check_type:
  //   VariableIs::OPTIONAL_NO_WARN: Do nothing.
  //   VariableIs::OPTIONAL_BUT_WARN: Print a message indicating the missing variable.
  //   VariableIs::REQUIRED: Throw an exception with information indicating the missing variable.
  // This shader does not need to be bound for this call to succeed.
  void CheckForTypedAttribute (const std::string &name, GLenum type, VariableIs check_type) const;

  // Returns a map, indexed by name, containing all the active uniforms in this shader program.
  // This shader does not need to be bound for this call to succeed.
  const VarInfoMap &UniformInfoMap () const { return m_uniform_info_map; }
  // Returns a map, indexed by name, containing all the active attributes in this shader program.
  // This shader does not need to be bound for this call to succeed.
  const VarInfoMap &AttributeInfoMap () const { return m_attribute_info_map; }

  // Returns true iff the shader uniform exists.
  // This shader does not need to be bound for this call to succeed.
  bool HasUniform (const std::string &name) const { return m_uniform_info_map.find(name) != m_uniform_info_map.end(); }
  // Returns the VarInfo data for the requested uniform, or throws if that uniform is not found.
  // This shader does not need to be bound for this call to succeed.
  const VarInfo &UniformInfo (const std::string &name) const {
    VarInfoMap::const_iterator it = m_uniform_info_map.find(name);
    if (it == m_uniform_info_map.end()) {
      throw ShaderException("no uniform named \"" + name + "\" found in shader program");
    }
    return it->second;
  }
  // Returns true iff the shader attribute exists.
  // This shader does not need to be bound for this call to succeed.
  bool HasAttribute (const std::string &name) const { return m_attribute_info_map.find(name) != m_attribute_info_map.end(); }
  // Returns the VarInfo data for the requested attribute, or throws if that attribute is not found.
  // This shader does not need to be bound for this call to succeed.
  const VarInfo &AttributeInfo (const std::string &name) const {
    VarInfoMap::const_iterator it = m_attribute_info_map.find(name);
    if (it == m_attribute_info_map.end()) {
      throw ShaderException("no attribute named \"" + name + "\" found in shader program");
    }
    return it->second;
  }
  // Returns the location of the requested uniform (its handle into the GL apparatus) or -1 if not found.
  // The -1 return value is what is used by the glUniform* functions as a sentinel value for "this
  // uniform is not found, so do nothing silently".
  // This shader does not need to be bound for this call to succeed.
  GLint LocationOfUniform (const std::string &name) const {
    auto it = m_uniform_info_map.find(name);
    return it != m_uniform_info_map.end() ? it->second.Location() : -1;
  }
  // Returns the location of the requested attribute (its handle into the GL apparatus) or -1 if not found.
  // The -1 return value is what is used by the glUniform* functions as a sentinel value for "this
  // uniform is not found, so do nothing silently".
  // This shader does not need to be bound for this call to succeed.
  GLint LocationOfAttribute (const std::string &name) const {
    auto it = m_attribute_info_map.find(name);
    return it != m_attribute_info_map.end() ? it->second.Location() : -1;
  }

  // These SetUniform* methods require this shader to currently be bound.  They are named
  // with type annotators to avoid confusion in situations where types are implicitly coerced.
  // The uniform has a fixed type in the shader, so the call to SetUniform* should reflect that.

  // Sets the named uniform to the given bool value (casted to GLint).
  // This shader must be bound for this call to succeed.
  void SetUniformi (const std::string &name, bool value) const {
    SetUniformi(name, GLint(value));
  }
  // Sets the named uniform to the given GLint value.
  // This shader must be bound for this call to succeed.
  void SetUniformi (const std::string &name, GLint value) const {
    // TODO: type checking
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR(
      glUniform1i(LocationOfUniform(name), value)
    );
  }
  // Sets the named uniform to the given GLfloat value.
  // This shader must be bound for this call to succeed.
  void SetUniformf (const std::string &name, GLfloat value) const {
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR(
      glUniform1f(LocationOfUniform(name), value)
    );
  }
  // Sets the named uniform to the given value which must be a packed 
  // POD type consisting of exactly 1, 2, 3, or 4 GLint values.
  // This shader must be bound for this call to succeed.
  template <typename T_>
  void SetUniformi (const std::string &name, const T_ &value) const {
    static_assert(sizeof(T_)%sizeof(GLint) == 0, "sizeof(T_) must be divisible by sizeof(GLint)");
    static_assert(UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLint components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::eval(LocationOfUniform(name), 1, reinterpret_cast<const GLint *>(&value))
    ));
  }
  // Sets the named uniform to the given value which must be a packed 
  // POD type consisting of exactly 1, 2, 3, or 4 GLfloat values.
  // This shader must be bound for this call to succeed.
  template <typename T_>
  void SetUniformf (const std::string &name, const T_ &value) const {
    static_assert(sizeof(T_)%sizeof(GLfloat) == 0, "sizeof(T_) must be divisible by sizeof(GLfloat)");
    static_assert(UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLfloat components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::eval(LocationOfUniform(name), 1, reinterpret_cast<const GLfloat *>(&value))
    ));
  }

  // Uniform modifiers to specify arrays of data, where the array component has various
  // sizes (indicated by the number in the method name).

  // Sets the named uniform array to the given std::vector of GLint values.
  // This shader must be bound for this call to succeed.
  void SetUniformArrayi (const std::string &name, const std::vector<GLint> &array) const {
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR(
      glUniform1iv(LocationOfUniform(name), static_cast<GLsizei>(array.size()), array.data())
    );
  }
  // Sets the named uniform array to the given std::vector of GLfloat values.
  // This shader must be bound for this call to succeed.
  void SetUniformArrayf (const std::string &name, const std::vector<GLfloat> &array) const {
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR(
      glUniform1fv(LocationOfUniform(name), static_cast<GLsizei>(array.size()), array.data())
    );
  }
  // Sets the named uniform array to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly 1, 2, 3, or 4 GLint values.
  // This shader must be bound for this call to succeed.
  template <typename T_>
  void SetUniformArrayi (const std::string &name, const std::vector<T_> &array) const {
    static_assert(sizeof(T_)%sizeof(GLint) == 0, "sizeof(T_) must be divisible by sizeof(GLint)");
    static_assert(UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::exists, "There is no known glUniform*iv function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLint components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformFunction<GLint,sizeof(T_)/sizeof(GLint)>::eval(LocationOfUniform(name), array.size(), reinterpret_cast<const GLint *>(array.data()))
    ));
  }
  // Sets the named uniform array to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly 1, 2, 3, or 4 GLfloat values.
  // This shader must be bound for this call to succeed.
  template <typename T_>
  void SetUniformArrayf (const std::string &name, const std::vector<T_> &array) const {
    static_assert(sizeof(T_)%sizeof(GLfloat) == 0, "sizeof(T_) must be divisible by sizeof(GLfloat)");
    static_assert(UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::exists, "There is no known glUniform*i function for size of given T_");
    // TODO: somehow check that T_ is actually a POD containing only GLfloat components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformFunction<GLfloat,sizeof(T_)/sizeof(GLfloat)>::eval(LocationOfUniform(name), array.size(), reinterpret_cast<const GLfloat *>(array.data()))
    ));
  }

  // Note that there are no GLint-based matrix-uniform-setting functions (because there are none in the GLSL spec)

  // Sets the named uniform to the given value which must be a packed POD type
  // consisting of exactly ROWS_*COLUMNS_ GLfloat values.  The matrix storage
  // convention must be specified; either ROW_MAJOR or COLUMN_MAJOR.  Note that
  // the ROWS_ and COLUMNS_ template parameters can't be deduced from the argument,
  // so they must be explicitly declared.
  // This shader must be bound for this call to succeed.
  template <size_t ROWS_, size_t COLUMNS_, typename T_>
  void SetUniformMatrixf (const std::string &name, const T_ &matrix, MatrixStorageConvention matrix_storage_convention) const {
    static_assert(UniformMatrixFunction<ROWS_,COLUMNS_>::exists, "There is no glUniformMatrix* function matching the requested ROWS_ and COLUMNS_");
    static_assert(sizeof(T_) == ROWS_*COLUMNS_*sizeof(GLfloat), "T_ must be a POD type having exactly ROWS_*COLUMNS_ components of type GLfloat");
    // TODO: somehow check that T_ is actually a POD containing only GLType_ components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformMatrixFunction<ROWS_,COLUMNS_>::eval(LocationOfUniform(name), 1, matrix_storage_convention == ROW_MAJOR, reinterpret_cast<const GLfloat *>(&matrix))
    ));
  }
  // Sets the named uniform array to the given std::vector of values each of which must be
  // a packed POD type consisting of exactly ROWS_*COLUMNS_ GLfloat values.  The matrix
  // storage convention must be specified; either ROW_MAJOR or COLUMN_MAJOR.  Note that
  // the ROWS_ and COLUMNS_ template parameters can't be deduced from the argument,
  // so they must be explicitly declared.
  // This shader must be bound for this call to succeed.
  template <size_t ROWS_, size_t COLUMNS_, typename T_>
  void SetUniformMatrixArrayf (const std::string &name, const std::vector<T_> &array, MatrixStorageConvention matrix_storage_convention) const {
    static_assert(UniformMatrixFunction<ROWS_,COLUMNS_>::exists, "There is no glUniformMatrix* function matching the requested ROWS_ and COLUMNS_");
    static_assert(sizeof(T_) == ROWS_*COLUMNS_*sizeof(GLfloat), "T_ must be a POD type having exactly ROWS_*COLUMNS_ components of type GLfloat");
    // TODO: somehow check that T_ is actually a POD containing only GLType_ components.
    assert(CurrentlyBoundProgramHandle() == m_program_handle && "trying to set a uniform without having first called GLShader::Bind on this");
    GL_THROW_UPON_ERROR((
      UniformMatrixFunction<ROWS_,COLUMNS_>::eval(LocationOfUniform(name), array.size(), matrix_storage_convention == ROW_MAJOR, reinterpret_cast<const GLfloat *>(array.data()))
    ));
  }

  // Returns (enum_name_string, type_name_string) for the given shader variable type.  Throws an
  // error if that type is not a shader variable type.
  static const std::string &VariableTypeString (GLenum type);
  
  static const std::map<GLenum,std::string> OPENGL_2_1_UNIFORM_TYPE_MAP;
  static const std::map<GLenum,std::string> OPENGL_3_3_UNIFORM_TYPE_MAP;

private:

  // Compiles the specified type of shader program, using the given source.  If an error
  // in encountered, a std::logic_error is thrown.
  static GLuint Compile (GLuint type, const std::string &source);

  GLuint m_vertex_shader;   ///< Handle to the vertex shader in the GL apparatus.
  GLuint m_fragment_shader; ///< Handle to the fragment shader in the GL apparatus.
  GLuint m_program_handle;            ///< Handle to the shader program in the GL apparatus.

  VarInfoMap m_uniform_info_map;
  VarInfoMap m_attribute_info_map;
};

// Template specializations of UniformFunction and UniformMatrixFunction.
// TODO: Try to consolidate these into fewer specializations (or none)

/// @cond false
// we don't want these showing up in the class list.
template <> struct UniformFunction<GLint,1> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform1iv(location, count, value); } };
template <> struct UniformFunction<GLint,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform2iv(location, count, value); } };
template <> struct UniformFunction<GLint,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform3iv(location, count, value); } };
template <> struct UniformFunction<GLint,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLint *value) { glUniform4iv(location, count, value); } };
template <> struct UniformFunction<GLfloat,1> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform1fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform2fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform3fv(location, count, value); } };
template <> struct UniformFunction<GLfloat,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, const GLfloat *value) { glUniform4fv(location, count, value); } };

template <> struct UniformMatrixFunction<2,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<2,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2x3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<2,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix2x4fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3x2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<3,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix3x4fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,2> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4x2fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,3> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4x3fv(location, count, transpose, value); } };
template <> struct UniformMatrixFunction<4,4> { static const bool exists = true; static void eval (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { glUniformMatrix4fv(location, count, transpose, value); } };
/// @endcond

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
