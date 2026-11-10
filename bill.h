#ifndef BILL_H
#define BILL_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX 100

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int billId;
    char billName[50];
    float amount;
    Date dueDate;
} Bill;

typedef struct {
    Bill heap[MAX];
    int size;
} PriorityQueue;

/* Heap functions */
int compareDates(Date d1, Date d2);
void initQueue(PriorityQueue *pq);
int insertBill(PriorityQueue *pq, Bill newBill);
Bill peekMostUrgent(PriorityQueue *pq);
Bill extractMostUrgent(PriorityQueue *pq);

/* Alert functions */
Date getCurrentSystemDate(void);
void checkAndDisplayAlerts(PriorityQueue *pq);

/* Interface helpers (defined in main.c) */
void seedSampleData(PriorityQueue *pq);
void displayAllBillsFormatted(PriorityQueue *pq);

#endif