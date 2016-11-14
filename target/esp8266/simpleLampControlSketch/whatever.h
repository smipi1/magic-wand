

struct rgb_s{
    float r;       // percent
    float g;       // percent
    float b;       // percent
};
typedef rgb_s rgb;

struct hsv_s{
    float h;       // angle in degrees
    float s;       // percent
    float v;       // percent
};
typedef hsv_s hsv;

struct acc_s {
    int16_t AcX;
    int16_t AcY;
    int16_t AcZ;
    int16_t Tmp;
};
typedef  acc_s acc_t;

