#ifndef __GW_DRAW_H_
#define __GW_DRAW_H_

#define INLINE static inline

void gw_plot(u16 x, u16 y, COLOR clr);
void gw_line(u16 x1, u16 y1, u16 x2, u16 y2, COLOR clr);
void gw_line_width(u16 x1, u16 y1, u16 x2, u16 y2, u16 width, COLOR color);
void gw_rect(u16 left, u16 top, u16 right, u16 bottom, COLOR clr);







#endif
