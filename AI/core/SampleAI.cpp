//
// Created by Tetsuya Shiota on 2018/07/21.
//

#include <ctime>
#include <vector>
#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"


#define FILE (strrchr(__BASE_FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)



int main(int argc, char **argv){

    /* モデルの内容受け取る */

    cout  << argv[1] << endl;
    Model model(argv[1]);

    assert(model.R != 0);

    for(int i =0; i<model.R; i++) {
        // モデルへのアクセス
        // cout << model(i, 0, 0) << endl;
        // cout << model(coordinate(i, 0, 0)) << endl;
    }



    vector<Command*> commands;
    /* AIの処理 */

    commands.push_back(new Wait());
    commands.push_back(new Flip());
    commands.push_back(new SMove(DIR_X, 10));
    commands.push_back(new SMove(coordinate(10, 0, 0)));
    commands.push_back(new LMove(DIR_X, -3, DIR_Y, 5));
    commands.push_back(new LMove(coordinate(-3, 0, 0), coordinate(0, 5, 0)));
    commands.push_back(new Fill(coordinate(0,1,0)));
    commands.push_back(new Fill(coordinate(0,0,1)));
    commands.push_back(new FusionP(coordinate(1, 0, 0)));
    commands.push_back(new FusionS(coordinate(-1, 0, 0)));
    commands.push_back(new Fission(coordinate(1, 0, 0), DIR_Y));
    commands.push_back(new Halt());


    /* トレースを出す */
    dumpTrace(argv[2], commands);
}
