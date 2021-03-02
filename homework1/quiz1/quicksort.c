#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <time.h>

typedef struct __node {                   
    int value;
    struct __node *next;
} node_t;

static inline void list_add_node_t(node_t **list, node_t *n) {
    n->next = *list;
    *list = n;
}

static inline void list_concat(node_t **left, node_t *right) {
    while (*left)
        left = &((*left)->next);
    *left = right;
}

void quicksort_r(node_t **list)
{
    if (!*list)
        return;

    node_t *pivot = *list;
    int value = pivot->value;
    node_t *p = pivot->next;
    pivot->next = NULL;

    node_t *left = NULL, *right = NULL;
    while (p) {
        node_t *n = p;
        p = p->next;
        list_add_node_t(n->value > value ? &right : &left, n);
    }

    quicksort_r(&left);
    quicksort_r(&right);

    node_t *result = NULL;
    list_concat(&result, left);
    list_concat(&result, pivot);
    list_concat(&result, right);
    *list = result;
}

void quicksort_nr(node_t **list) {
    // non-recursive version of quicksort
    #define MAX_LEVELS 64
    node_t **L[MAX_LEVELS], *R[MAX_LEVELS]; // stack

    int i = 0;
    L[0] = list, R[0] = NULL;
    while (i >= 0) {
        // pop from stack
        node_t **LL = L[i], *RR = R[i--];
        if (*LL == RR)
            continue;

        // choose first element as pivot
        // partition [pivot->next:RR)
        node_t *pivot = *LL;
        *LL = NULL;
        node_t *p = pivot->next;
        pivot->next = NULL;

        node_t *left = NULL, *right = NULL;
        int cntl = 0, cntr = 0;
        while (p != RR) {
            node_t *n = p;
            p = p->next;
            if (n->value > pivot->value) list_add_node_t(&right, n), ++cntr;
            else list_add_node_t(&left, n), ++cntl;
        }
        // keep list linked together
        list_concat(&right, RR);
        list_concat(&pivot, right);
        list_concat(&left, pivot);
        list_concat(LL, left);

        // push into stack
        L[++i] = LL, R[i] = pivot; // [*LL:pivot)
        L[++i] = &(pivot->next), R[i] = RR; // [pivot->next:RR)
        // handle shorter segment
        if (cntl < cntr) {
            {
                node_t **tmp = L[i - 1];
                L[i - 1] = L[i];
                L[i] = tmp;
            }
            {
                node_t *tmp = R[i - 1];
                R[i - 1] = R[i];
                R[i] = tmp;
            }
        }
    }
}

static bool list_is_ordered(node_t *list) {
    bool first = true;
    int value;
    while (list) {
        if (first) {
            value = list->value;
            first = false;
        } else {
            if (list->value < value)
                return false;
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

static void list_display(node_t *list) {
    printf("%s IN ORDER : ", list_is_ordered(list) ? "   " : "NOT");
    int mx = 20; // maximum number of output
    while (mx-- && list) {
        printf("%d ", list->value);
        list = list->next;
    }
    if (mx)
        printf("...");
    printf("\n");
}

static void list_make_node_t(node_t **list, int value) {
    // make a new node_t and put in the front of list
    node_t *n = malloc(sizeof(node_t));
    n->value = value;
    n->next = *list;
    *list = n;
}

static void list_free(node_t **list) {
    // free the whole list
    node_t *tmp = *list, *f;
    while (tmp) {
        f = tmp;
        tmp = tmp->next;
        free(f);
    }
    *list = NULL;
}

int main(int argc, char **argv) {
    size_t count = 200000;

    srandom(time(NULL)); // random seed

    node_t *list = NULL;
    while (count--)
        //list = list_make_node_t(list, random() % 1024); // original code
        list_make_node_t(&list, random() % 1024);

    list_display(list);
    quicksort_nr(&list);
    list_display(list);

    if (!list_is_ordered(list))
        return EXIT_FAILURE;

    list_free(&list);
    return EXIT_SUCCESS;
}
