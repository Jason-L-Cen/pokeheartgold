#include <nitro/types.h>
#include <nitro/code32.h>
#include "tree-iterator4.h"
#pragma force_active on
static inline void *Left(void *node){return *reinterpret_cast<void**>(static_cast<unsigned char*>(node));}
static inline void *Right(void *node){return *reinterpret_cast<void**>(static_cast<unsigned char*>(node)+4);}
static inline void *Parent(void *node){return reinterpret_cast<void*>(*reinterpret_cast<u32*>(static_cast<unsigned char*>(node)+8)&~1UL);}
void Iterator::Advance(){if(Right(current)){void*next=Right(current);void*left;while((left=Left(next))!=0)next=left;current=next;return;}void *node=current;void *parent=Parent(node);while(node!=Left(parent)){current=parent;node=current;parent=Parent(node);}current=parent;}
