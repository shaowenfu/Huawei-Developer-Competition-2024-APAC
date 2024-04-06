#include"execute.cpp"

int main(){

    init();
    output.OK();
    robot[5].ifwait = 0;
    robot[5].route->x = 22;
    robot[5].route->y = 184;
    robot[0].ifwait = 0;
    robot[0].route->x = 22;
    robot[0].route->y = 183;
    
        
    // robot[9].ifwait = 0;
    // robot[9].route->x = 22;
    // robot[9].route->y = 184;
    while (true)
    {
        ReadFrame();
        //cerr<<nowtime<<endl;
        
        
        
        
        execute();
        
    }
    return 0;

}
