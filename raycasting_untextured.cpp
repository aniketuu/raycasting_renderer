#include "SDL.h"
#include <bits/stdc++.h>

using namespace std;

// global variables
const int window_width = 640;
const int window_height = 480;
const int fps = 60;

SDL_Window* window = NULL;
SDL_Renderer* screen = NULL;

// world map
const int world_map_width = 24;
const int world_map_height = 24;
int world_map[world_map_height][world_map_width] =
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,2,0,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,2,0,2,0,0,0,0,3,0,0,0,0,0,3,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,2,0,2,0,0,0,0,3,0,0,0,0,0,3,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,3,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,4,5,0,5,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,4,5,0,5,0,0,0,0,0,5,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,4,5,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,4,4,4,4,0,0,0,0,0,5,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// player details
double player_posX = 1.15;
double player_posY = 1.15;
double player_dirX = 1.0;
double player_dirY = 0.0;
double player_step_size = 1.0;
// angles used for turning
double cos_ang = 0.70710678;
double sin_ang = 0.70710678;

// camera details
// as player direction has value 1 the value of size of camera will determine the field of vision
double camera_planeX = 0.0;
double camera_planeY = 0.66;

// functions
void init(){
    // init sdl
    SDL_Init(SDL_INIT_EVERYTHING);
    // window
    window = SDL_CreateWindow("work ffs",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              window_width,window_height,
                              SDL_WINDOW_RESIZABLE);
    // screen
    screen = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    return;
}

void quit(){
    SDL_DestroyRenderer(screen);
    SDL_DestroyWindow(window);
    screen = NULL;
    window = NULL;
    SDL_Quit();
    return;
}

