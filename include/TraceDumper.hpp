//
// Created by Tetsuya Shiota on 2018/07/21.
//

#ifndef ICFPC2018_TRACEDUMPER_HPP
#define ICFPC2018_TRACEDUMPER_HPP

#include<iostream>
#include "model.hpp"

using namespace std;

enum DIR{
    DIR_X = 0b01,
    DIR_Y = 0b10,
    DIR_Z = 0b11
};

using bytes = vector<int>;

class Command{
public:

     virtual bytes to_b(){
     };

};

class Halt : Command {
public:
    virtual bytes to_b()const{
        return bytes({0b11111111});
    }
};

class Wait : Command {
public:
    bytes to_b(){
        return bytes({0b11111110});
    }
};

class Flip : public Command {
public:
    virtual bytes to_b() override{
        bytes b = bytes({0b11111101});
        return b;
    }
};

class SMove : Command {
public:
    DIR dir;
    int64_t dist;
    SMove(){}
    SMove(const coordinate &c){
        if(c.x != 0){
            dir = DIR_X;
            dist = c.x;
        }
        else if(c.y != 0){
            dir = DIR_Y;
            dist = c.y;
        }
        else if(c.z != 0){
            dir = DIR_Z;
            dist = c.z;
        }
        else assert(false);
    }
    SMove(DIR _dir, int64_t _dist): dir(_dir), dist(_dist){};
    bytes to_b(){
        bytes ret;
        assert(dir >= 1 && dir <=3);
        ret.push_back((dir << 4) | 0b0100);
        assert(dist <= 0b11111);
        ret.push_back(0b11111 & dist);
        return ret;
    }
};

class LMove : Command {
public:
    SMove m1, m2;
    LMove(const coordinate &c1, const coordinate &c2){
        m1 = SMove(c1);
        m2 = SMove(c2);
    }
    LMove(DIR _dir1, int64_t _dist1, DIR _dir2, int64_t _dist2){
        m1 = SMove(_dir1, _dist1);
        m2 = SMove(_dir2, _dist2);
    }
    bytes to_b(){
        bytes ret = m1.to_b();
        for(byte &&b : m2.to_b()){
            ret.push_back(b);
        }
        return ret;
    }
};

class FusionP : Command {
public:
    byte nd;
    FusionP(byte _nd) : nd(_nd){}
    bytes to_b() {
        return bytes({(nd<<3) | 0b111});
    }
};

class FusionS : Command {
public:
    byte nd;
    FusionS(byte _nd) : nd(_nd){}
    bytes to_b() {
        return bytes({(nd<<3) | 0b110});
    }
};


class Fission : Command {
public:
    byte nd, m;
    Fission(byte _nd, byte _m) : nd(_nd), m(_m){}
    bytes to_b() {
        return bytes({(nd<<3) | 0b101, m});
    }
};

class Fill : Command {
public:
    byte nd;
    Fill(byte _nd) : nd(_nd){};
    bytes to_b() {
        return bytes({(nd<<3) | 0b011});
    }
};


void dumpTrace(ofstream ofs,  vector<Command*> &commands){
    for(auto &&com : commands){
        for(auto b : com->to_b()){
            ofs << (char)(b);
        }
    }
    ofs << flush;
}

#endif //ICFPC2018_TRACEDUMPER_HPP
