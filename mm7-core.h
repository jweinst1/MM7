#ifndef MM7_CORE_H
#define MM7_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * MM7 - A currency trading model
 */
 
// The global ID for any currency 
typedef unsigned long mm7_ID;

// Keeps track of currency id and amount.
typedef struct {
    mm7_ID id;
    double amount;
} mm7_Currency;

typedef struct {
    double* amounts;
    mm7_ID lastCur;
} mm7_CurrencyBank;

typedef struct {
    double currentRate;
    double tempSelling; // Used per run of strategies
    double tempBuying; // used per run of strategies
} mm7_CurrencyInfo;
/**
 * Meant to be a 2D array, tracks $S -> $b rates, as well as the orders.
 * for all currencies. It has bidirectional lookup.
 */
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
    size_t turns;
} mm7_Exchange;

void
mm7_CurrencyBank_init(mm7_CurrencyBank* bank, mm7_ID last)
{
    bank->curs = calloc(sizeof(double), last + 1);
    assert(bank->curs != NULL);
    bank->lastCur = last;
}

void
mm7_CurrencyInfoStore_init(mm7_CurrencyInfoStore* st, mm7_ID last)
{
    size_t total_size = (last + 1) * (last + 1);
    st->infos = calloc(sizeof(mm7_CurrencyInfo), total_size);
    assert(st->infos != NULL);
    st->lastCur = last;
}

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



#endif // MM7_CORE_H