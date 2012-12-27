//#include "input.hpp"
//#include "video.hpp"

//#include <SDL.h>

//#include <cmath>
//#include <chrono>
//#include <vector>

//using namespace video;

//namespace {

//void update(const int elapsed_time)
//{
//   //speed modifiers
//   double moveSpeed = elapsed_time / 1000.0 * 3.;
//   double rotSpeed = elapsed_time / 1000.0 * 3.;

//   if (input::is_key_down(SDLK_UP)) //move forward if no wall in front of you
//   {
//      if(video::world_map[pos_x + dir_x * moveSpeed][pos_y] == 0) pos_x += dir_x * moveSpeed;
//      if(video::world_map[pos_x][pos_y + dir_y * moveSpeed] == 0) pos_y += dir_y * moveSpeed;
//   }

//   if (input::is_key_down(SDLK_DOWN)) //move backwards if no wall behind you
//   {
//      if(video::world_map[pos_x - dir_x * moveSpeed][pos_y] == 0) pos_x -= dir_x * moveSpeed;
//      if(video::world_map[pos_x][pos_y - dir_y * moveSpeed] == 0) pos_y -= dir_y * moveSpeed;
//   }

//   if (input::is_key_down(SDLK_RIGHT)) // Rotate to the right.
//   {
//      // Rotate direction plane.
//      auto old_dir_x = dir_x;
//      dir_x = old_dir_x * std::cos(-rotSpeed) - dir_y * std::sin(-rotSpeed);
//      dir_y = old_dir_x * std::sin(-rotSpeed) + dir_y * std::cos(-rotSpeed);

//      // Rotate camera plane.
//      auto old_plane_x = camplane_x;
//      camplane_x = old_plane_x * std::cos(-rotSpeed) - camplane_y * std::sin(-rotSpeed);
//      camplane_y = old_plane_x * std::sin(-rotSpeed) + camplane_y * std::cos(-rotSpeed);
//   }

//   if (input::is_key_down(SDLK_LEFT)) // Rotate to the left.
//   {
//      // Rotate direction plane.
//      auto old_dir_x = dir_x;
//      dir_x = old_dir_x * std::cos(rotSpeed) - dir_y * std::sin(rotSpeed);
//      dir_y = old_dir_x * std::sin(rotSpeed) + dir_y * std::cos(rotSpeed);

//      // Rotate camera plane.
//      auto old_plane_x = camplane_x;
//      camplane_x = old_plane_x * std::cos(rotSpeed) - camplane_y * std::sin(rotSpeed);
//      camplane_y = old_plane_x * std::sin(rotSpeed) + camplane_y * std::cos(rotSpeed);
//   }
//}

//void mainloop()
//{
//   // An update call will update the game status by this amount of time.
//   const std::chrono::milliseconds delta_time(2);

//   auto old_time = std::chrono::milliseconds(SDL_GetTicks());
//   auto game_time = std::chrono::milliseconds::zero();
//   auto accumulator = std::chrono::milliseconds::zero();

//   while(!input::done)
//   {
//      const auto new_time = std::chrono::milliseconds(SDL_GetTicks());
//      auto frame_time = new_time - old_time;
//      old_time = new_time;

//      // Number of ms the game lacks behind and has to be updated for.
//      accumulator += frame_time;

//      while (accumulator >= delta_time)
//      {
//         input::handle_input();
//         update(delta_time.count());
//         accumulator -= delta_time;
//         game_time += delta_time;
//      }

//      video::render();
//   }
//}

//} // namespace

#include "RcDemo.hpp"

#include <iostream>

int main()
{
   try
   {
      RcDemo app;
      app.Start();
   }
   catch (const char* e)
   {
      std::cout << "ERROR: " << e << std::endl;
   }
   catch (...)
   {
      std::cout << "Unknown exception thrown!" << std::endl;
   }
}
