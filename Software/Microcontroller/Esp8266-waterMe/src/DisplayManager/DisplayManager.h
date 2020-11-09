#include <Adafruit_GFX.h>
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
    String* messages[8];
    int max_number_of_messages;
    int width;
    int height;
} display;


public:
    DisplayManager(int width, int height);
    ~DisplayManager();

    void updateDisplay();
    void clearDisplay();

    void addLine(String new_line);
    void addLine(String description, float value);

    void setLine(String new_line, int index);
    void setLine(String description, float value, int index);
    
    void resetLine(int index);
    void resetAllLines();

    String* getMessage(int index);
};

