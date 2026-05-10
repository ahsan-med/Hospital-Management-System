#include "GUI.h"

int main() {
    GUIApp* app = new GUIApp(); 
    app->run();
    delete app;
    return 0;
}