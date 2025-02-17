#include "stm32f10x.h"
#include "adxl345.h"
#include "ledmatrix.h"
#include "math.h"
#include "LedControl.h"
#include "stdlib.h"
#include "mydelay.h"


#define	MATRIX_A	0
#define	MATRIX_B	1
int gravity;
uint8_t flag = 0;

// 假设传感器返回的俯仰角和滚转角的范围是[-90, 90]度
float pitch = 0.0f;  // 俯仰角
float roll = 0.0f;   // 滚转角

void single() {
	int row,col,i;
  for(row=0;row<8;row++) {
    for(col=0;col<8;col++) {
      delay_ms(100);
      setLed(0,row,col,1);
      delay_ms(100);
      for(i=0;i<col;i++) {
        setLed(0,row,col,0);
        delay_ms(100);
        setLed(0,row,col,1);
        delay_ms(100);
      }
    }
  }
}

coord getDown(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y+1;
  return xy;
}
coord getLeft(int x, int y) {
  coord xy;
  xy.x = x-1;
  xy.y = y;
  return xy;
}
coord getRight(int x, int y) {
  coord xy;
  xy.x = x;
  xy.y = y+1;
  return xy;
}


uint8_t canGoLeft(int addr, int x, int y) {
  if (x == 0) return 0; // not available
  return !_getXY(addr, getLeft(x, y)); // you can go there if this is empty
}
uint8_t canGoRight(int addr, int x, int y) {
  if (y == 7) return 0; // not available
  return !_getXY(addr, getRight(x, y)); // you can go there if this is empty
}
uint8_t canGoDown(int addr, int x, int y) {
  if (y == 7) return 0; // not available
  if (x == 0) return 0; // not available
  if (!canGoLeft(addr, x, y)) return 0;
  if (!canGoRight(addr, x, y)) return 0;
  return !_getXY(addr, getDown(x, y)); // you can go there if this is empty
}

void goDown(int addr, int x, int y) {
  setXY(addr, x, y, 0);
  _setXY(addr, getDown(x,y), 1);
}
void goLeft(int addr, int x, int y) {
  setXY(addr, x, y, 0);
  _setXY(addr, getLeft(x,y), 1);
}
void goRight(int addr, int x, int y) {
  setXY(addr, x, y, 0);
  _setXY(addr, getRight(x,y), 1);
}


int countParticles(int addr) {
  int c = 0;
  byte y,x;
  for (y=0; y<8; y++) {
    for (x=0; x<8; x++) {
      if (getXY(addr, x, y)) {
        c++;
      }
    }
  }
  return c;
}



uint8_t moveParticle(int addr, int x, int y)
{
  uint8_t can_GoLeft;
  uint8_t can_GoRight;
  uint8_t can_GoDown;
  if (!getXY(addr,x,y))//获取x,y坐标对应的灯珠的状态，为1才可以移动
  {
    return 0;
  }
  //判断灯珠左右是否没有点亮，是则可以移动
  can_GoLeft = canGoLeft(addr, x, y);
  can_GoRight = canGoRight(addr, x, y);

  if (!can_GoLeft && !can_GoRight) {
    return 0; // we're stuck
  }

  can_GoDown = canGoDown(addr, x, y);
  //灯珠可以往下就往下走，否则往左或右走，如果左右都可以走就随机选一个
  if (can_GoDown) {
    goDown(addr, x, y);
  } else if (can_GoLeft&& !can_GoRight) {
    goLeft(addr, x, y);
  } else if (can_GoRight && !can_GoLeft) {
    goRight(addr, x, y);
  } else if ((rand() % 2) == 1) { // we can go left and right, but not down
    goLeft(addr, x, y);
  } else {
    goRight(addr, x, y);
  }
  return 1;
}

//填充64个点(一个led矩阵)
//为什么不按顺序操作？以(7,7)为起点斜着点亮所用LED这样可以留出顶角空四个，
//如果按顺序点亮就会导致不亮的四个在同一行
//setled确定那几个灯不点亮
void fill(int addr, int maxcount)
{
  int n = 8;
  byte x,y,z,j,slice;
  int count = 0;
  for (slice = 0; slice < 2*n-1; ++slice)
  {
    z = slice<n ? 0 : slice-n + 1;
    for (j = z; j <= slice-z; ++j)
	{
      y = 7-j;
      x = (slice-j);
      setXY(addr, x, y, (++count <= maxcount));//maxcount = 60 留四个灯不点亮 此时要点亮的灯位置已经确定
	  //delay_ms(200);
    }
  }
}
//-90~90之间取值
int getGravity() {
	float pitch,roll;
	get_angle(&pitch,&roll);
  if (pitch < -45)  { return 0;   }
  if (roll > 45) { return 90;  }
  if (pitch > 45) { return 180; }
  if (roll < -45)  { return 270; }
}

int getTopMatrix() {
  return (getGravity() == 90) ? MATRIX_A : MATRIX_B;
}

int getBottomMatrix() {
  return (getGravity() != 90) ? MATRIX_A : MATRIX_B;
}

//清屏然后点亮60个灯珠
void resetTime() {
	byte i;
  for (i=0; i<2; i++) {
    clearDisplay(i);
  }
  fill(getTopMatrix(), 60);
 // NonBlockDelay_Delay_ms(1000);
}

/**
 * Traverse matrix and check if particles need to be moved
 */
uint8_t updateMatrix() {
  int n = 8;
  uint8_t somethingMoved = 0;
  byte x,y,z,j,slice;
  uint8_t direction;
  for (slice = 0; slice < 2*n-1; ++slice)//也是更新了64个点
  {
    direction = ((rand() % 2) == 1); // randomize if we scan from left to right or from right to left, so the grain doesn't always fall the same direction
     z = slice<n ? 0 : slice-n + 1;
    for (j = z; j <= slice-z; ++j)
	{
      y = direction ? (7-j) : (7-(slice-j));
      x = direction ? (slice-j) : j;
      if (moveParticle(MATRIX_B, x, y))
	  {
        somethingMoved = 1;
      }
      if (moveParticle(MATRIX_A, x, y))
	  {
        somethingMoved = 1;
      }
    }
  }
  return somethingMoved;
}


/**
 * Let a particle go from one matrix to the other
 */
uint8_t dropParticle()
{
  if (NonBlockDelay_Timeout()) {
    NonBlockDelay_Delay_ms(1000);
    if (gravity == 0 || gravity == 180) {
      if ((getRawXY(MATRIX_A, 0, 0) && !getRawXY(MATRIX_B, 7, 0)) ||//获取灯珠的状态 模块A的（0，0）亮模块B的(7,0)不亮或者 模块A的(0,0)不亮模块B的(7,0)亮才可以掉落
          (!getRawXY(MATRIX_A, 0,0) && getRawXY(MATRIX_B, 7, 0))
      ) {
        invertRawXY(MATRIX_A, 0, 0);
        invertRawXY(MATRIX_B, 7, 0);
        return 1;
      }
    }
  }
  return 0;
}



 int main(void)
 {	
	 delay_init();
	 ADXL345_init();
	 LedControlInit();
	 resetTime();
	 TIM3_Int_Init(99,719);//定时1ms 
  while(1)
	{
	   gravity = getGravity();
	   setRotation((0 + gravity) % 360);
	   updateMatrix();
	   dropParticle();
	}
 }
 
 

