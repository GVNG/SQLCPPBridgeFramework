//
//  time_tracker.h
//  SQLCPPBridgeFramework
//
//  Created by Roman Makhnenko on 03/07/2019.
//  Copyright © 2019 DataArt. All rights reserved.
//

#pragma once
#ifndef time_tracker_h
#define time_tracker_h

#include <chrono>

class time_tracker
{
public:
    time_tracker()
        : stamp_(std::chrono::system_clock::now())
        {}
    ~time_tracker()
    {
        std::chrono::duration<float> dif(std::chrono::system_clock::now() - stamp_);
        std::cout << "The delay is " << dif.count() << " sec." << std::endl;
    };
private:
    std::chrono::system_clock::time_point stamp_;
};

#endif /* time_tracker_h */
