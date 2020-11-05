#include <Wire.h>
#include <string.h>
#include <DisplayManager/DisplayManager.h>


DisplayManager::DisplayManager(int width, int height)
{
    this->display.height = height;
    this->display.width = width;
    this->display.number_of_messages = 0;
    
    this->display.screen = Adafruit_SSD1306(width, height, &Wire);
    this->display.screen.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    this->display.screen.setTextSize(1);
    this->display.screen.setTextColor(SSD1306_WHITE);
    this->display.screen.clearDisplay();
    this->display.screen.display();

    this->display.view_mode = AlwaysOn; //While in dev AlwaysOn is better
    
}

DisplayManager::~DisplayManager(){}

void DisplayManager::updateDisplay(String message)
{
    this->display.screen.setCursor(0,0);
    this->display.screen.print(message);
    this->display.screen.display();

}

void DisplayManager::updateDisplay()
{
    this->display.screen.setCursor(0,0);

    if (this->display.view_mode == DisplayFormat::AlwaysOn)
    {
        int allowed_number_of_lines = display.height/8;

        for (int i = 0; i < allowed_number_of_lines; i++)
        {
            this->display.screen.println(display.messages[i]);
        }
    }
    this->display.screen.display();
}

void DisplayManager::clearDisplay()
{
    this->display.screen.clearDisplay();
    this->display.screen.display();
}

void DisplayManager::addLine(String new_line)
{
    this->display.messages[++this->display.number_of_messages] = new_line;
}

void DisplayManager::addLine(String description, float value)
{
    this->display.messages[++this->display.number_of_messages] = description + value;
}

String DisplayManager::removeLine(int index)
{
    if (index<this->display.number_of_messages)
    {
        String deleted_message = this->display.messages[index];
        this->display.messages->remove(index);
        this->display.number_of_messages--;
        return deleted_message;
    }
    return "";
}

bool DisplayManager::removeAllLines()
{
    for (size_t i = 0; i < this->display.messages->length(); i++)
    {
        this->display.messages->remove(i);
    }

    if (this->display.messages->isEmpty())
    {
        this->display.number_of_messages = 0;
        return true;
    }
    return false;
}

Adafruit_SSD1306 DisplayManager::getScreen()
{
    return this->display.screen;
}