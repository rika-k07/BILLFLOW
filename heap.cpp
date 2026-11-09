#include "bill.h"

int compareDates(Date d1, Date d2) {
    if (d1.year != d2.year) return d1.year - d2.year;
    if (d1.month != d2.month) return d1.month - d2.month;
    return d1.day - d2.day;
}

void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

void swap(Bill *a, Bill *b) {
    Bill temp = *a;
    *a = *b;
    *b = temp;
}

int insertBill(PriorityQueue *pq, Bill newBill) {
    if (pq->size >= MAX) return 0; // Overflow check
    int i = pq->size++;
    pq->heap[i] = newBill;

    // Heapify Up (Min-Heap based on due date: earliest date bubbles to root)
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (compareDates(pq->heap[i].dueDate, pq->heap[parent].dueDate) < 0) {
            swap(&pq->heap[i], &pq->heap[parent]);
            i = parent;
        } else {
            break;
        }
    }
    return 1;
}

Bill peekMostUrgent(PriorityQueue *pq) {
    return pq->heap[0];
}

Bill extractMostUrgent(PriorityQueue *pq) {
    Bill root = pq->heap[0];
    pq->heap[0] = pq->heap[--pq->size];

    int i = 0;
    while (2 * i + 1 < pq->size) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (compareDates(pq->heap[left].dueDate, pq->heap[smallest].dueDate) < 0)
            smallest = left;
        if (right < pq->size && compareDates(pq->heap[right].dueDate, pq->heap[smallest].dueDate) < 0)
            smallest = right;

        if (smallest != i) {
            swap(&pq->heap[i], &pq->heap[smallest]);
            i = smallest;
        } else {
            break;
        }
    }
    return root;
}