#include <string.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef struct __element {
    struct list_head list;
    /* Put struct list_head in the initial(first) member */
    char *value;
} list_ele_t;

static struct list_head *get_middle(struct list_head *head)
{
    struct list_head *fast = head->next, *slow = head->next;
    while (fast->next != head && fast->next->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

static void list_merge(struct list_head *lhs,
                       struct list_head *rhs,
                       struct list_head *head)
{
    INIT_LIST_HEAD(head);

    while (!list_empty(lhs) && !list_empty(rhs)) {
        char *lv = ((list_ele_t *)lhs->next)->value;
        char *rv = ((list_ele_t *)rhs->next)->value;
        struct list_head *tmp = strcmp(lv, rv) <= 0 ? lhs->next : rhs->next;
        list_del(tmp);
        list_add_tail(tmp, head);
    }
    list_splice_tail(list_empty(lhs) ? rhs : lhs, head);
}

void list_merge_sort(struct list_head *q)
{
    if (list_empty(q) || list_is_singular(q))
        return;

    struct list_head left, sorted;
    INIT_LIST_HEAD(&left);
    list_cut_position(&left, q, get_middle(q));
    list_merge_sort(&left);
    list_merge_sort(q);
    list_merge(&left, q, &sorted);
    INIT_LIST_HEAD(q);
    list_splice_tail(&sorted, q);
}

/*
 * Testing
 */

static bool validate(struct list_head *q)
{
    struct list_head *node;
    for (node = q->next; node->next != q; node = node->next) {
        if (strcmp(((list_ele_t *)node)->value,
                   ((list_ele_t *)node->next)->value) > 0)
            return false;
    }
    return true;
}

static struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q) return NULL;

    INIT_LIST_HEAD(q);
    return q;
}

static void q_free(struct list_head *q)
{
    struct list_head *current = q->next;
    while (current != q) {
        struct list_head *tmp = current;
        current = current->next;
        free(((list_ele_t *)tmp)->value);
        free((list_ele_t *)tmp);
    }
    free(q);
}

bool q_insert_head(struct list_head *q, char *s)
{
    list_ele_t *newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    char *new_value = strdup(s);
    if (!new_value) {
        free(newh);
        return false;
    }

    newh->value = new_value;
    list_add_tail((struct list_head *)newh, q->next);

    return true;
}

static void q_show(struct list_head *q)
{
    struct list_head *node;
    list_for_each (node, q) {
        printf("%s", ((list_ele_t *)node)->value);
    }
}

int main(void)
{
    FILE *fp = fopen("cities.txt", "r");
    if (!fp) {
        perror("failed to open cities.txt");
        exit(EXIT_FAILURE);
    }

    struct list_head *q = q_new();
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
