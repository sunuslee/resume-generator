#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cv_maker.h"
#include "utility.h"

_div divs[128];
int divs_idx = 0;

void get_div_attribute(char *dest, char *line)
{
    while(*line != '<')
        line++;
    while(*line != '>')
        *dest++ = *line++;
    *dest = '>';
    *(dest+1) = '\0';
}
int get_divs(char *input, int offset)
{
    char line[1024];
    _div *self = NULL;
    _div **join;
    while(1)
    {
        offset += readline(input + offset, line);
        if(line[2] == EOF) /* here might be dangerous! */
            break;
        if(line[0] == '\n')
            continue;
        if(strstr(line, "<div "))
        {
            if(self == NULL)
            {
                self = &divs[divs_idx];
                join = &(self->child);
                get_div_attribute(self->attribute, line);
            }
            else
            {
                while(*join)
                    join = &((*join)->next);
                divs_idx++;
                *join = &(divs[divs_idx]);
                offset = get_divs(input, offset - strlen(line));
            }
        }
        else if(strstr(line, "</div>"))
        {
            return offset;
        }
        else
        {
            strcat(self->content, line);
        }
    }
    return -1;
}
_div *find_div(_div *root, const char *name)
{
    _div *result = NULL;
    _div *cur = root;
    if(strstr(cur->content, name))
        return cur;
    if(cur->child)
        result = find_div(cur->child, name);
    if(!result && cur->next)
        result = find_div(cur->next, name);
    return result;
}


static _div *root;
static _div *set_name, *set_postion, *set_project,
            *set_mail, *set_phone,   *set_about,
            *set_skill, *set_minor,  *set_edu,
            *set_footer;
static _div *set_name_postion;
static _div *set_contact;
void setters_init()
{
    root        = &divs[0];
    set_name    = SETTER_INIT(name);
    set_postion = SETTER_INIT(postion);
    set_phone   = SETTER_INIT(phone);
    set_mail    = SETTER_INIT(mail);
    set_about   = SETTER_INIT(about);
    set_skill   = SETTER_INIT(skills);
    set_project = SETTER_INIT(projects);
    set_minor   = SETTER_INIT(minors);
    set_edu     = SETTER_INIT(edu);
    set_footer  = SETTER_INIT(footer);
    set_contact = set_phone;
    set_name_postion = set_name;
    set_minor->printable = set_skill->printable =  set_project->printable = -1;
}

