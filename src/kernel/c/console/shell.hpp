#pragma once

class Shell {
public:
    Shell(); // Constructor
    void init();
    void update(); // Main loop
private:
    int command_count;
};
