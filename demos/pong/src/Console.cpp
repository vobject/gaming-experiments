#include "Console.hpp"
#include "Renderer.hpp"

#include <iostream>

// ugly hack! how to access Console member from CmdHandler?
static Console* gConsole = nullptr;
static std::map<std::string, CVarCallback>* gCVarCallbacks = nullptr;

static void ConsoleCmdHandler(ConsoleInformation* console, char* command)
{
   const std::map<std::string, CVarCallback>& cvarBallbacks = *gCVarCallbacks;

   const auto it = cvarBallbacks.find(command);
   if (it == cvarBallbacks.end()) {
      CON_Out(console, (std::string("Unknown command: \"") + command + "\"").c_str());
      return;
   }
   it->second(*gConsole, command);


//	/*	This command handler is very stupid. Normally you would save your commands
//		using an array of char* and pointers to functions. You will need something like this
//		anyway if you want to make use of the tabfunction because you will have
//		to implement the tabulator completion search function yourself

//		To make myself clear: I use this construct in my own programs:

//		typedef struct {
//			char* commandname;
//			int (*my_func)(int argc, char* argv[]);
//		} command_t;

//		command_t* cmd_table[] = {
//								{ "quit", quit_function },
//								{ "alpha", set_alpha },
//								{ NULL, NULL }
//								};

//		and to search for a command:

//		command_t* cmd;

//		for (cmd = cmd_table; cmd->commandname; cmd++) {
//			if(!strcasecmd(cmd->commandname, argv[0])) {
//				command found, now start the function
//				return cmd->myfunc;
//			}
//		}
//		if we land here: command not found
}

static char* ConsoleTabHandler(char* command)
{
   (void) command;
   return nullptr;
}

Console::Console(const std::string& font, Renderer& r)
{
   const auto screen = GetSdlSurface(r);

   mSdlConsole = CON_Init(font.c_str(), screen, LINE_COUNT,
                             { 0, 0,
                               static_cast<Uint16>(screen->w),
                               static_cast<Uint16>(screen->h * .75f) });
   if (!mSdlConsole) {
      throw "Unable to create console.";
   }

   CON_Alpha(mSdlConsole, ALPHA_LEVEL);
   CON_Topmost(mSdlConsole);

   CON_SetPrompt(mSdlConsole, "> ");
   CON_SetHideKey(mSdlConsole, SDLK_BACKQUOTE);

   CON_SetExecuteFunction(mSdlConsole, ConsoleCmdHandler);
   CON_SetTabCompletion(mSdlConsole, ConsoleTabHandler);

   r.RegisterPostRenderHook([&](void*){ CON_DrawConsole(mSdlConsole); });

   gConsole = this;
   gCVarCallbacks = &mCVarCallbacks;
}

Console::~Console()
{
   CON_Destroy(mSdlConsole);
}

bool Console::ProcessInput(SDL_Event* event)
{
   return CON_Events(event);
}

void Console::ToggleVisible()
{
   if(CON_isVisible(mSdlConsole)){
      CON_Hide(mSdlConsole);
   } else {
      CON_Show(mSdlConsole);
   }
}

void Console::Print(const std::string& msg)
{
   CON_Out(mSdlConsole, msg.c_str());
}

void Console::RegisterCVar(const std::string& var, CVarCallback cb)
{
   mCVarCallbacks[var] = cb;
}

SDL_Surface *Console::GetSdlSurface(const Renderer& r) const
{
   // TODO: Check if the renderer actually DOES return a SDL_Surface.
   return static_cast<SDL_Surface*>(r.GetUnderlying());
}
