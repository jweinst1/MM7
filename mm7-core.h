#ifndef MM7_CORE_H
#define MM7_CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * MM7 - A currency trading model
 */
 
#ifdef __cplusplus
extern "C" {
#endif
 
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
    mm7_Currency tempSelling; // Used per run of strategies
    mm7_Currency tempBuying; // used per run of strategies
} mm7_CurrencyInfo;
/**
 * Meant to be a 2D array, tracks $S -> $b rates, as well as the orders.
 * for all currencies. It has bidirectional lookup.
 */
typedef struct {
    mm7_CurrencyInfo* infos;
    mm7_ID lastCur;
} mm7_CurrencyInfoStore;

#define MM7_CURINFO_STORE_SIZE(cif) ((cif->lastCur + 1) * (cif->lastCur + 1))

typedef struct {
    mm7_Currency selling;
    mm7_Currency buying;
    double exrate; // to cache precalculated rate.
    double revrate; // to cache precalculated reverse rate.
} mm7_Order;

typedef enum {
    MM7_STRATEGY_CMP_LE,
    MM7_STRATEGY_CMP_GE
} mm7_StrategyCmp;

typedef struct {
    mm7_ID sName;
    mm7_ID bName;
    mm7_StrategyCmp cmp;
    double targetEx;
    double toSell;
    double toBuy;
} mm7_Strategy;

typedef struct {
    mm7_Strategy* strats;
    size_t len;
    size_t cap;
} mm7_StrategyBuf;

// Top level state machine
typedef struct {
    mm7_StrategyBuf sBuf;
    mm7_CurrencyInfoStore cInfos;
    mm7_CurrencyBank cBank;
    size_t turns;
} mm7_Exchange;

void
mm7_CurrencyBank_init(mm7_CurrencyBank* bank, mm7_ID last)
{
    bank->amounts = calloc(last + 1, sizeof(double));
    assert(bank->amounts != NULL);
    bank->lastCur = last;
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

void
mm7_Strategy_init(mm7_Strategy* s,
                 mm7_ID sName,
                 mm7_ID bName,
                 mm7_StrategyCmp cmp,
                 double targetEx,
                 double toSell,
                 double toBuy)
{
    s->sName = sName;
    s->bName = bName;
    s->cmp = cmp;
    s->targetEx = targetEx;
    s->toSell = toSell;
    s->toBuy = toBuy;
}

#ifndef MM7_STRATEGY_BUF_DEFAULT_CAP
#define MM7_STRATEGY_BUF_DEFAULT_CAP 10
#endif

void
mm7_StrategyBuf_init(mm7_StrategyBuf* b, size_t cap)
{
    b->strats = calloc(sizeof(mm7_Strategy), cap);
    assert(b->strats != NULL);
    b->len = 0;
    b->cap = cap;
}

void
mm7_StrategyBuf_grow(mm7_StrategyBuf* b, size_t factor)
{
    b->cap *= factor;
    b->strats = realloc(b->strats, b->cap);
    assert(b->strats != NULL);
}

static inline int
mm7_StrategyBuf_isFull(const mm7_StrategyBuf* b)
{
    return b->len == b->cap;
}

void
mm7_CurrencyInfoStore_init(mm7_CurrencyInfoStore* st, mm7_ID last)
{
    mm7_ID iSell;
    mm7_ID iBuy;
    mm7_ID bound = last + 1;
    size_t totalSize = (bound) * (bound);
    st->infos = calloc(totalSize, sizeof(mm7_CurrencyInfo));
    assert(st->infos != NULL);
    st->lastCur = last;
    // Initialize the ID's of currency store in the info structs.
    for (iSell = 0 ; iSell < bound ; iSell++) {
        mm7_CurrencyInfo* sellLevel = st->infos + (last * iSell);
        for (iBuy = 0 ; iBuy < bound ; iBuy++) {
            sellLevel[iBuy].tempSelling.id = iSell;
            sellLevel[iBuy].tempBuying.id = iBuy;
        }
    }
}

void
mm7_CurrencyInfoStore_add(mm7_CurrencyInfoStore* st, mm7_ID sellId,
                          double sellAmount, mm7_ID buyId,
                          double buyAmount)
{
    assert(sellId <= st->lastCur && buyId <= st->lastCur);
    mm7_CurrencyInfo* info = st->infos + (sellId * st->lastCur + buyId); 
    info->tempSelling.amount += sellAmount;
    info->tempBuying.amount += buyAmount;
}

void
mm7_CurrencyInfoStore_addOrder(mm7_CurrencyInfoStore* st, const mm7_Order* o)
{
    mm7_CurrencyInfoStore_add(st, o->selling.id,
                                  o->selling.amount,
                                  o->buying.id,
                                  o->buying.amount);
}

void
mm7_CurrencyInfoStore_deInit(mm7_CurrencyInfoStore* st)
{
    assert(st->infos != NULL);
    free(st->infos);
    st->infos = NULL;
}

#ifdef __cplusplus
}
#endif

#endif // MM7_CORE_H