#include "Utils.h"

using namespace std;

int classifyMotion(float Orientation, float thresholdRange, bool isRadians = true, bool showRange = false)
{
    float pi;

    if(isRadians)
        pi = PI;
    else
        pi = 180;

    float Range = thresholdRange * pi/2;
    float vectorOrientation = Orientation;

    //Horizontal is at 0 or 2pi rad
    //Right is at 0 or 2*pi rad
    //Up is at pi/2 rad
    //Left is  pi rad
    //Down is at 3pi/4

    float U = (pi/2);
    float D = (3*pi/2);
    float L = (pi);
    float Ru = (0.00);
    float Rd = (2*pi);

    float Up = (pi/2) + Range;
    float Un = (pi/2) - Range;
    float Dp = (3*pi/2) + Range;
    float Dn = (3*pi/2) - Range;
    float Lp = pi + Range;
    float Ln = pi - Range;
    float Rp = 0 + Range;
    float Rn = (2*pi) - Range;

    bool isNotInRange = false;

    bool isUp = false;
    bool isDown = false;
    bool isLeft = false;
    bool isRight = false;

    if(showRange)
    {
        cout<<"\nUP Range: "<<Up<<" - "<<Un;
        cout<<"\nDOWN Range: "<<Dp<<" - "<<Dn;
        cout<<"\nLEFT Range: "<<Lp<<" - "<<Ln;
        cout<<"\nRIGHT Range: "<<Rp<<" - "<<Rn;
        cout<<"\nRange: "<<Range;
        cout<<"\nInput:"<<vectorOrientation;
    }

    if(((vectorOrientation<= Up)&&(vectorOrientation >= U)) || ((vectorOrientation<= U)&&(vectorOrientation >= Un)) )
    {
        isUp = true;
        //cout<<"\nUp!";
    }

    if(((vectorOrientation<= Dp)&&(vectorOrientation >= D)) || ((vectorOrientation<= D)&&(vectorOrientation >= Dn)) )
    {
        isDown = true;
        //cout<<"\nDown!";
    }

    if(((vectorOrientation<= Lp)&&(vectorOrientation >= L)) || ((vectorOrientation<= L)&&(vectorOrientation >= Ln)) )
    {
        isLeft = true;
        //cout<<"\nLeft";
    }

    if(((vectorOrientation<= Rp)&&(vectorOrientation >= Ru)) || ((vectorOrientation<= Rd)&&(vectorOrientation >= Rn)) )
    {
        isRight = true;
        //cout<<"\nRight";
    }



    if(isRight || isLeft || isUp || isDown)
        isNotInRange = false;
    else
        isNotInRange = true;

    if(isNotInRange)
    {
        cerr<<"\nNot in Range!\n";

        float distRp = vectorOrientation - Rp;
        float distRn = Rn - vectorOrientation;

        float distLp = vectorOrientation - Lp;
        float distLn = Ln - vectorOrientation;

        float distUp = vectorOrientation - Up;
        float distUn = Un - vectorOrientation;

        float distDp = vectorOrientation - Dp;
        float distDn = Dn - vectorOrientation;

        float distR = (distRn > distRp) ? distRp : distRn;
        float distL = (distLn > distLp) ? distLp : distLn;
        float distD = (distDn > distDp) ? distDp : distDn;
        float distU = (distUn > distUp) ? distUp : distUn;

        float dist;

        if (distU >= distD && distU >= distR)
                dist = UP;
        else if (distD >=distU && distD >=distR)
                dist = DOWN;
        else if (distR >= distU && distR >=distD)
                dist = RIGHT;

        if(dist == UP)
            return ((distU > distL) ? LEFT : UP );

        else if (dist == DOWN)
            return ((distD > distL) ? LEFT : DOWN );

        else if (dist == RIGHT)
            return ((distR > distL) ? LEFT : RIGHT );

        return -1;
    }

    if(isUp)
        return UP;
    if(isDown)
        return DOWN;
    if(isLeft)
        return LEFT;
    if (isRight)
        return RIGHT;
}