#define SETTERCAT(setter, d)    \
    do                              \
{                               \
    _div *join;                 \
    join = d->next;             \
    APPEND_DIV(setter, d);      \
    d->next = join;             \
}while(0)
void to_editablediv(_div *d, int type)
{
    editable *ed1, *ed2;
    if(type & EDITABLE_BASIC)
    {

        ed1 = create_editable(EDITABLE_BASIC);
        ed2 = create_editable(EDITABLE_BASIC);
        if(type & EDITABLE_PHONE)
        {
            // it modifies phone and mail 
            strbetween(((editable_basic *)ed1)->content, d->content, "<h3>", "</h3>", 2);
            strbetween(((editable_basic *)ed2)->content, d->content, "mailto:", "\"", 1);
            SET_FILED(d, 2, editable_basic, ed1, content, DIV_UPDATE_REPLACE);
            SET_FILED(d, 0, editable_basic, ed2, content, DIV_UPDATE_REPLACE);
            SET_FILED(d, 1, editable_basic, ed2, content, DIV_UPDATE_REPLACE);
            strcpy(d->content, set_contact->content);
            set_contact = set_phone = set_mail = d;
        }
        else if(type & EDITABLE_NAME)
        {
            /* in fact, it modifies name and postion */
            strbetween(((editable_basic *)ed1)->content, d->content, "<h1>", "</h1>", 1);
            strbetween(((editable_basic *)ed2)->content, d->content, "<h2>", "</h2>", 1);
            SET_FILED(d, 0, editable_basic, ed1, content, DIV_UPDATE_REPLACE);
            SET_FILED(d, 1, editable_basic, ed2, content, DIV_UPDATE_REPLACE);
            strcpy(d->content, set_name_postion->content);
            set_name = set_postion = set_name_postion = d;
        }
    }
    else if(type & EDITABLE_ABOUT)
    {
        ed1 = create_editable(EDITABLE_ABOUT);
        strbetween(((editable_about *)ed1)->content, d->content, "<p class=\"enlarge\">", "</p>", 1);
        SET_FILED(d, 0, editable_about, ed1, content, DIV_UPDATE_REPLACE);
        strcpy(d->content, set_about->content);
        set_about = d;
    }
    else if(type & EDITABLE_SKILL)
    {
        SETTERCAT(set_skill, d);
        while(1)
        {
            ed1 = create_editable(EDITABLE_SKILL);
            strbetween(((editable_skill *)ed1)->skill_name, d->content, "<h2>", "</h2>", 1);
            strbetween(((editable_skill *)ed1)->skill_desp, d->content, "<p>", "</p>", 1);
            SET_FILED(d, 0, editable_skill, ed1, skill_name, DIV_UPDATE_APPEND_ADVANCE);
            SET_FILED(d, 1, editable_skill, ed1, skill_desp, DIV_UPDATE_APPEND_ADVANCE);
            strcpy(d->content, set_skill->content);
            if(d->next)
                d = d->next;
            else
                break;
        }
    }
    else if(type & EDITABLE_MINOR)
    {
        SETTERCAT(set_minor, d);
        while(1)
        {
            ed1 = create_editable(EDITABLE_MINOR);
            strbetween(((editable_minor *)ed1)->minor_name_and_level, d->content, "<li class=\"last\">", "</li>", 1);
            strbetween(((editable_minor *)ed1)->minor_desp, d->content, "<li class=\"last\">", "</li>", 2);
            SET_FILED(d, 0, editable_minor, ed1, minor_name_and_level, DIV_UPDATE_APPEND_ADVANCE);
            SET_FILED(d, 1, editable_minor, ed1, minor_desp, DIV_UPDATE_APPEND_ADVANCE);
            strcpy(d->content, set_minor->content);
            if(d->next)
                d = d->next;
            else
                break;
        }
    }
    else if(type & EDITABLE_PROJECT)
    {
        SETTERCAT(set_project, d);
        while(1)
        {
            ed1 = create_editable(EDITABLE_PROJECT);
            strbetween(((editable_project *)ed1)->project_name, d->content, "<h2>", "</h2>", 1);
            strbetween(((editable_project *)ed1)->project_date, d->content, "<h4>", "</h4>", 1);
            strbetween(((editable_project *)ed1)->project_desp, d->content, "<p>", "</p>", 1);
            SET_FILED(d, 0, editable_project, ed1, project_name, DIV_UPDATE_APPEND_ADVANCE);
            SET_FILED(d, 1, editable_project, ed1, project_date, DIV_UPDATE_APPEND_ADVANCE);
            SET_FILED(d, 2, editable_project, ed1, project_desp, DIV_UPDATE_APPEND_ADVANCE);
            strcpy(d->content, set_project->content);
            if(d->next)
                d = d->next;
            else
                break;
        }
    }
    else if(type & EDITABLE_EDU)
    {
        ed1 = create_editable(EDITABLE_EDU);
        strbetween(((editable_edu *)ed1)->edu_basic, d->content, "<h2>", "</h2>", 1);
        strbetween(((editable_edu *)ed1)->edu_more, d->content, "<p>", "</p>", 1);
        SET_FILED(d, 0, editable_edu, ed1, edu_basic, DIV_UPDATE_REPLACE);
        SET_FILED(d, 1, editable_edu, ed1, edu_more, DIV_UPDATE_REPLACE);
        strcpy(d->content, set_edu->content);
    }
}
void setters_init_from_templete(int root_idx)
{
    _div *p;
    root        = &divs[root_idx];
    /* name and postion*/
    p           = SETTER_INIT(name);
    to_editablediv(p, EDITABLE_BASIC|EDITABLE_NAME);

    /* phone and mail */
    p           = SETTER_INIT(phone);
    to_editablediv(p, EDITABLE_BASIC|EDITABLE_PHONE);

    p   = SETTER_INIT(about);
    to_editablediv(p, EDITABLE_ABOUT);

    p   = SETTER_INIT(skills);
    to_editablediv(p, EDITABLE_SKILL);

    p   = SETTER_INIT(minors);
    to_editablediv(p, EDITABLE_MINOR);

    p   = SETTER_INIT(projects);
    to_editablediv(p, EDITABLE_PROJECT);

    p   = SETTER_INIT(edu);
    to_editablediv(p, EDITABLE_EDU);

    set_footer  = SETTER_INIT(footer);
    set_contact = set_phone;
    set_name_postion = set_name;
    set_minor->printable = set_skill->printable =  set_project->printable = -1;
}
void *create_editable(int type)
{
    void *p = NULL;
    if(type & (EDITABLE_BASIC|EDITABLE_PHONE|EDITABLE_MAIL|EDITABLE_NAME|EDITABLE_POSTION))
    {
        MALLOC_S(editable_basic, p, sizeof(editable_basic));
        type |= EDITABLE_BASIC;
    }
    else if(type & EDITABLE_ABOUT)
    {
        MALLOC_S(editable_about, p, sizeof(editable_about));
    }
    else if(type & EDITABLE_SKILL)
    {
        MALLOC_S(editable_skill, p, sizeof(editable_skill));
    }
    else if(type & EDITABLE_MINOR)
    {
        MALLOC_S(editable_minor, p, sizeof(editable_minor));
    }
    else if(type & EDITABLE_PROJECT)
    {
        MALLOC_S(editable_project, p, sizeof(editable_project));
    }
    else if(type & EDITABLE_EDU)
    {
        MALLOC_S(editable_edu, p, sizeof(editable_edu));
    }
    else
    {
        printf("UNKNOWN EDITABLE TYPE! %08x\n", type);
        return NULL;
    }
    EDITABLE_TYPE(p) = type;
    return p;
}

