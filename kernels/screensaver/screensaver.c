#include <am.h>
#include <klib-macros.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "amdev.h"

#define MAX_W 400
#define MAX_H 300

static uint32_t img_buffer[MAX_W*MAX_H];
static uint32_t colors[8] = {
  0x000000, 0xff0000, 0x00ff00, 0x0000ff,
  0xffff00, 0xff00ff, 0x00ffff, 0xffffff,
};

void draw(uint32_t color) {
  // change the code below
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  for(int i = 0; i < w * h; i++){
    img_buffer[i] = color;
  }
  io_write(AM_GPU_FBDRAW, 0, 0, img_buffer, w, h, true);
}

//现在事实上draw已经完成显示一个颜色的功能了，只不过目前只会显示0x000000ff蓝，现在我们需要
//写一个新的算法函数，用来计算下一个地址，并将其运算结果作为一个uint32_t传给draw
//还得修改循环

static uint32_t cal_color_code(uint32_t c_pri, uint32_t c_next, int i, int k){
	int r0 = (c_pri >> 16) & 0xff, g0 = (c_pri >> 8) & 0xff, b0 = c_pri & 0xff;
  	int r1 = (c_next >> 16) & 0xff, g1 = (c_next >> 8) & 0xff, b1 = c_next & 0xff;
 	int r = r0 + (r1 - r0) * i / k;
 	int g = g0 + (g1 - g0) * i / k;
 	int b = b0 + (b1 - b0) * i / k;
  return (r << 16) | (g << 8) | b;//返回的是第i步的色号
}

//改一下循环逻辑
int main() {
	ioe_init(); // initialization for GUI
	srand(io_read(AM_TIMER_UPTIME).us);

	uint32_t cur = colors[0];
	uint32_t dst = colors[rand() % 8];
	int steps = 60;
	int i = 0;
	long last = 0;
	while (1) {
		unsigned long now = io_read(AM_TIMER_UPTIME).us / 1000;	//now以毫秒为单位进行更新，每次循环都比上一次大一毫秒
		if (now - last < 1000/60) continue;//此处理解成双锚点，两锚点若相差还不足1000/60 毫秒，即1/60秒，则last暂时不跟进至now
		last = now;		//这两个数只是控制while循环的频率，不进入函数

		draw(cal_color_code(cur, dst, i, steps));
		i++;
		if(i > steps){
			cur = dst;
			dst = colors[rand() % 8];
			i = 0;
		}
	}
	return 0;
}