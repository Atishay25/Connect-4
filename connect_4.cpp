#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <iostream>
#include <string>
using namespace std;

int chance = 0; //A global variable that ensures that players play thier moves alternatively
bool gameover = false;   //Boolean variable to check whether game is over or not

class Object : public Fl_Widget { //Abstract Class inherited from predefined class Fl_Widget
protected:
    int x,y,w,h;  //Dimensions of Object
public:
    Object (int x,int y,int w,int h) : Fl_Widget(x,y,w,h) { //Constructor
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

    virtual void fill(int clicked_x, int clicked_y) = 0; //function to fill color
    virtual void end() = 0;  //function to check/implement ending of game
    virtual int handle(int event){ //handle event
        if(event == FL_PUSH && !gameover){
            fill(Fl::event_x(),Fl::event_y()); 
            end();
        }
        return 1;
    }
};

class Ball : public Object { //Class of Balls
    int start_x;   //store top left x-coordinate of a ball
    int ballcolor; //store color of ball
public:
    Ball (int x, int y, int w, int h) : Object(x,y,w,h){
        ballcolor = 0;
        start_x = x;
    }      
    
    virtual void draw(){  //draw black balls by default
        fl_color(0);
        fl_pie(x,y,w,h,0,360);
    }
    virtual void fill_ball(){  //fill color in balls when player clicks
        if(chance%2) {  
            fl_color(95);
            fl_pie(x,y,w,h,0,360);
            ballcolor = 95;
        }
        else {
            fl_color(4);
            fl_pie(x,y,w,h,0,360);
            ballcolor = 4;
        }
        chance++;
    }
    int get_color(){  //access current color of a ball
        return ballcolor;
    }
    virtual void fill(int a, int b){} //empty as no need here
    virtual void end(){}   
    int get_x(){   // access x-coordinate of top-left corner of ball
        return start_x;
    }
};

class Message : public Fl_Widget {  //Class for text messages
    string text;
public:
    Message(int x,int y,int w,int h, string s) : Fl_Widget(x,y,w,h){
        text = s;
    }
       
    virtual void draw(){  //invoking set function here
        set(text);
    }
    void set(string message){
        fl_color(231);
        fl_font(8,20);  //used for changing font and size of text
        fl_draw(message.c_str(), 20, 570);
        fl_draw("Player 2 : Yellow", 400,570);
    }
};

class Grid : public Object { //Class to draw Outer-boundary grid around balls
public:
    Grid (int x, int y, int w, int h) : Object(x,y,w,h){}
        
    virtual void draw(){
        fl_draw_box(FL_FLAT_BOX,x,y,w,h,128);
        fl_frame2("AAAA",x,y,w,h);
    }
    void end(){};  //empty as no need here
    void fill(int clicked_x,int clicked_y){};
};

class Board : public Object {  //Class containing all balls and main components of game
protected:
    Ball *balls[10][10];
    int col[10];   //variable to ensure color filling in desired correct order
    int win_col;   //variable to store winning color of winning player
public :
    Board(int x,int y,int w,int h) : Object(x,y,w,h){  //Constructor
        for (int i = 1; i <= 10; i++){
            col[i-1] = 1;
            win_col = 0;
            for(int j = 1; j <= 10; j++){
                balls[i-1][j-1] = new Ball(50*i + 2,50*j + 2,45,45);
                balls[i-1][j-1]->hide();//hide balls under Board class
                                        //so that user is able to click on Board Widget properly
            }
        }
    }
        
