//
// Created by Tetsuya Shiota on 2018/07/21.
//

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

    commands.push_back(new Halt());
    commands.push_back(new Wait());
    commands.push_back(new Flip());
    commands.push_back(new SMove(DIR_X, 10));
    commands.push_back(new SMove(coordinate(10, 0, 0)));
    commands.push_back(new LMove(DIR_X, 10, DIR_Y, 20));
    commands.push_back(new LMove(coordinate(10, 0, 0), coordinate(0, 20, 0)));
    commands.push_back(new FusionP(1));
    commands.push_back(new FusionS(1));
    commands.push_back(new Fission(1, 1));
    commands.push_back(new Fill(0));



    /* トレースを出す */
    stringstream ss;
    time_t result = std::time(nullptr);
    ss <<  result << "_" << FILE << ".nbt";
    cout << "OUTPUT: " << ss.str() <<endl;
    dumpTrace(ofstream(ss.str()), commands);
}