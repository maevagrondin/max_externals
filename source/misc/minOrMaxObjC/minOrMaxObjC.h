#ifndef minOrMax_minOrMaxObjC_h
#define minOrMax_minOrMaxObjC_h


@interface MinMax : NSObject{
    long am_list[MAX_SIZE];
    long am_count;
    long am_valright;
    long am_res;
}

- (void) am_new: (long) value; 
- (void) am_list:(Atom *)av andLength:(short)ac;
- (void) am_in1:(long)value;
- (void) am_bang;
- (long) am_calculate:(long)v1 andV2:(long) v2;
- (long) am_getRes;

@end



typedef struct minOrMaxObjC{
    t_object am_ob;
    MinMax * am_objc;
    void * struct_out;
} MinOrMaxObjC;

/************************************ DECLARATION ***************************************************/
void * minOrMaxObjC_new(long value);
void minOrMaxObjC_list(MinOrMaxObjC * x, Symbol * s, short ac, Atom * av);
void minOrMaxObjC_in1(MinOrMaxObjC * x, long value);
void minOrMaxObjC_bang(MinOrMaxObjC * x);


#endif
