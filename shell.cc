#include "labs/shell.h"
#include "labs/vgatext.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "labs/coroutine.h"

// initialize shellstate
//
void shell_init(shellstate_t& state){
  state.keys_pressed = 0;
  for (int i=0;i<50;i++) {
    state.input[i] = '`';
    state.answer[i] = '`';
  }
  for (int i=0;i<5;i++) {
    state.process_status[i] = 0;
  }

  state.offset = 80;
  state.p_ret = 0;
  // state.long_computation_n = 0;
  state.flag = false;
  state.long_answer_computed = false;
  state.computing_answer = 0;
  state.received_long_command = 0;
  state.fiber_scheduler_counter = 0;
  state.init_process = 0;
  // state.running_processes = 0;
  // state.fiber_command = false;
}
//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//
char keytochar(uint8_t scankey)
{   switch(scankey)
    {     case 0x2: return '1'; break; case 0x13: return 'r'; break; case 0x24: return 'j'; break;
          case 0x3: return '2'; break; case 0x14: return 't'; break; case 0x25: return 'k'; break;
          case 0x4: return '3'; break; case 0x15: return 'y'; break; case 0x26: return 'l'; break;
          case 0x5: return '4'; break; case 0x16: return 'u'; break; case 0x2c: return 'z'; break;
          case 0x6: return '5'; break; case 0x17: return 'i'; break; case 0x2d: return 'x'; break;
          case 0x7: return '6'; break; case 0x18: return 'o'; break; case 0x2e: return 'c'; break;
          case 0x8: return '7'; break; case 0x19: return 'p'; break; case 0x2f: return 'v'; break;
          case 0x9: return '8'; break; case 0x1e: return 'a'; break; case 0x30: return 'b'; break;
          case 0xa: return '9'; break; case 0x1f: return 's'; break; case 0x31: return 'n'; break;
          case 0xb: return '0'; break; case 0x20: return 'd'; break; case 0x32: return 'm'; break;
          case 0x10: return 'q'; break; case 0x21: return 'f'; break; case 0x1: return '%'; break;
          case 0x11: return 'w'; break; case 0x22: return 'g'; break; case 0x1c: return '#'; break;
          case 0x12: return 'e'; break; case 0x23: return 'h'; break; case 0x39: return ' '; default: return '~'; break;
    }
}

void reset_state (shellstate_t& state) {
  if (state.received_long_command == 3 || state.received_long_command == 4) {
    state.init_process = 0;
  }
  state.offset += 160 - state.keys_pressed;
  state.keys_pressed = 0;
  for (int i=0; state.input[i] != '`';i++)
    state.input[i] = '`';
  for (int i=0; state.answer[i] != '`';i++)
    state.answer[i] = '`';
  state.flag = false;
  state.long_answer_computed = false;
  state.received_long_command = 0;
}

void shell_update(uint8_t scankey, shellstate_t& stateinout){
    // take in scankey and update corres char to input command
    // state to be updated is stateinout
    char val = keytochar(scankey);
    // if ubove if executes then below if will not call reset because below if sees stateinout.input array of previous iteration
    for (int i=0; stateinout.input[i] != '`';i++) {
      if (stateinout.input[i] == '#') 
        reset_state(stateinout);
    }
    int loc = stateinout.keys_pressed;
    if (stateinout.input[loc] == '`')
      stateinout.input[loc] = val;
    stateinout.keys_pressed++; 
    stateinout.offset++;
}

// void is_done(shellstate_t& stateinout) {
//   // if (stateinout.long_answer_computed && !stateinout.computing_answer) {
//     int ans = stateinout.p_ret;
//     int count = 0;
//     char ans_arr[10];
//     for(int i=0;i<10;i++){ans_arr[i]='`';}
//     while (ans) {
//       int temp = ans % 10;
//       ans_arr[count] = temp+'0';
//       ans = ans / 10;
//       count += 1;
//     }
//     hoh_debug("in shell = " << ans_arr);
//     for(int i=0;i<count;i++)
//       stateinout.answer[i] = ans_arr[count-i-1];
//     stateinout.flag = true;
//     // stateinout.long_computation_n = 0;
//     // stateinout.long_answer_computed = false;
//     // stateinout.received_long_command = false;
//     hoh_debug("the answer array is = " << stateinout.answer);
//     // hoh_debug("FLag val is: " << (int)stateinout.flag);
//   // }
// }

