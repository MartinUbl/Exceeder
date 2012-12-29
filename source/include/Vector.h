#ifndef GENERAL_EXCDR_VECTOR_H
#define GENERAL_EXCDR_VECTOR_H

static float numBounds(float value)
{
    if (fabs(value) < (1 / 1000000.0f))
        return 0;
    else
        return value;
}

struct CVector3
{
    CVector3() {};
    CVector3(float nx, float ny, float nz): x(nx), y(ny), z(nz) {};

    float x, y, z;

    float mySize()
    {
        return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
    }
    void multiply(float a)
    {
        x *= a;
        y *= a;
        z *= a;
    }
    void unitMultiply(float a)
    {
        if (mySize() == 0)
            return;

        multiply(a/mySize());
    }
    CVector3 vectorMultiply(CVector3 second)
    {
        CVector3 result;
        result.x = y*second.z - z*second.y;
        result.y = z*second.x - x*second.z;
        result.z = x*second.y - y*second.x;

        return result;
    }
    void makeNull()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    CVector3 operator+(const CVector3 &sec)
    {
        x = x + sec.x;
        y = y + sec.y;
        z = z + sec.z;
        return (*this);
    }
    CVector3 operator-(const CVector3 &sec)
    {
        x = x - sec.x;
        y = y - sec.y;
        z = z - sec.z;
        return (*this);
    }
    CVector3 operator*(float sec)
    {
        x = x * sec;
        y = y * sec;
        z = z * sec;
        return (*this);
    }
    float operator*(const CVector3 &sec)
    {
        return numBounds((x*sec.x)+(y*sec.y)+(z*sec.z));
    }

    CVector3 rotate(CVector3 &axis, float angle)
    {
        CVector3 v = *this;

        return ((v - axis * (axis * v)) * cos(angle)) + (axis.vectorMultiply(v) * sin(angle)) + (axis * (axis * v));
    }
};

struct CVector2
{
    float x, y;

    float mySize()
    {
        return sqrt(pow(x,2)+pow(y,2));
    }
    void multiply(float a)
    {
        x *= a;
        y *= a;
    }
    void unitMultiply(float a)
    {
        if (mySize() == 0)
            return;

        multiply(a/mySize());
    }
    void makeNull()
    {
        x = 0;
        y = 0;
    }
    float operator*(const CVector2 &sec)
    {
        return numBounds((x*sec.x)+(y*sec.y));
    }
};

#endif
