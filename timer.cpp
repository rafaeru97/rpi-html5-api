#include "timer.h"

//JSON handling
#include <iostream>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

Timer::Timer(int delay, QObject *parent) : QObject(parent)
{
    _delay = delay;
    _timer = new QTimer();
    QObject::connect(_timer, SIGNAL(timeout()), this, SLOT(timerFunction()));
    _timer->start(delay);
}

Timer::~Timer()
{
    _timer->stop();
}

int Timer::timerGetDelay()
{
    return _delay;
}

void Timer::timerSetDelay(int delay)
{
    _delay = delay;
    _timer->setInterval(delay);
}

bool Timer::timerStatus()
{
    return _timer->isActive();
}

void Timer::timerSetServer(WebSocket *server)
{
    _server = server;
}

void Timer::timerFunction()
{
    QDateTime dt = QDateTime::currentDateTime();
    qDebug() << "[" << dt.toString("hh:mm:ss.z") << "]";

    if(init)
    {
        init = false;
        json m;
        m["act_temp"] = temp_start;
        m["act_humi"] = humi_start;

        QString json_message = QString::fromStdString(m.dump());
        std::cout << "Message sended: " << json_message.toStdString() << std::endl;
        _server->sendMessage(json_message);
    }

    QString message = _server->reciveMessage();

    if(message.length() > 1) {
        json j = json::parse(message.toStdString());
        std::cout << "Message recived: " << message.toStdString() << std::endl;
        std::cout << "JSON conversion: " << j.dump(4) << std::endl;
        std::cout << "Temperature: " << j["set_temp"] << std::endl;
        std::cout << "Humidity: " << j["set_humi"] << std::endl;
        std::cout << "Window: " << j["set_wind"] << std::endl;

        double set_temp = std::stod(j["set_temp"].get<std::string>());
        double set_humi = std::stod(j["set_humi"].get<std::string>());
        bool set_wind = j["set_wind"].get<bool>();

        double out_pid_temp = temp_pid.calculate(set_temp);
        double out_pid_humi = humi_pid.calculate(set_humi);

        double disr_temp = 0;
        double disr_humi = 0;

        if (set_wind) {
            disr_temp = -10;
            disr_humi = 20;
        }

        out_pid_temp = out_pid_temp + disr_temp;
        out_pid_humi = out_pid_humi + disr_humi;

        double obj_temp = temp_object.calculate(out_pid_temp);
        double obj_humi = humi_object.calculate(out_pid_humi);

        temp_pid.setValue(obj_temp);
        humi_pid.setValue(obj_humi);

        double out_temp = temp_pid.getValue();
        double out_humi = humi_pid.getValue();

        json m;
        m["act_temp"] = out_temp;
        m["act_humi"] = out_humi;
        m["uchyb_temp"] = temp_pid.getError();
        m["uchyb_humi"] = humi_pid.getError();
        if(set_wind) {
            m["ster_temp"] = out_pid_temp - disr_temp;
            m["ster_humi"] = out_pid_humi - disr_humi;
            m["zakl_temp"] = 10;
            m["zakl_humi"] = 20;
        } else {
            m["ster_temp"] = out_pid_temp;
            m["ster_humi"] = out_pid_humi;
            m["zakl_temp"] = 0;
            m["zakl_humi"] = 0;
        }

        QString json_message = QString::fromStdString(m.dump());
        std::cout << "Message sended: " << json_message.toStdString() << std::endl;
        _server->sendMessage(json_message);
    }
}
