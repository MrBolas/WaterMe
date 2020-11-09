#include <Wire.h>
#include <string.h>
#include <DisplayManager/DisplayManager.h>


DisplayManager::DisplayManager(int width, int height)
{
    this->display.height = height;
    this->display.width = width;
    this->display.max_number_of_messages = height/8;
    
    this->display.screen = Adafruit_SSD1306(width, height, &Wire);
    this->display.screen.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    this->display.screen.setTextSize(1);
    this->display.screen.setTextColor(SSD1306_WHITE);
    this->display.screen.clearDisplay();
    this->display.screen.display();

    this->display.view_mode = AlwaysOn; //While in dev AlwaysOn is better
    
}

DisplayManager::~DisplayManager(){}

void DisplayManager::updateDisplay()
{
    this->display.screen.setCursor(0,0);

    if (this->display.view_mode == DisplayFormat::AlwaysOn)
    {
        for (int i = 0; i < this->display.max_number_of_messages; i++)
        {
            if (this->display.messages[i] != nullptr)
            {
                this->display.screen.println(*this->display.messages[i]);
            }else{
                this->display.screen.println("");
            }
            
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
    for (int i = 0; i < this->display.max_number_of_messages; i++)
    {
        if (this->display.messages[i] == nullptr)
        {
            this->display.messages[i] = & new_line;
        }
    }
    this->updateDisplay();
}

void DisplayManager::addLine(String description, float value)
{
    for (int i = 0; i < this->display.max_number_of_messages; i++)
    {
        if (this->display.messages[i] == nullptr)
        {
            String new_line = description+" "+String(value).c_str();
            this->display.messages[i] = & new_line;
        }
    }
    this->updateDisplay();
}

void DisplayManager::setLine(String new_line, int index)
{
    if (index < this->display.max_number_of_messages)
    {
        this->display.messages[index] = & new_line;
    }
    this->updateDisplay();
}

void DisplayManager::setLine(String description, float value, int index)
{
    if (index < this->display.max_number_of_messages)
    {
        String new_line = description+" "+String(value).c_str();
        this->display.messages[index] = & new_line;
    }
    this->updateDisplay();
}

void DisplayManager::resetLine(int index)
{
    if (index < this->display.max_number_of_messages)
    {
        this->display.messages[index] = nullptr;
    }
    this->updateDisplay();
}

void DisplayManager::resetAllLines()
{
    for (int i = 0; i < this->display.max_number_of_messages; i++)
    {
        this->display.messages[i] = nullptr;
    }
    this->updateDisplay();
}

String* DisplayManager::getMessage(int index)
{
    return this->display.messages[index];
}