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

int8_t to_nd(const coordinate &c) {
    int dx = c.x;
    int dy = c.y;
    int dz = c.z;
    assert(-1 <= dx && dx <= 1);
    assert(-1 <= dy && dy <= 1);
    assert(-1 <= dz && dz <= 1);
    return (dx + 1) * 9 + (dy + 1) * 3 + (dz + 1);
}

class Command{
public:

     virtual void write(ofstream& ofs){
         assert(false);
     };

};

class Halt : public Command {
public:
    void write(ofstream& ofs) override{
        const char buf[1] = {(char)0b11111111};
        ofs.write(buf, 1);
    }
};

class Wait : public Command {
public:
    void write(ofstream& ofs) override{
        const char buf[1] = {(char)0b11111110};
        ofs.write(buf, 1);
    }
};

class Flip : public Command {
public:
    void write(ofstream& ofs) override{
        const char buf[1] = {(char)0b11111101};
        ofs.write(buf, 1);
    }
};

class SMove : public Command {
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
    void write(ofstream& ofs) override{
        assert(dir >= 1 && dir <=3);
        assert(-15 <= dist && dist <= 15);

        const char buf[2] = {(char)((dir << 4) | 0b0100), (char)(0b11111 & (dist+15))};
        ofs.write(buf, 2);
    }
};

class LMove : public Command {
public:
    DIR dir1, dir2;
    int64_t dist1, dist2;
    LMove(){}
    LMove(const coordinate &c1,const coordinate &c2){
        if(c1.x != 0){
            dir1 = DIR_X;
            dist1 = c1.x;
        }
        else if(c1.y != 0){
            dir1 = DIR_Y;
            dist1 = c1.y;
        }
        else if(c1.z != 0){
            dir1 = DIR_Z;
            dist1 = c1.z;
        }
        else assert(false);
        if(c2.x != 0){
            dir2 = DIR_X;
            dist2 = c2.x;
        }
        else if(c2.y != 0){
            dir2 = DIR_Y;
            dist2 = c2.y;
        }
        else if(c2.z != 0){
            dir2 = DIR_Z;
            dist2 = c2.z;
        }
        else assert(false);
    }
    LMove(DIR _dir1, int64_t _dist1, DIR _dir2, int64_t _dist2): dir1(_dir1), dist1(_dist1), dir2(_dir2), dist2(_dist2){};
    void write(ofstream& ofs) override{
        assert(dir1 >= 1 && dir1 <=3);
        assert(-5 <= dist1 && dist1 <= 5);
        assert(dir2 >= 1 && dir2 <=3);
        assert(-5 <= dist2 && dist2 <= 5);

        const char buf[2] = {(char)((dir2 << 6) | (dir1 << 4) | 0b1100), (char)(((dist2 + 5) << 4) | (dist1 + 5))};
        ofs.write(buf, 2);
    }
};

class FusionP : public Command {
public:
    byte nd;
    FusionP(const coordinate &c){
        nd = to_nd(c);
    }
    void write(ofstream& ofs) override{
        const char buf[1] = {(char)((nd << 3) | 0b111)};
        ofs.write(buf, 1);
    }
};

class FusionS : public Command {
public:
    byte nd;
    FusionS(const coordinate &c){
        nd = to_nd(c);
    }
    void write(ofstream& ofs) override{
        const char buf[2] = {(char)((nd << 3) | 0b110)};
        ofs.write(buf, 1);
    }
};


class Fission : public Command {
public:
    byte nd, m;
    Fission(const coordinate &c, int8_t _m){
        nd = to_nd(c);
        m = _m;
    }
    void write(ofstream& ofs) override{
        // ほんと？
        assert(m > 0);
        assert(m <= 20);
        const char buf[2] = {(char)((nd << 3) | 0b101), (char)m};
        ofs.write(buf, 2);
    }
};

class Fill : public Command {
public:
    int8_t nd;
    Fill(const coordinate &c){
        nd = to_nd(c);
    };
    void write(ofstream& ofs) override{
        const char buf[1] = {(char)((nd << 3) | 0b011)};
        ofs.write(buf, 1);
    }
};


void dumpTrace(const char* filepath, vector<Command*> &commands){
    ofstream ofs(filepath, ios::binary);
    for(auto &&com : commands){
        com->write(ofs);
    }
    ofs << flush;
}

#endif //ICFPC2018_TRACEDUMPER_HPP
