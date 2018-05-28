#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#include "olcConsoleGameEngine.h"		//mechanics of the graphics - please note to add this header propely for visual studio go to properites of project and enable UNICODE set of char! very important


struct sBall
{
	float px, py;	//positions
	float vx, vy;	//speeds
	float ax, ay;	//accelerations
	float radius;	//of the ball of course
	float mass;

	int id;
};


class CirclePhysics : public olcConsoleGameEngine
{
public:
	
	CirclePhysics()
	{
		m_sAppName = L"Circle Physics";
	}

private:
	vector<pair<float, float>> modelCircle;
	vector<sBall> vecBalls;
	sBall *pSelectedBall = nullptr;		// comment this to avoid mouse selection


	// Adds a ball to the vector
	void AddBall(float x, float y, float r = 5.0f)
	{
		sBall b;
		b.px = x; b.py = y;
		//b.vx = rand()%50-25; b.vy = rand()%50-2.5;	//brownian motion condition
		b.vx = rand() % 86 ; b.vy = rand() % 10 - 5;	//flow conditon vy - temperature
		b.ax = 0; b.ay = 0;
		b.radius = r;
		b.mass = r * 10.0f;

		b.id = vecBalls.size();
		vecBalls.emplace_back(b);
	}


public:
	bool OnUserCreate()
	{
		// Define Circle Model
		modelCircle.push_back({ 0.0f, 0.0f });
		int nPoints = 20;
		for (int i = 0; i < nPoints; i++)
			modelCircle.push_back({ cosf(i / (float)(nPoints - 1) * 2.0f * 3.14159f) , sinf(i / (float)(nPoints - 1) * 2.0f * 3.14159f) });

		float fDefaultRad = 8.0f;
		//AddBall(ScreenWidth() * 0.25f, ScreenHeight() * 0.5f, fDefaultRad);
		//AddBall(ScreenWidth() * 0.75f, ScreenHeight() * 0.5f, fDefaultRad);

		
		int ballsScale = 2;	//set scale of the balls 2-very small, 5- quite big
		int ballsDiffrence = 7; //1 - the same balls, - larger - larger diffence between balls

		for (int i = 0; i <250; i++)		// Add X Random Balls

			AddBall(rand() % ScreenWidth(), rand() % ScreenHeight(), rand() % ballsDiffrence + ballsScale);
			//std::this_thread::sleep_for(std::chrono::milliseconds(777));
	
		return true;
	}

