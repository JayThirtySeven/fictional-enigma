#ifndef anim_dict_h
#define anim_dict_h

#include <wchar.h>
#include "anim.h"

typedef struct anim_node_t anim_node_t;
struct anim_node_t{
    anim_t *data;
    anim_node_t *next;
    wchar_t name[32];
};

typedef struct anim_dict_t anim_dict_t;
struct anim_dict_t{
    anim_node_t *head;
};

anim_dict_t *anim_dict_create(void);
anim_t *anim_dict_get(anim_dict_t *dict, const wchar_t *name);
void anim_dict_delete(anim_dict_t *dict);

#endif
