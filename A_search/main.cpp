#include"execute.cpp"

int main(){
    init();
    arrayToVector(map,blockedmap);
    segmentMap(blockedmap);
    getBerthConvenience();
    output.OK();
    while (true)
    {
        ReadFrame();
        //cerr<<"dispatch"<<endl;
        //DispatchInterrupt();
        //cerr<<"interrupt"<<endl;
        RobotDispatch();
        //cerr<<"Ship"<<endl;
        ShipDispatch();
        //cerr<<"excute"<<endl;
        execute();
//        if(nowtime > 14995)
//        {
//            for(int i = 0;i < 10;i++)
//            {
//                int valueerwhgea = 0;
//                for(int j = 0;j < berth[i].goodsprice.tail;j++)
//                {
//                    valueerwhgea += berth[i].goodsprice.node[j];
//                }
//                cerr<<valueerwhgea<<endl;
//            }
//            cerr<<endl;
//        }

        if(nowtime % 3000 == 0)
        {
            cerr<<sum<<endl;
            //cerr<<"sumofgoods : "<<sumofgoods<<endl;
        }

    }
    return 0;

}
