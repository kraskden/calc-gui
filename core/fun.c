#include "fun.h"
#include <stdlib.h>

void fun_init_base(fun_list **head)
{
    fun add;
    add.type = fun_base;
    add.par_amount = 1;
#define ADD_F(NAME) strcpy(add.name, (NAME)); PUSH(*head, add)
    ADD_F("sin"); ADD_F("cos"); ADD_F("tan"); ADD_F("ctg");
    ADD_F("asin"); ADD_F("acos"); ADD_F("atan"); ADD_F("actg");
    ADD_F("exp"); ADD_F("ln"); ADD_F("log10"); ADD_F("sqrt");
    add.par_amount = 2;
    ADD_F("logbase");
    //ADD_F("negative");
#undef ADD_F
}

const fun* name_to_fun(const fun_list *head, const char* name)
{
    for (; head; head = head->next) {
        if (!strcmp(name, head->function.name)) {
            return &(head->function);
        }
    }
    return NULL;
}

void fun_var_clear(fun_list *head)
{
    for (; head; head = head->next) {
        if (head->function.type == fun_extended) {
            CLEAR_VAR(head->function.par);
        }
    }
}

void fun_to_list(fun_list **head, fun arg)
{
    int is_exist = 0;
    fun_list *tmp;
    for (tmp = *head; tmp; tmp = tmp->next) {
        if ((is_exist = !strcmp(tmp->function.name, arg.name)))
            break;
    }
    if (is_exist) {
        tmp->function = arg;
    } else
        PUSH(*head, arg);
}

void fun_delete(fun_list **head, const char *name)
{
    if (!(*head))
        return;
    if (!strcmp((*head)->function.name, name)) {
        fun_list *to_delete = *head;
        *head = (*head)->next;
        free(to_delete);
        return;
    }
    fun_list *prev;
    fun_list *curr = NULL;
    for (prev = (*head); prev->next; prev = prev->next) {
        curr = prev->next;
        if (!strcmp(curr->function.name, name)) {
            prev->next = curr->next;
            free(curr);
            return;
        }
    }
}
