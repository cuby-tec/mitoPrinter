#ifndef ARAYTAG_H
#define ARAYTAG_H

#include "structtag.h"
//#include "mitoaction.h"

#define EXAMPLE 1

class ArrayTag
{
public:
#if EXAMPLE == 1
    static ArrayTag* instance(){
        static ArrayTag p;
        return &p;
    }
#else
        ArrayTag();
#endif
/*
    mitoAction* tagG0();
    mitoAction * tagG1();
    mitoAction * tagG2();
    mitoAction * tagG3();
    mitoAction * tagG4();
    mitoAction * tagG6();
    mitoAction * tagG10();
    mitoAction * tagG28();
    mitoAction * tagG29_1();
    mitoAction * tagG29_2();
    mitoAction * tagG30();
    mitoAction * tagG33();
    mitoAction * tagG90();
    mitoAction * tagG91();
    mitoAction * tagG92();
    mitoAction * tagG92_1();
    mitoAction * tagM82();
    mitoAction * tagM84();
    mitoAction * tagM104();
    mitoAction * tagM106();
    mitoAction * tagM107();
    mitoAction * tagM109();
    mitoAction * tagM140();
    mitoAction * tagM190();
    mitoAction * tagM550();
*/
    void *getTagValue( eGCodeTag index);


private:
    sTags_t tags;
//    mitoAction * action;
#if EXAMPLE==1
    ArrayTag();

    ~ArrayTag() {}

    ArrayTag(ArrayTag const&) = delete ;
    void operator = (ArrayTag const&) = delete ;
#endif
};

#endif // ARAYTAG_H
