//
// Created by Tetsuya Shiota on 2018/07/21.
//

#ifndef ICFPC2018_LMOVE_HPP
#define ICFPC2018_LMOVE_HPP

bool checkFilled(P p1, P p2, const set<P>& filled){
    P d = p1 - p2;
    int cnt = 0;
    if (d.x != 0) ++cnt;
    if (d.y != 0) ++cnt;
    if (d.z != 0) ++cnt;
    if(cnt != 1)throw __LINE__;
    int xmin = min(p1.x, p2.x), xmax = max(p1.x, p2.x);
    int ymin = min(p1.y, p2.y), ymax = max(p1.y, p2.y);
    int zmin = min(p1.z, p2.z), zmax = max(p1.z, p2.z);
    for (int x = xmin; x <= xmax; ++x)
        for (int y = ymin; y <= ymax; ++y)
            for (int z = zmin; z <= zmax; ++z) {
                if (filled.count(P(x, y, z)))return true;
            }
    return false;
}

Command* check_move(P p1, P p2, P d, const set<P>& filled) {
    int cnt = 0;
    if (d.x != 0) ++cnt;
    if (d.y != 0) ++cnt;
    if (d.z != 0) ++cnt;

    // SMOVE
    if (cnt == 1
        && -15 <= d.x && d.x <= 15
        && -15 <= d.y && d.y <= 15
        && -15 <= d.z && d.z <= 15) {
        if(checkFilled(p1, p2, filled))return nullptr;
        return new SMove(d);
    }

    // LMOVE
    if(cnt == 2
       && -5 <= d.x && d.x <= 5
       && -5 <= d.y && d.y <= 5
       && -5 <= d.z && d.z <= 5) {
        P mid1, mid2;
        if(d.x == 0){
            mid1 = P(p1.x, p1.y, p2.z);
            mid2 = P(p1.x, p2.y, p1.z);
        }else if (d.y == 0){
            mid1 = P(p1.x, p1.y, p2.z);
            mid2 = P(p2.x, p1.y, p1.z);
        }else if(d.z == 0){
            mid1 = P(p1.x, p2.y, p1.z);
            mid2 = P(p2.x, p1.y, p1.z);
        }
        else {
            assert(false);
        }

        if(!checkFilled(p1, mid1, filled) && !checkFilled(mid1, p2, filled)){
            return new LMove(mid1 - p1, p2-mid1);
        }
        if(!checkFilled(p1, mid2, filled) && !checkFilled(mid2, p2, filled)){
            return new LMove(mid2 - p1, p2-mid2);
        }
    }
    return nullptr;
}

#endif //ICFPC2018_LMOVE_HPP
