#include <string.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef struct __element {
    char *value;
    struct list_head list;
} list_ele_t;

typedef struct {
    struct list_head list;
} queue_t;

static list_ele_t *get_middle(struct list_head *list)
{
    struct list_head *fast = list->next, *slow;
    list_for_each (slow, list) {
        if (fast->next == list || fast->next->next == list)
            break;
        fast = fast->next->next;
    }
    return list_entry(slow, list_ele_t, list);
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);
    if (list_empty(lhs)) {
        list_splice_tail(lhs, head);
        return;
    }
    if (list_empty(rhs)) {
        list_splice_tail(rhs, head);
        return;
    }

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = list_entry(lhs->next, list_ele_t, list)->value;
        char *rv = list_entry(rhs->next, list_ele_t, list)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(queue_t *q)
{
    if (list_is_singular(&q->list))
        return;

    queue_t left;
    struct list_head sorted;
    INIT_LIST_HEAD(&left.list);
    list_cut_position(&left.list, &q->list, &get_middle(&q->list)->list);
    list_merge_sort(&left);
    list_merge_sort(q);
    list_merge(&left.list, &q->list, &sorted);
    INIT_LIST_HEAD(&q->list);
    list_splice_tail(&sorted, &q->list);
}

/*
 * testing
 */

static bool validate(queue_t *q)
{
    struct list_head *node;
    list_for_each (node, &q->list) {
        if (node->next == &q->list)
            break;
        if (strcmp(list_entry(node, list_ele_t, list)->value,
                   list_entry(node->next, list_ele_t, list)->value) > 0)
            return false;
    }
    return true;
}

static queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q) return NULL;

    INIT_LIST_HEAD(&q->list);
    return q;
}

static void q_free(queue_t *q)
{
    if (!q) return;

    struct list_head *current = q->list.next;
    while (current != &q->list) {
        struct list_head *tmp = current;
        current = current->next;
        free(list_entry(tmp, list_ele_t, list)->value);
        free(list_entry(tmp, list_ele_t, list));
    }
    free(q);
}

bool q_insert_head(queue_t *q, char *s)
{
    if (!q) return false;

    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    newh->value = new_value;
    list_add_tail(&newh->list, q->list.next);

    return true;
}

static void q_show(queue_t *q)
{
    struct list_head *node;
    list_for_each (node, &q->list) {
        printf("%s", list_entry(node, list_ele_t, list)->value);
    }
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    queue_t *q = q_new();
    char buf[256];
    while (fgets(buf, 256, fp))
        q_insert_head(q, buf);
    fclose(fp);

    list_merge_sort(q);
    q_show(q);
    assert(validate(q));

    q_free(q);

    return 0;
}
