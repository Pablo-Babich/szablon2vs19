#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>


extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
#define UNI_WIDTH 100
#define UNI_HIGHT 90
#define M_G  980
#define NUM_OF_BOXS 3
#define NUM_OF_CLOUDS 5
#define NUM_OF_PLATFORMS 5
#define SPACE_BETWEEN_BOXS 400
#define SPACE_BETWEEN_CLOUDS 180
#define TIME_IN_DASH 2
#define SPEED_OF_UNICORN 600

void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt środka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o długości l w pionie (gdy dx = 0, dy = 1) 
// bądź poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostokąta o długości boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};



//Texture wrapper class
class Texture
{
public:
	//Initializes variables
	Texture()
	{
		//Initialize
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	};

	//Deallocates memory
	~Texture()
	{
		//Deallocate
		free();
	};

	//Loads image at specified path
	bool Texture::CreateFromSprite(SDL_Renderer* gRenderer, SDL_Surface* sprite)
	{
		//Get rid of preexisting texture
		free();

		//The final texture
		SDL_Texture* newTexture = NULL;

		//Load image at specified path

		if (sprite == NULL)
		{
			printf("Unable to load bmp! SDL_image Error: \n");
		}
		else
		{
			//Color key image
			SDL_SetColorKey(sprite, SDL_TRUE, SDL_MapRGB(sprite->format, 0, 0xFF, 0xFF));

			//Create texture from surface pixels
			newTexture = SDL_CreateTextureFromSurface(gRenderer, sprite);
			if (newTexture == NULL)
			{
				printf("Unable to create texture from BMP SDL Error");
			}
			else
			{
				//Get image dimensions
				mWidth = sprite->w;
				mHeight = sprite->h;
			}

			//Get rid of old loaded surface
			SDL_FreeSurface(sprite);
		}

		//Return success
		mTexture = newTexture;
		return mTexture != NULL;
	};
	void UpdateTexture(SDL_Renderer* gRenderer, SDL_Surface* sprite) {
		SDL_UpdateTexture(mTexture, NULL, sprite->pixels, sprite->pitch);
	}
	//Deallocates texture
	void free()
	{
		//Free texture if it exists
		if (mTexture != NULL)
		{
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
		}
	};

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue)
	{
		//Modulate texture rgb
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	};

	//Set blending
	void setBlendMode(SDL_BlendMode blending)
	{
		//Set blending function
		SDL_SetTextureBlendMode(mTexture, blending);
	};

	//Set alpha modulation
	void setAlpha(Uint8 alpha)
	{
		//Modulate texture alpha
		SDL_SetTextureAlphaMod(mTexture, alpha);
	}

	//Renders texture at given point
	void render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
	{
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL)
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	};
	//Gets image dimensions
	int getWidth()
	{
		return mWidth;
	};

	int getHeight()
	{
		return mHeight;
	};

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
class Unicorn
{
public:
	//The dimensions of the uni
	static const int UNICORN_WIDTH = 100;
	static const int UNICORN_HEIGHT = 90;
		//Maximum axis velocity of the uni
	static const int DOT_VEL =10;
	

	//Initializes the variables
	Unicorn()
	{

		//Initialize the offsets
		mPosX = 0;
		mPosY = SCREEN_HEIGHT/2 - UNICORN_HEIGHT/2;
		Speed = SPEED_OF_UNICORN;
		//Initialize the velocity
		mVelX = 0;
		mVelY =0;
		isDashWasUsed = false;
		time_before_jump = -4;
		isInFly = 0;
	}
	//Takes key presses and adjusts the uni velocity
	void handleEvent(SDL_Event& e)
	{

		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: mVelY -= DOT_VEL; break;
			case SDLK_DOWN: mVelY += DOT_VEL; break;
			case SDLK_LEFT: mVelX -= DOT_VEL; break;
			case SDLK_RIGHT: mVelX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: mVelY += DOT_VEL; break;
			case SDLK_DOWN: mVelY -= DOT_VEL; break;
			case SDLK_LEFT: mVelX += DOT_VEL; break;
			case SDLK_RIGHT: mVelX -= DOT_VEL; break;
			}
		}
	}
	void goHome() {
		mPosX = 0;
		mPosY = SCREEN_HEIGHT / 2 - UNICORN_HEIGHT / 2;
		mVelX = 0;
		mVelY = 0;
	}

	//Moves the unicorn
	void move(int boxsOX[], int boxHeight, int boxWidth)
	{
		//Move the dot up or down
		mPosY += mVelY;

		mPosX += mVelX;

		//If the dot went too far to the left or right
		if ((mPosX < 0) || (mPosX + UNI_WIDTH > SCREEN_WIDTH))
		{
			//Move back
			mPosX -= mVelX;
		}
		//If the dot went too far up or down
		if ((mPosY < 0) || (mPosY + UNICORN_HEIGHT > SCREEN_HEIGHT))
		{
			//Move back
			mPosY -= mVelY;
		
		}
	}
	bool endOfGame(int boxsOX[], int boxHeight, int boxWidth) {
		for (size_t i = 0; i < NUM_OF_BOXS; i++)
		{
			if (boxsOX[i] > UNICORN_WIDTH - 10 && boxsOX[i] < UNICORN_WIDTH + 10 && mPosY>SCREEN_HEIGHT - (UNICORN_HEIGHT + boxHeight))
			{
				return true;
			}
		}
		return false;
	}
	//Shows the dot on the screen
	void render(SDL_Renderer* gRenderer, Texture* UnicornTexture)
	{
		//Show the uni
		UnicornTexture->render(gRenderer, mPosX, mPosY);
	};