#define SET_EDITABLE_MSG(e, message, type, textarea)    printf(message);\
    readtext(((type *)e)->textarea)
#define get_new_div(pdiv, from) (pdiv = &(divs[++divs_idx]), *pdiv = *from)


int set_editable_to_div(void *editable, int update_method)
{
    int type = EDITABLE_TYPE(editable);
    _div *pnewdiv;
    if(type & EDITABLE_BASIC)
    {
        if(type & EDITABLE_PHONE)
        {
            SET_EDITABLE_MSG(editable, "input phone:\n", editable_basic, content);
            SET_FILED(set_contact, 2, editable_basic, editable, content, update_method);
            set_footer->fileds[3] = set_contact->fileds[2];
            get_new_div(pnewdiv, set_contact);
        }
        else if(type & EDITABLE_MAIL)
        {
            SET_EDITABLE_MSG(editable, "input mail:\n", editable_basic, content);
            SET_FILED(set_contact, 0, editable_basic, editable, content, update_method);
            SET_FILED(set_contact, 1, editable_basic, editable, content, update_method);
            SET_FILED(set_footer, 1, editable_basic, editable, content, update_method);
            SET_FILED(set_footer, 2, editable_basic, editable, content, update_method);
            get_new_div(pnewdiv, set_contact);
        }
        else if(type & EDITABLE_NAME)
        {
            SET_EDITABLE_MSG(editable, "input name:\n", editable_basic, content);
            SET_FILED(set_name_postion, 0, editable_basic, editable, content, update_method);
            SET_FILED(set_footer, 0, editable_basic, editable, content, update_method);
            get_new_div(pnewdiv, set_name_postion);
        }
        else if(type & EDITABLE_POSTION)
        {
            SET_EDITABLE_MSG(editable, "input postion:\n", editable_basic, content);
            SET_FILED(set_name_postion, 1, editable_basic, editable, content, update_method);
            get_new_div(pnewdiv, set_name_postion);
        }
    }
    else if(type & EDITABLE_ABOUT)
    {
        SET_EDITABLE_MSG(editable, "input about:\n", editable_about, content);
        SET_FILED(set_about, 0, editable_about, editable, content, update_method);
        get_new_div(pnewdiv, set_about);
    }
    else if(type & EDITABLE_SKILL)
    {
        SET_EDITABLE_MSG(editable, "input skill name:\n", editable_skill, skill_name);
        SET_EDITABLE_MSG(editable, "input skill description:\n", editable_skill, skill_desp);
        SET_FILED(set_skill, 0, editable_skill, editable, skill_name, update_method);
        SET_FILED(set_skill, 1, editable_skill, editable, skill_desp, update_method);
        get_new_div(pnewdiv, set_skill);
        pnewdiv->printable = TRUE;
        APPEND_DIV(set_skill, pnewdiv);
    }
    else if(type & EDITABLE_MINOR)
    {
        SET_EDITABLE_MSG(editable, "input minor name and level:\n", editable_minor, minor_name_and_level);
        SET_EDITABLE_MSG(editable, "input minor description:\n", editable_minor, minor_desp);
        get_new_div(pnewdiv, set_minor);
        SET_FILED(pnewdiv, 0, editable_minor, editable, minor_name_and_level, update_method);
        SET_FILED(pnewdiv, 1, editable_minor, editable, minor_desp, update_method);
        pnewdiv->printable = TRUE;
        APPEND_DIV(set_minor, pnewdiv);
    }
    else if(type & EDITABLE_PROJECT)
    {
        SET_EDITABLE_MSG(editable, "input project name:\n", editable_project, project_name);
        SET_EDITABLE_MSG(editable, "input project date:\n", editable_project, project_date);
        SET_EDITABLE_MSG(editable, "input project description:\n", editable_project, project_desp);
        SET_FILED(set_project, 0, editable_project, editable, project_name, update_method);
        SET_FILED(set_project, 1, editable_project, editable,project_date, update_method);
        SET_FILED(set_project, 2, editable_project, editable, project_desp, update_method);
        get_new_div(pnewdiv, set_project);
        pnewdiv->printable = TRUE;
        APPEND_DIV(set_project, pnewdiv);
    }
    else if(type & EDITABLE_EDU)
    {
        SET_EDITABLE_MSG(editable, "input edu_basic info:\n", editable_edu, edu_basic);
        SET_EDITABLE_MSG(editable, "input more edu info:\n", editable_edu, edu_more);
        SET_FILED(set_edu, 0, editable_edu, editable, edu_basic, update_method);
        SET_FILED(set_edu, 1, editable_edu, editable, edu_more, update_method);
        get_new_div(pnewdiv, set_edu);
    }
    return 0;
}

