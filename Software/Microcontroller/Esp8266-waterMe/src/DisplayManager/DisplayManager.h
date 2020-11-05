#include <Adafruit_SSD1306.h>

class DisplayManager
{
private:

enum DisplayFormat {
    AlwaysOff = 0,   //Always on Display
    AlwaysOn,      //Always Off display
    Scrolling,      //Scrolling information Vertically
    OnSensorActivity//Turns On when SMS activates
};

struct DisplayOptions
{
    Adafruit_SSD1306 screen;
    enum DisplayFormat view_mode;
    String messages[];
    int number_of_messages;
    int width;
    int height;
} display;


public:
    DisplayManager(int width, int height);
    ~DisplayManager();

    void updateDisplay();
    void updateDisplay(String message);
    void clearDisplay();

    void addLine(String new_line);
    void addLine(String description, float value);
    
    String removeLine(int index);
    bool removeAllLines();

    Adafruit_SSD1306 getScreen();
    
    
};