    virtual void draw(){}  //It does not draw anything
    virtual void fill(int clicked_x, int clicked_y){  //function to find correct ball to fill and fill it
        int index;   //to store index of desired ball
        for(int i = 0; i < 10; i++){  //for finding correct index of correct ball to fill
            for(int j = 0; j < 10; j++){
                int ball_x = (balls[i][j])->get_x();
                if(clicked_y>=50 && clicked_y<= 500){
                    if((clicked_x >= ball_x) && (clicked_x <= (ball_x + 50)) ){
                           index = i;
                    }
                }
            }
        }

        if(col[index] <= 10){ //fill the ball that we got above
            balls[index][10-col[index]]->fill_ball(); //invoke fill_ball() function of desired ball
            (col[index])++;
        }
    }
    virtual void end(){  //Implement the end of game
        if(check_h()||check_v()||check_dDown()||check_dUp()){
            Fl::wait(2);
            if(win_col == 4){ //Show Winner
                fl_draw_box(FL_FLAT_BOX,150,50,300,150,79);
                fl_color(4);
                fl_font(1, 30);
                fl_draw("GAME OVER", 200,100);
                fl_font(1,20);
                fl_draw("Player 1 (Blue) WINS!",175,150);
                int time_0 = time(0);
                cout << "Player 1 (Blue) WON!" << endl;
                Fl::wait(20);
            }
            else if(win_col == 95){ //Show winner
                fl_draw_box(FL_FLAT_BOX,150,50,300,150,72);
                fl_color(95);
                fl_font(1, 30);
                fl_draw("GAME OVER", 200,100);
                fl_font(1,20);    
                fl_draw("Player 2 (Yellow) WINS!",175,150);
                cout << "Player 2 (Yellow) WON!" << endl;  
                Fl::wait(20);
            }
        }
    }
    bool check_h(){  //check horizontal win
        for(int j = 0; j < 10; j++){
            for(int i = 0; i < 7; i++){
                int col1 = balls[i][j]->get_color();
                int col2 = balls[i+1][j]->get_color();
                int col3 = balls[i+2][j]->get_color();
                int col4 = balls[i+3][j]->get_color();
                if((col1==col2) && (col2==col3) && (col3==col4) && (col4==col1) && (col1>0)){
                    win_col = col1;
                    gameover = true;
                    return true;
                }
            }
        }
        return false;
    }
    bool check_v(){  //check vertical win
        for(int i = 0; i < 10; i++){
            for(int j = 0; j < 7; j++){
                int col1 = balls[i][j]->get_color();
                int col2 = balls[i][j+1]->get_color();
                int col3 = balls[i][j+2]->get_color();
                int col4 = balls[i][j+3]->get_color();
                if((col1==col2) && (col2==col3) && (col3==col4) && (col4==col1) && (col1>0)){
                    win_col = col1;
                    gameover = true;
                    return true;
                }
            }
        }
        return false;
    }
    bool check_dDown(){  //check diagonal (down/negative slope) win
        for(int i = 3; i < 10; i++){
            for(int j = 0; j < 7; j++){
                int col1 = balls[i][j]->get_color();
                int col2 = balls[i-1][j+1]->get_color();
                int col3 = balls[i-2][j+2]->get_color();
                int col4 = balls[i-3][j+3]->get_color();
                if((col1==col2) && (col2==col3) && (col3==col4) && (col4==col1) && (col1>0)){
                    win_col = col1;
                    gameover = true;
                    return true;
                }
            }
        }
        return false;
    }
    bool check_dUp(){  //check diagonal (up/positive slope) win
        for(int i = 9; i > 2; i--){
            for(int j = 9; j > 2; j--){
                int col1 = balls[i][j]->get_color();
                int col2 = balls[i-1][j-1]->get_color();
                int col3 = balls[i-2][j-2]->get_color();
                int col4 = balls[i-3][j-3]->get_color();
                if((col1==col2) && (col2==col3) && (col3==col4) && (col4==col1) && (col1>0)){
                    win_col = col1;
                    gameover = true;
                    return true;
                } 
            }
        }
        return false;
    }
};

int main (int argc, char *argv[]){  //main program

    Fl_Window *window;  //create outer window
    window = new Fl_Window(600,600,"CONNECT 4");

    for (int i = 1; i <= 10; i++){  
        for(int j = 1; j <= 10; j++){
            new Grid(50*i,50*j,50,50);
            Ball *balls[i-1][j-1];
            balls[i-1][j-1] = new Ball(50*i + 2,50*j + 2,45,45);
        }
    }

    Message Player(0,0,500,500,"Player 1 : Blue");

    Board *board;
    board = new Board(50,50,550,550);

    window->color(56); //set color of window
    window->end();
    window->show();  //show widgets in window
    Fl::check();
    return(Fl::run());  //main event loop
}
