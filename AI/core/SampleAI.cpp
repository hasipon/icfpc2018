//
// Created by Tetsuya Shiota on 2018/07/21.
//

#include <vector>
#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"



int main(){

    /* モデルの内容受け取る */


    Model model("./problemsL/LA001_tgt.mdl");

    assert(model.R != 0);
    cout << "model.R == " << model.R <<endl;

    for(int i =0; i<model.R; i++) {
        // モデルへのアクセス
        // cout << model(i, 0, 0) << endl;
        // cout << model(coordinate(i, 0, 0)) << endl;
    }



    vector<Command> commands;
    /* AIの処理 */

    /* トレースを出す */
    dumpTrace(commands);
}