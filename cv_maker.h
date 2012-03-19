#define FALSE 0
#define TRUE !FALSE

void *create_editable(int type);
struct __div
{
    int printable;
    char attribute[80];
    char content[4096];
    char *fileds[16];
    struct __div *child;
    struct __div *next;
};
typedef struct __div _div;

typedef void editable;
struct _editable_basic
{
    int type;
    char content[64];
};
typedef struct _editable_basic editable_basic;

struct _editable_about
{
    int type;
    char content[2048];
};
typedef struct _editable_about editable_about;

struct _editable_skill
{
    int type;
    char skill_name[32];
    char skill_desp[2048];
};
typedef struct _editable_skill editable_skill;

struct _editable_minor
{
    int type;
    char minor_name_and_level[32];
    char minor_desp[2048];
};
typedef struct _editable_minor editable_minor;

struct _editable_project
{
    int type;
    char project_name[64];
    char project_date[32];
    char project_desp[2048];
};
typedef struct _editable_project editable_project;

struct _editable_edu
{
    int type;
    char edu_basic[1024];
    char edu_more[2048];
};
typedef struct _editable_edu editable_edu;
#define EDITABLE_BASIC   0x1 << 1 //including name, postion, mail, phone 
#define EDITABLE_PHONE   0x1 << 2
#define EDITABLE_NAME    0x1 << 3
#define EDITABLE_POSTION 0x1 << 4
#define EDITABLE_MAIL    0x1 << 5
#define EDITABLE_ABOUT   0x1 << 6
#define EDITABLE_SKILL   0x1 << 7
#define EDITABLE_MINOR   0x1 << 8
#define EDITABLE_PROJECT 0x1 << 9
#define EDITABLE_EDU     0x1 << 10
#define EDITABLE_TYPE(e)    (*((int *)e))

#define DIV_UPDATE_REPLACE 0xff
#define DIV_UPDATE_APPEND_BASIC     0xee
#define DIV_UPDATE_APPEND_ADVANCE   0xaa
#define PRIVATE_SETTER_NAME(name) "__"#name"__"

#define SETTER_INIT(name) find_div(root, PRIVATE_SETTER_NAME(name))
#define SET_FILED(setter, i, type, e, textarea, update_method)                          \
do                                                                                      \
{                                                                                       \
    if((update_method == DIV_UPDATE_REPLACE)                                            \
     ||(update_method == DIV_UPDATE_APPEND_ADVANCE))                                    \
        setter->fileds[i] = ((type *)e)->textarea;                                      \
    else if(update_method == DIV_UPDATE_APPEND_BASIC)                                   \
        setter->fileds[i] = strjoin(setter->fileds[i], "\n",                            \
                                    setter->fileds[i], ((type *)e)->textarea, NULL);    \
    else                                                                                \
        printf("invalid update_method %08x\n", update_method);                          \
}while(0)

#define APPEND_DIV(head, tail)          \
do                                      \
{                                       \
    typeof(head) *_p = &(head->next);   \
    while(*_p)                          \
    {                                   \
        _p = &((*_p)->next);            \
    }                                   \
    (*_p) = tail;                       \
    tail->next = NULL;                  \
}while(0)

