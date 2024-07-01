#ifndef CLEANUP_H
#define CLEANUP_H

typedef void (*cleanup_task_t)(void);

void cleanup_add(cleanup_task_t task);
void cleanup_remove(cleanup_task_t task);
void cleanup_execute(void);

#endif // CLEANUP_H
