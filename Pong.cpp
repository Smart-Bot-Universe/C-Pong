#define OLC_PGE_APPLICATION
#include "../vendor/olcPixelGameEngine/olcPixelGameEngine.h"

// Bugs
// 1: Ball glitch into paddles - Somewhat fixed. Doesn't glitch behind the paddle for long, but it still noticable if you hit it right.
//                               Possible and reasonable fix is to make it a circle intersecting a line collision rather than a rectangle. Maybe fixed
namespace Pong
{
	class Pong : public olc::PixelGameEngine
	{
		struct Vector
		{
			float x, y;
		};

		struct Keyset
		{
			olc::Key up, down;

			Keyset(olc::Key up, olc::Key down)
			{
				this->up = up;
				this->down = down;
			}
		};

		class Ball
		{
			public:
				float x, y, radius;
				Vector velocity;
				bool hitSomething = false;

				Ball(int startX, int startY)
				{
					x = startX;
					y = startY;

					radius = 2;
					velocity.x = 100;
					velocity.y = 100;
				}

				void OnUserUpdate(Pong* pong, float fElapsedTime)
				{
					// Ignore hit something here because I feel like it
					if (y > pong->ScreenHeight())
					{
						velocity.y *= -1;
						y = pong->ScreenHeight() - radius;
					}
					else if (y < 0)
					{
						velocity.y *= -1;
						y = radius;
					}

					if (hitSomething)
					{
						x += radius * (velocity.x / abs(velocity.x)); // offsetX
						y += radius * (velocity.y / abs(velocity.y)); // offsetY
						hitSomething = false;
					}

					x += velocity.x * fElapsedTime;
					y += velocity.y * fElapsedTime;
					pong->FillCircle(x, y, radius, olc::WHITE);
				}
		};

		class Player
		{
			public:
				float x, y, width, height;
				float speed = 50.0f;
				int score;
				Keyset* keyset;

				Player(int x, int y, Keyset* keyset)
				{
					this->x = x;
					this->y = y;
					width = 3;
					height = 20;
					score = 0;
					this->keyset = keyset;
				}

				void OnUserUpdate(Pong* pong, float fElapsedTime)
				{
					if (pong->GetKey(keyset->up).bHeld)
						y -= speed * fElapsedTime;
					else if (pong->GetKey(keyset->down).bHeld)
						y += speed * fElapsedTime;

					pong->FillRect(x, y, width, height, olc::WHITE);
				}
		};

		private:
			Player* players[2];
			Ball* ball;
		public:
			bool OnUserCreate() override
			{
				players[0] = new Player(5, (ScreenHeight() >> 1) - 10, new Keyset(olc::Key::W, olc::Key::S));
				players[1] = new Player(ScreenWidth() - 5, (ScreenHeight() >> 1) - 10, new Keyset(olc::Key::UP, olc::Key::DOWN));

				ball = new Ball(ScreenWidth() >> 1, ScreenHeight() >> 1);
				return true;
			}

			bool OnUserUpdate(float fElapsedTime) override
			{
				Clear(olc::BLACK);

				ball->OnUserUpdate(this, fElapsedTime);

				for (Player* player : players)
				{
					if (BallIntersectsWithPlayer(*player, *ball))
					{
						ball->velocity.x *= -1;
						ball->hitSomething = true;

						if (ball->y < player->y || ball->y > player->y + player->height)
						{
							ball->velocity.y *= -1;
						}
					}
					else if (ball->x < 0)
					{
						ball->x = (ScreenWidth() >> 1);
						ball->y = (ScreenHeight() >> 1);
						ball->velocity.x *= -1;
						players[1]->score++;
					}
					else if (ball->x > ScreenWidth())
					{
						ball->x = (ScreenWidth() >> 1);
						ball->y = (ScreenHeight() >> 1);
						ball->velocity.x *= -1;
						players[0]->score++;
					}
					player->OnUserUpdate(this, fElapsedTime);
				}
				DrawString((ScreenWidth() >> 1) - 100, 2, std::to_string(players[0]->score), olc::WHITE);
				DrawString((ScreenWidth() >> 1) + 100, 2, std::to_string(players[1]->score), olc::WHITE);
				return true;
			}

			inline float Clamp(float value, float min, float max)
			{
				return std::max(min, std::min(max, value));
			}

			bool BallIntersectsWithPlayer(Player r, Ball c)
			{
				Vector rPos =
				{
					Clamp(c.x, r.x, r.x + r.width),
					Clamp(c.y, r.y, r.y + r.height)
				};
				float distance = abs(sqrt(pow(rPos.x - c.x, 2) + pow(rPos.y - c.y, 2)));
				return (distance <= c.radius);
			}
	};
}

int main()
{
	Pong::Pong pong;
	if (pong.Construct(256, 125, 4, 4, false, true, false))
	{
		pong.Start();
	}
	return 0;
}
