#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;
using std::cout;
using std::cin;
using std::endl;

#define Enter              13
#define Escape             27
#define UpArrow            72
#define DownArrow          80

#define MIN_TANK_CAPACITY  20
#define MAX_TANK_CAPACITY  120

class Tank
{
    const int CAPACITY;
    double fuel_level;
public:
    int get_CAPACITY() const
    {
        return CAPACITY;
    }
    double get_fuel_level()const
    {
        return fuel_level;
    }
    double fill(double amount)
    {
        if (amount < 0) return fuel_level;
        fuel_level += amount;
        if (fuel_level > CAPACITY) fuel_level = CAPACITY;
        return fuel_level;
    }
    double give_fuel(double amount)
    {
        fuel_level -= amount;
        if (fuel_level < 0) fuel_level = 0;
        return fuel_level;
    }

    // CONSTRUCTORS
    Tank(int capacity)
        : CAPACITY
        (
            capacity<MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY :
            capacity>MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY :
            capacity
        ), fuel_level(0) //Инициализация в заголовке.
    {
        cout << "Tank is ready:\t" << this << "\n";
    }
    ~Tank()
    {
        cout << "Destructor for:\t" << this << "\n";
    }

    void info()const
    {
        cout << "Capacity\t" << get_CAPACITY() << " liters\n";
        cout << "Fuel level\t" << get_fuel_level() << " liters\n";
    }
};

#define MIN_ENGINE_CONSUMPTION   4
#define MAX_ENGINE_CONSUMPTION  30

class Engine
{
    const double CONSUMPTION;
    double consumption_per_second;
    bool is_started;

public:
    double get_CONSUMPTION()const
    {
        return CONSUMPTION;
    }
    double get_consumption_per_second()const
    {
        return consumption_per_second;
    }
    void set_consumption_per_second(int speed)
    {
        // Расход топлива зависит от скорости
        if (speed >= 1 && speed <= 60) {
            consumption_per_second = 0.0020 / 3e-5; // Для расхода 10 л/100 км
        }
        else if (speed >= 61 && speed <= 100) {
            consumption_per_second = 0.0014 / 3e-5;
        }
        else if (speed >= 101 && speed <= 140) {
            consumption_per_second = 0.0020 / 3e-5;
        }
        else if (speed >= 141 && speed <= 200) {
            consumption_per_second = 0.0025 / 3e-5;
        }
        else if (speed >= 201 && speed <= 250) {
            consumption_per_second = 0.0030 / 3e-5;
        }
        else {
            consumption_per_second = 0.0;
        }
    }

    // CONSTRUCTORS
    Engine(double consumption) :CONSUMPTION
    (
        consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
        consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
        consumption
    )
    {
        set_consumption_per_second(0); // Изначально скорость равна нулю
        is_started = false;
        cout << "Engine is ready:\t" << this << "\n";
    }
    ~Engine()
    {
        cout << "Engine is over:\t" << this << "\n";
    }

    void start() {
        is_started = true;
    }

    void stop() {
        is_started = false;
    }

    bool started()const {
        return is_started;
    }

    void info()const
    {
        cout << "Consumption:\t\t" << get_CONSUMPTION() << " liters\n";
        cout << "Consumption per second:\t" << get_consumption_per_second() << " liters/s\n";
    }
};

#define MIN_SPEED_LIMIT 30
#define MAX_SPEED_LIMIT 408

class Car {
    Engine engine;
    Tank tank;
    bool driver_inside;
    int speed;
    const int MAX_SPEED = 180;
    int acceleration;
    struct
    {
        std::thread panel_thread;
        std::thread engine_idle_thread;
        std::thread free_wheeling_thread;
    }control;
public:
    Car(double consumption, int capacity, int max_speed) :
        engine(consumption),
        tank(capacity),
        MAX_SPEED(
            max_speed < MIN_SPEED_LIMIT ? MIN_SPEED_LIMIT :
            max_speed > MAX_SPEED_LIMIT ? MAX_SPEED_LIMIT :
            max_speed) {
        acceleration = 10;
        driver_inside = false;
        speed = 0;
        cout << "Your car is ready to go!" << "\n";
        cout << "Press Enter to get in." << "\n";
    }
    ~Car() {
        cout << "Your car is over.";
    }
    void get_in() {
        driver_inside = true;
        control.panel_thread = std::thread(&Car::panel, this);
    }
    void get_out() {
        driver_inside = false;
        if (control.panel_thread.joinable()) {
            control.panel_thread.join();
            cout << "Вы вышли из машины!\n";
        }
    }
    void panel()const {
        while (driver_inside) {
            system("cls");
            cout << "Fuel level: " << tank.get_fuel_level() << " liters.\n";
            if (tank.get_fuel_level() < 5) {
                HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                SetConsoleTextAttribute(hConsole, 0xCF);
                cout << "LOW FUEL!!!\n";
                SetConsoleTextAttribute(hConsole, 0x07);
            }
            cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
            cout << "Speed " << speed << " km/h" << endl;
            cout << "Current fuel consumption per second: " << engine.get_consumption_per_second() << " liters/s\n"; // Добавили отображение расхода топлива за секунду
            Sleep(500);
        }
    }

    void engine_idle() {
        while (tank.give_fuel(engine.get_consumption_per_second()) && engine.started()) {
            std::this_thread::sleep_for(1s);
        }
    }

    void start() {
        if (tank.get_fuel_level()) {
            engine.start();
            control.engine_idle_thread = std::thread(&Car::engine_idle, this);
        }
        else cout << "Проверьте уровень топлива\n";
    }

    void stop() {
        engine.stop();
        if (control.engine_idle_thread.joinable())
            control.engine_idle_thread.join();
    }

    void free_wheeling() {
        while (speed > 0) {
            speed -= 3;
            if (speed < 0) speed = 0;
            std::this_thread::sleep_for(1s);
        }
    }

    void accelerate() {
        if (driver_inside && engine.started()) {
            speed += acceleration;
            if (speed > MAX_SPEED) speed = MAX_SPEED;
            engine.set_consumption_per_second(speed); 
            if (speed > 0 && !control.free_wheeling_thread.joinable())
                control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
            std::this_thread::sleep_for(1s);
        }
    }

    void slow_down() {
        if (driver_inside && speed > 0) {
            speed -= acceleration;
            if (speed < 0) speed = 0;
            if (speed == 0 && control.engine_idle_thread.joinable())
                control.free_wheeling_thread.join();
            std::this_thread::sleep_for(1s);
        }
    }
    void control_car() {
        char key = 0;
        do {
            key = 0;
            if (_kbhit()) key = _getch();
            switch (key) {
            case Enter:
                driver_inside ? get_out() : get_in();
                break;
            case 'F':
            case 'f':
                if (engine.started() || driver_inside) {
                    cout << "Для начала заглушите мотор." << "\n";
                }
                else {
                    double fuel_level;
                    cout << "Введите объём топлива: ";
                    cin >> fuel_level;
                    tank.fill(fuel_level);
                }
                break;
            case 'I':
            case 'i':
                if (driver_inside) engine.started() ? stop() : start();
                break;
            case 'W':
            case 'w':
            case UpArrow:
                accelerate();
                break;
            case 'S':
            case 's':
            case DownArrow:
                slow_down();
                break;
            case Escape:
                stop();
                get_out();
            }
        } while (key != Escape);
    }
    void info()const {
        engine.info();
        tank.info();
    }
};

void main()
{
    system("chcp 1251 > NUL");

    Car bmw(10.0, 80, 180);
    bmw.control_car();
    bmw.info();
}