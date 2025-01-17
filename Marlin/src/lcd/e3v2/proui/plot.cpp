/**
 * DWIN Single var plot
 * Author: Miguel A. Risco-Castillo
 * Version: 2.2.3
 * Date: 2023/01/29
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial applications additional licenses can be requested
 */

#include "../../../inc/MarlinConfig.h"

#if ALL(DWIN_LCD_PROUI, PROUI_TUNING_GRAPH)

#include "../../marlinui.h"
#include "dwin.h"
#include "plot.h"

#define Plot_Bg_Color RGB(1, 12, 8)

Plot plot;

Plot::PlotData Plot::data;

void Plot::draw(const frame_rect_t &frame, const_celsius_float_t max, const_celsius_float_t ref/*=0*/) {
  data.graphframe = frame;
  data.graphpoints = 0;
  data.scale = frame.h / max;
  data.x2 = frame.x + frame.w - 1;
  data.y2 = frame.y + frame.h - 1;
  data.r = LROUND((data.y2) - ref * data.scale);
  DWINUI::Draw_Box(1, Plot_Bg_Color, frame);
  for (uint8_t i = 1; i < 4; i++) if (i * 60 < frame.w) DWIN_Draw_VLine(Line_Color, i * 60 + frame.x, frame.y, frame.h);
  DWINUI::Draw_Box(0, Color_White, DWINUI::ExtendFrame(frame, 1));
  DWIN_Draw_HLine(Color_Red, frame.x, data.r, frame.w);
}

void Plot::update(const_celsius_float_t value) {
  if (!data.scale) { return; }
  const uint16_t y = LROUND((data.y2) - value * data.scale);
  if (data.graphpoints < data.graphframe.w) {
    if (data.graphpoints < 1) {
      DWIN_Draw_Point(Color_Yellow, 1, 1, data.graphframe.x, y);
    }
    else {
      DWIN_Draw_Line(Color_Yellow, data.graphpoints + data.graphframe.x - 1, data.yP, data.graphpoints + data.graphframe.x, y);
    }
  }
  else {
    DWIN_Frame_AreaMove(1, 0, 1, Plot_Bg_Color, data.graphframe.x, data.graphframe.y, data.x2, data.y2);
    if ((data.graphpoints % 60) == 0) DWIN_Draw_VLine(Line_Color, data.x2 - 1, data.graphframe.y + 1, data.graphframe.h - 2);
    DWIN_Draw_Point(Color_Red, 1, 1, data.x2 - 1, data.r);
    DWIN_Draw_Line(Color_Yellow, data.x2 - 2, data.yP, data.x2 - 1, y);
  }
  data.yP = y;
  data.graphpoints++;
  TERN_(HAS_BACKLIGHT_TIMEOUT, ui.refresh_backlight_timeout());
}

#endif // DWIN_LCD_PROUI && PROUI_TUNING_GRAPH
