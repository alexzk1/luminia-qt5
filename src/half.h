/********************************************************************************
** Lumina is a flexible plattform independent development envrionment for
** GLSL shaders. It uses ECMA-script for tools and emulating opengl engines.
**
** Copyright (C) 2007-2008  oc2k1
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*********************************************************************************
**
** Algorithm is taken from the OGRE engine OgreBitwise.h
**
*********************************************************************************/
#include <cstdint>

class half
{
private:
    uint16_t val;
public:


    half& operator= (const double& d)
    {
        //fuck me... this is DEAD UB with strict-aliasing...damn
        union
        {
            float f;
            int32_t i;
        } v;
        v.f = d;

        int32_t s =  (v.i >> 16) & 0x00008000;
        int32_t e = ((v.i >> 23) & 0x000000ff) - (127 - 15);
        int32_t m =   v.i        & 0x007fffff;

        if (e <= 0)
        {
            if (e < -10)
            {
                val = 0;
                return *this;
            }
            m = (m | 0x00800000) >> (1 - e);
            val = s | (m >> 13);
            return *this;
        }
        else
            if (e == 0xff - (127 - 15))
            {
                if (m == 0)  // Inf
                {
                    val = s | 0x7c00;
                    return *this;
                }
                else     // NAN
                {
                    m >>= 13;
                    val = s | 0x7c00 | m | (m == 0);
                    return *this;
                }
            }
            else
            {
                if (e > 30)  // Overflow
                {
                    val = s | 0x7c00;
                    return *this;
                }
                val = s | (e << 10) | (m >> 13);
                return *this;
            }
    }

    operator double () const
    {
        union
        {
            float f;
            int32_t i;
        } v;
        int32_t s = (val >> 15) & 0x00000001;
        int32_t e = (val >> 10) & 0x0000001f;
        int32_t m =  val        & 0x000003ff;

        if (e == 0)
        {
            if (m == 0)  // Plus or minus zero
            {
                v.i = s << 31;
                return v.f;
            }
            else  // Denormalized number -- renormalize it
            {
                while (!(m & 0x00000400))
                {
                    m <<= 1;
                    e -=  1;
                }
                e += 1;
                m &= ~0x00000400;
            }
        }
        else
            if (e == 31)
            {
                if (m == 0)  // Inf
                {
                    v.i = (s << 31) | 0x7f800000;
                    return v.f;
                }
                else  // NaN
                {
                    v.i = (s << 31) | 0x7f800000 | (m << 13);
                    return v.f;
                }
            }

        e = e + (127 - 15);
        m = m << 13;

        v.i = (s << 31) | (e << 23) | m;
        return v.f;
    }
};
