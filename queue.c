#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return NULL;

    INIT_LIST_HEAD(&new_element->list);
    return &new_element->list;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *iter, *tmp;
    list_for_each_entry_safe (iter, tmp, l, list) {
        if (iter->value)
            free(iter->value);
        if (iter)
            free(iter);
    }
    free(list_entry(l, element_t, list));
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        goto ERR_LIST_NULL;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        goto ERR_NEW_ELEMENT;

    size_t size = strlen(s) + 1;
    new_element->value = malloc(sizeof(char) * strlen(s) + 1);
    if (!new_element->value)
        goto ERR_NEW_VALUE;

    strncpy(new_element->value, s, size);
    list_add(&new_element->list, head);
    return true;

ERR_NEW_VALUE:
    free(new_element);
ERR_NEW_ELEMENT:
ERR_LIST_NULL:
    return false;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    struct list_head *last_entry = NULL;
    last_entry = &list_last_entry(head, element_t, list)->list;
    return q_insert_head(last_entry, s);
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *first_element = list_first_entry(head, element_t, list);
    if (sp)
        strncpy(sp, first_element->value, bufsize);

    list_del(&first_element->list);
    return first_element;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *last_element = list_last_entry(head, element_t, list);
    if (sp)
        strncpy(sp, last_element->value, bufsize);

    list_del(&last_element->list);
    return last_element;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;

    int len = 0;
    struct list_head *iter;
    list_for_each (iter, head) {
        ++len;
    }
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    size_t len = (q_size(head) >> 1) + 1;
    while (len--) {
        head = head->next;
    }

    element_t *mid_element = list_entry(head, element_t, list);
    if (mid_element->value)
        free(mid_element->value);
    list_del(head);
    free(mid_element);
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    element_t *now, *tmp, *last = NULL;
    bool dup_flag = false;
    list_for_each_entry_safe (now, tmp, head, list) {
        if (last && !strcmp(last->value, now->value)) {
            list_del(&now->list);
            free(now->value);
            free(now);
            dup_flag = true;
        } else if (dup_flag) {
            list_del(&last->list);
            free(last->value);
            free(last);
            dup_flag = false;
            last = now;
        } else {
            last = now;
        }
    }
    if (dup_flag) {
        list_del(&last->list);
        free(last->value);
        free(last);
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    element_t *left = list_first_entry(head, element_t, list);
    element_t *right = list_last_entry(head, element_t, list);
    size_t cnt = q_size(head) >> 1;

    // Swap for size/2 times
    while (cnt--) {
        char *tmp = left->value;
        left->value = right->value;
        right->value = tmp;
        left = list_entry(left->list.next, element_t, list);
        right = list_entry(right->list.prev, element_t, list);
    }
}

void merge_sort(struct list_head *l, struct list_head *r, size_t len)
{
    if (len <= 1 || l == r)
        return;
    struct list_head *mid = l;
    int cnt = len >> 1;
    int r_len = len - cnt;
    while (cnt--)
        mid = mid->next;
    merge_sort(l, mid->prev, len >> 1);
    merge_sort(mid, r, r_len);

    // sorting
    struct list_head *mid_iter = mid;
    struct list_head *l_iter = l;
    char *tmp[len];
    for (int i = 0; i < len; i++)
        tmp[i] = NULL;
    int idx = 0;
    while (l_iter != mid && mid_iter != r->next) {
        if (strcmp(list_entry(l_iter, element_t, list)->value,
                   list_entry(mid_iter, element_t, list)->value) <= 0) {
            tmp[idx++] = list_entry(l_iter, element_t, list)->value;
            l_iter = l_iter->next;
        } else {
            tmp[idx++] = list_entry(mid_iter, element_t, list)->value;
            mid_iter = mid_iter->next;
        }
    }

    // cleanup
    while (l_iter != mid) {
        tmp[idx++] = list_entry(l_iter, element_t, list)->value;
        l_iter = l_iter->next;
    }
    while (mid_iter != r->next) {
        tmp[idx++] = list_entry(mid_iter, element_t, list)->value;
        mid_iter = mid_iter->next;
    }

    // write back
    element_t *now = list_entry(l, element_t, list);
    for (idx = 0; idx < len; idx++) {
        now->value = tmp[idx];
        now = list_entry(now->list.next, element_t, list);
    }
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || q_size(head) <= 1)
        return;

    merge_sort(&list_first_entry(head, element_t, list)->list,
               &list_last_entry(head, element_t, list)->list, q_size(head));
}