void draw(){
    // clear screen
    SDL_SetRenderDrawColor(screen,98,98,98,255);
    SDL_RenderClear(screen);    // also works as celling color

    // draw floor
    SDL_Rect floor = {0,window_height/2,window_width,window_height/2};
    SDL_SetRenderDrawColor(screen,158,158,158,255);
    SDL_RenderFillRect(screen,&floor);

    // time to draw the walls
    // loop through every pixel column
    for(int X = 0;X<window_width;X++){
        // extract a multiplier that would shorten the camera plane's one end 
        double squisher = 2*X/double(window_width) - 1.0;
        // squisher goes from -1 to 1
        // direction of our current ray
        double ray_dirX = player_dirX + camera_planeX*squisher;
        double ray_dirY = player_dirY + camera_planeY*squisher;

        // consider the vertical dist ray travels as it passes between 2 vertical lines as deltaY
        // vice versa for deltaX
        // some basic geometry shows
        /*
        double deltaY = ray_dirY == 0? 0: (ray_dirX == 0? numeric_limits<double>::max(): abs(ray_dirY/ray_dirX));
        double deltaX = ray_dirX == 0? 0: (ray_dirY == 0? numeric_limits<double>::max(): abs(ray_dirX/ray_dirY));
        */
        double deltaY = abs(ray_dirY/ray_dirX);
        double deltaX = abs(ray_dirX/ray_dirY);

        // (x,y_intercept) is the point where the ray touches first vertical line and (x_intercept,y) is the point where ray touches first horizontal line
        double x;
        double y_intercept;

        double x_intercept;
        double y;
        
        // value of sideX,Y depend on ray_dir
        int sideX = ray_dirX < 0? -1: 1;
        int sideY = ray_dirY < 0? -1: 1;

        if(ray_dirX < 0){
            double y_offset = (player_posX - int(player_posX)) * deltaY;
            y_intercept = player_posY + sideY*y_offset;

            x = std::floor(player_posX);
        }
        else{
            double y_offset = (1.0 - (player_posX-int(player_posX))) * deltaY;
            y_intercept = player_posY + sideY*y_offset;

            x = int(player_posX) + 1.0;
        }
        if(ray_dirY < 0){
            double x_offset = (player_posY - int(player_posY)) * deltaX;
            x_intercept = player_posX + sideX * x_offset;

            y = std::floor(player_posY);
        }
        else{
            double x_offset = (1.0 - (player_posY-int(player_posY))) * deltaX;
            x_intercept = player_posX + sideX * x_offset;

            y = int(player_posY) + 1.0;
        }

        // bools to check if wall is hit and what kind of wall is  hit
        bool wall_is_hit = false;
        bool wall_is_hit_on_vertical_face;

        int wallX; // will be used later for color
        int wallY;
        

        while(!wall_is_hit){
            if(abs(y_intercept - player_posY) < abs(y - player_posY)){ // might be wrong
                // check if wall is hit
                if(sideX > 0 && (world_map[int(y_intercept)][int(x)] != 0)){
                    wall_is_hit = true;
                    wall_is_hit_on_vertical_face = true;
                    wallX = int(x);
                    wallY = int(y_intercept);
                    //break;
                }
                else if(sideX < 0 && (world_map[int(y_intercept)][int(x-1)] != 0)){
                    wall_is_hit = true;
                    wall_is_hit_on_vertical_face = true;
                    wallX = int(x-1);
                    wallY = int(y_intercept);
                    //break;
                }                                
                else{
                    y_intercept = y_intercept + sideY*deltaY;
                    x = x + sideX; 
                }
            }
            else { // if(x_intercept < x)
                if(sideY > 0 && (world_map[int(y)][int(x_intercept)] != 0)){
                    wall_is_hit = true;
                    wall_is_hit_on_vertical_face = false;
                    wallX = int(x_intercept);
                    wallY = int(y);
                }
                else if(sideY < 0 && (world_map[int(y-1)][int(x_intercept)] != 0)){
                    wall_is_hit = true;
                    wall_is_hit_on_vertical_face = false;
                    wallX = int(x_intercept);
                    wallY = int(y-1);
                    //cout<<wallX<<" "<<wallY<<endl;
                    //return;
                }
                else{
                    x_intercept = x_intercept + sideX * deltaX;
                    y = y + sideY;
                }
            }
        }

        // now that we have location of impact lets find distance

        // lets create player as  temp origin
        double temp_player_posX = 0.0;
        double temp_player_posY = 0.0;

        double temp_wall_hitX;
        double temp_wall_hitY;

        if(wall_is_hit_on_vertical_face){
            temp_wall_hitX = x - player_posX;
            temp_wall_hitY = y_intercept - player_posY;
        }
        else{
            temp_wall_hitX = x_intercept - player_posX;
            temp_wall_hitY = y - player_posY;
        }

        double perp_dist = temp_wall_hitX * player_dirX + temp_wall_hitY * player_dirY;

        if(perp_dist < 0){
            fprintf(stderr,"perpendicular dist is negative\n");
            break;
        }

        // now line height
        int line_height = int(window_height/perp_dist);
        int line_start = window_height/2 - line_height/2;
        if(line_start < 0){
            line_start = 0;
        }
        int line_end = window_height/2 + line_height/2;
        if(line_end > window_height){
            line_end = window_height;
        }

        // select color        
        int r,g,b;
        switch(world_map[wallY][wallX]){
            case 1:
            r = 255; g = 0; b = 0;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;
            
            case 2:
            r = 0; g = 255; b = 0;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;

            case 3:
            r = 0; g = 0; b = 255;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;
            
            case 4:
            r = 255; g = 255; b = 0;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;

            case 5:
            r = 255; g = 0; b = 255;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;
                        
            default:
            r = 0; g = 0; b = 0;
            if(wall_is_hit_on_vertical_face){
                r = r/2; g = g/2; b = b/2;
            }
            break;
        }

        // finally start drawing
        SDL_SetRenderDrawColor(screen,r,g,b,255);
        SDL_RenderDrawLine(screen,X,line_start,X,line_end);
    }

    // update
    SDL_RenderPresent(screen);
}

