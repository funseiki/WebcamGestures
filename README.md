Webcam Gestures
---------------

Using OpenCV to detect and track hand gestures

Authors
-------

    Ashutosh Dhar
    Ashwin Sai Sankar
    Vinit Shah

Instructions
------------

    To run

        1) Open bin/WebcamGestures.exe from the command line or a folder
        2) Wait until the red border disappears for the most part

            - You are waiting for the background subtracter to train

    To build

        - Compilation requires either visual studio or gcc

        - OpenCV is also required

        - Setting up with VS2012

            1) Copy opencv/build/include to opencv/build/x86/vc11/

            2) Create an environment variable named "OPENCV_32"

            3) Set the value to be "path/to/opencv/build/x86/vc11/"

            4) Add "%OPENCV_32%/bin/" to your PATH

