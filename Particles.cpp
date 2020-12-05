//
// Created by Raffaele Montella on 12/5/20.
//

#include "Particles.hpp"

Particles::Particles() {
    // Logger configuration
    log4cplus::BasicConfigurator config;
    config.configure();
    logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("WaComM"));

    LOG4CPLUS_INFO(logger, "Empty Particles");
}

Particles::Particles(string fileName) {
    log4cplus::BasicConfigurator config;
    config.configure();
    logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("WaComM"));

    LOG4CPLUS_INFO(logger, "Reading restart file:"+fileName);

    std::ifstream infile(fileName);

    int count=0;
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        if (count > 0)
        {
            double x, y, z, health, tpart;
            if (!(iss >> x >> y >> z >> health >> tpart )) { break; } // error
            Particle particle(x,y,z,health,tpart);
            push_back(particle);
        }
        count++;
    }

}

void Particles::save(string fileName)
{

}

Particles::~Particles() {
}