int fib (int argument) {
  if (argument == 0 || argument == 1) {
    return 1;
  }
  else return (fib (argument-1) + fib(argument-2));
}

int fact (int a) {
  int ans = 1;
  if (a==1 || a==0) return 1;
  else return (fact(a-1) * a);
}
 
int pow (int a, int b) {
  int ans = 1;
  for (int i=0;i<b;i++) 
    ans = ans*a;
  return ans;
}

int prime (int p) {
  int ans;
  for (int i=0;i<p;i++)
    for (int j=0;j<p;j++)
      for (int k=0;k<p;k++) 
        ans = ans + j*i*p;

  for (int i=0;i<p;i++)
    for (int j=0;j<p;j++)
        ans = ans - (ans % p);
  return ans;
}


int char_to_int (char func_arg[]) {
  int number = 0;
  int ans = 0;
  for (int i=0;i<10;i++) {
    if (func_arg[i] != '`') number += 1;
  }
  for (int i=0;i<number;i++) {
    ans = ans + (int)(func_arg[i]-'0')*(pow(10,(number-i-1)));
    // hoh_debug("ans in loop = "<<ans);
  }
  // hoh_debug("answer === "<<ans);
  return ans;
}

void echo(shellstate_t& stateinout)
{ 
  for(int i=5;i<100 && stateinout.input[i]!='#';i++)
     stateinout.answer[i-5] = stateinout.input[i];
  stateinout.flag = true;
}

void fibb(shellstate_t& stateinout)
{
  char a[10], ans_arr[10];int i;
  for(int i=0;i<10;i++){a[i]='`';ans_arr[i]='`';}
  for( i=4;stateinout.input[i]!=' ' && stateinout.input[i]!='#' && i<10; i++)
      a[i-4] = stateinout.input[i];
    int n = char_to_int(a);
    int ans = fib(n);
    int count = 0;
      while (ans ) {
        int temp = ans % 10;
        ans_arr[count] = temp+'0';
        ans = ans / 10;
        count += 1;
      }
      for(int i=0;i<count;i++)
        stateinout.answer[i] = ans_arr[count-i-1];
  stateinout.flag = true;
  hoh_debug("fib calculated");
}

void factt(shellstate_t& stateinout)
{
  char a[10], ans_arr[10];int i;
  for(int i=0;i<10;i++){a[i]='`';ans_arr[i]='`';}
  for( i=5;stateinout.input[i]!=' ' && stateinout.input[i]!='#' && i<10; i++)
      a[i-5] = stateinout.input[i];
    int n = char_to_int(a);
    int ans = fact(n);
    // hoh_debug("ans is "<<ans);
    int count = 0;
      while (ans ) {
        int temp = ans % 10;
        ans_arr[count] = temp+'0';
        ans = ans / 10;
        count += 1;
      }
      for(int i=0;i<count;i++)
        stateinout.answer[i] = ans_arr[count-i-1];
  stateinout.flag = true;
   // hoh_debug("FLag val is: " << (int)stateinout.flag);
}

void primee(shellstate_t& stateinout)
{
  char a[10], ans_arr[10];int i;
  for(int i=0;i<10;i++){a[i]='`';ans_arr[i]='`';}
  for( i=6;stateinout.input[i]!=' ' && stateinout.input[i]!='#' && i<10; i++)
      a[i-6] = stateinout.input[i];
    int n = char_to_int(a);
    // hoh_debug("arr a is " << a);
    int ans = prime(n);
    // hoh_debug("n for prime is " << n);
    // hoh_debug("ans for prime is "<<ans);
    int count = 0;
      while (ans ) {
        int temp = ans % 10;
        ans_arr[count] = temp+'0';
        ans = ans / 10;
        count += 1;
      }
      for(int i=0;i<count;i++)
        stateinout.answer[i] = ans_arr[count-i-1];
  stateinout.flag = true;
}

