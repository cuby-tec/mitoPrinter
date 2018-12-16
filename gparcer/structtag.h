#ifndef STRUCTTAG_H
#define STRUCTTAG_H

#include <QString>

enum eGCodeTag{
    eG0=0, eG1, eG2, eG3, eG4, eG6, eG10, eG20, eG21 , eG28, eG29_1, eG29_2, eG30, eG33
    , eG90, eG91, eG92, eG92_1, eM82, eM83, eM84, eM104, eM106, eM107, eM109, eM140, eM190, eM550
    , eF, eS, eT, eError
};

struct NumberedTag{
    uint n;
};

struct sMover: NumberedTag{
    double_t x;
    double_t y;
    double_t z;
    double_t e;
    double_t f;
    void add(sMover* src){
    	x += src->x;
    	y += src->y;
    	z += src->z;
    	e += src->e;
    }
    void init(){
    	x = 0.0;
    	y = 0.0;
    	z = 0.0;
    	e = 0.0;
    	f = 0.0;
    }
};


//Rapid linear Move
struct sG0_t : sMover {
//    double x;
//    double y;
//    double z;
//    double e;
//    double f;
    double s;
    void set(sG0_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        e = src->e;
        f = src->f;
        s = src->s;
        n = src->n;
    }

    void get(sG0_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->e = e;
        dst->f = f;
        dst->s = s;
    }
};

//Linear Move
struct sG1_t : sMover{
//    double x;
//    double y;
//    double z;
//    double e;
//    double f;
    double s;
    void set(sG1_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        e = src->e;
        f = src->f;
        s = src->s;
        n = src->n;
    }

    void get(sG1_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->e = e;
        dst->f = f;
        dst->s = s;
        dst->n = n;
    }
};

//Controlled Arc Move (Clockwise Arc)
struct sG2_t :NumberedTag {
    double x;
    double y;
    double e;
    double f;
    double i;
    double j;
    void set(sG2_t* src){
        x = src->x;
        y = src->y;
        e = src->e;
        f = src->f;
        i = src->i;
        j = src->j;
        n = src->n;
    }

    void get(sG2_t* dst){
        dst->x = x;
        dst->y = y;
        dst->e = e;
        dst->f = f;
        dst->i = i;
        dst->j = j;
        dst->n = n;
    }
};

//Controlled Arc Move (Counter-Clockwise Arc)
struct sG3_t :NumberedTag {
    double x;
    double y;
    double e;
    double f;
    double i;
    double j;
    void set(sG3_t* src){
        x = src->x;
        y = src->y;
        e = src->e;
        f = src->f;
        i = src->i;
        j = src->j;
        n = src->n;
    }

    void get(sG3_t* dst){
        dst->x = x;
        dst->y = y;
        dst->e = e;
        dst->f = f;
        dst->i = i;
        dst->j = j;
        dst->n = n;
    }
};

//Dwell
struct sG4_t :NumberedTag {
    double p;
    double s;
    void set(sG4_t* src){
        p = src->p;
        s = src->s;
        n = src->n;
    }
    void get(sG4_t* dst){
        dst->p = p;
        dst->s = s;
        dst->n = n;
    }
};

//Direct Stepper Move
struct sG6_t :NumberedTag {
    double a;
    double b;
    double c;
    bool r;
    void set(sG6_t* src){
        a = src->a;
        b = src->b;
        c = src->c;
        r = src->r;
        n = src->n;
    }
    void get(sG6_t *dst){
        dst->a = a;
        dst->b = b;
        dst->c = c;
        dst->r = r;
        dst->n = n;
    }


};


//G10: Tool Offset
struct sG10_t :NumberedTag {
    double x;
    double y;
    double z;
    double p;
    double r;
    double s;
    double u;
    double v;
    double w;
    void set(sG10_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        p = src->p;
        r = src->r;
        s = src->s;
        u = src->u;
        v = src->v;
        w = src->w;
        n = src->n;
    }

    void get(sG10_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->p = p;
        dst->r = r;
        dst->s = s;
        dst->u = u;
        dst->v = v;
        dst->w = w;
        dst->n = n;
    }

};

//G20: Set Units to Inches - 2
//G21: Set Units to Millimeters - 1
struct sG20_21_t :NumberedTag{
    uint8_t a;
    sG20_21_t():a(true){}// Millimeters
    void set(sG20_21_t * src){
        a = src->a;
        n = src->n;
    }

    void get(sG20_21_t * dst){
        dst->a = a;
        dst->n = n;
    }
};



//Move to Origin (Home)
struct sG28_t : sMover {
//    bool x;
//    bool y;
//    bool z;
//	sG28_t():x(0.0),y(0.0),z(0.0){}
/*    void reset(){
        x = false;
        y = false;
        z = false;
    }*/
    void set(sG28_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        n = src->n;
    }
    void get(sG28_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->n = n;
    }
};

//
struct sG29_1_t:NumberedTag {
    double x;
    double y;
    double z;
    void set(sG29_1_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        n = src->n;
    }
    void get(sG29_1_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->n = n;
    }
};

