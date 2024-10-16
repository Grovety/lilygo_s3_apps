#pragma once
#include <stdarg.h>
#include <stdint.h>

/*!
 * \brief Interface to displays.
 */
class IDisplay {
public:
  enum class Font : unsigned {};
  enum class Rotation : unsigned {
    PORTRAIT,
    UPSIDE_DOWN,
  };
  virtual ~IDisplay() = default;
  /*!
   * \brief Set font.
   * \param f Font.
   */
  virtual void setFont(Font f) const = 0;
  /*!
   * \brief Set display rotation.
   * \param rot Rotation.
   */
  virtual void setRotation(Rotation rot = Rotation::UPSIDE_DOWN) const = 0;
  /*!
   * \brief Print string.
   * \param x X coordinate.
   * \param y Y coordinate.
   * \param fmt Formatted message string.
   */
  virtual void print_string(unsigned x, unsigned y, const char *fmt, ...) = 0;
  /*!
   * \brief Draw bitmap in defined location.
   * \param x X coordinate.
   * \param y Y coordinate.
   * \param w Width.
   * \param h Height.
   * \param bitmap Pointer to bitmap.
   */
  virtual void draw(unsigned x, unsigned y, unsigned w, unsigned h,
                    const uint8_t *bitmap) = 0;
  /*!
   * \brief Send buffer to display.
   */
  virtual void send() = 0;
  /*!
   * \brief Clear display buffer.
   */
  virtual void clear() = 0;
};