int get_func(shellstate_t& s)
{ 
    if(s.input[0]=='e' && s.input[1]=='c' && s.input[2]=='h' && s.input[3]=='o' && s.input[4]==' ')
      return 1;
    if(s.input[0]=='f' && s.input[1]=='i' && s.input[2]=='b' && s.input[3]==' ')
      return 2;
    if(s.input[0]=='f' && s.input[1]=='a' && s.input[2]=='c' && s.input[3]=='t' && s.input[4]==' ')
      return 3;
    if(s.input[0]=='p' && s.input[1]=='r' && s.input[2]=='i' && s.input[3]=='m' && s.input[4]=='e' && s.input[5]==' ')
      return 4;
    if(s.input[0]=='n' && s.input[1]=='p' && s.input[2]=='r' && s.input[3]=='i' && s.input[4]=='m' && s.input[5]=='e' && s.input[6]==' ')
      return 5;
    if(s.input[0]=='f' && s.input[1]=='p' && s.input[2]=='r' && s.input[3]=='i' && s.input[4]=='m' && s.input[5]=='e' && s.input[6]==' ')
      return 6;
    if(s.input[0]=='l' && s.input[1]=='o' && s.input[2]=='n' && s.input[3]=='g' && s.input[4]=='1' && s.input[5]==' ')
      return 7;
    if(s.input[0]=='l' && s.input[1]=='o' && s.input[2]=='n' && s.input[3]=='g' && s.input[4]=='2' && s.input[5]==' ')
      return 8;
    else return 0;
}

void shell_step(shellstate_t& stateinout) {
  // if(stateinout.flag)
  //     reset_state(stateinout);
  char val = stateinout.input[stateinout.keys_pressed-1];
  if (val == '#') {
      int func_id = get_func(stateinout);
      switch(func_id)
      { 
        case 1: echo(stateinout); break;
        case 2: fibb(stateinout); break;
        case 3: factt(stateinout); break;
        case 4: primee(stateinout); break;
        case 5: stateinout.received_long_command = 1; break;
        case 6: stateinout.received_long_command = 2; break;
        case 7: stateinout.received_long_command = 3; break;
        case 8: stateinout.received_long_command = 4; break;
        default: //garbage(stateinout);
        break;
      }
    }    
    return;  
  }

void shell_render(const shellstate_t& shell, renderstate_t& render){  
  char temp = shell.input[shell.keys_pressed-1];
  render.output = temp;
  render.flag = shell.flag;
  render.offset = shell.offset;
  render.keys_pressed = shell.keys_pressed;
  // hoh_debug("input array of shell is = " << shell.input);
  // hoh_debug("output while rendering = "<<render.output);
  // hoh_debug("flag while rendering = "<<render.flag);
  if (render.flag) {
    int i = 0;
    // hoh_debug("answer is shell is = " << shell.answer);
    render.offset = shell.offset-shell.keys_pressed+80;
    for (i=0; shell.answer[i] != '`';i++) 
      render.answer[i] = shell.answer[i];
    render.answer[i] = '`';
    // hoh_debug("answer in render is = " << render.answer);
  }
  return;
}

//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
  return false;//(a.offset==b.offset);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);

//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){
// print render state on screen using vgatext::write
// hoh_debug("offset ---> "<< state.offset);
// hoh_debug("final arr to be printed = "<<state.answer);
  if (!state.flag) 
    vgatext::writechar(state.offset, state.output ,w,h,vgatext_base);
  else {  
    // hoh_debug("I came here!");
    for (int i=0;state.answer[i]!='`' && i<100 ;i++){
      // hoh_debug("Character is: " << state.answer[i]);
      vgatext::writechar(state.offset+i,state.answer[i],w,h,vgatext_base);  
    }
  }
}

//
//
// helper functions
//
//

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}