struct sG29_2_t:NumberedTag {
    double x;
    double y;
    double z;
    void set(sG29_2_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        n = src->n;
    }
    void get(sG29_2_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->n = n;
    }
};

//Single Z-Probe
struct sG30_t:NumberedTag {
    double x;
    double y;
    double z;
    double a;
    bool d;
    bool e;
    double h;
    bool i;
    double p;
    bool r;
    bool s;
    bool t;
    bool u;
    void set(sG30_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        a = src->a;
        d = src->d;
        e = src->e;
        h = src->h;
        i = src->i;
        p = src->p;
        r = src->r;
        s = src->s;
        t = src->t;
        u = src->u;
        n = src->n;
    }
    void get(sG30_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->a = a;
        dst->d = d;
        dst->e = e;
        dst->h = h;
        dst->i = i;
        dst->p = p;
        dst->r = r;
        dst->s = s;
        dst->t = t;
        dst->u = u;
        dst->n = n;
    }
};

//Delta Auto Calibration
struct sG33_t:NumberedTag {
    double c;
    double e;
    double f;
    double p;
    double s;
    double t;
    double v;
    void set(sG33_t* src){
        c = src->c;
        e = src->e;
        f = src->f;
        p = src->p;
        s = src->s;
        t = src->t;
        v = src->v;
        n = src->n;
    }
    void get(sG33_t* dst){
        dst->c = c;
        dst->e = e;
        dst->f = f;
        dst->p = p;
        dst->s = s;
        dst->t = t;
        dst->v = v;
        dst->n = n;
    }
};

//Set to Absolute Positioning
struct sG90_t:NumberedTag {
    bool value; // false - Relative; true - Absolute.
    sG90_t():value(true){}
};

//Set to Relative Positioning
struct sG91_t:NumberedTag {

};


//Set Position
struct sG92_t: sMover {
//    double x;
//    double y;
//    double z;
//    double e;
    void set(sG92_t* src){
        x = src->x;
        y = src->y;
        z = src->z;
        e = src->e;
        n = src->n;
    }
    void get(sG92_t* dst){
        dst->x = x;
        dst->y = y;
        dst->z = z;
        dst->e = e;
        dst->n = n;
    }
    void init(sG92_t* dst){
        dst->x = std::nan("1");
        dst->y = std::nan("2");
        dst->z = std::nan("3");
        dst->e = std::nan("4");
    }
};

struct sG92_1_t:NumberedTag {
    double x;
    double y;
    double z;
    double a;
    double b;
    double c;
    double u;
    double v;
    double w;
};

//Set Extruder Temperature
struct sM104_t:NumberedTag {
    double r;
    double s;
    void set(sM104_t* src){
        r = src->r;
        s = src->s;
        n = src->n;
    }
    void get(sM104_t* dst){
        dst->r = r;
        dst->s = s;
        dst->n = n;
    }

};

//Fan On
struct sM106_t:NumberedTag {
    double p; //Fan number (optional, defaults to 0)
    double s; //Fan speed (0 to 255; RepRapFirmware also accepts 0.0 to 1.0))
    void set(sM106_t* src){
        p = src->p;
        s = src->s;
        n = src->n;
    }
    void get(sM106_t* dst){
        dst->p = p;
        dst->s = s;
        dst->n = n;
    }
};

//Fan Off
struct sM107_t:NumberedTag {
//Deprecated in Teacup firmware and in RepRapFirmware. Use M106 S0 instead.
};

//Set Extruder Temperature and Wait
struct sM109_t:NumberedTag {
    double r;
    double s;
    void set(sM109_t* src){
        r = src->r;
        s = src->s;
        n = src->n;
    }
    void get(sM109_t* dst){
        dst->r = r;
        dst->s = s;
        dst->n = n;
    }

};

//Set Bed Temperature (Fast)
struct sM140_t:NumberedTag {
    double s;
};

//Wait for bed temperature to reach target temp
struct sM190_t:NumberedTag {
    double s;
};

//Set Name
struct sM550_t:NumberedTag {
    QString s;
};

//Set extruder to absolute mode
struct sM82_t:NumberedTag {
    bool a; // a=true - Absolute mode, false - relative.
    sM82_t():a(true){}
};

//Stop idle hold
struct sM84_t:NumberedTag {
    bool a; // a=true - hold mode, false - stoped.
    sM84_t():a(true){}
};

struct sT0_t:NumberedTag {

};



struct sTags_t {
    sG0_t       G0;
    sG1_t       G1;
    sG2_t       G2;
    sG3_t       G3;
    sG4_t       G4;
    sG6_t       G6;
    sG10_t      G10;
    sG20_21_t   G20;
    sG20_21_t   G21;
    sG28_t      G28;
    sG29_1_t    G29_1;
    sG29_2_t    G29_2;
    sG30_t      G30;
    sG33_t      G33;
    sG90_t      G90;
    sG91_t      G91;
    sG92_t      G92;
    sG92_1_t    G92_1;
    sM82_t      M82;
    sM84_t      M84;
    sM104_t     M104;
    sM106_t     M106;
    sM107_t     M107;
    sM109_t     M109;
    sM140_t     M140;
    sM190_t     M190;
    sM550_t     M550;
};

#endif // STRUCTTAG_H
