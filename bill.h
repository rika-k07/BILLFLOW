#ifndef BILL_H
#define BILL_H

#include <stdio.h>
#include <string.h>

#define MAX 100

// Date Structure
typedef struct {
    int day;
    int month;
    int year;
} Date;

// Bill Structure
typedef struct {
    int billId;
    char billName[50];
    float amount;
    Date dueDate;
} Bill;

// Priority Queue (Min-Heap) Structure
typedef struct {
    Bill heap[MAX];
    int size;
} PriorityQueue;

// --- Function Prototypes ---

// Member 1: Heap & Priority Queue Functions (heap.c)
int compareDates(Date d1, Date d2);
void initQueue(PriorityQueue *pq);
int insertBill(PriorityQueue *pq, Bill newBill);
Bill peekMostUrgent(PriorityQueue *pq);
Bill extractMostUrgent(PriorityQueue *pq);

// Member 2: Alert System & Risk Engine Functions (alerts.c)
int dateToDays(Date d);
void checkAndDisplayAlerts(PriorityQueue *pq, Date currentDate);

// Member 3: Interface & Seeding Functions (main.c)
void seedSampleData(PriorityQueue *pq);
void displayAllBillsFormatted(PriorityQueue *pq);

#endif