	bool OnUserUpdate(float fElapsedTime)
	{
		auto DoCirclesOverlap = [](float x1, float y1, float r1, float x2, float y2, float r2)		//check from pitagoras is the value of the distance between two circles smaller than two radiuses
		{
			return fabs((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) <= (r1 + r2)*(r1 + r2);
		};

		auto IsPointInCircle = [](float x1, float y1, float r1, float px, float py)
		{
			return fabs((x1 - px)*(x1 - px) + (y1 - py)*(y1 - py)) < (r1 * r1);
		};
		//		/*	//comment this to avoid mouse selection
		if (m_mouse[0].bPressed || m_mouse[1].bPressed)		//what happend if we click on mouse button
		{
			pSelectedBall = nullptr;
			for (auto &ball : vecBalls)
			{
				if (IsPointInCircle(ball.px, ball.py, ball.radius, m_mousePosX, m_mousePosY))
				{
					pSelectedBall = &ball;
					break;
				}
			}
		}

		if (m_mouse[1].bHeld)		//what happend if we stay on clicked left button
		{
			if (pSelectedBall != nullptr)
			{
				pSelectedBall->px = m_mousePosX;	//we are setting coordinates by dragging with mouse
				pSelectedBall->py = m_mousePosY;
			}
		}

		if (m_mouse[1].bReleased)	
		{
			pSelectedBall = nullptr;
		}

		if (m_mouse[0].bReleased)	//what happend if we relase click on mouse left button
		{
			if (pSelectedBall != nullptr)
			{
				// Apply velocity
				pSelectedBall->vx = 100.0f * ((pSelectedBall->px) - (float)m_mousePosX);
				pSelectedBall->vy = 100.0f * ((pSelectedBall->py) - (float)m_mousePosY);
			}

			pSelectedBall = nullptr;
		}
		//		*/   //comment this to avoid mouse selection

		vector<pair<sBall*, sBall*>> vecCollidingPairs;		//select colliding balls

		
		//count the average move of the balls
		//int moveHorizontal[101] = {};
		

		// Update Ball Positions

		for (auto &ball : vecBalls)
		{

			float friction = 0.10;		//set friction beetween balls		 1.0-nofriction	0.1<flows	0.17>stucks

			// Add Drag to emulate rolling friction
			ball.ax = -ball.vx * friction;// +1.0f;
			ball.ay = -ball.vy * friction;

			// Update ball physics
			ball.vx += ball.ax * fElapsedTime;
			ball.vy += ball.ay * fElapsedTime;
			ball.px += ball.vx * fElapsedTime;
			ball.py += ball.vy * fElapsedTime;

			
			float frictionWx = 0.7;		//set friction on walls x
			float frictionWy = 0.6;		//set friction on walls y
			
			// Wrap the balls around screen
			
			int speeding = 56; // set speeding constant on x wall


			if (ball.px < 0)
			{
				ball.px += (float)ScreenWidth();
			}
			if (ball.px >= ScreenWidth()) 
			{ 
				ball.px -= (float)ScreenWidth();
				ball.vx += speeding;

				//int box =( -ball.py+ScreenHeight() ) / 100;
				//moveHorizontal[box]++;
			}


			if (ball.py < ball.radius)	//if (ball.py < ScreenHeight()) ball.py += (float)ScreenHeight();     <- for overlap in y
			{
				//ball.py += (float)ScreenHeight();
				ball.vx *= (frictionWx);
				ball.vy *= (-1)*(frictionWy);
				ball.py++;
			}
				
			if (ball.py >=	(ScreenHeight()-ball.radius))	//if (ball.py >= ScreenHeight()) ball.py -= (float)ScreenHeight();	<- for overlap in y
			{
				//ball.py -= (float)ScreenHeight();
				ball.vx *= (frictionWx);
				ball.vy *= (-1)*(frictionWy);
				ball.py--;
			}
			
		
		}

		// Static collisions, i.e. overlap
		for (auto &ball : vecBalls)
		{
			for (auto &target : vecBalls)
			{
				if (ball.id != target.id)
				{
					if (DoCirclesOverlap(ball.px, ball.py, ball.radius, target.px, target.py, target.radius))
					{
						// Collision has occured
						vecCollidingPairs.push_back({ &ball, &target });

						// Distance between ball centers
						float fDistance = sqrtf((ball.px - target.px)*(ball.px - target.px) + (ball.py - target.py)*(ball.py - target.py));

						// Calculate displacement required
						float fOverlap = 1.0f * (fDistance - ball.radius - target.radius);  //changed factor from 0.5 to 1.0

						// Displace Current Ball away from collision	(diffrence is only the sign)
						ball.px -= fOverlap * (ball.px - target.px) / fDistance;	//fDistance is to normalize this, but not nessesary
						ball.py -= fOverlap * (ball.py - target.py) / fDistance;

						// Displace Target Ball away from collision
						target.px += fOverlap * (ball.px - target.px) / fDistance;
						target.py += fOverlap * (ball.py - target.py) / fDistance;
					}
				}
			}
		}

		// Now work out dynamic collisions
		for (auto c : vecCollidingPairs)
		{
			sBall *b1 = c.first;
			sBall *b2 = c.second;

			// Distance between balls
			float fDistance = sqrtf((b1->px - b2->px)*(b1->px - b2->px) + (b1->py - b2->py)*(b1->py - b2->py));

			// Normal
			float nx = (b2->px - b1->px) / fDistance;
			float ny = (b2->py - b1->py) / fDistance;

			// Tangent
			float tx = -ny;
			float ty = nx;

			// Dot Product Tangent
			float dpTan1 = b1->vx * tx + b1->vy * ty;
			float dpTan2 = b2->vx * tx + b2->vy * ty;

			// Dot Product Normal
			float dpNorm1 = b1->vx * nx + b1->vy * ny;
			float dpNorm2 = b2->vx * nx + b2->vy * ny;

			// Conservation of momentum in 1D
			float m1 = (dpNorm1 * (b1->mass - b2->mass) + 2.0f * b2->mass * dpNorm2) / (b1->mass + b2->mass);
			float m2 = (dpNorm2 * (b2->mass - b1->mass) + 2.0f * b1->mass * dpNorm1) / (b1->mass + b2->mass);

			// Update ball velocities
			b1->vx = tx * dpTan1 + nx * m1;
			b1->vy = ty * dpTan1 + ny * m1;
			b2->vx = tx * dpTan2 + nx * m2;
			b2->vy = ty * dpTan2 + ny * m2;

			// Wikipedia Version - Maths is smarter but same
			//float kx = (b1->vx - b2->vx);
			//float ky = (b1->vy - b2->vy);
			//float p = 2.0 * (nx * kx + ny * ky) / (b1->mass + b2->mass);
			//b1->vx = b1->vx - p * b2->mass * nx;
			//b1->vy = b1->vy - p * b2->mass * ny;
			//b2->vx = b2->vx + p * b1->mass * nx;
			//b2->vy = b2->vy + p * b1->mass * ny;
		}

		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), ' ');

		// Draw Balls
		for (auto ball : vecBalls)
			DrawWireFrameModel(modelCircle, ball.px, ball.py, atan2f(ball.vy, ball.vx), ball.radius, FG_WHITE);

		// Draw static collisions
		for (auto c : vecCollidingPairs)
			DrawLine(c.first->px, c.first->py, c.second->px, c.second->py, PIXEL_SOLID, FG_RED);

		// Draw Cue
		if (pSelectedBall != nullptr)																				//comment this to avoid mouse selection
			DrawLine(pSelectedBall->px, pSelectedBall->py, m_mousePosX, m_mousePosY, FG_BLUE);			//comment this to avoid mouse selection

		return true;

	}
	
};


int main()
{
	CirclePhysics game;
	int resolution = 4;					//	8 in error
	int height = 1600/resolution;		//	1360/resolution		160 - in error
	int length = 760/resolution;		//	760 / resolution	120 - in error
	
	if (game.ConstructConsole(height, length, resolution, resolution))		// if program exit with 0x0 try this setting  - fixed resolution (160, 120, 8, 8) an
		game.Start();
	else
		wcout << L"Could not construct console" << endl;

	



	return 0;
};
