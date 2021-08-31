// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
// Copyright 2021 Rasmus Edgar
int (*fstrncmp)(const char *, const char *, size_t) = strncmp;
char *(*fstrstr)(const char *, const char *) = strstr;
void appenditem(struct item *item, struct item **list, struct item **last);