int main(int argc,char* argv[]){
    init();

    // event handling and main loop
    SDL_Event event;
    bool running = true;
    
    // main loop
    while(running){
        // fps mainteners
        Uint32 start_time = SDL_GetTicks();
        Uint32 end_time;

        // event loop
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = false;
                break;
            }
            else if(event.type == SDL_KEYDOWN){
                double tempX;
                double tempY;
                double temp_dirX;
                double temp_dirY;
                switch(event.key.keysym.sym){
                    case SDLK_w:
                    tempX = player_posX + player_dirX*player_step_size;
                    tempY = player_posY + player_dirY*player_step_size;
                    if(world_map[int(tempY)][int(tempX)] == 0){
                        player_posX = tempX;
                        player_posY = tempY;
                    }
                    break;

                    case SDLK_s:
                    tempX = player_posX - player_dirX*player_step_size;
                    tempY = player_posY - player_dirY*player_step_size;
                    if(world_map[int(tempY)][int(tempX)] == 0){
                        player_posX = tempX;
                        player_posY = tempY;
                    }
                    break;

                    case SDLK_a:
                    // minus 90 deg rot of direction and then move
                    temp_dirX = 0*player_dirX + 1*player_dirY;
                    temp_dirY = -1*player_dirX + 0*player_dirY;
                    tempX = player_posX + temp_dirX*player_step_size;
                    tempY = player_posY + temp_dirY*player_step_size;
                    if(world_map[int(tempY)][int(tempX)] == 0){
                        player_posX = tempX;
                        player_posY = tempY;
                    }
                    break;

                    case SDLK_d:
                    // minus 90 deg rot of direction and then move
                    temp_dirX = 0*player_dirX + 1*player_dirY;
                    temp_dirY = -1*player_dirX + 0*player_dirY;
                    tempX = player_posX - temp_dirX*player_step_size;
                    tempY = player_posY - temp_dirY*player_step_size;
                    if(world_map[int(tempY)][int(tempX)] == 0){
                        player_posX = tempX;
                        player_posY = tempY;
                    }
                    break;

                    // now turnns
                    case SDLK_j:
                    //cout<<player_dirX<<" "<<player_dirY<<endl;
                    temp_dirX = player_dirX;
                    temp_dirY = player_dirY;
                    // ang <= 90 deg
                    player_dirX = cos_ang*temp_dirX + sin_ang*temp_dirY;
                    player_dirY = -1*sin_ang*temp_dirX + cos_ang*temp_dirY;
                    //cout<<player_dirX<<" "<<player_dirY<<endl;

                    // turn camera too
                    temp_dirX = camera_planeX;
                    temp_dirY = camera_planeY;
                    camera_planeX = cos_ang*temp_dirX + sin_ang*temp_dirY;
                    camera_planeY = -1*sin_ang*temp_dirX + cos_ang*temp_dirY;
                    //cout<<camera_planeX<<" "<<camera_planeY<<endl;
                    //cout<<endl;
                    break;
                    
                    case SDLK_l:
                    //cout<<player_dirX<<" "<<player_dirY<<endl;
                    temp_dirX = player_dirX;
                    temp_dirY = player_dirY;
                    player_dirX = cos_ang*temp_dirX - sin_ang*temp_dirY;
                    player_dirY = sin_ang*temp_dirX + cos_ang*temp_dirY;
                    //cout<<player_dirX<<" "<<player_dirY<<endl;

                    // turn camera too
                    temp_dirX = camera_planeX;
                    temp_dirY = camera_planeY;
                    camera_planeX = cos_ang*temp_dirX - sin_ang*temp_dirY;
                    camera_planeY = sin_ang*temp_dirX + cos_ang*temp_dirY;
                    //cout<<camera_planeX<<" "<<camera_planeY<<endl;
                    //cout<<endl;
                    break;
                    
                    
                    default:
                    break;
                }
            }
        }

        // draw
        draw();

        // fps maintainers
        end_time = SDL_GetTicks();
        SDL_Delay((1000/fps)>(end_time-start_time)?(1000/fps)-(end_time-start_time):0);
        
    }
    
    quit();
    return 0;
}