#define PF(d, i)    (d->fileds[i])
char pagebuf[40960];
char *ppagebuf = pagebuf;
void print_page(_div *d, int with_header_footer)
{
    static char header_footer_buf[1024];
    static int header_footer_flag;
    char *phff = NULL;
    char title[128];
    if(with_header_footer == TRUE)
        header_footer_flag = -1;
    if(header_footer_flag == -1)
    {
        sprintf(title, "<title>%s | %s | %s</title>", PF(set_name_postion, 0), PF(set_name_postion, 1), PF(set_contact, 0));// name, postion, mail
        phff = strjoin(header_footer_buf, "\n",
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">",
                "<!-- this resume is generated by sunus resume generator, he writes this software in order to get a pretty resume and a dream inter job:) -->"
                "<html>",
                "<head>",
                (char *)title,
                "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\" />",
                "<meta name=\"keywords\" content=\"\" />",
                "<meta name=\"description\" content=\"\" />",
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"reset-fonts-grids.css\" media=\"all\" />",
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"resume.css\" media=\"all\" />",
                "</head>",
                "<body>",
                NULL);
        ppagebuf += sprintf(ppagebuf, "%s\n", phff);
        phff = strjoin(header_footer_buf, "\n",
                "</body>",
                "</html>",
                NULL);
        header_footer_flag = 0x73756e75;
    }
    if(d->printable != -1)
    {
        ppagebuf += sprintf(ppagebuf, "%s\n", d->attribute);
        if(d->content)
        {
            ppagebuf += sprintf(ppagebuf, d->content, PF(d, 0), PF(d, 1), PF(d, 2), PF(d, 3), PF(d, 4));
        }
    }
    if(d->child)
        print_page(d->child, 0);
    if(d->printable != -1)
        ppagebuf += sprintf(ppagebuf, "</div>\n");
    if(d->next)
        print_page(d->next, 0);
    if(phff)
    {
        ppagebuf += sprintf(ppagebuf, "%s\n", phff);
        printf("\n%s", pagebuf);
    }
}
// do some misc, cleanup stuffs
void finishoff(_div *root)
{
    //change the last project class from job to job last
    _div *d;
    for(d = set_project; d->next; d = d->next)
        ;
    strrep(d->attribute, "job", "job last", 1);
    printf("new:\n%s", d->attribute);
    free_all();
}
int main()
{
    char opt[32];
    int update_method;
    FILE *fd_templete;
    FILE *fd_edit;
    editable *ed;
    fd_templete = fopen("templete.html", "r");
    fread(pagebuf, sizeof(pagebuf), 1, fd_templete);
    fclose(fd_templete);
    get_divs(pagebuf, 0);
    setters_init();
    while(1)
    {
        printf("Option:\n");
        scanf("%s %c%*c", opt, &opt[31]);
        if(!strcmp(opt, "name"))
        {
            ed = create_editable(EDITABLE_BASIC|EDITABLE_NAME);
        }
        else if(!strcmp(opt, "postion"))
        {
            ed = create_editable(EDITABLE_BASIC|EDITABLE_POSTION);
        }
        else if(!strcmp(opt, "mail"))
        {
            ed = create_editable(EDITABLE_BASIC|EDITABLE_MAIL);
        }
        else if(!strcmp(opt, "phone"))
        {
            ed = create_editable(EDITABLE_BASIC|EDITABLE_PHONE);
        }
        else if(!strcmp(opt, "about"))
        {
            ed = create_editable(EDITABLE_ABOUT);
        }
        else if(!strcmp(opt, "skill"))
        {
            ed = create_editable(EDITABLE_SKILL);
        }
        else if(!strcmp(opt, "minor"))
        {
            ed = create_editable(EDITABLE_MINOR);
        }
        else if(!strcmp(opt, "project"))
        {
            ed = create_editable(EDITABLE_PROJECT);
        }
        else if(!strcmp(opt, "edu"))
        {
            ed = create_editable(EDITABLE_EDU);
        }
        else if(!strcmp(opt, "load"))
        {
            FILE *fd_load;
            int offset;
            int root_idx;
            printf("load from file:\n");
            scanf("%s", opt);
            fd_load = fopen(opt, "r");
            fread(pagebuf, sizeof(pagebuf), 1, fd_load);
            offset = (unsigned int)strstr(pagebuf, "<div") - (unsigned int)pagebuf;
            if(offset < 0)
                printf("Load file error!\n");
            root_idx = ++divs_idx;
            get_divs(pagebuf, offset);
            setters_init_from_templete(root_idx);
            fclose(fd_load);
            continue;
        }
        else if(!strcmp(opt, "done"))
        {
            break;
        }
        else
        {
            printf("invalid command!\n");
            continue;
        }
        if(opt[31] == 'r')
        {
            update_method = DIV_UPDATE_REPLACE;
        }
        else if(opt[31] == 'a')
        {
            if(EDITABLE_TYPE(ed) & (EDITABLE_PHONE|EDITABLE_MAIL|EDITABLE_NAME|EDITABLE_POSTION))
                update_method = DIV_UPDATE_APPEND_BASIC;
            else
                update_method = DIV_UPDATE_APPEND_ADVANCE;
        }
        else
        {
            printf("invalid update_method!\n");
        }
        if(set_editable_to_div(ed, update_method) == 0)
            printf("updating successed!\n");
        else
            printf("updating failed!\n");
    }
    finishoff(root);
    print_page(root, TRUE);
    fd_edit = fopen("r.html", "wb");
    fwrite(pagebuf, strlen(pagebuf) - 1, 1, fd_edit);
    fclose(fd_edit);
    return 0;
}
