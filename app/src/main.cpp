#include "../include/application.h"

int main()
{
    Application app(1280, 720);
    if(!app.init()) return 1;
    app.run();

    return 0;
}