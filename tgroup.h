#ifndef TGROUP_H
#define TGROUP_H

#include <iostream>
#include <string>
#include "dlst.h"

void add_task(std::string &tname,struct mandel_task_info *ti);
void get_task(std::string *tname,struct mandel_task_info *ti);

void add_tgid(std::string &tgid);
struct dlst *get_task_list(std::string &tgid);
struct dlst *get_result_list(std::string &tgid);
bool is_tgid(std::string &tgid);
void rm_tgid(std::string &tgid);
int get_tgid_num(void);
int get_first_tgid(std::string *gettgid);

#endif
