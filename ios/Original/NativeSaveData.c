// SPDX-License-Identifier: GPL-3.0-or-later
#include "friend_group.h"
#include "friend_roster_internal.h"
#include "save_frontier.h"
#include "mail_message.h"

_Static_assert(sizeof(FriendRoster)==0x8C0 && sizeof(FrontierSave)==0x1628,"Original save mirrors");
_Static_assert(sizeof(MailMessageTemplate)==8,"Original tower message template");
SAV_FRIEND_GRP *gFriendGroupSave;
FriendRoster gFriendRoster;
_Alignas(4) FrontierSave gFrontierSave;
// asm/frontier_data_rodata.s, represented with the original field types.
_Alignas(4) const MailMessageTemplate gTowerMessageTemplates[4] = {
    {0,0,{4,-1},{7,0}}, {1,0,{4,-1},{33,0}}, {2,0,{8,-1},{10,0}}, {1,4,{4,-1},{1,0}}
};
