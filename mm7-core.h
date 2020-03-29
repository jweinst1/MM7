#ifndef MM7_CORE_H
#define MM7_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * MM7 - A currency trading model
 */
 
typedef unsigned long mm7_ID;

// Keeps track of currency id and amount.
typedef struct {
    mm7_ID id;
    double amount;
} mm7_Currency;

typedef struct {
    double amount;
    double rate;
} mm7_CurrencyInfo;

typedef struct {
    mm7_CurrencyInfo* infos;
    mm7_ID lastCur;
} mm7_CurrencyInfoStore;

typedef struct {
    mm7_Currency selling;
    mm7_Currency buying;
    double exrate; // to cache precalculated rate.
    double revrate;
} mm7_Order;

typedef struct {
    mm7_Order* orders;
    size_t len;
    size_t cap;
} mm7_OrderBuf;

typedef struct {
    mm7_OrderBuf* stores;
    mm7_ID lastCur;
} mm7_OrderStore;

typedef enum {
    MM7_STRATEGY_CMP_LE,
    MM7_STRATEGY_CMP_GE
} mm7_StraegyCmp;

typedef struct {
    mm7_ID s_name;
    mm7_ID b_name;
    mm7_StraegyCmp cmp;
    double target_ex;
    double to_sell;
    double to_buy;
} mm7_Straegy;

typedef struct {
    mm7_Straegy* strats;
    size_t len;
    size_t cap;
} mm7_StraegyBuf;

// Top level state machine
typedef struct {
    mm7_StraegyBuf sbuf;
    mm7_CurrencyInfoStore cinfos;
    mm7_OrderStore ords;
} mm7_Exchange;

void
mm7_Order_init(mm7_Order* o, mm7_ID s_name, double s_amount,
                       mm7_ID b_name, double b_amount)
{
    assert(s_amount != b_amount);
    o->selling.id = s_name;
    o->selling.amount = s_amount;
    o->buying.id = b_name;
    o->buying.amount = b_amount;
    o->exrate = s_amount / b_amount;
    o->revrate = 1 / o->exrate;
}

void mm7_OrderBuf_init(mm7_OrderBuf* b, size_t cap)
{
    b->orders = calloc(sizeof(mm7_Order), cap);
    assert(b->orders != NULL);
    b->cap = cap;
    b->len = 0;
}

#define MM7_ORDERBUF_ACTIVE(buf) (buf->orders != NULL)

void mm7_OrderBuf_grow(mm7_OrderBuf* b, size_t factor)
{
    b->cap *= factor;
    b->orders = rellaoc(b->orders, sizeof(mm7_Order) * b->cap);
    assert(b->orders != NULL);
}

void mm7_OrderBuf_place(mm7_OrderBuf* b, mm7_ID s_name, double s_amount,
                                         mm7_ID b_name, double b_amount)
{
    if (b->len == b->cap)
        mm7_OrderBuf_grow(b, 2);
    mm7_Order_init(b->orders + b->len++, s_name, s_amount, b_name, b_amount);
}



#endif // MM7_CORE_H