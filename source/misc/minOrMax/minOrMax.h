#ifndef minOrMax_minOrMax_h
#define minOrMax_minOrMax_h


typedef struct minOrMax{
    t_object am_ob;
    long am_list[MAX_SIZE];
    long am_count;
    long am_valright;
    void * am_out;
} MinMax;


/************************************ DECLARATION ***************************************************/
void * minOrMax_new(long value);
void minOrMax_list(MinMax * x, Symbol * s, short ac, Atom * av);
void minOrMax_in1(MinMax * x, long value);
void minOrMax_bang(MinMax * x);
long minOrMax_calculate(MinMax * x, long v1, long v2);


#endif
