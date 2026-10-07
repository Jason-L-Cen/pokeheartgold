#pragma force_active on
// clang-format off
#include "global.h"
#include <nitro/code32.h>
// clang-format on
#include "to90_login_callback_private.h"
typedef struct{u32 word0,word1,word2;}DWCAcc;
typedef struct{int active;void*pointer;int a,b,c,d,e;u8 flags[4];int f,g,h;int cb[12];}State;
typedef struct{int a,b;State*state;int c;}Global;
typedef struct{int id;char a[31],b[21],c[31],d[31],e[51];}FriendMatch;
typedef struct{int status,count;u32 operation;FriendMatch*rows;}W39099Result;
extern Global ov00_0221A918;
extern u32 DWC_GetFriendDataType(DWCAcc*);
extern int ov00_021F09A4(u8*,int,int),ov00_02200540(GPConnection*,int,int*),ov00_021F0C48(u32),ov00_021F0B58(int);
extern void DWC_SetGsProfileId(void*,u32),DWCi_SetBuddyFriendData(void*),ov00_021F0550(int);
void ov00_021F0CBC(GPConnection*,void*,void*);
void ov00_021F0CBC(GPConnection*h,void*arg,void*param){W39099Result*r=arg;int index=(int)(u32)param;int i,profile;
 if(r->status==0&&r->count!=0&&DWC_GetFriendDataType(((DWCAcc *)(u32)ov00_0221A918.state->e)+index)!=0){
  if(ov00_0221A918.state->active!=1)return;
  for(i=0;i<r->count;i++){
   if(ov00_021F09A4((u8 *)(u32)ov00_0221A918.state->e,index,r->rows[i].id)){
    ov00_0221A918.state->flags[0]++;ov00_0221A918.state->flags[2]=1;r->operation=0x601;return;
   }
  }
  for(i=0;i<r->count;i++){
   ov00_021F0C48(ov00_02200540(h,r->rows[i].id,&profile));
   if(profile==-1)ov00_021F0B58(r->rows[i].id);
   else{
    DWC_SetGsProfileId(((DWCAcc *)(u32)ov00_0221A918.state->e)+index,r->rows[0].id);
    DWCi_SetBuddyFriendData(((DWCAcc *)(u32)ov00_0221A918.state->e)+index);
    ov00_021F0550(index);ov00_0221A918.state->flags[0]++;ov00_0221A918.state->flags[2]=1;
    r->operation=0x601;ov00_0221A918.state->flags[1]=1;return;
   }
  }
  if(r->operation==0x600)return;
  ov00_0221A918.state->flags[0]++;ov00_0221A918.state->flags[2]=1;
 }else{
  if(r->status!=0){if(ov00_021F0C48(r->status))return;}else{
  if(ov00_0221A918.state->active!=1&&DWC_GetFriendDataType(((DWCAcc *)(u32)ov00_0221A918.state->e)+index)!=0)return;
  ov00_0221A918.state->flags[0]++;ov00_0221A918.state->flags[2]=1;
  }
 }
}
