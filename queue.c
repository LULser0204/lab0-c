#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (new) {
        INIT_LIST_HEAD(new);
        return new;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *tmp;
    list_for_each_safe (node, tmp, head) {
        element_t *entry = list_entry(node, element_t, list);
        free(entry->value);
        list_del(node);
        free(entry);
    }

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_head = malloc(sizeof(element_t));

    if (!new_head || !s)
        return false;
    new_head->value = (char *) malloc(strlen(s) + 1);
    if (!new_head->value) {
        free(new_head);
        return false;
    }
    strncpy(new_head->value, s, strlen(s));
    new_head->value[strlen(s)] = '\0';

    list_add(&new_head->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_tail = malloc(sizeof(element_t));
    if (!new_tail)
        return false;

    new_tail->value = (char *) malloc(strlen(s) + 1);

    if (!new_tail->value) {
        free(new_tail);
        return false;
    }
    strncpy(new_tail->value, s, strlen(s));
    new_tail->value[strlen(s)] = '\0';

    list_add_tail(&new_tail->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head) {
        return NULL;
    }
    struct list_head *first_element = head->next;
    element_t *entry = list_entry(first_element, element_t, list);

    if (sp != NULL && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(first_element);
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head && head->next != head) {
        struct list_head *last_element = head->prev;
        element_t *entry = list_entry(last_element, element_t, list);
        if (sp != NULL && bufsize > 0) {
            strncpy(sp, entry->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        }
        list_del(last_element);
        return entry;
    }
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    for (; fast != head && fast->next != head; fast = fast->next->next)
        slow = slow->next;

    list_del(slow);

    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    if (list_is_singular(head))
        return true;
    struct list_head *before = head->next;

    while (before != head && before->next != head) {
        element_t *entry = list_entry(before, element_t, list);
        bool flag = false;
        struct list_head *tmp = before->next;

        while (tmp != head &&
               strcmp(entry->value, list_entry(tmp, element_t, list)->value) ==
                   0) {
            flag = true;
            struct list_head *current = tmp;
            tmp = tmp->next;

            list_del(current);
            q_release_element(list_entry(current, element_t, list));
        }
        struct list_head *dummy = before;
        before = tmp;

        if (flag) {
            list_del(dummy);
            q_release_element(list_entry(dummy, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *cur;
    cur = head->next;
    while (cur != head && cur->next != head) {
        struct list_head *tmp = cur->next;
        struct list_head *nextnext = tmp->next;
        list_del(cur);
        list_add(cur, tmp);
        cur = nextnext;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *current, *safe;
    list_for_each_safe (current, safe, head) {
        list_del(current);
        list_add(current, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *mergeSort(struct list_head *left, struct list_head *right);
/* Split the linked list until one node or NULL */
struct list_head *split(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *fast = head, *slow = head;
    while (fast != NULL && fast->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct list_head *mid = slow;
    slow->prev->next = NULL;

    struct list_head *left = split(head);
    struct list_head *right = split(mid);

    return mergeSort(left, right);
}

/* merge two sorted linked list */
struct list_head *mergeSort(struct list_head *left, struct list_head *right)
{
    struct list_head *dummy = NULL;
    struct list_head **indirect = &dummy;

    for (; left && right;) {
        struct list_head *cur = NULL;

        if (strcmp(list_entry(left, element_t, list)->value,
                   list_entry(right, element_t, list)->value) >= 0) {
            cur = right;
            right = right->next;
        } else {
            cur = left;
            left = left->next;
        }

        *indirect = cur;
        indirect = &cur->next;
    }

    if (left != NULL) {
        *indirect = left;
    } else {
        *indirect = right;
    }

    return dummy;
}

/* sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || head->next == head || head->next->next == head) {
        return;
    }

    // break the link
    head->prev->next = NULL;
    head->next = split(head->next);

    struct list_head *before = head, *after = head->next;

    for (; after != NULL; after = after->next) {
        after->prev = before;
        before = after;
    }

    before->next = head;
    head->prev = before;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    q_reverse(head);
    struct list_head *current = head->next;
    char *max = list_entry(head->next, element_t, list)->value;

    struct list_head *safe;

    while (current != head) {
        element_t *tmp = list_entry(current, element_t, list);
        safe = current->next;

        if (strcmp(max, tmp->value) > 0) {
            max = tmp->value;
        } else {
            list_del(current);
            q_release_element(tmp);
        }

        current = safe;
    }
    q_reverse(head);

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    q_reverse(head);
    struct list_head *current = head->next;
    char *max = list_entry(head->next, element_t, list)->value;

    struct list_head *safe;
    while (current != head) {
        element_t *tmp = list_entry(current, element_t, list);
        safe = current->next;

        if (strcmp(max, tmp->value) < 0) {
            max = tmp->value;
        } else {
            list_del(current);
            q_release_element(tmp);
        }

        current = safe;
    }
    q_reverse(head);

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