private:
	//The X and Y offsets of the uni
	int mPosX, mPosY, isInFly;
	double time_before_jump, time_of_up, Speed, time, thndJumpOY;
	bool isDashWasUsed;
	//The velocity of the uni
	int mVelX, mVelY;
};

void close(SDL_Renderer* gRenderer, SDL_Window* Window, Texture gDotTexture, Texture gBGTexture)
{
	//Free loaded images
	gDotTexture.free();
	gBGTexture.free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(Window);
	Window = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems

	SDL_Quit();
}
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* args[])
{
	int t1, t2, frames, rc, boxsOX[NUM_OF_BOXS];
	double delta, worldTime, fpsTimer, bgdistance, fps, distance, BgSpeed, time_before_jump, boxSpeed, boxDistance1, boxDistance2, finishOfDash;
	bool AutoMod = false;
	bool quit = false;
	bool isInDash = false;
	bool fix = false;

	//The window we'll be rendering to
	SDL_Window* Window = NULL;

	//The window renderer
	SDL_Renderer* renderer = NULL;
	//events handler
	SDL_Event e;

	Unicorn unicorn;

	//Scene textures
	Texture UnicornTexture;
	Texture gBGTexture;
	Texture BlueRect;
	Texture BoxTexture;
	Texture PlatfomTexture;
	Texture CloudTexture;

	SDL_Surface* rect, * charset;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}
	Window = SDL_CreateWindow("My project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (Window == NULL)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

	}
	renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

	}
	else
	{
		//Initialize renderer color
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	}
	//load uni
	if (!UnicornTexture.CreateFromSprite(renderer, SDL_LoadBMP("./sprits/uni.bmp")))
	{
		printf("Failed to load dot texture!\n");

	}

	//Load background texture
	if (!gBGTexture.CreateFromSprite(renderer, SDL_LoadBMP("./sprits/bg.bmp")))
	{
		printf("Failed to load background texture!\n");

	}
	charset = SDL_LoadBMP("./sprits/cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);
	//Main loop flag

	char text[128];
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_ShowCursor(SDL_DISABLE);

	rect = SDL_CreateRGBSurface(0, SCREEN_WIDTH, 40, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	int czarny = SDL_MapRGB(rect->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(rect->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(rect->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(rect->format, 0x11, 0x11, 0xCC);
	

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	worldTime = 0;
	distance = 0;
	bgdistance = 0;
	boxDistance1 = 0;
	boxDistance2 = 0;
	BgSpeed = 70;//100px per sec
	boxSpeed = 200;
	finishOfDash = 0;

	for (int i = 0; i < NUM_OF_BOXS; i++)
	{
		boxsOX[i] = SCREEN_WIDTH + (SPACE_BETWEEN_BOXS * i);
	}

	time_before_jump = 0;

	if (!BlueRect.CreateFromSprite(renderer, rect))
	{
		printf("Failed to load blue texture!\n");

	}
	rect = SDL_CreateRGBSurface(0, SCREEN_WIDTH, 40, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	DrawRectangle(rect, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
	sprintf(text, "template for the second project, elapsed time = %.1lf s  %.0lf frames / s", worldTime, distance);
	DrawString(rect, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 10, text, charset);
	sprintf(text, "Esc - wyjscie,  'Z'-jump,  'N'-restart");
	DrawString(rect, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 26, text, charset);
	BlueRect.UpdateTexture(renderer, rect);
	t1 = SDL_GetTicks();
	//While application is running
	while (!quit)
	{
		t2 = SDL_GetTicks();

		// w tym momencie t2-t1 to czas w milisekundach,
		// jaki uplynał od ostatniego narysowania ekranu
		// delta to ten sam czas w sekundach
		// here t2-t1 is the time in milliseconds since
		// the last screen was drawn
		// delta is the same time in seconds
		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;
		bgdistance += BgSpeed * delta;
		quit = unicorn.endOfGame(boxsOX, BoxTexture.getHeight(), BoxTexture.getWidth());
		//Handle events on queue
		while (SDL_PollEvent(&e))
		{
			if (fix)
			{
				unicorn.handleEvent(e);
			}
			switch (e.type) {
			case SDL_KEYDOWN:
				//User requests quit
				if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				else if (e.key.keysym.sym == SDLK_f) {
					if (fix)
					{
						fix = false;
						unicorn.goHome();
					}
					else
					{
						fix = true;
					}

				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			};
			//Handle input for the dot
		}
		//Scroll background

		if (-bgdistance < -gBGTexture.getWidth())
		{
			bgdistance = 0;
		}

		//Clear screen
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		//Render background
		gBGTexture.render(renderer, -bgdistance, 0);
		gBGTexture.render(renderer, gBGTexture.getWidth() - bgdistance, 0);
	
		unicorn.move(boxsOX, BoxTexture.getHeight(), BoxTexture.getWidth());
		

		rect = SDL_CreateRGBSurface(0, SCREEN_WIDTH, 40, 32,
			0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
		DrawRectangle(rect, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		fpsTimer += delta;
		if (fpsTimer > 0.5)
		{
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};
		sprintf(text, "template for the second project, elapsed time = %.1lf s  %.0lf frames / s", worldTime, fps);
		DrawString(rect, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 10, text, charset);
		sprintf(text, "Esc - wyjscie,  'Z'-jump,  'N'-restart");
		DrawString(rect, SCREEN_WIDTH / 2 - strlen(text) * 8 / 2, 26, text, charset);
		BlueRect.UpdateTexture(renderer, rect);
		//Render objects
		BlueRect.render(renderer, 0, 0);
		unicorn.render(renderer, &UnicornTexture);

		//Update screen
		SDL_RenderPresent(renderer);
		frames++;

	}

	//Free resources and close SDL
	close(renderer, Window, UnicornTexture, gBGTexture);

	return 0;


}
