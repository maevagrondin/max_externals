
// init
#include "ext.h"
void *this_class;


// structure of object
typedef struct _diff {
    t_object d_ob; // this line must be present, always the same
    long d_valleft; // last value sent to left inlet
    long d_valright; // last value sent to right inlet
    long d_valtotal; // value to be sent to outlet
    void *d_out; // pointer to outlet, need one to each outlet
} t_diff;



// Prototypes for methods: you need a method for each message you want to respond to
void *diff_new(long value);
void diff_int(t_diff *diff, long value);
void diff_in1(t_diff *diff, long value);
void diff_bang(t_diff *diff);
void diff_assist(t_diff *diff, Object *b, long msg, long arg, char *s); // Assistance method




void ext_main(void * r){
    // set up our class: create a class definition
    // sixth arg = indicates that object accepts one integer argument
    setup((t_messlist **) &this_class, (method)diff_new, 0L, (short)sizeof(t_diff), 0L, A_DEFLONG, 0);
    
    addbang((method)diff_bang); //bind method "diff_bang" to the "bang" message (in the left inlet)
    addint((method)diff_int); // bind method "diff_int" to int's received in the left inlet
    addinx((method)diff_in1,1); //bind method "diff_in1" to int's received in the right inlet
    addmess((method)diff_assist, "assist", A_CANT, 0);
    finder_addclass("All Objects", "diff_assist"); // add class to the New object list
}





/********************************************************************************** 
 diff_new(long value)
 inputs: value -- the integer from the typed argument in the object box description: creates a new instance of our class diff. Called once when the external
 object is loaded. returns: pointer to new instance
 *************************************************************************************/


void *diff_new(long value) {
    t_diff *diff;
    diff=(t_diff*)newobject(this_class); // Create new instance and return a pointer to it
    diff->d_valright = value; // Initialize the difference value
    diff->d_valleft = 0;
    diff->d_valtotal = -value;
    diff->d_out=intout(diff); //Create our outlet
    intin(diff, 1); // create the right inlet
    return(diff); // must return  pointer to the new instance
}






/********************************************************************************** 
 diff_int(t_diff *a, long value)
 inputs: diff - pointer to t_diff object value - value received in the inlet
 description: substracts the right value with the incoming value. Stores the new left inlet value as well as the total and outputs the total.
 returns: nothing 
 ***********************************************************************************/

void diff_int(t_diff *diff, long value) {
    diff->d_valleft=value; //Store the value received in the left inlet
    diff->d_valtotal= diff->d_valleft - diff->d_valright; //Subtracts the right inlet
                                                            // value from the left
    diff_bang(diff); // Call bang method right away since it's the left inlet
}




/************************************************************************************* 
 diff_in1(t_diff *diff, long value)
 inputs: diff - pointer to our object
 value - value received in the inlet
 description: stores the new right value, calculates and stores the new difference between the left and right value
 returns: nothing 
 *************************************************************************************/

void diff_in1(t_diff *diff,long value) {
    diff->d_valright = value; // Store the value
    diff->d_valtotal = diff->d_valleft - value; // Update new difference
}




/************************************************************************************* 
 diff_bang(t_diff *a)
 inputs: diff - pointer to our object
 description: method called when bang is received: it outputs the current difference
 of the left and right values returns: nothing
 *************************************************************************************/
void diff_bang(t_diff *diff) {
    outlet_int(diff->d_out,diff->d_valtotal); //put out the current total
}





void diff_assist(t_diff *diff, Object *b, long msg, long arg, char * s){
    if(msg == ASSIST_INLET){
        switch(arg){
            case 0: sprintf(s, "%s", "Left Operand"); break;
            case 1: sprintf(s, "%s", "Right Operand"); break;
            default: break;
        }
    }
    else if(msg == ASSIST_OUTLET){
        sprintf(s, "%s", "Result");
    }
}



    
    
    
    


