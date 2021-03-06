#pragma once

#include "EigenTypes.h"

#include <cstdint>

class Color {
public:
  Color() {
    m_data << 1.0f, 1.0f, 1.0f, 1.0f;
  }
  //TODO: make this constructor not stamp on the other one somehow
  Color(float r, float g, float b, float a = 1.0f) {
    m_data << r, g, b, a;
  }
  Color(const EigenTypes::Vector3& c) {
    m_data << static_cast<float>(c.x()), static_cast<float>(c.y()), static_cast<float>(c.z()), 1.0f;
  }
  Color(const EigenTypes::Vector3f& c) {
    m_data << c, 1.0f;
  }
  Color(const EigenTypes::Vector4& c) {
    m_data << static_cast<float>(c.x()), static_cast<float>(c.y()), static_cast<float>(c.z()), static_cast<float>(c.w());
  }
  Color(const EigenTypes::Vector4f& c) {
    m_data = c;
  }
  Color(const Color& other, float blend) {
    m_data << other.R(), other.G(), other.B(), blend*other.A();
  }
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    m_data << r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f;
  }

  // convert to single brightness based on human perception
  float Luminance() const { return 0.2126f*R() + 0.7152f*G() + 0.0722f*B(); }

  // simple getters
  float R() const { return m_data[0]; }
  float G() const { return m_data[1]; }
  float B() const { return m_data[2]; }
  float A() const { return m_data[3]; }
  float& R() { return m_data[0]; }
  float& G() { return m_data[1]; }
  float& B() { return m_data[2]; }
  float& A() { return m_data[3]; }
  const EigenTypes::Vector4f& Data() const { return m_data; }

  // HSV conversions
  void FromHSV(float h, float s, float v, float a = 1.0f);
  void ToHSV(float& h, float& s, float& v) const;

  // common colors
  static const Color& White() { static Color c(1.0f, 1.0f, 1.0f, 1.0f); return c; }
  static const Color& LightGrey() { static Color c(0.85f, 0.85f, 0.85f, 1.0f); return c; }
  static const Color& Grey() { static Color c(0.6f, 0.6f, 0.6f, 1.0f); return c; }
  static const Color& DarkGrey() { static Color c(0.3f, 0.3f, 0.3f, 1.0f); return c; }
  static const Color& Black() { static Color c(0.0f, 0.0f, 0.0f, 1.0f); return c; }
  static const Color& Red() { static Color c(1.0f, 0.0f, 0.0f, 1.0f); return c; }
  static const Color& Blue() { static Color c(0.0f, 0.0f, 1.0f, 1.0f); return c; }
  static const Color& Green() { static Color c(0.0f, 1.0f, 0.0f, 1.0f); return c; }
  static const Color& Yellow() { static Color c(1.0f, 1.0f, 0.0f, 1.0f); return c; }
  static const Color& Purple() { static Color c(1.0f, 0.0f, 1.0f, 1.0f); return c; }
  static const Color& Orange() { static Color c(1.0f, 0.6f, 0.0f, 1.0f); return c; }
  static const Color& Teal() { static Color c(0.0f, 1.0f, 1.0f, 1.0f); return c; }
  static const Color& Transparent() { static Color c(0.0f, 0.0f, 0.0f, 0.0f); return c; }

private:

  EigenTypes::Vector4f m_data;
};
